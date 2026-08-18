// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: mserv.c,v 1.33 2003/06/05 20:34:48 hurdler Exp $
//
// Copyright (C) 2026 Edward Richardson
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//      Commands used for communicate with the master server
//      Completely rewritten to use DTLS for secure communication and IPv6 support.
//
//-----------------------------------------------------------------------------


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <curl/curl.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
// ws2_32 is already linked, but if there's a compile error uncomment the following line
//#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")
#define CLOSE_SOCKET closesocket
#define SET_NONBLOCKING(s) { u_long mode = 1; ioctlsocket(s, FIONBIO, &mode); }
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#ifndef SOCKET
#define SOCKET int
#endif
#define INVALID_SOCKET -1
#define CLOSE_SOCKET close
#define SET_NONBLOCKING(s) { fcntl(s, F_SETFL, O_NONBLOCK); }
#endif

#ifdef __OS2__
#include <errno.h>
#endif

#include "doomdef.h"
#include "command.h"
#include "console.h"
#include "mserv.h"
#include "i_tcp.h"
#include "i_net.h"
#include "i_system.h"
#include "d_clisrv.h"
#include "z_zone.h"
#include "uuid.h"

// ================================ DEFINITIONS ===============================

#define PACKET_SIZE 1024

#define  MS_NO_ERROR                   0
#define  MS_SOCKET_ERROR            -201
#define  MS_CONNECT_ERROR           -203
#define  MS_WRITE_ERROR             -210
#define  MS_READ_ERROR              -211
#define  MS_CLOSE_ERROR             -212
#define  MS_GETHOSTBYNAME_ERROR     -220
#define  MS_GETHOSTNAME_ERROR       -221
#define  MS_TIMEOUT_ERROR           -231

#define MS_TOKEN 0
#define MS_PING 1
#define MS_DELETE 2
#define MS_LISTSERVERS 3
#define MS_RES_TOKEN 4
#define MS_RES_BADTOKEN 5
#define MS_RES_PONG 6

#define DTLS_PACKET_WAITING -2

struct socketaddrm_s
{
    union
    {
        struct sockaddr_in ipv4;
        struct sockaddr_in6 ipv6;
    };
    int i_family; // AF_INET or AF_INET6
} typedef socketaddrms_t;

static void Command_Listserv_f(void);
//TODO: when we change the port or ip, unregister to the old master server, register to the new one

#define DEF_PORT "28910"
consvar_t cv_internetserver = { "internetserver", "Yes", CV_SAVE, CV_YesNo };
consvar_t cv_masterserver = { "masterserver", "master.crantime.org:28910", CV_SAVE, NULL };
consvar_t cv_masterservercert = { "masterservercert", "https://crantime.org/content/legacyneo", CV_SAVE, NULL };
consvar_t cv_servername = { "servername", "DooM Legacy server", CV_SAVE, NULL };
consvar_t cv_defaultipv6 = { "defaultipv6", "No", CV_SAVE, CV_YesNo };

enum { MSCS_NONE, MSCS_WAITING, MSCS_REGISTERED, MSCS_FAILED } con_state = MSCS_NONE;

struct socket_s
{
    SOCKET sock; // The underlying socket
    SSL* ssl; // The SSL object for DTLS
    uint64_t lastActive; // Timestamp of the last activity on the socket
} typedef socket_t;

static socket_t msSocket[2]; // Master server sockets
static SSL_CTX* sslCtx = NULL; // OpenSSL context for DTLS
static byte msToken[16]; // Token received from the master server for registration
static BOOL bNetworkInit = FALSE;
static BOOL bDTLSMSConfigured = FALSE; // Is the master server certificate configured for DTLS?
static BOOL bHaveToken = FALSE; // Do we have a valid token from the master server?

static int  MS_Connect(char* ip_addr, int async);

// Register the console variables and commands related to the master server
void AddMServCommands(void)
{
    CV_RegisterVar(&cv_internetserver);
    CV_RegisterVar(&cv_masterserver);
    CV_RegisterVar(&cv_masterservercert);
    CV_RegisterVar(&cv_servername);
    CV_RegisterVar(&cv_defaultipv6);
    COM_AddCommand("listserv", Command_Listserv_f);
}

