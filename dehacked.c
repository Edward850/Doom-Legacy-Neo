// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: dehacked.c,v 1.16 2003/11/21 17:52:05 darkwolf95 Exp $
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
// $Log: dehacked.c,v $
// Revision 1.16  2003/11/21 17:52:05  darkwolf95
// added "Monsters Infight" for Dehacked patches
//
// Revision 1.15  2002/01/12 12:41:05  hurdler
// very small commit
//
// Revision 1.14  2002/01/12 02:21:36  stroggonmeth
// Big commit
//
// Revision 1.13  2001/07/16 22:35:40  bpereira
// - fixed crash of e3m8 in heretic
// - fixed crosshair not drawed bug
//
// Revision 1.12  2001/06/30 15:06:01  bpereira
// fixed wronf next level name in intermission
//
// Revision 1.11  2001/04/30 17:19:24  stroggonmeth
// HW fix and misc. changes
//
// Revision 1.10  2001/02/10 12:27:13  bpereira
// no message
//
// Revision 1.9  2001/01/25 22:15:41  bpereira
// added heretic support
//
// Revision 1.8  2000/11/04 16:23:42  bpereira
// no message
//
// Revision 1.7  2000/11/03 13:15:13  hurdler
// Some debug comments, please verify this and change what is needed!
//
// Revision 1.6  2000/11/02 17:50:06  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.5  2000/08/31 14:30:55  bpereira
// no message
//
// Revision 1.4  2000/04/16 18:38:07  bpereira
// no message
//
// Revision 1.3  2000/04/05 15:47:46  stroggonmeth
// Added hack for Dehacked lumps. Transparent sprites are now affected by colormaps.
//
// Revision 1.2  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      Load dehacked file and change table and text from the exe
//
//-----------------------------------------------------------------------------


#include "doomdef.h"

#include "command.h"
#include "console.h"

#include "g_game.h"

#include "sounds.h"
#include "info.h"

#include "m_cheat.h"
#include "d_think.h"
#include "dstrings.h"
#include "m_argv.h"
#include "p_inter.h"

//SoM: 4/4/2000: DEHACKED LUMPS
#include "z_zone.h"
#include "w_wad.h"

extern boolean infight; //DarkWolf95:November 21, 2003: Monsters Infight!

boolean deh_loaded = false;


#define MAXLINELEN  200

// the code was first write for a file
// converted to use memory with this functions
typedef struct {
    char *data;
    char *curpos;
    int size;
} MYFILE;

#define myfeof( a )  (a->data+a->size<=a->curpos)

char *myfgets(char *buf, int bufsize, MYFILE *f)
{
    int i=0;
    if( myfeof(f) )
        return NULL;
    // we need on byte for null terminated string
    bufsize--;
    while(i<bufsize && !myfeof(f) )
    {
        char c = *f->curpos++;
        if( c!='\r' )
            buf[i++]=c;
        if( c=='\n' )
            break;
    }
    buf[i] = '\0';
    //CONS_Printf("fgets [0]=%d [1]=%d '%s'\n",buf[0],buf[1],buf);

    return buf;
}

size_t  myfread( char *buf, size_t size, size_t count, MYFILE *f )
{
    size_t byteread = size-(f->curpos-f->data);
    if( size*count < byteread )
        byteread = size*count;
    if( byteread>0 )
    {
        ULONG i;
        for(i=0;i<byteread;i++)
        {
            char c=*f->curpos++;
            if( c!='\r' )
                buf[i]=c;
            else
                i--;
        }
    }
    return byteread/size;
}

static int deh_num_error=0;

static void deh_error(char *first, ...)
{
    va_list     argptr;

    if (devparm)
    {
       char buf[1000];

       va_start (argptr,first);
       vsprintf (buf, first,argptr);
       va_end (argptr);

       CONS_Printf("%s\n",buf);
    }

    deh_num_error++;
}

