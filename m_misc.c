// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: m_misc.c,v 1.10 2004/04/18 12:40:14 hurdler Exp $
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
// $Log: m_misc.c,v $
// Revision 1.10  2004/04/18 12:40:14  hurdler
// Jive's request for saving screenshots
//
// Revision 1.9  2003/10/15 14:09:47  darkwolf95
// Fixed screenshots filename bug
//
// Revision 1.8  2001/03/03 06:17:33  bpereira
// no message
//
// Revision 1.7  2001/02/24 13:35:20  bpereira
// no message
//
// Revision 1.6  2001/01/25 22:15:42  bpereira
// added heretic support
//
// Revision 1.5  2000/10/08 13:30:01  bpereira
// no message
//
// Revision 1.4  2000/09/28 20:57:15  bpereira
// no message
//
// Revision 1.3  2000/04/16 18:38:07  bpereira
// no message
//
// Revision 1.2  2000/02/26 00:28:42  hurdler
// Mostly bug fix (see borislog.txt 23-2-2000, 24-2-2000)
//
//
// DESCRIPTION:
//      Default Config File.
//      PCX Screenshots.
//      File i/o
//      Common used routines
//
//-----------------------------------------------------------------------------


#ifndef __WIN32__
#include <unistd.h>
#endif
#include <fcntl.h>

#include "doomdef.h"
#include "g_game.h"
#include "m_misc.h"
#include "hu_stuff.h"
#include "v_video.h"
#include "z_zone.h"
#include "g_input.h"
#include "i_video.h"
#include "d_main.h"
#include "m_argv.h"

#ifdef HWRENDER
#include "hardware/hw_main.h"
#endif

// ==========================================================================
//                         FILE INPUT / OUTPUT
// ==========================================================================


//
// FIL_WriteFile
//
#ifndef O_BINARY
#define O_BINARY 0
#endif

boolean FIL_WriteFile ( char const*   name,
                        void*         source,
                        int           length )
{
    int         handle;
    int         count;

    handle = open ( name, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0666);

    if (handle == -1)
        return false;

    count = write (handle, source, length);
    close (handle);

    if (count < length)
        return false;

    return true;
}


//
// FIL_ReadFile : return length, 0 on error
//
//Fab:26-04-98:
//  appends a zero byte at the end
int FIL_ReadFile ( char const*   name,
                   byte**        buffer )
{
    int    handle, count, length;
    struct stat fileinfo;
    byte   *buf;

    handle = open (name, O_RDONLY | O_BINARY, 0666);
    if (handle == -1)
        return 0;

    if (fstat (handle,&fileinfo) == -1)
        return 0;

    length = fileinfo.st_size;
    buf = Z_Malloc (length+1, PU_STATIC, 0);
    count = read (handle, buf, length);
    close (handle);

    if (count < length)
        return 0;

    //Fab:26-04-98:append 0 byte for script text files
    buf[length]=0;

    *buffer = buf;
    return length;
}


//
// checks if needed, and add default extension to filename
//
void FIL_DefaultExtension (char *path, char *extension)
{
    char    *src;

  // search for '.' from end to begin, add .EXT only when not found
    src = path + strlen(path) - 1;

    while (*src != '/' && src != path)
    {
        if (*src == '.')
            return;                 // it has an extension
        src--;
    }

    strcat (path, extension);
}


//  Creates a resource name (max 8 chars 0 padded) from a file path
//
void FIL_ExtractFileBase ( char*  path,  char* dest )
{
    char*       src;
    int         length;

    src = path + strlen(path) - 1;

    // back up until a \ or the start
    while (src != path
           && *(src-1) != '\\'
           && *(src-1) != '/')
    {
        src--;
    }

    // copy up to eight characters
    memset (dest,0,8);
    length = 0;

    while (*src && *src != '.')
    {
        if (++length == 9)
            I_Error ("Filename base of %s >8 chars",path);

        *dest++ = toupper((int)*src++);
    }
}