// Global Networking Initialisation 
static void InitNetworking(void)
{
    if (bNetworkInit)
    {
        return;
    }
    bNetworkInit = TRUE;

    I_InitTcpDriver();

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    memset(msSocket, 0, sizeof(msSocket));
}

// Callback function for CURL to write received data into a BIO memory buffer
static size_t curl_to_bio_callback(void* contents, size_t size, size_t nmemb, void* userp) 
{
    BIO* mem_bio = (BIO*)userp;
    size_t total_bytes = size * nmemb;
    BIO_write(mem_bio, contents, (int)total_bytes);
    return total_bytes;
}

// Grab the master server certificate from a remote URL and parse it into an X509 object
static X509* CURLFetchCertificate(const char* url)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();
    if (!curl)
    {
        return NULL;
    }

    BIO* mem_bio = BIO_new(BIO_s_mem());
    X509* cert = NULL;

    // Configure libcurl target and callbacks
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Safely follow any HTTP redirects automatically
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_to_bio_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)mem_bio);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); 

    CONS_Printf("[HTTP Sync] Fetching master asset via libcurl from: %s\n", url);

    // Synchronous download
    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK)
    {
        cert = PEM_read_bio_X509(mem_bio, NULL, NULL, NULL);
        if (cert) 
        {
            CONS_Printf("[HTTP Sync] Certificate successfully parsed from network memory stream!\n");
        }
        else 
        {
            CONS_Printf("[HTTP Error] Failed to decode raw PEM.\n");
        }
    }
    else 
    {
        CONS_Printf("[HTTP Error] libcurl network transport request failed: %s\n", curl_easy_strerror(res));
    }

    // Cleanup
    BIO_free(mem_bio);
    curl_easy_cleanup(curl);
    return cert;
}

// Returns 1 if the certificate is expired, or 0 if it is still valid
static int DTLSIsCertExpired(X509* cert)
{
    if (!cert)
    {
        return 1;
    }

    // Extract the expiration timestamp from the certificate
    const ASN1_TIME* not_after = X509_get0_notAfter(cert);
    if (!not_after) 
    {
        CONS_Printf("[Crypto Warning] Certificate is missing an expiration timestamp!\n");
        return 1; // Treat as invalid if it has no time data
    }

    // Compare the certificate's expiration timestamp against current system time
    int time_check = X509_cmp_time(not_after, NULL);
    if (time_check < 0) 
    {
        CONS_Printf("[Crypto Check] Certificate has EXPIRED!\n");
        return 1;
    }

    CONS_Printf("[Crypto Check] Certificate is valid and active.\n");
    return 0;
}

// Attempt to load the master server certificate from a local cache, and if that fails, fetch it from a remote URL
static X509* DTLSGetMasterCertificate(const char* cache_path, const char* fallback_url)
{
    X509* cert = NULL;

    // Attempt to read the certificate from the local disk cache
    BIO* file_in = BIO_new_file(cache_path, "r");
    if (file_in)
    {
        cert = PEM_read_bio_X509(file_in, NULL, NULL, NULL); // Reads standard ASCII PEM out of file
        BIO_free(file_in);
    }

    if (cert && !DTLSIsCertExpired(cert))
    {
        CONS_Printf("[Cache Hit] Loaded master certificate successfully from local disk: %s\n", cache_path);
        return cert;
    }

    CONS_Printf("[Cache Miss] No local copy found available. Fetching remote copy...\n");
    cert = CURLFetchCertificate(fallback_url);
    if (!cert)
    {
        CONS_Printf("[CRITICAL] Could not resolve master certificate via local cache OR remote fallback server!\n");
        return NULL;
    }

    // Save the downloaded object to local disk for future sessions
    BIO* file_out = BIO_new_file(cache_path, "w");
    if (file_out)
    {
        if (PEM_write_bio_X509(file_out, cert) == 1)
        {
            CONS_Printf("[Cache Saved] Successfully cached public verification copy to disk: %s\n", cache_path);
        }
        else
        {
            CONS_Printf("[Cache Warning] Failed to parse and write memory object to file path storage.\n");
        }
        BIO_free(file_out);
    }

    return cert;
}

