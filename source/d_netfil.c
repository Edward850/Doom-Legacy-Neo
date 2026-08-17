// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: d_netfil.c,v 1.25 2003/05/04 04:28:33 sburke Exp $
//
// Copyright (C) 1998-2000 by DooM Legacy Team.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
//
// $Log: d_netfil.c,v $
// Revision 1.25  2003/05/04 04:28:33  sburke
// Use SHORT to convert network data between big- and little-endian format.
//
// Revision 1.24  2001/07/28 16:18:37  bpereira
// no message
//
// Revision 1.23  2001/05/21 16:23:32  crashrl
// *** empty log message ***
//
// Revision 1.22  2001/05/21 14:57:05  crashrl
// Readded directory crawling file search function
//
// Revision 1.21  2001/05/16 17:12:52  crashrl
// Added md5-sum support, removed recursiv wad search
//
// Revision 1.20  2001/05/14 19:02:58  metzgermeister
//   * Fixed floor not moving up with player on E3M1
//   * Fixed crash due to oversized string in screen message ... bad bug!
//   * Corrected some typos
//   * fixed sound bug in SDL
//
// Revision 1.19  2001/04/17 22:26:07  calumr
// Initial Mac add
//
// Revision 1.18  2001/03/30 17:12:49  bpereira
// no message
//
// Revision 1.17  2001/02/24 13:35:19  bpereira
// no message
//
// Revision 1.16  2001/02/13 20:37:27  metzgermeister
// *** empty log message ***
//
// Revision 1.15  2001/02/10 12:27:13  bpereira
// no message
//
// Revision 1.14  2001/01/25 22:15:41  bpereira
// added heretic support
//
// Revision 1.13  2000/10/08 13:30:00  bpereira
// no message
//
// Revision 1.12  2000/10/02 18:25:44  bpereira
// no message
//
// Revision 1.11  2000/09/28 20:57:14  bpereira
// no message
//
// Revision 1.10  2000/09/10 10:39:06  metzgermeister
// *** empty log message ***
//
// Revision 1.9  2000/08/31 14:30:55  bpereira
// no message
//
// Revision 1.8  2000/08/11 19:10:13  metzgermeister
// *** empty log message ***
//
// Revision 1.7  2000/08/10 14:52:38  ydario
// OS/2 port
//
// Revision 1.6  2000/04/16 18:38:07  bpereira
// no message
//
// Revision 1.5  2000/03/07 03:32:24  hurdler
// fix linux compilation
//
// Revision 1.4  2000/03/05 17:10:56  bpereira
// no message
//
// Revision 1.3  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.2  2000/02/26 00:28:42  hurdler
// Mostly bug fix (see borislog.txt 23-2-2000, 24-2-2000)
//
//
// DESCRIPTION:
//      Transfer a file using HSendPacket
//
//-----------------------------------------------------------------------------


#include <stdio.h>
#include <fcntl.h>
#ifdef __OS2__
#include <sys/types.h>
#endif // __OS2__
#include <sys/stat.h>

#include <time.h>

#if defined( WIN32) || defined( __DJGPP__ ) 
#include <io.h>
#include <direct.h>
#else
#include <sys/types.h>
#include <dirent.h>
#include <utime.h>
#endif

#ifndef __WIN32__
#include <unistd.h>
#else
#include <sys/utime.h>
#endif
#ifdef __DJGPP__
#include <dir.h>
#include <utime.h>
#endif

#include "doomdef.h"
#include "doomstat.h"
#include "d_clisrv.h"
#include "g_game.h"
#include "i_net.h"
#include "i_system.h"
#include "m_argv.h"
#include "d_net.h"
#include "w_wad.h"
#include "d_netfil.h"
#include "z_zone.h"
#include "byteptr.h"
#include "p_setup.h"
#include "m_misc.h"
#include "m_menu.h"
#include "md5.h"
#include "filesrch.h"

