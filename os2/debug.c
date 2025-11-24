/*
 *  Debug.c (c) 1995 Yuri DARIO
 *  $Id: debug.c,v 1.3 2000/08/10 11:07:51 ydario Exp $
 *
 *  Codice visualizzazione messaggi Debug
 *
*/
#define INCL_DOS
#include <os2.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "Debug.h"

#define PIPEME
#undef fprintf

HPIPE   hp = NULLHANDLE;

void Debug2( FILE* err, char *format, ...)
{
   va_list  data;
   char     buffer[ 1024];

   va_start( data, format);
   vsprintf( buffer, format, data);
   va_end( data);
   
   if (err==stderr)                      // to debug pipe
      printf( buffer);
   else
      fprintf( err, "%s", buffer);       // it is a disk file
}


void Debug0( char *format, ...)
{
#ifdef PIPEME
   va_list  data;
   char     buffer[ 1024];
   RESULTCODES resCod;
   APIRET      rc;
   ULONG       ulAction, cbW;

   if (!hp) {
      rc = DosOpen( DEBUG_PIPE_NAME, &hp, &ulAction, 0,
            FILE_NORMAL, OPEN_ACTION_OPEN_IF_EXISTS,
            OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYNONE,
            (PEAOP2) NULL);
#ifdef DEBUG_TRY_OPEN
      if (rc) {
         /*sprintf( buffer, "DosOpen 1:error#%d\n", rc);
         somPrintf( buffer);*/
         rc = DosExecPgm( buffer, sizeof( buffer), EXEC_BACKGROUND,
               NULL, NULL, &resCod, "d:\\rd\\sxpa\\Debug.cmd");
         /*somPrintf( buffer);
         sprintf( buffer, "DosExec:error#%d\n", rc);
         somPrintf( buffer);*/
         if (!rc) {
            cbW = 0;
            do {
               DosSleep( 1000);     /* attendi un po' */
               /* prova a riaprire la finestra risultati */
               rc = DosOpen( DEBUG_PIPE_NAME, &hp, &ulAction, 0,
                     FILE_NORMAL, OPEN_ACTION_OPEN_IF_EXISTS,
                     OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYNONE,
                     (PEAOP2) NULL);
               cbW++;
            } while( rc && cbW<10);     /* ritenta 10 volte */
            /*sprintf( buffer, "DosOpen 2:error#%d\n", rc);
            somPrintf( buffer);*/
         }
      }
#endif /* DEBUG_TRY_OPEN */
   }

   va_start( data, format);
   vsprintf( buffer, format, data);
   va_end( data);

   if (hp) {
      rc = DosWrite( hp, buffer, strlen(buffer), &cbW);     /* Writes to the pipe    */
      if (rc) {
         DosClose( hp);
         hp = NULLHANDLE;
      } /*else
         DosResetBuffer( hp);*/
   }
#else
   va_list  data;
   char     buffer[ 1024];
   FILE     *dat;

   va_start( data, format);
   vsprintf( buffer, format, data);
   va_end( data);

   dat = fopen( "\\00debug.log", "a");
   if (dat) {
      fprintf( dat, "%s", buffer);
   }
   fclose( dat);

#endif

}