// Get or create the SSL_CTX for DTLS connections
static SSL_CTX* GetDTLSContext()
{
    if (sslCtx)
    {
        return sslCtx;
    }

    const SSL_METHOD* method = DTLSv1_2_client_method();
    sslCtx = SSL_CTX_new(method);
    if (!sslCtx)
    {
        CONS_Printf("Unable to create SSL context\n");
        return NULL;
    }

    return sslCtx;
}

// Cleanup the SSL_CTX and reset the DTLS configuration state
static void CleanupDTLSContext()
{
    if (sslCtx)
    {
        SSL_CTX_free(sslCtx);
        sslCtx = NULL;
    }

    bDTLSMSConfigured = FALSE;
}

// Cleanup the DTLS socket and free associated resources
static void CleanupSocket(socket_t* dtls_socket)
{
    if (dtls_socket->ssl)
    {
        SSL_shutdown(dtls_socket->ssl);
        SSL_free(dtls_socket->ssl);
        dtls_socket->ssl = NULL;
    }
    if (dtls_socket->sock != INVALID_SOCKET)
    {
        CLOSE_SOCKET(dtls_socket->sock);
        dtls_socket->sock = INVALID_SOCKET;
    }
}

// Read a packet from the DTLS socket
static int DTLSReadPacket(socket_t* dtls_socket, char* buffer, size_t buffer_size)
{
    int bytes_read, ssl_error;

    if (!dtls_socket || !dtls_socket->ssl)
    {
        CONS_Printf("[DTLS Error] Attempted to read packet on uninitialized DTLS socket.\n");
        return -1;
    }
    bytes_read = SSL_read(dtls_socket->ssl, buffer, (int)buffer_size);
    if (bytes_read <= 0)
    {
        ssl_error = SSL_get_error(dtls_socket->ssl, bytes_read);
        if (ssl_error == SSL_ERROR_WANT_READ || ssl_error == SSL_ERROR_WANT_WRITE)
        {
            // Non-blocking operation
            return DTLS_PACKET_WAITING;
        }
        CONS_Printf("[DTLS Error] Failed to read packet. SSL_read returned %d, SSL_get_error code: %d\n", bytes_read, ssl_error);
        CleanupSocket(dtls_socket);
        return -1;
    }
    dtls_socket->lastActive = I_GetTime();
    return bytes_read;
}

// Send a packet over the DTLS socket
static void DTLSSendPacket(socket_t* dtls_socket, const char* data, size_t length)
{
    int bytes_sent, ssl_error;
    while (true)
    {
        if (!dtls_socket || !dtls_socket->ssl)
        {
            CONS_Printf("[DTLS Error] Attempted to send packet on uninitialized DTLS socket.\n");
            return;
        }

        bytes_sent = SSL_write(dtls_socket->ssl, data, (int)length);
        if (bytes_sent <= 0)
        {
            ssl_error = SSL_get_error(dtls_socket->ssl, bytes_sent);
            if (ssl_error == SSL_ERROR_WANT_READ || ssl_error == SSL_ERROR_WANT_WRITE)
            {
                // Non-blocking operation, retry
                continue;
            }

            CONS_Printf("[DTLS Error] Failed to send packet. SSL_write returned %d, SSL_get_error code: %d\n", bytes_sent, ssl_error);
            CleanupSocket(dtls_socket);
        }
        return;
    }
}