/* ======================================================================== */
// Load a dehacked file format 6 I (BP) don't know other format
/* ======================================================================== */
/* a sample to see
                   Thing 1 (Player)       {           // MT_PLAYER
int doomednum;     ID # = 3232              -1,             // doomednum
int spawnstate;    Initial frame = 32       S_PLAY,         // spawnstate
int spawnhealth;   Hit points = 3232        100,            // spawnhealth
int seestate;      First moving frame = 32  S_PLAY_RUN1,    // seestate
int seesound;      Alert sound = 32         sfx_None,       // seesound
int reactiontime;  Reaction time = 3232     0,              // reactiontime
int attacksound;   Attack sound = 32        sfx_None,       // attacksound
int painstate;     Injury frame = 32        S_PLAY_PAIN,    // painstate
int painchance;    Pain chance = 3232       255,            // painchance
int painsound;     Pain sound = 32          sfx_plpain,     // painsound
int meleestate;    Close attack frame = 32  S_NULL,         // meleestate
int missilestate;  Far attack frame = 32    S_PLAY_ATK1,    // missilestate
int deathstate;    Death frame = 32         S_PLAY_DIE1,    // deathstate
int xdeathstate;   Exploding frame = 32     S_PLAY_XDIE1,   // xdeathstate
int deathsound;    Death sound = 32         sfx_pldeth,     // deathsound
int speed;         Speed = 3232             0,              // speed
int radius;        Width = 211812352        16*FRACUNIT,    // radius
int height;        Height = 211812352       56*FRACUNIT,    // height
int mass;          Mass = 3232              100,            // mass
int damage;        Missile damage = 3232    0,              // damage
int activesound;   Action sound = 32        sfx_None,       // activesound
int flags;         Bits = 3232              MF_SOLID|MF_SHOOTABLE|MF_DROPOFF|MF_PICKUP|MF_NOTDMATCH,
int raisestate;    Respawn frame = 32       S_NULL          // raisestate
                                         }, */

static int searchvalue(char *s)
{
  while(s[0]!='=' && s[0]!='\0') s++;
  if (s[0]=='=')
    return atoi(&s[1]);
  else
  {
    deh_error("No value found\n");
    return 0;
  }
}

static void readthing(MYFILE *f,int num)
{
  char s[MAXLINELEN];
  char *word;
  int value;

  do{
    if(myfgets(s,sizeof(s),f)!=NULL)
    {
      if(s[0]=='\n') break;
      value=searchvalue(s);
      // set the value in apropriet field
      word=strtok(s," ");
           if(!strcmp(word,"ID"))           mobjinfo[num].doomednum   =value;
      else if(!strcmp(word,"Initial"))      mobjinfo[num].spawnstate  =value;
      else if(!strcmp(word,"Hit"))          mobjinfo[num].spawnhealth =value;
      else if(!strcmp(word,"First"))        mobjinfo[num].seestate    =value;
      else if(!strcmp(word,"Alert"))        mobjinfo[num].seesound    =value;
      else if(!strcmp(word,"Reaction"))     mobjinfo[num].reactiontime=value;
      else if(!strcmp(word,"Attack"))       mobjinfo[num].attacksound =value;
      else if(!strcmp(word,"Injury"))       mobjinfo[num].painstate   =value;
      else if(!strcmp(word,"Pain"))
           {
             word=strtok(NULL," ");
             if(!strcmp(word,"chance"))     mobjinfo[num].painchance  =value;
             else if(!strcmp(word,"sound")) mobjinfo[num].painsound   =value;
           }
      else if(!strcmp(word,"Close"))        mobjinfo[num].meleestate  =value;
      else if(!strcmp(word,"Far"))          mobjinfo[num].missilestate=value;
      else if(!strcmp(word,"Death"))
           {
             word=strtok(NULL," ");
             if(!strcmp(word,"frame"))      mobjinfo[num].deathstate  =value;
             else if(!strcmp(word,"sound")) mobjinfo[num].deathsound  =value;
           }
      else if(!strcmp(word,"Exploding"))    mobjinfo[num].xdeathstate =value;
      else if(!strcmp(word,"Speed"))        mobjinfo[num].speed       =value;
      else if(!strcmp(word,"Width"))        mobjinfo[num].radius      =value;
      else if(!strcmp(word,"Height"))       mobjinfo[num].height      =value;
      else if(!strcmp(word,"Mass"))         mobjinfo[num].mass        =value;
      else if(!strcmp(word,"Missile"))      mobjinfo[num].damage      =value;
      else if(!strcmp(word,"Action"))       mobjinfo[num].activesound =value;
      else if(!strcmp(word,"Bits"))         mobjinfo[num].flags       =value;
      else if(!strcmp(word,"Bits2"))        mobjinfo[num].flags2      =value;
      else if(!strcmp(word,"Respawn"))      mobjinfo[num].raisestate  =value;
      else deh_error("Thing %d : unknow word '%s'\n",num,word);
    }
  } while(s[0]!='\n' && !myfeof(f)); //finish when the line is empty
}
/*
Sprite number = 10
Sprite subnumber = 32968
Duration = 200
Next frame = 200
*/
static void readframe(MYFILE* f,int num)
{
  char s[MAXLINELEN];
  char *word1,*word2;
  int value;
  do{
    if(myfgets(s,sizeof(s),f)!=NULL)
    {
      if(s[0]=='\n') break;
      value=searchvalue(s);
      // set the value in apropriet field
      word1=strtok(s," ");
      word2=strtok(NULL," ");

      if(!strcmp(word1,"Sprite"))
      {
             if(!strcmp(word2,"number"))     states[num].sprite   =value;
        else if(!strcmp(word2,"subnumber"))  states[num].frame    =value;
      }
      else if(!strcmp(word1,"Duration"))     states[num].tics     =value;
      else if(!strcmp(word1,"Next"))         states[num].nextstate=value;
      else deh_error("Frame %d : unknow word '%s'\n",num,word1);
    }
  } while(s[0]!='\n' && !myfeof(f));
}