// sender structure
typedef struct filetx_s {
    int      ram;
    char     *filename;   // name of the file or ptr of the data in ram
    ULONG    size;
    char     fileid;
    int      node;        // destination
    struct filetx_s *next; // a queu
} filetx_t;

// current transfers (one for eatch node)
struct {
   filetx_t  *txlist; 
   ULONG      position;
   FILE*      currentfile;
} transfer[MAXNETNODES];

// read time of file : stat _stmtime
// write time of file : utime

// receiver structure
int fileneedednum;
fileneeded_t fileneeded[MAX_WADFILES];
char *downloaddir="DOWNLOAD";


// fill the serverinfo packet with wad file loaded
char *PutFileNeeded(void)
{
    int   i;
    char *p;
    char  wadfilename[MAX_WADPATH];

    p=(byte *)&netbuffer->u.serverinfo.fileneeded;
    for(i=0;i<numwadfiles;i++)
    {
        WRITEULONG(p,wadfiles[i]->filesize);
        strcpy(wadfilename,wadfiles[i]->filename);
        nameonly(wadfilename);
        WRITESTRING(p,wadfilename);
        WRITEMEM(p,wadfiles[i]->md5sum,16);
    }
    netbuffer->u.serverinfo.fileneedednum=i;
    return p;
}

// parce the serverinfo packet and fill fileneeded table on client 
void D_ParseFileneeded(int fileneedednum_parm, char *fileneededstr)
{
    int i;
    byte *p;

    fileneedednum=fileneedednum_parm;
    p=fileneededstr;
    for(i=0;i<fileneedednum;i++)
    {
        fileneeded[i].status = FS_NOTFOUND;
        fileneeded[i].totalsize = READULONG(p);
        fileneeded[i].phandle = NULL;
        READSTRING(p,fileneeded[i].filename);
        READMEM(p,fileneeded[i].md5sum,16);
    }
}

// 
void CL_PrepareDownloadSaveGame(const char *tmpsave)
{
    fileneedednum = 1;
    fileneeded[0].status = FS_REQUESTED;
    fileneeded[0].totalsize = -1;
    fileneeded[0].phandle = NULL;
    memset(fileneeded[0].md5sum,0,16);
    strcpy(fileneeded[0].filename,tmpsave);
}


// send the name of file who status is FS_NOTFOUND of the fileneeded table
boolean SendRequestFile(void)
{
    char  *p;
    int   i;
    ULONG totalfreespaceneeded=0;
    INT64 availablefreespace;

    if( M_CheckParm("-nodownload") )
    {
        char s[1024]="";
//        char *smd5;
//        int j;

        for(i=0;i<fileneedednum;i++)
            if( fileneeded[i].status!=FS_FOUND )
            {
                strcat(s,"  \"");
                strcat(s,fileneeded[i].filename);
                strcat(s,"\"");
                if(fileneeded[i].status==FS_NOTFOUND)
                {
                    strcat(s," not found");
                } 
                else if(fileneeded[i].status==FS_MD5SUMBAD)
                {
                    int j;
                    int strl;

                    strcat(s," has wrong md5sum, needs: ");
                    strl = strlen(s);

                    for(j=0; j<16; j++)
                    {
                        sprintf(&s[strl+2*j],"%02x", fileneeded[i].md5sum[j]);
                    }
                    s[strl+32]='\0';
                }
                else if(fileneeded[i].status==FS_OPEN)
                    strcat(s," found, ok");
                strcat(s,"\n");
            }
        I_Error("To play with this server you should have this files:\n%s\n"
                "remove -nodownload if you want to download the files!\n",s);
    }

    netbuffer->packettype = PT_REQUESTFILE;
    p=netbuffer->u.textcmd;
    for(i=0;i<fileneedednum;i++)
        if( fileneeded[i].status==FS_NOTFOUND || fileneeded[i].status == FS_MD5SUMBAD)
        {
            if( fileneeded[i].status==FS_NOTFOUND )
                totalfreespaceneeded += fileneeded[i].totalsize;
            nameonly(fileneeded[i].filename);
            WRITECHAR(p,i);  // fileid
            WRITESTRING(p,fileneeded[i].filename);
            // put it in download dir 
            strcatbf(fileneeded[i].filename,downloaddir,"/");
            fileneeded[i].status = FS_REQUESTED;
        }
    WRITECHAR(p,-1);
    I_GetDiskFreeSpace(&availablefreespace);
//    CONS_Printf("free byte %d\n",availablefreespace);
    if(totalfreespaceneeded>availablefreespace)
        I_Error("To play on this server you should download %dKb\n"
                "but you have only %dKb freespace on this drive\n",
                totalfreespaceneeded,availablefreespace);

    // prepare to download
    I_mkdir(downloaddir,0755);
    return HSendPacket(servernode,true,0,p-(char *)netbuffer->u.textcmd);
}