//  Returns true if a filename extension is found
//  There are no '.' in wad resource name
//
boolean FIL_CheckExtension (char *in)
{
    while (*in++)
        if (*in=='.')
            return true;

    return false;
}


// ==========================================================================
//                        CONFIGURATION FILE
// ==========================================================================

//
// DEFAULTS
//

char   configfile[MAX_WADPATH];

// ==========================================================================
//                          CONFIGURATION
// ==========================================================================
boolean         gameconfig_loaded = false;      // true once config.cfg loaded
                                                //  AND executed


void Command_SaveConfig_f (void)
{
    char tmpstr[MAX_WADPATH];

    if (COM_Argc()!=2)
    {
        CONS_Printf("saveconfig <filename[.cfg]> : save config to a file\n");
        return;
    }
    strcpy(tmpstr,COM_Argv(1));
    FIL_DefaultExtension (tmpstr,".cfg");

    M_SaveConfig(tmpstr);
    CONS_Printf("config saved as %s\n",configfile);
}

void Command_LoadConfig_f (void)
{
    if (COM_Argc()!=2)
    {
        CONS_Printf("loadconfig <filename[.cfg]> : load config from a file\n");
        return;
    }

    strcpy(configfile,COM_Argv(1));
    FIL_DefaultExtension (configfile,".cfg");
/*  for create, don't check

    if ( access (tmpstr,F_OK) )
    {
        CONS_Printf("Error reading file %s (not exist ?)\n",tmpstr);
        return;
    }
*/
    COM_BufInsertText (va("exec \"%s\"\n",configfile));

}

void Command_ChangeConfig_f (void)
{
    if (COM_Argc()!=2)
    {
        CONS_Printf("changeconfig <filaname[.cfg]> : save current config and load another\n");
        return;
    }

    COM_BufAddText (va("saveconfig \"%s\"\n",configfile));
    COM_BufAddText (va("loadconfig \"%s\"\n",COM_Argv(1)));
}

//
// Load the default config file
//
void M_FirstLoadConfig(void)
{
    int p;

    //  configfile is initialised by d_main when sherching for the wad ?!

    // check for a custom config file
    p = M_CheckParm ("-config");
    if (p && p<myargc-1)
    {
        strcpy (configfile, myargv[p+1]);
        CONS_Printf ("config file: %s\n",configfile);
    }

    // load default control
    G_Controldefault();

    // load config, make sure those commands doesnt require the screen..
    CONS_Printf("\n");
    COM_BufInsertText (va("exec \"%s\"\n",configfile));
    COM_BufExecute ();       // make sure initial settings are done

    // make sure I_Quit() will write back the correct config
    // (do not write back the config if it crash before)
    gameconfig_loaded = true;
}

//  Save all game config here
//
void M_SaveConfig (char *filename)
{
    FILE    *f;

    // make sure not to write back the config until
    //  it's been correctly loaded
    if (!gameconfig_loaded)
        return;

    // can change the file name
    if(filename)
    {
        f = fopen (filename, "w");
        // change it only if valide
        if(f)
            strcpy(configfile,filename);
        else
        {
            CONS_Printf ("Couldn't save game config file %s\n",filename);
            return;
        }
    }
    else
    {
        f = fopen (configfile, "w");
        if (!f)
        {
            CONS_Printf ("Couldn't save game config file %s\n",configfile);
            return;
        }
    }

    // header message
    fprintf (f, "// Doom Legacy configuration file.\n");

    //FIXME: save key aliases if ever implemented..

    CV_SaveVariables (f);
    G_SaveKeySetting(f);

    fclose (f);
}



// ==========================================================================
//                            SCREEN SHOTS
// ==========================================================================



typedef struct
{
    char                manufacturer;
    char                version;
    char                encoding;
    char                bits_per_pixel;

    unsigned short      xmin;
    unsigned short      ymin;
    unsigned short      xmax;
    unsigned short      ymax;

    unsigned short      hres;
    unsigned short      vres;

    unsigned char       palette[48];

    char                reserved;
    char                color_planes;
    unsigned short      bytes_per_line;
    unsigned short      palette_type;

    char                filler[58];
    unsigned char       data;           // unbounded
} pcx_t;