static void readsound(MYFILE* f,int num,char *savesfxnames[])
{
  char s[MAXLINELEN];
  char *word;
  int value;

  do{
    if(myfgets(s,sizeof(s),f)!=NULL)
    {
      if(s[0]=='\n') break;
      value=searchvalue(s);
      word=strtok(s," ");
           if(!strcmp(word,"Offset"))   {
                                          value-=150360;
                                          if(value<=64) value/=8;
                                          else if(value<=260) value=(value+4)/8;
                                          else value=(value+8)/8;
                                          if(value>=-1 && value<sfx_freeslot0-1)
                                              S_sfx[num].name=savesfxnames[value+1];
                                          else
                                              deh_error("Sound %d : offset out of bound\n",num);
                                        }
      else if(!strcmp(word,"Zero/One")) S_sfx[num].singularity=value;
      else if(!strcmp(word,"Value"))    S_sfx[num].priority   =value;
      else deh_error("Sound %d : unknow word '%s'\n",num,word);
    }
  } while(s[0]!='\n' && !myfeof(f));
}

static void readtext(MYFILE* f,int len1,int len2,char *savesfxname[],char *savesprnames[])
{
  char s[2001];
  int i;

  // it is hard to change all the text in doom
  // here i implement only vital things
  // yes text change somes tables like music, sound and sprite name
  if(len1+len2 > 2000)
  {
      deh_error("Text too big\n");
      return;
  }

  if(myfread(s,len1+len2,1,f))
  {
    s[len1+len2]='\0';
    // sound table
    for(i=0;i<sfx_freeslot0;i++)
      if(!strncmp(savesfxname[i],s,len1))
      {
        strncpy(S_sfx[i].name,&(s[len1]),len2);
        S_sfx[i].name[len2]='\0';
        return;
      }
    // sprite table
    for(i=0;i<NUMSPRITES;i++)
      if(!strncmp(savesprnames[i],s,len1))
      {
        strncpy(sprnames[i],&(s[len1]),len2);
        sprnames[i][len2]='\0';
        return;
      }
    // music table
    for(i=1;i<NUMMUSIC;i++)
      if( S_music[i].name && (!strncmp(S_music[i].name,s,len1)) )
      {
        strncpy(S_music[i].name,&(s[len1]),len2);
        S_music[i].name[len2]='\0';
        return;
      }
    // text table
    for(i=0;i<SPECIALDEHACKED;i++)
    {
      if(!strncmp(text[i],s,len1) && strlen(text[i])==(unsigned)len1)
      {
        // SoM: This is causing the problems! Apparently, VC++ doesn't
        // think we should be able to write to the text...
        // Hurdler: can we free the memory before allocating a new one
        //          -> free(text[i]); ?
        //if(strlen(text[i])<(unsigned)len2)         // increase size of the text
        //{
           text[i]=(char *)malloc(len2+1);
           if(text[i]==NULL)
               I_Error("ReadText : No More free Mem");
        //}

        strncpy(text[i],s + len1,len2);
        text[i][len2]='\0';
        return;
      }
    }

    // special text : text changed in Legacy but with dehacked support
    for(i=SPECIALDEHACKED;i<NUMTEXT;i++)
    {
       int temp = strlen(text[i]);

       if(len1>temp && strstr(s,text[i]))
       {
           char *t;

           // remove space for center the text
           t=&s[len1+len2-1];
           while(t[0]==' ') { t[0]='\0'; t--; }
           // skip the space
           while(s[len1]==' ') len1++;

           // remove version string identifier
           t=strstr(&(s[len1]),"v%i.%i");
           if(!t) {
              t=strstr(&(s[len1]),"%i.%i");
              if(!t) {
                 t=strstr(&(s[len1]),"%i");
                 if(!t) {
                      t=s+len1+strlen(&(s[len1]));
                 }
              }
           }
           t[0]='\0';
           len2=strlen(&s[len1]);

           if(strlen(text[i])<(unsigned)len2)         // incresse size of the text
           {
              text[i]=(char *)malloc(len2+1);
              if(text[i]==NULL)
                  I_Error("ReadText : No More free Mem");
           }

           strncpy(text[i],&(s[len1]),len2);
           text[i][len2]='\0';
           return;
       }
    }

    s[len1]='\0';
    deh_error("Text not changed :%s\n",s);
  }
}
/*
Ammo type = 2
Deselect frame = 11
Select frame = 12
Bobbing frame = 13
Shooting frame = 17
Firing frame = 10
*/
static void readweapon(MYFILE *f,int num)
{
  char s[MAXLINELEN];
  char *word;
  int value;
  do{
    if(myfgets(s,sizeof(s),f)!=NULL)
    {
      if(s[0]=='\n') break;
      value=searchvalue(s);
      word=strtok(s," ");

           if(!strcmp(word,"Ammo"))       doomweaponinfo[num].ammo      =value;
      else if(!strcmp(word,"Deselect"))   doomweaponinfo[num].upstate   =value;
      else if(!strcmp(word,"Select"))     doomweaponinfo[num].downstate =value;
      else if(!strcmp(word,"Bobbing"))    doomweaponinfo[num].readystate=value;
      else if(!strcmp(word,"Shooting"))   doomweaponinfo[num].atkstate  = doomweaponinfo[num].holdatkstate = value;
      else if(!strcmp(word,"Firing"))     doomweaponinfo[num].flashstate=value;
      else deh_error("Weapon %d : unknow word '%s'\n",num,word);
    }
  } while(s[0]!='\n' && !myfeof(f));
}
/*
Max ammo = 400
Per ammo = 40
*/