// get request filepak and put it to the send queue
void Got_RequestFilePak(int node)
{
    char *p;

    p=netbuffer->u.textcmd;
    while(*p!=-1)
    {
        SendFile(node,p+1,*p);
        p++; // skip fileid
        SKIPSTRING(p);
    }
}


// client check if the filedeened arent already loaded or on the disk
int CL_CheckFiles(void)
{
    int  i,j;
    char wadfilename[MAX_WADPATH];
    int  ret=1;

    if( M_CheckParm("-nofiles") )
        return 1;

    // the first is the iwad (the main wad file)
    // do not check file date, also don't donwload it (copyright problem) 
    strcpy(wadfilename,wadfiles[0]->filename);
    nameonly(wadfilename);
    if( stricmp(wadfilename,fileneeded[0].filename)!=0 )
    {
        M_StartMessage(va("You cannot connect to this server\n"
                          "since it uses %s\n"
                          "You are using %s\n",
                          fileneeded[0].filename,wadfilename), NULL, MM_NOTHING);
        return 2;
    }
    fileneeded[0].status=FS_OPEN;

    for (i=1;i<fileneedednum;i++)
    {
        if(devparm) CONS_Printf("searching for '%s' ",fileneeded[i].filename);
        
        // check in allready loaded files
        for(j=1;wadfiles[j];j++)
        {
            strcpy(wadfilename,wadfiles[j]->filename);
            nameonly(wadfilename);
            if( stricmp(wadfilename,fileneeded[i].filename)==0 &&
                 !memcmp(wadfiles[j]->md5sum, fileneeded[i].md5sum, 16))
            {
                if(devparm) CONS_Printf("already loaded\n");
                fileneeded[i].status=FS_OPEN;
                break;
            }
        }
        if( fileneeded[i].status!=FS_NOTFOUND )
           continue;

        fileneeded[i].status = findfile(fileneeded[i].filename,fileneeded[i].md5sum,true);
        if(devparm) CONS_Printf("found %d\n",fileneeded[i].status);
        if( fileneeded[i].status != FS_FOUND )
            ret=0;
    }
    return ret;
}

// load it now
void CL_LoadServerFiles(void)
{
    int i;
    
    for (i=1;i<fileneedednum;i++)
    {
        if( fileneeded[i].status == FS_OPEN )
            // allready loaded
            continue;
        else
        if( fileneeded[i].status == FS_FOUND )
        {
            P_AddWadFile(fileneeded[i].filename,NULL);
            fileneeded[i].status = FS_OPEN;
        }
        else
        if( fileneeded[i].status == FS_MD5SUMBAD) 
        {
            P_AddWadFile(fileneeded[i].filename,NULL);
            fileneeded[i].status = FS_OPEN;
            CONS_Printf("\2File %s found but with differant md5sum\n",fileneeded[i].filename);
        }
        else
            I_Error("Try to load file %s with status of %d\n",fileneeded[i].filename,fileneeded[i].status);
    }
}