//
// WritePCXfile
//
boolean WritePCXfile ( char*         filename,
                    byte*         data,
                    int           width,
                    int           height,
                    byte*         palette )
{
    int         i;
    int         length;
    pcx_t*      pcx;
    byte*       pack;

    pcx = Z_Malloc (width*height*2+1000, PU_STATIC, NULL);

    pcx->manufacturer = 0x0a;           // PCX id
    pcx->version = 5;                   // 256 color
    pcx->encoding = 1;                  // uncompressed
    pcx->bits_per_pixel = 8;            // 256 color
    pcx->xmin = 0;
    pcx->ymin = 0;
    pcx->xmax = SHORT(width-1);
    pcx->ymax = SHORT(height-1);
    pcx->hres = SHORT(width);
    pcx->vres = SHORT(height);
    memset (pcx->palette,0,sizeof(pcx->palette));
    pcx->color_planes = 1;              // chunky image
    pcx->bytes_per_line = SHORT(width);
    pcx->palette_type = SHORT(1);       // not a grey scale
    memset (pcx->filler,0,sizeof(pcx->filler));


    // pack the image
    pack = &pcx->data;

    for (i=0 ; i<width*height ; i++)
    {
        if ( (*data & 0xc0) != 0xc0)
            *pack++ = *data++;
        else
        {
            *pack++ = 0xc1;
            *pack++ = *data++;
        }
    }

    // write the palette
    *pack++ = 0x0c;     // palette ID byte
    for (i=0 ; i<768 ; i++)
        *pack++ = *palette++;

    // write output file
    length = pack - (byte *)pcx;
    i = FIL_WriteFile (filename, pcx, length);

    Z_Free (pcx);
    return i;
}


//
// M_ScreenShot
//
void M_ScreenShot (void)
{
    int         i;
    byte*       linear;
    char        lbmname[MAX_WADPATH];
    boolean     ret = false;

#ifdef HWRENDER
    if (rendermode!=render_soft)
        ret = HWR_Screenshot (lbmname);
    else
#endif
    {
        // munge planar buffer to linear
        linear = screens[2];
        I_ReadScreen (linear);

        // find a file name to save it to
        strcpy(lbmname,"DOOM0000.pcx");
        for (i=0 ; i<10000; i++)
        {
            lbmname[4] = '0' + ((i/1000) % 10);
            lbmname[5] = '0' + ((i/100) % 10);
            lbmname[6] = '0' + ((i/10) % 10);
            lbmname[7] = '0' + ((i/1) % 10);
            if (access(lbmname,0) == -1)
                break;      // file doesn't exist
        }
        if (i<10000)
        {
            // save the pcx file
            ret = WritePCXfile (lbmname, linear,
                                vid.width, vid.height,
                                W_CacheLumpName ("PLAYPAL",PU_CACHE));
        }
    }

    if( ret )
        CONS_Printf("screen shot %s saved\n", lbmname);
    else
        //CONS_Printf("Couldn't create screen shot\n");
        CONS_Printf("%s\n", lbmname);
}


// ==========================================================================
//                        MISC STRING FUNCTIONS
// ==========================================================================


//  Temporary varargs CONS_Printf
//
char*   va(char *format, ...)
{
    va_list      argptr;
    static char  string[1024];

    va_start (argptr, format);
    vsprintf (string, format,argptr);
    va_end (argptr);

    return string;
}


// creates a copy of a string, null-terminated
// returns ptr to the new duplicate string
//
char *Z_StrDup (const char *in)
{
    char    *out;

    out = ZZ_Alloc (strlen(in)+1);
    strcpy (out, in);
    return out;
}


// s1=s2+s3+s1
void strcatbf(char *s1,char *s2,char *s3)
{
    char tmp[1024];

    strcpy(tmp,s1);
    strcpy(s1,s2);
    strcat(s1,s3);
    strcat(s1,tmp);
}