// Configure the DTLS context to trust the master server's public certificate, enabling secure communication
static void DTLSConfigureMasterServerCertificate(const char* certAddress)
{
    if (bDTLSMSConfigured)
    {
        return;
    }

    SSL_CTX* ctx = GetDTLSContext();
    if (ctx == NULL)
    {
        return;
    }

    const char* cert_file = "master_public.crt";
    const char* initalPath = I_GetConfigDir();
    const size_t path_length = strlen(initalPath) + strlen(cert_file) + 2;
    const size_t serverPath_length = strlen(certAddress) + strlen(cert_file) + 2;
    char* filePath = Z_Malloc(path_length, PU_STATIC, NULL);
    char* serverPath = Z_Malloc(serverPath_length, PU_STATIC, NULL);
    snprintf(filePath, path_length, "%s/%s", initalPath, cert_file);
    snprintf(serverPath, serverPath_length, "%s/%s", certAddress, cert_file);

    X509* master_cert = DTLSGetMasterCertificate(filePath, serverPath);
    Z_Free(filePath);
    Z_Free(serverPath);

    X509_STORE* store = SSL_CTX_get_cert_store(ctx);
    if (X509_STORE_add_cert(store, master_cert) != 1)
    {
        CONS_Printf("[DTLS Error] Failed to register downloaded certificate into active memory store.\n");
        return;
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    X509_free(master_cert);

    bDTLSMSConfigured = true;
}

// Establish a DTLS connection to the specified remote IP and port, returning a socket_t structure
static socket_t DTLSConnectSocket(socketaddrms_t* server_addr)
{
    socket_t dtls_socket;
    dtls_socket.sock = INVALID_SOCKET;
    dtls_socket.ssl = NULL;
    dtls_socket.lastActive = I_GetTime();
    SSL_CTX* ctx = GetDTLSContext();
    const int port = server_addr->i_family == AF_INET6 ? ntohs(server_addr->ipv6.sin6_port) : ntohs(server_addr->ipv4.sin_port);
    const int server_addrlen = server_addr->i_family == AF_INET6 ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in);
    char ip_str[INET6_ADDRSTRLEN];

    // UDP socket
    dtls_socket.sock = socket(server_addr->i_family, SOCK_DGRAM, 0);
    if (dtls_socket.sock == INVALID_SOCKET)
    {
        CONS_Printf("Socket creation failed\n");
        return dtls_socket;
    }

    // Bind socket to remote endpoint so OpenSSL's BIO knows exactly where to direct datagrams
    if (connect(dtls_socket.sock, (struct sockaddr*)server_addr, server_addrlen) < 0)
    {
        CONS_Printf("UDP socket connect failed\n");
        CleanupSocket(&dtls_socket);
        return dtls_socket;
    }

    // Bind OpenSSL to the socket via a Datagram BIO (Basic Input/Output)
    BIO* bio = BIO_new_dgram((int)dtls_socket.sock, BIO_NOCLOSE);
    BIO_ctrl(bio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, server_addr);

    dtls_socket.ssl = SSL_new(ctx);
    SSL_set_bio(dtls_socket.ssl, bio, bio);


    if (server_addr->i_family == AF_INET)
    {
        inet_ntop(AF_INET, &server_addr->ipv4.sin_addr, ip_str, INET6_ADDRSTRLEN);
    }
    else
    {
        inet_ntop(AF_INET6, &server_addr->ipv6.sin6_addr, ip_str, INET6_ADDRSTRLEN);
    }

    // Perform the DTLS Handshake
    // Set the MTU to 1200 bytes to avoid fragmentation issues
    BIO_ctrl(bio, BIO_CTRL_DGRAM_SET_MTU, 1200, NULL);
    SET_NONBLOCKING(dtls_socket.sock);
    CONS_Printf("Initiating DTLS handshake to %s:%hu...\n", ip_str, port);

    const int timeoutTics = TICRATE * 5; // 5 seconds timeout
    uint64_t start_time = I_GetTime();

    int handshake_result = SSL_connect(dtls_socket.ssl);
    if (handshake_result <= 0)
    {
        int ssl_error = SSL_get_error(dtls_socket.ssl, handshake_result);
        while (ssl_error == SSL_ERROR_WANT_READ || ssl_error == SSL_ERROR_WANT_WRITE)
        {
            if (I_GetTime() - start_time >= timeoutTics)
            {
                break; // Timeout reached, exit the loop
            }
            handshake_result = SSL_connect(dtls_socket.ssl);
            if (handshake_result > 0)
            {
                break; // Handshake successful, exit the loop
            }
            ssl_error = SSL_get_error(dtls_socket.ssl, handshake_result);
            DTLSv1_handle_timeout(dtls_socket.ssl);
        }
    }
    if (handshake_result <= 0)
    {
        int ssl_error = SSL_get_error(dtls_socket.ssl, handshake_result);
        if(ssl_error == SSL_ERROR_WANT_READ || ssl_error == SSL_ERROR_WANT_WRITE)
        {
            CONS_Printf("[DTLS Error] Handshake timed out after 5 seconds.\n");
        }
        else
        {
            CONS_Printf("\n[DTLS Error Detected] Handshake Result: %d, SSL_get_error code: %d\n", handshake_result, ssl_error);

            // Read the descriptive text directly from the OpenSSL thread queue
            unsigned long err_code;
            char error_string[256];

            while ((err_code = ERR_get_error()) != 0)
            {
                ERR_error_string_n(err_code, error_string, sizeof(error_string));
                CONS_Printf("OpenSSL Queue Error: %s\n", error_string);
            }

            // Check specific structural code contexts
            if (ssl_error == SSL_ERROR_SYSCALL)
            {
#ifdef _WIN32
                CONS_Printf("System Socket Error: %d\n", WSAGetLastError());
#else
                CONS_Printf("System Socket Error: %d\n", errno);
#endif
            }
        }

        CleanupSocket(&dtls_socket);
        return dtls_socket;
    }

    CONS_Printf("Connected securely to %s:%hu!\n", ip_str, port);
    return dtls_socket;
}