// little optimization to test if there is a file in the queue
static int filetosend=0;

void SendFile(int node,char *filename, char fileid)
{
    filetx_t **q,*p;
    int i;
    boolean found=0;
    char  wadfilename[MAX_WADPATH];

    q=&transfer[node].txlist;
    while(*q) q=&((*q)->next);
    *q=(filetx_t *)malloc(sizeof(filetx_t));
    if(!*q)
       I_Error("SendFile : No more ram\n");
    p=*q;
    p->filename=(char *)malloc(MAX_WADPATH);
    strcpy(p->filename,filename);
    
    // a minimum of security, can't get only file in legacy direcory
    nameonly(p->filename);

    // check first in wads loaded the majority of case
    for(i=0;wadfiles[i] && !found;i++)
    {
        strcpy(wadfilename,wadfiles[i]->filename);
        nameonly(wadfilename);
        if(stricmp(wadfilename,p->filename)==0)
        {
            found = true;
            // copy filename with full path
            strcpy(p->filename,wadfiles[i]->filename);
        }
    }
    
    if( !found )
    {
        DEBFILE(va("%s not found in wadfiles\n",filename));
        if(findfile(p->filename,NULL,true)==0)
        {
            // not found
            // don't inform client (probably hacker)
            DEBFILE(va("Client %d request %s : not found\n",node,filename));
            free(p->filename);
            free(p);
            *q=NULL;
            return;
        }
        else
            return;
    }

    DEBFILE(va("Sending file %s to %d (id=%d)\n",filename,node,fileid));
    p->ram=SF_FILE;
    // size initialized at file open 
    //p->size=size;
    p->fileid=fileid;
    p->next=NULL; // end of list

    filetosend++;
}

void SendRam(int node,byte *data, ULONG size,freemethode_t freemethode, char fileid)
{
    filetx_t **q,*p;

    q=&transfer[node].txlist;
    while(*q) q=&((*q)->next);
    *q=(filetx_t *)malloc(sizeof(filetx_t));
    if(!*q) 
       I_Error("SendRam : No more ram\n");
    p=*q;
    p->ram=freemethode;
    p->filename=data;
    p->size=size;
    p->fileid=fileid;
    p->next=NULL; // end of list

    DEBFILE(va("Sending ram %x( size:%d) to %d (id=%d)\n",p->filename,size,node,fileid));

    filetosend++;
}

void EndSend(int node)
{
    filetx_t *p=transfer[node].txlist;
    switch (p->ram) {
    case SF_FILE:
        if( transfer[node].currentfile )
            fclose(transfer[node].currentfile);
        free(p->filename);
        break;
    case SF_Z_RAM:
        Z_Free(p->filename);
        break;
    case SF_RAM:
        free(p->filename);
    case SF_NOFREERAM:
        break;
    }
    transfer[node].txlist = p->next;
    transfer[node].currentfile = NULL;
    free(p);                 
    filetosend--;
}

#define PACKETPERTIC net_bandwidth/(TICRATE*software_MAXPACKETLENGTH)

