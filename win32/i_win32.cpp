#ifdef __WIN32__
#include "i_win32.h"
#include <ShlObj.h>
#include <KnownFolders.h>
#include <string>
#include <filesystem>

#define GAMEDIR L"\\Doom Legacy Neo"

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
    std::wstring testPath = std::wstring(pwszPath) + L"\\Saved Games" GAMEDIR;
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
#endif