// Check if the master server is connected, and if not, attempt to reconnect. Returns 1 if connected, 0 otherwise.
static int CheckMasterConnected()
{
    // If the socket has been inactive for more than the timeout, destroy it and reconnect
    const int timeoutTics = ((TICRATE * 60) * 2) - ((TICRATE * 60) / 4); // 1 minute and 45 seconds
    if (msSocket[0].lastActive + timeoutTics < I_GetTime())
    {
        CleanupSocket(&msSocket[0]);
    }

    // Attempt to connect to the master server if not already connected
    if (msSocket[0].sock == INVALID_SOCKET || msSocket[0].ssl == NULL)
    {
        MS_Connect(cv_masterserver.string, 0);
    }

    return (msSocket[0].sock != INVALID_SOCKET && msSocket[0].ssl != NULL);
}

// An old get server list function that seems to have been mothballed. Will need to restore later.
static int GetServersList(void)
{
#if 1
#else
    msg_t   msg;
    int     count = 0;

    msg.type = GET_SERVER_MSG;
    msg.length = 0;
    if (MS_Write(&msg) < 0)
        return MS_WRITE_ERROR;

    while (MS_Read(&msg) >= 0)
    {
        if (msg.length == 0)
        {
            if (!count)
                CONS_Printf("No server currently running.\n");
            return MS_NO_ERROR;
        }
        count++;
        CONS_Printf(msg.buffer);
    }
#endif
    return MS_READ_ERROR;
}