void FiletxTicker(void)
{
    static int currentnode=0;
    filetx_pak *p;
    ULONG      size;
    filetx_t   *f;
    int        ram,i;
    int        packetsent = PACKETPERTIC;

    if( filetosend==0 )
        return;
    if(packetsent==0) packetsent++;
    // (((sendbytes-nowsentbyte)*TICRATE)/(I_GetTime()-starttime)<(ULONG)net_bandwidth)
    while( packetsent-- && filetosend!=0)
    {
    for(i=currentnode,ram=0;ram<MAXNETNODES;i=(i+1)%MAXNETNODES,ram++)
        if(transfer[i].txlist)
            goto found;
    // no transfer to do
    I_Error("filetosend=%d but no filetosend found\n",filetosend);
found:
    currentnode=(i+1)%MAXNETNODES;
    f=transfer[i].txlist;
    ram=f->ram;

    if(!transfer[i].currentfile) // file not allready open
    {
        if(!ram) {
            long filesize;

            transfer[i].currentfile = fopen(f->filename,"rb");

            if(!transfer[i].currentfile)
                I_Error("File %s not exist", f->filename);

            fseek(transfer[i].currentfile, 0, SEEK_END);
            filesize = ftell(transfer[i].currentfile);

            // nobody wants to transfer a file bigger than 4GB!
            // and computers will never need more than 640kb of RAM ;-)
            if(-1 == filesize)
            {
                perror("Error");
                I_Error("Error getting filesize of %s\n", f->filename);
            }

            f->size = filesize;
            fseek(transfer[i].currentfile, 0, SEEK_SET);            
        }
        else
            transfer[i].currentfile = (FILE *)1;
        transfer[i].position=0;
    }

    p=&netbuffer->u.filetxpak;
    size=software_MAXPACKETLENGTH-(FILETXHEADER+BASEPACKETSIZE);
    if( f->size-transfer[i].position<size )
        size=f->size-transfer[i].position;
    if(ram)
        memcpy(p->data,&f->filename[transfer[i].position],size);
    else
    {
        if( fread(p->data,size,1,transfer[i].currentfile) != 1 )
            I_Error("FiletxTicker : can't get %d byte on %s at %d",size,f->filename,transfer[i].position);
    }
    p->position = transfer[i].position;
    // put flag so receiver know the totalsize
    if( transfer[i].position+size==f->size )
        p->position |= 0x80000000;
    p->position = LONG(p->position);
    p->size     = SHORT(size);
    p->fileid   = f->fileid;
    netbuffer->packettype=PT_FILEFRAGMENT;
    if (!HSendPacket(i,true,0,FILETXHEADER+size ) ) // reliable SEND
    { // not sent for some od reason
      // retry at next call
         if( !ram )
             fseek(transfer[i].currentfile,transfer[i].position,SEEK_SET);
         // exit the while (can't sent this one why should i sent the next ?
         break;
    }
    else // success
    {
        transfer[i].position+=size;
        if(transfer[i].position==f->size) //  finish ?
            EndSend(i);
    }
    }
}

void Got_Filetxpak(void)
{
    int filenum=netbuffer->u.filetxpak.fileid;
static int time=0;

    if(filenum>=fileneedednum)
    {
        DEBFILE(va("fileframent not needed %d>%d\n",filenum,fileneedednum));
        return;
    }

    if( fileneeded[filenum].status==FS_REQUESTED )
    {
        if(fileneeded[filenum].phandle) I_Error("Got_Filetxpak : allready open file\n");
        fileneeded[filenum].phandle=fopen(fileneeded[filenum].filename,"wb");
        if(!fileneeded[filenum].phandle) I_Error("Can't create file %s : disk full ?",fileneeded[filenum].filename);
        CONS_Printf("\r%s...",fileneeded[filenum].filename);
        fileneeded[filenum].currentsize = 0; 
        fileneeded[filenum].status=FS_DOWNLOADING;
    }

    if( fileneeded[filenum].status==FS_DOWNLOADING )
    {
	// Swap file position and size on big_endian machines.
	netbuffer->u.filetxpak.position	= LONG (netbuffer->u.filetxpak.position);
	netbuffer->u.filetxpak.size	= SHORT(netbuffer->u.filetxpak.size);

        // use a special tric to know when file is finished (not allways used)
        // WARNING: filepak can arrive out of order so don't stop now !
        if( netbuffer->u.filetxpak.position & 0x80000000 ) 
        {
            netbuffer->u.filetxpak.position &= ~0x80000000;
            fileneeded[filenum].totalsize = netbuffer->u.filetxpak.position + netbuffer->u.filetxpak.size;
        }
        // we can receive packet in the wrong order, anyway all os support gaped file
        fseek(fileneeded[filenum].phandle,netbuffer->u.filetxpak.position,SEEK_SET);
        if( fwrite(netbuffer->u.filetxpak.data,netbuffer->u.filetxpak.size,1,fileneeded[filenum].phandle)!=1 )
            I_Error("Can't write %s : disk full ?\n",fileneeded[filenum].filename);
        fileneeded[filenum].currentsize+=netbuffer->u.filetxpak.size;
        if(time==0)
        {
            Net_GetNetStat();
            CONS_Printf("\r%s %dK/%dK %.1fK/s",fileneeded[filenum].filename,
                                               fileneeded[filenum].currentsize>>10,
                                               fileneeded[filenum].totalsize>>10,
                                               ((float)getbps)/1024);
        }

        // finish ?
        if(fileneeded[filenum].currentsize==fileneeded[filenum].totalsize) 
        {
            fclose(fileneeded[filenum].phandle);
            fileneeded[filenum].phandle=NULL;
            fileneeded[filenum].status=FS_FOUND;
            CONS_Printf("\rDownloading %s...(done)\n",fileneeded[filenum].filename);
        }
    }
    else
        I_Error("Received a file not requested\n");
    // send ack back quickly

    if(++time==4)
    {
        Net_SendAcks(servernode);
        time=0;
    }

}