extern int clipammo[];
extern int GetWeaponAmmo[];

static void readammo(MYFILE *f,int num)
{
  char s[MAXLINELEN];
  char *word;
  int value;
  do{
    if(myfgets(s,sizeof(s),f)!=NULL)
    {
      if(s[0]=='\n') break;
      value=searchvalue(s);
      word=strtok(s," ");

           if(!strcmp(word,"Max"))  maxammo[num] =value;
      else if(!strcmp(word,"Per")) { clipammo[num]=value;GetWeaponAmmo[num] = 2*value; }
      else if(!strcmp(word,"Perweapon")) GetWeaponAmmo[num] = 2*value; 
      else deh_error("Ammo %d : unknow word '%s'\n",num,word);
    }
  } while(s[0]!='\n' && !myfeof(f));
}
// i don't like that but do you see a other way ?
extern int idfa_armor;
extern int idfa_armor_class;
extern int idkfa_armor;
extern int idkfa_armor_class;
extern int god_health;
extern int initial_health;
extern int initial_bullets;
extern int MAXHEALTH;
extern int max_armor;
extern int green_armor_class;
extern int blue_armor_class;
extern int maxsoul;
extern int soul_health;
extern int mega_health;


static void readmisc(MYFILE *f)
{
  char s[MAXLINELEN];
  char *word,*word2;
  int value;
  do{
    if(myfgets(s,sizeof(s),f)!=NULL)
    {
      if(s[0]=='\n') break;
      value=searchvalue(s);
      word=strtok(s," ");
      word2=strtok(NULL," ");

      if(!strcmp(word,"Initial"))
      {
         if(!strcmp(word2,"Health"))          initial_health=value;
         else if(!strcmp(word2,"Bullets"))    initial_bullets=value;
      }
      else if(!strcmp(word,"Max"))
      {
         if(!strcmp(word2,"Health"))          MAXHEALTH=value;
         else if(!strcmp(word2,"Armor"))      max_armor=value;
         else if(!strcmp(word2,"Soulsphere")) maxsoul=value;
      }
      else if(!strcmp(word,"Green"))         green_armor_class=value;
      else if(!strcmp(word,"Blue"))          blue_armor_class=value;
      else if(!strcmp(word,"Soulsphere"))    soul_health=value;
      else if(!strcmp(word,"Megasphere"))    mega_health=value;
      else if(!strcmp(word,"God"))           god_health=value;
      else if(!strcmp(word,"IDFA"))
      {
         word2=strtok(NULL," ");
         if(!strcmp(word2,"="))               idfa_armor=value;
         else if(!strcmp(word2,"Class"))      idfa_armor_class=value;
      }
      else if(!strcmp(word,"IDKFA"))
      {
         word2=strtok(NULL," ");
         if(!strcmp(word2,"="))               idkfa_armor=value;
         else if(!strcmp(word2,"Class"))      idkfa_armor_class=value;
      }
      else if(!strcmp(word,"BFG"))            doomweaponinfo[wp_bfg].ammopershoot = value;
      else if(!strcmp(word,"Monsters"))       infight = true; //DarkWolf95:November 21, 2003: Monsters Infight!
      else deh_error("Misc : unknow word '%s'\n",word);
    }
  } while(s[0]!='\n' && !myfeof(f));
}