#define NUM_LIST_SERVER 32
msg_server_t *GetShortServersList(void)
{
    static msg_server_t server_list[NUM_LIST_SERVER+1]; // +1 for easy test
    int                 i;

    if (CheckMasterConnected() == 0)
    {
        CONS_Printf("cannot connect to the master server\n");
        return NULL;
    }

    const byte send_data[1] = { MS_LISTSERVERS };
    DTLSSendPacket(&msSocket[0], send_data, sizeof(send_data));

    const int timeoutTics = TICRATE * 5; // 5 seconds timeout
    uint64_t start_time = I_GetTime();
    byte recv_data[1500];
    int  recv_len = 0;
    int  count = 0;

    while(I_GetTime() - start_time < timeoutTics)
    {
        recv_len = DTLSReadPacket(&msSocket[0], (char*)recv_data, sizeof(recv_data));
        if (recv_len != DTLS_PACKET_WAITING)
        {
            break;
        }
    }

    if (recv_len == DTLS_PACKET_WAITING)
    {
        CONS_Printf("Server timed out\n");
        return server_list;
    }
    else if (recv_len <= -1)
    {
        CONS_Printf("Some kind of server error\n");
        return server_list;
    }

    if (recv_len <= 0 || (recv_len % 6) != 0)
    {
        CONS_Printf("No server currently running.\n");
        return server_list;
    }
    for (i = 0; i < NUM_LIST_SERVER; ++i)
    {
        if (count + 6 > recv_len)
            break;
        snprintf(server_list[i].ip, sizeof(server_list[i].ip), "%d.%d.%d.%d",
                 recv_data[count], recv_data[count + 1],
                 recv_data[count + 2], recv_data[count + 3]);
        server_list[i].port[0] = '\0';
        snprintf(server_list[i].port, sizeof(server_list[i].port), "%d", (recv_data[count + 4] << 8) | recv_data[count + 5]);
        server_list[i].name[0] = '\0';
        server_list[i].version[0] = '\0';
        server_list[i].header[0] = 1;
		count += 6;
    }
	server_list[i].header[0] = 0;
	return server_list;
}

static void Command_Listserv_f(void)
{
    if (con_state == MSCS_WAITING)
    {
        CONS_Printf("Not yet registered to the master server.\n");
        return;
    }

    CONS_Printf("Retrieving server list...\n");

    if (MS_Connect(cv_masterserver.string, 0))
    {
        CONS_Printf("cannot connect to the master server\n");
        return;
    }

    if (GetServersList())
        CONS_Printf("cannot get server list\n");

    CleanupSocket(&msSocket[0]);
}

int ConnectionFailed(void)
{
    con_state = MSCS_FAILED;
    CONS_Printf("Connection to master server failed\n");
    CleanupSocket(&msSocket[0]);
    return MS_CONNECT_ERROR;
}

static int RemoveFromMasterSever(void)
{
    const byte send_data[3] = { MS_DELETE, (sock_port >> 8) & 0xFF, sock_port & 0xFF };
    DTLSSendPacket(&msSocket[0], send_data, sizeof(send_data));
    return MS_NO_ERROR;
}

static void CheckServerPingResponses(socket_t* dtls_socket)
{
    if (dtls_socket->sock == INVALID_SOCKET)
    {
        return;
    }

    byte recv_data[1500];
    int  recv_len = 0;
    recv_len = DTLSReadPacket(dtls_socket, (char*)recv_data, sizeof(recv_data));
    if(recv_len <= 0)
    {
        return;
    }
    if (recv_data[0] == MS_RES_BADTOKEN)
    {
        bHaveToken = false;
    }
    else if(recv_data[0] == MS_RES_PONG)
    {
        // No action needed, just a response to our ping
    }
    else if(recv_data[0] == MS_RES_TOKEN && recv_len == 17)
    {
        bHaveToken = true;
        memcpy(msToken, &recv_data[1], 16);
    }
    else
    {
        CONS_Printf("Received unknown response from master server: %d\n", recv_data[0]);
    }
}

void SendPingToMasterServer(void)
{
    static tic_t   next_time = 0;
    static int     lastSendFrom = 0;
    tic_t          cur_time;

    CheckServerPingResponses(&msSocket[0]);
    CheckServerPingResponses(&msSocket[1]);

    cur_time = I_GetTime();
    if (cur_time > next_time && sock_port >= 0 && CheckMasterConnected()) // ping every 30 seconds if possible
    {
        if (!bHaveToken)
        {
            next_time = cur_time + 10 * TICRATE;

            // Generate and send a new token to the master server for registration
            // This token is not kept around, the server uses it to identify that both connections are from the same server
            const byte send_data[19] = { MS_TOKEN, (sock_port >> 8) & 0xFF, sock_port & 0xFF };
            const byte* token_ptr = GenerateUUID();
            memcpy((void*)&send_data[3], token_ptr, 16);

            DTLSSendPacket(&msSocket[0], send_data, sizeof(send_data));
            if (msSocket[1].sock != INVALID_SOCKET)
            {
                DTLSSendPacket(&msSocket[1], send_data, sizeof(send_data));
            }
        }
        else
        {
            next_time = cur_time + 30 * TICRATE;

            const byte send_data[17] = { MS_PING };
            memcpy((void*)&send_data[1], msToken, 16);
            lastSendFrom = (lastSendFrom+1) & 1; // Alternate between the two sockets for sending pings
            if (msSocket[lastSendFrom].sock == INVALID_SOCKET)
            {
                lastSendFrom = 0;
            }
            DTLSSendPacket(&msSocket[lastSendFrom], send_data, sizeof(send_data));
        }
    }
}