void AbortSendFiles(int node)
{
    while(transfer[node].txlist)
        EndSend(node);
}

void CloseNetFile(void)
{
    int i;
    // is sending ?
    for( i=0;i<MAXNETNODES;i++)
        AbortSendFiles(i);

    // receiving a file ?
    for( i=0;i<MAX_WADFILES;i++ )
        if( fileneeded[i].status==FS_DOWNLOADING && fileneeded[i].phandle)
        {
            fclose(fileneeded[i].phandle);
            // file is not compete delete it
            remove(fileneeded[i].filename);
        }

    // remove FILEFRAGMENT from acknledge list
    Net_AbortPacketType(PT_FILEFRAGMENT);
}

// functions cut and pasted from doomatic :)

void nameonly(char *s)
{
  int j;

  for(j=strlen(s);j>=0;j--)
      if( (s[j]=='\\') || (s[j]==':') || (s[j]=='/') )
      {
          memcpy(s, &(s[j+1]), strlen(&(s[j+1]))+1 );
          return;
      }
}

#if defined(LINUX) || defined(__MACOS__)
#define O_BINARY 0
#endif

// UNUXED for now
boolean fileexist(char *filename,time_t time)
{
   int handel;
   handel=open(filename,O_RDONLY|O_BINARY);
   if( handel!=-1 )
   {
         close(handel);
         if(time!=0)
         {
            struct stat bufstat;
            stat(filename,&bufstat);
            if( time!=bufstat.st_mtime )
                return false;
         }
         return true;
   }
   else
       return false;
}

filestatus_t checkfilemd5(char *filename, unsigned char *wantedmd5sum)
{
    FILE *fhandle;
    unsigned char md5sum[16];
    filestatus_t return_val = FS_NOTFOUND;

    if((fhandle = fopen(filename,"rb")))
    {
        if(wantedmd5sum)
        {
            md5_stream(fhandle,md5sum);
            fclose(fhandle);
            if(!memcmp(wantedmd5sum, md5sum, 16))
                return_val = FS_FOUND;
            else
                return_val = FS_MD5SUMBAD; 
        }
        else
        {
            return_val = FS_FOUND;
        }
    }

    return return_val;
}

filestatus_t findfile(char *filename, unsigned char *wantedmd5sum, boolean completepath)
{
    //FIXME: implement wadpath-search
    //just for the start... recursive 10 levels from current dir should bring back old behaviour
    
    return filesearch(filename, ".", wantedmd5sum, completepath, 10);
}
