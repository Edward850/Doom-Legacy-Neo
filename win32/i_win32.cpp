#ifdef __WIN32__
#include "i_win32.h"
#include <ShlObj.h>
#include <KnownFolders.h>
#include <string>
#include <filesystem>
#include <dbghelp.h>

#pragma comment(lib, "dbghelp.lib")

constexpr wchar_t GAMEDIR[] = L"\\Doom Legacy Neo";

const char* I_CPPGetConfigDir(void)
{
    static char* pszPath = NULL;
    if (pszPath != NULL)
    {
		return pszPath;
    }
    PWSTR pwszPath = NULL;
    if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_SavedGames, KF_FLAG_CREATE, NULL, &pwszPath)))
    {
        return NULL;
    }
    std::wstring testPath = std::wstring(pwszPath) + GAMEDIR;
    CoTaskMemFree(pwszPath);

    if (GetFileAttributesW(testPath.c_str()) == INVALID_FILE_ATTRIBUTES)
    {
        if(!std::filesystem::create_directories(testPath))  // Create the directory if it doesn't exist
        {
            return NULL;
        }
    }

    // Convert the wide-character string to a narrow-character string.
    size_t pathSize = wcstombs(NULL, testPath.c_str(), 0) + 1; // Get the required size for the narrow string
    pszPath = (char*)malloc(pathSize);
    if (!pszPath)
    {
        return NULL;
    }
    wcstombs(pszPath, testPath.c_str(), pathSize); // Convert the wide string to a narrow string
	return pszPath;
}

const char* I_CPPGetSaveGameDir(void)
{
    static char* pszPath = NULL;
    if (pszPath != NULL)
    {
        return pszPath;
    }

    // Try to check OneDrive first
    PWSTR pwszPath = NULL;
    if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_OneDrive, 0, NULL, &pwszPath)))
    {
        return I_CPPGetConfigDir();
    }
    std::wstring testPath = std::wstring(pwszPath) + L"\\Saved Games" + GAMEDIR;
    CoTaskMemFree(pwszPath);

    if (GetFileAttributesW(testPath.c_str()) == INVALID_FILE_ATTRIBUTES)
    {
        if (!std::filesystem::create_directories(testPath))  // Create the directory if it doesn't exist
        {
            return I_CPPGetConfigDir();
        }
    }

    // Convert the wide-character string to a narrow-character string.
    size_t pathSize = wcstombs(NULL, testPath.c_str(), 0) + 1; // Get the required size for the narrow string
    pszPath = (char*)malloc(pathSize);
    if (!pszPath)
    {
        return I_CPPGetConfigDir();
    }

    wcstombs(pszPath, testPath.c_str(), pathSize); // Convert the wide string to a narrow string
    return pszPath;
}

static LONG WINAPI CrashHandler(EXCEPTION_POINTERS* pExceptionInfo) 
{
    HANDLE hFile = CreateFileA(
        "crash_dump.dmp",
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile != INVALID_HANDLE_VALUE) 
    {
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ExceptionPointers = pExceptionInfo;
        dumpInfo.ClientPointers = FALSE;

        BOOL success = MiniDumpWriteDump(
            GetCurrentProcess(),
            GetCurrentProcessId(),
            hFile,
            MiniDumpNormal, // Use MiniDumpWithFullMemory for full dumps
            &dumpInfo,
            NULL,
            NULL
        );

        CloseHandle(hFile);

        // Show a message box to inform the user about the crash
        MessageBoxA(NULL, "The application has crashed. A crash dump has been created as 'crash_dump.dmp'. Please send this file to the developers for further analysis.", "Application Crash", MB_OK | MB_ICONERROR);
    }
    else
    {
        // Show a message box to inform the user that the dump could not be created
        MessageBoxA(NULL, "The application has crashed. However, the crash dump could not be created.", "Application Crash", MB_OK | MB_ICONERROR);
    }

    // Terminate the process cleanly
    return EXCEPTION_EXECUTE_HANDLER;
}

void I_PlatformInit(void)
{
    // Register the crash handler
    SetUnhandledExceptionFilter(CrashHandler);
}

#endif
