// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: m_argv.c,v 1.5 2001/12/26 17:24:46 hurdler Exp $
//
// Copyright (C) 1993-1996 by id Software, Inc.
// Portions Copyright (C) 1998-2000 by DooM Legacy Team.
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
// $Log: m_argv.c,v $
// Revision 1.5  2001/12/26 17:24:46  hurdler
// Update Linux version
//
// Revision 1.4  2001/08/12 15:21:04  bpereira
// see my log
//
// Revision 1.3  2000/03/29 19:39:48  bpereira
// no message
//
// Revision 1.2  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//
//
//-----------------------------------------------------------------------------


#include <string.h>

#include "doomdef.h"
#include "command.h"

int             myargc;
char**          myargv;
static int      found;

//
// M_CheckParm
// Checks for the given parameter
// in the program's command line arguments.
// Returns the argument number (1 to argc-1)
// or 0 if not present
int M_CheckParm (char *check)
{
    int         i;

    for (i = 1;i<myargc;i++)
    {
        if ( !strcasecmp(check, myargv[i]) )
        {
            found=i;
            return i;
        }
    }
    found=0;
    return 0;
}

// return true if there is available parameters
boolean M_IsNextParm(void)
{
    if(found>0 && found+1<myargc && myargv[found+1][0] != '-' && myargv[found+1][0] != '+')
        return true;
    return false;
}

// return the next parameter after a M_CheckParm
// NULL if not found use M_IsNext to find if there is a parameter
char *M_GetNextParm(void)
{
    if(M_IsNextParm())
    {
        found++;
        return myargv[found];
    }
    return NULL;
}

// push all parameters begining by '+'
void M_PushSpecialParameters( void )
{
    int     i;
    char    s[256];
    boolean onetime=false;

    for (i = 1;i<myargc;i++)
    {
        if ( myargv[i][0]=='+' )
        {
            strcpy(s,&myargv[i][1]);
            i++;

            // get the parameter of the command too
            for(;i<myargc && myargv[i][0]!='+' && myargv[i][0]!='-' ;i++)
            {
                strcat(s," ");
                if(!onetime) { strcat(s,"\"");onetime=true; }
                strcat(s,myargv[i]);
            }
            if( onetime )    { strcat(s,"\"");onetime=false; }
            strcat(s,"\n");

            // push it
            COM_BufAddText (s);
            i--;
        }
    }
}

//
// Find a Response File
//
void M_FindResponseFile (void)
{
    int             i;
#define MAXARGVS        256

    for (i = 1;i < myargc;i++)
        if (myargv[i][0] == '@')
        {
            FILE *          handle;
            int             size;
            int             k;
            int             index;
            int             indexinfile;
            boolean         inquote = false;
            byte    *infile;
            char    *file;
            char    *moreargs[20];
            char    *firstargv;

            // READ THE RESPONSE FILE INTO MEMORY
            handle = fopen (&myargv[i][1],"rb");
            if (!handle)
            {
                I_Error ("\nResponse file %s not found !",&myargv[i][1]);
                exit(1);
            }
            CONS_Printf("Found response file %s!\n",&myargv[i][1]);
            fseek (handle,0,SEEK_END);
            size = ftell(handle);
            fseek (handle,0,SEEK_SET);
            file = malloc (size);
            fread (file,size,1,handle);
            fclose (handle);

            // KEEP ALL CMDLINE ARGS FOLLOWING @RESPONSEFILE ARG
            for (index = 0,k = i+1; k < myargc; k++)
                moreargs[index++] = myargv[k];

            firstargv = myargv[0];
            myargv = malloc(sizeof(char *)*MAXARGVS);
            if( !myargv ) I_Error("no enought memory");
            memset(myargv,0,sizeof(char *)*MAXARGVS);
            myargv[0] = firstargv;

            infile = file;
            indexinfile = k = 0;
            indexinfile++;  // SKIP PAST ARGV[0] (KEEP IT)
            do
            {
                inquote = infile[k] == '"';
                if ( inquote )       // strip encllosing double-quote
                    k++;
                myargv[indexinfile++] = &infile[k];
                while (k < size && ( (inquote && infile[k]!='"') 
                                  || (!inquote && infile[k]> ' ') ) )
                    k++;
                infile[k] = 0;
                while(k < size && (infile[k]<= ' '))
                    k++;
            } while(k < size);

            for (k = 0;k < index;k++)
                myargv[indexinfile++] = moreargs[k];
            myargc = indexinfile;

            // DISPLAY ARGS
            CONS_Printf("%d command-line args:\n",myargc);
            for (k=1;k<myargc;k++)
                CONS_Printf("%s\n",myargv[k]);

            break;
        }
}