extern byte cheat_mus_seq[];
extern byte cheat_choppers_seq[];
extern byte cheat_god_seq[];
extern byte cheat_ammo_seq[];
extern byte cheat_ammonokey_seq[];
extern byte cheat_noclip_seq[];
extern byte cheat_commercial_noclip_seq[];
extern byte cheat_powerup_seq[7][10];
extern byte cheat_clev_seq[];
extern byte cheat_mypos_seq[];
extern byte cheat_amap_seq[];

static void change_cheat_code(char *cheatseq,char* newcheat)
{
  unsigned char *i,*j;

  // encript data
  for(i=newcheat;i[0]!='\0';i++)
      i[0]=SCRAMBLE(i[0]);

  for(i=cheatseq,j=newcheat;j[0]!='\0' && j[0]!=0xff;i++,j++)
      if(i[0]==1 || i[0]==0xff) // no more place in the cheat
      {
         deh_error("Cheat too long\n");
         return;
      }
      else
         i[0]=j[0];

  // newcheatseq < oldcheat
  j=i;
  // search special cheat with 100
  for(;i[0]!=0xff;i++)
      if(i[0]==1)
      {
         *j++=1;
         *j++=0;
         *j++=0;
         break;
      }
  *j=0xff;

  return;
}

static void readcheat(MYFILE *f)
{
  char s[MAXLINELEN];
  char *word,*word2;
  char *value;

  do{
    if(myfgets(s,sizeof(s),f)!=NULL)
    {
      if(s[0]=='\n') break;
      strtok(s,"=");
      value=strtok(NULL," \n");         // skip the space
      strtok(NULL," \n");              // finish the string
      word=strtok(s," ");

      if(!strcmp(word     ,"Change"))        change_cheat_code(cheat_mus_seq,value);
      else if(!strcmp(word,"Chainsaw"))      change_cheat_code(cheat_choppers_seq,value);
      else if(!strcmp(word,"God"))           change_cheat_code(cheat_god_seq,value);
      else if(!strcmp(word,"Ammo"))
           {
             word2=strtok(NULL," ");

             if(word2 && !strcmp(word2,"&")) change_cheat_code(cheat_ammo_seq,value);
             else                            change_cheat_code(cheat_ammonokey_seq,value);
           }
      else if(!strcmp(word,"No"))
           {
             word2=strtok(NULL," ");
             if(word2)
                word2=strtok(NULL," ");

             if(word2 && !strcmp(word2,"1")) change_cheat_code(cheat_noclip_seq,value);
             else                            change_cheat_code(cheat_commercial_noclip_seq,value);

           }
      else if(!strcmp(word,"Invincibility")) change_cheat_code(cheat_powerup_seq[0],value);
      else if(!strcmp(word,"Berserk"))       change_cheat_code(cheat_powerup_seq[1],value);
      else if(!strcmp(word,"Invisibility"))  change_cheat_code(cheat_powerup_seq[2],value);
      else if(!strcmp(word,"Radiation"))     change_cheat_code(cheat_powerup_seq[3],value);
      else if(!strcmp(word,"Auto-map"))      change_cheat_code(cheat_powerup_seq[4],value);
      else if(!strcmp(word,"Lite-Amp"))      change_cheat_code(cheat_powerup_seq[5],value);
      else if(!strcmp(word,"BEHOLD"))        change_cheat_code(cheat_powerup_seq[6],value);
      else if(!strcmp(word,"Level"))         change_cheat_code(cheat_clev_seq,value);
      else if(!strcmp(word,"Player"))        change_cheat_code(cheat_mypos_seq,value);
      else if(!strcmp(word,"Map"))           change_cheat_code(cheat_amap_seq,value);
      else deh_error("Cheat : unknow word '%s'\n",word);
    }
  } while(s[0]!='\n' && !myfeof(f));
}


