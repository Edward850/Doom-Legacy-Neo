
#define DEBUG_PIPE_NAME "\\PIPE\\DebugWindow"

#define ID_DEBUGAPPNAME                  1000
#define ID_DEBUGKEYNAME                  1001


void  Debug( char *format, ...);
void  Debug2( FILE*, char *format, ...);
int   outToPipe( char c);

//#define printf Debug
#define fprintf Debug2
