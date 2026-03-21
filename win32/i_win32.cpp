#ifdef __WIN32__
#include "i_win32.h"
#include <ShlObj.h>
#include <KnownFolders.h>

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

	// Convert the wide-character string to a narrow-character string.
    size_t pathSize = wcstombs(NULL, pwszPath, 0) + strlen("\\Doom Legacy Neo") + 1; // Get the required size for the narrow string
    pszPath = (char*)malloc(pathSize);
    if (pszPath)
    {
        wcstombs(pszPath, pwszPath, pathSize); // Convert the wide string to a narrow string
		strcat(pszPath, "\\Doom Legacy Neo");

		// Make sure the directory exists, create it if it doesn't
        if (GetFileAttributesA(pszPath) == INVALID_FILE_ATTRIBUTES)
        {
            if (!CreateDirectoryA(pszPath, NULL))
            {
                free(pszPath);
                pszPath = NULL;
            }
        }
    }
    CoTaskMemFree(pwszPath); // Free the memory allocated by SHGetKnownFolderPath
	return pszPath;
}
#endif