void DEH_LoadDehackedFile(MYFILE* f)
{
  
  char       s[1000];
  char       *word,*word2;
  int        i;
  // do a copy of this for cross references probleme
  actionf_t  saveactions[NUMSTATES];
  char       *savesprnames[NUMSPRITES];
  char       *savesfxnames[NUMSFX];

  deh_num_error=0;
  // save value for cross reference
  for(i=0;i<NUMSTATES;i++)
      saveactions[i]=states[i].action;
  for(i=0;i<NUMSPRITES;i++)
      savesprnames[i]=sprnames[i];
  for(i=0;i<NUMSFX;i++)
      savesfxnames[i]=S_sfx[i].name;

  // it don't test the version of doom
  // and version of dehacked file
  while(!myfeof(f))
  {
    myfgets(s,sizeof(s),f);
    if(s[0]=='\n' || s[0]=='#')
      continue;
    word=strtok(s," ");
    if(word!=NULL)
    {
      if((word2=strtok(NULL," "))!=NULL)
      {
        i=atoi(word2);

        if(!strcmp(word,"Thing"))
        {
          i--; // begin at 0 not 1;
          if(i<NUMMOBJTYPES && i>=0)
            readthing(f,i);
          else
            deh_error("Thing %d don't exist\n",i);
        }
        else if(!strcmp(word,"Frame"))
             {
               if(i<NUMSTATES && i>=0)
                  readframe(f,i);
               else
                  deh_error("Frame %d don't exist\n",i);
             }
        else if(!strcmp(word,"Pointer"))
             {
               word=strtok(NULL," "); // get frame
               if((word=strtok(NULL,")"))!=NULL)
               {
                 i=atoi(word);
                 if(i<NUMSTATES && i>=0)
                 {
                   if(myfgets(s,sizeof(s),f)!=NULL)
                     states[i].action=saveactions[searchvalue(s)];
                 }
                 else
                    deh_error("Pointer : Frame %d don't exist\n",i);
               }
               else
                   deh_error("pointer (Frame %d) : missing ')'\n",i);
             }
        else if(!strcmp(word,"Sound"))
             {
               if(i<NUMSFX && i>=0)
                   readsound(f,i,savesfxnames);
               else
                   deh_error("Sound %d don't exist\n");
             }
        else if(!strcmp(word,"Sprite"))
             {
               if(i<NUMSPRITES && i>=0)
               {
                 if(myfgets(s,sizeof(s),f)!=NULL)
                 {
                   int k;
                   k=(searchvalue(s)-151328)/8;
                   if(k>=0 && k<NUMSPRITES)
                       sprnames[i]=savesprnames[k];
                   else
                       deh_error("Sprite %i : offset out of bound\n",i);
                 }
               }
               else
                  deh_error("Sprite %d don't exist\n",i);
             }
        else if(!strcmp(word,"Text"))
             {
               int j;

               if((word=strtok(NULL," "))!=NULL)
               {
                 j=atoi(word);
                 readtext(f,i,j,savesfxnames,savesprnames);
               }
               else
                   deh_error("Text : missing second number\n");

             }
        else if(!strcmp(word,"Weapon"))
             {
               if(i<NUMWEAPONS && i>=0)
                   readweapon(f,i);
               else
                   deh_error("Weapon %d don't exist\n",i);
             }
        else if(!strcmp(word,"Ammo"))
             {
               if(i<NUMAMMO && i>=0)
                   readammo(f,i);
               else
                   deh_error("Ammo %d don't exist\n",i);
             }
        else if(!strcmp(word,"Misc"))
               readmisc(f);
        else if(!strcmp(word,"Cheat"))
               readcheat(f);
        else if(!strcmp(word,"Doom"))
             {
               int ver = searchvalue(strtok(NULL,"\n"));
               if( ver!=19)
                  deh_error("Warning : patch from a different doom version (%d), only verion 1.9 is supported\n",ver);
             }
        else if(!strcmp(word,"Patch"))
             {
               word=strtok(NULL," ");
               if(word && !strcmp(word,"format"))
               {
                  if(searchvalue(strtok(NULL,"\n"))!=6)
                     deh_error("Warning : Patch format not supported");
               }
             }
        //SoM: Support for Boom Extras (BEX)
/*        else if(!strcmp(word, "[STRINGS]"))
             {
             }
        else if(!strcmp(word, "[PARS]"))
             {
             }
        else if(!strcmp(word, "[CODEPTR]"))
             {
             }*/
        else deh_error("Unknow word : %s\n",word);
      }
      else
          deh_error("missing argument for '%s'\n",word);
    }
    else
        deh_error("No word in this line:\n%s\n",s);

  } // end while
  if (deh_num_error>0)
  {
      CONS_Printf("%d warning(s) in the dehacked file\n",deh_num_error);
      if (devparm)
          getchar();
  }

  deh_loaded = true;
}

// read dehacked lump in a wad (there is special trick for for deh 
// file that are converted to wad in w_wad.c)
void DEH_LoadDehackedLump(int lump)
{
    MYFILE f;
    
    f.size = W_LumpLength(lump);
    f.data = Z_Malloc(f.size + 1, PU_STATIC, 0);
    W_ReadLump(lump, f.data);
    f.curpos = f.data;
    f.data[f.size] = 0;

    DEH_LoadDehackedFile(&f);
    Z_Free(f.data);
}