void UnregisterServer()
{
    // We can cheat and just close the socket, SSL will alert the master server that we are gone already.
    bHaveToken = false;
    CleanupSocket(&msSocket[0]);
}

static int ResolveAddress(const char* input_str, const char* default_port, socketaddrms_t* out_addr4, socketaddrms_t* out_addr6)
{
    char* input_copy = strdup(input_str);
    if (!input_copy) return -1;

    char* host = input_copy;
    char* port = NULL;

    // Handle IPv6 literal addresses like [::1]:8080
    if (host[0] == '[') 
    {
        host++;
        char* end_bracket = strchr(host, ']');
        if (end_bracket) 
        {
            *end_bracket = '\0';
            if (*(end_bracket + 1) == ':') 
            {
                port = end_bracket + 2;
            }
        }
    }
    else 
    {
        // Handle IPv4 or regular domain names like example.com:8080
        char* last_colon = strrchr(host, ':');
        // Ensure it's a port colon and not part of an unbracketed IPv6 address
        if (last_colon && !strchr(host, ':') != (last_colon == strchr(host, ':'))) 
        {
            *last_colon = '\0';
            port = last_colon + 1;
        }
    }

    // If no port was found in the string, use the fallback default port
    if (!port || strlen(port) == 0) 
    {
        port = (char*)default_port;
    }

    // Configure hints for getaddrinfo
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     // Allow both IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    struct addrinfo* result = NULL;
    int s = getaddrinfo(host, port, &hints, &result);
    if (s != 0) 
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(s));
        free(input_copy);
        return -1;
    }

    // Find both the first IPv4 and IPv6 addresses
    struct addrinfo* rp;
    boolean found_ipv4 = false;
    boolean found_ipv6 = false;
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        if (rp->ai_family == AF_INET && out_addr4 != NULL && !found_ipv4)
        {
            memcpy(&out_addr4->ipv4, rp->ai_addr, rp->ai_addrlen);
            out_addr4->i_family = AF_INET;
            found_ipv4 = true;
        }
        else if (rp->ai_family == AF_INET6 && out_addr6 != NULL && !found_ipv6)
        {
            memcpy(&out_addr6->ipv6, rp->ai_addr, rp->ai_addrlen);
            out_addr6->i_family = AF_INET6;
            found_ipv6 = true;
        }
    }

    freeaddrinfo(result);
    free(input_copy);
    return found_ipv4 || found_ipv6 ? 1 : -1;
}

// Attempt connection to the master server.
static int MS_Connect(char *ip_addr, int all)
{
    InitNetworking();
    DTLSConfigureMasterServerCertificate(cv_masterservercert.string);

    bHaveToken = false;

    CleanupSocket(&msSocket[0]);
    CleanupSocket(&msSocket[1]);

    socketaddrms_t addr4;
    socketaddrms_t addr6;

    memset(&addr4, 0, sizeof(addr4));
    memset(&addr6, 0, sizeof(addr6));

    if (ResolveAddress(ip_addr, DEF_PORT, &addr4, &addr6) <= 0)
        return MS_GETHOSTBYNAME_ERROR;

    int in = 0;
    if (addr4.i_family == AF_INET) 
    {
        msSocket[in] = DTLSConnectSocket(&addr4);
        if (msSocket[in].sock != INVALID_SOCKET)
        {
            in++;
        }
    }
    if (addr6.i_family == AF_INET6)
    {
        msSocket[in] = DTLSConnectSocket(&addr6);
    }
    
    return 0;
}
