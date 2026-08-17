
#if defined(WIN32)
#define I_sprintf(buffer, bufferCount, format, ...) sprintf_s(buffer, bufferCount, format, __VA_ARGS__)
#define I_strcpy(dest, destSize, src) strcpy_s(dest, destSize, src)
#define I_strcat(dest, destSize, src) strcat_s(dest, destSize, src)
#define I_stricmp _stricmp
#else
#define I_sprintf(buffer, bufferCount, format, ...) sprintf(buffer, format, __VA_ARGS__)
#define I_strcpy(dest, destSize, src) strcpy(dest, src)
#define I_strcat(dest, destSize, src) strcat(dest, src)
#define I_stricmp stricmp
#endif
