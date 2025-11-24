// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: dstrings.c,v 1.11 2003/09/14 12:49:12 darkwolf95 Exp $
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
// $Log: dstrings.c,v $
// Revision 1.11  2003/09/14 12:49:12  darkwolf95
// Grammar mistake - 'catched up' changed to 'caught' in rocket death
//
// Revision 1.10  2003/07/14 21:29:36  hurdler
// go RC1
//
// Revision 1.9  2001/08/20 20:40:39  metzgermeister
// *** empty log message ***
//
// Revision 1.8  2001/05/16 21:21:14  bpereira
// no message
//
// Revision 1.7  2001/01/25 22:15:41  bpereira
// added heretic support
//
// Revision 1.6  2000/11/02 17:50:06  stroggonmeth
// Big 3Dfloors & FraggleScript commit!!
//
// Revision 1.5  2000/04/20 21:49:53  hurdler
// fix a bug in dehacked
//
// Revision 1.4  2000/04/16 18:38:07  bpereira
// no message
//
// Revision 1.3  2000/04/04 00:32:45  stroggonmeth
// Initial Boom compatability plus few misc changes all around.
//
// Revision 1.2  2000/02/27 00:42:10  hurdler
// fix CR+LF problem
//
// Revision 1.1.1.1  2000/02/22 20:32:32  hurdler
// Initial import into CVS (v1.29 pr3)
//
//
// DESCRIPTION:
//      Globally defined strings.
// 
//-----------------------------------------------------------------------------

#include "dstrings.h"

char *text[NUMTEXT] = {
    "Development mode ON.\n",
    "CD-ROM Version: default.cfg from c:\\doomdata\n",
    "press a key.",
    "press y or n.",
    "only the server can do a load net game!\n\npress a key.",
    "you can't quickload during a netgame!\n\npress a key.",
    "you haven't picked a quicksave slot yet!\n\npress a key.",
    "you can't save if you aren't playing!\n\npress a key.",
    "quicksave over your game named\n\n'%s'?\n\npress y or n.",
    "do you want to quickload the game named\n\n'%s'?\n\npress y or n.",
    "you can't start a new game\n" "while in a network game.\n\n",
    "are you sure? this skill level\nisn't even remotely fair.\n\npress y or n.",
    "this is the shareware version of doom.\n\nyou need to order the entire trilogy.\n\npress a key.",
    "Messages OFF",
    "Messages ON",
    "you can't end a netgame!\n\npress a key.",
    "are you sure you want to end the game?\n\npress y or n.",

    "%s\n\n(press y to quit to dos.)",

    "High detail",
    "Low detail",
    "Gamma correction OFF",
    "Gamma correction level 1",
    "Gamma correction level 2",
    "Gamma correction level 3",
    "Gamma correction level 4",
    "empty slot",
    "Picked up the armor.",
    "Picked up the MegaArmor!",
    "Picked up a health bonus.",
    "Picked up an armor bonus.",
    "Picked up a stimpack.",
    "Picked up a medikit that you REALLY need!",
    "Picked up a medikit.",
    "Supercharge!",

    "Picked up a blue keycard.",
    "Picked up a yellow keycard.",
    "Picked up a red keycard.",
    "Picked up a blue skull key.",
    "Picked up a yellow skull key.",
    "Picked up a red skull key.",

    "Invulnerability!",
    "Berserk!",
    "Partial Invisibility",
    "Radiation Shielding Suit",
    "Computer Area Map",
    "Light Amplification Visor",
    "MegaSphere!",

    "Picked up a clip.",
    "Picked up a box of bullets.",
    "Picked up a rocket.",
    "Picked up a box of rockets.",
    "Picked up an energy cell.",
    "Picked up an energy cell pack.",
    "Picked up 4 shotgun shells.",
    "Picked up a box of shotgun shells.",
    "Picked up a backpack full of ammo!",

    "You got the BFG9000!  Oh, yes.",
    "You got the chaingun!",
    "A chainsaw!  Find some meat!",
    "You got the rocket launcher!",
    "You got the plasma gun!",
    "You got the shotgun!",
    "You got the super shotgun!",

    "You need a blue key to activate this object",
    "You need a red key to activate this object",
    "You need a yellow key to activate this object",
    "You need a blue key to open this door",
    "You need a red key to open this door",
    "You need a yellow key to open this door",

    "game saved.",
    "[Message unsent]",

    "E1M1: Hangar",
    "E1M2: Nuclear Plant",
    "E1M3: Toxin Refinery",
    "E1M4: Command Control",
    "E1M5: Phobos Lab",
    "E1M6: Central Processing",
    "E1M7: Computer Station",
    "E1M8: Phobos Anomaly",
    "E1M9: Military Base",

    "E2M1: Deimos Anomaly",
    "E2M2: Containment Area",
    "E2M3: Refinery",
    "E2M4: Deimos Lab",
    "E2M5: Command Center",
    "E2M6: Halls of the Damned",
    "E2M7: Spawning Vats",
    "E2M8: Tower of Babel",
    "E2M9: Fortress of Mystery",

    "E3M1: Hell Keep",
    "E3M2: Slough of Despair",
    "E3M3: Pandemonium",
    "E3M4: House of Pain",
    "E3M5: Unholy Cathedral",
    "E3M6: Mt. Erebus",
    "E3M7: Limbo",
    "E3M8: Dis",
    "E3M9: Warrens",

    "E4M1: Hell Beneath",
    "E4M2: Perfect Hatred",
    "E4M3: Sever The Wicked",
    "E4M4: Unruly Evil",
    "E4M5: They Will Repent",
    "E4M6: Against Thee Wickedly",
    "E4M7: And Hell Followed",
    "E4M8: Unto The Cruel",
    "E4M9: Fear",
    "level 1: entryway",
    "level 2: underhalls",
    "level 3: the gantlet",
    "level 4: the focus",
    "level 5: the waste tunnels",
    "level 6: the crusher",
    "level 7: dead simple",
    "level 8: tricks and traps",
    "level 9: the pit",
    "level 10: refueling base",
    "level 11: 'o' of destruction!",

    "level 12: the factory",
    "level 13: downtown",
    "level 14: the inmost dens",
    "level 15: industrial zone",
    "level 16: suburbs",
    "level 17: tenements",
    "level 18: the courtyard",
    "level 19: the citadel",
    "level 20: gotcha!",

    "level 21: nirvana",
    "level 22: the catacombs",
    "level 23: barrels o' fun",
    "level 24: the chasm",
    "level 25: bloodfalls",
    "level 26: the abandoned mines",
    "level 27: monster condo",
    "level 28: the spirit world",
    "level 29: the living end",
    "level 30: icon of sin",

    "level 31: wolfenstein",
    "level 32: grosse",

    "level 1: congo",
    "level 2: well of souls",
    "level 3: aztec",
    "level 4: caged",
    "level 5: ghost town",
    "level 6: baron's lair",
    "level 7: caughtyard",
    "level 8: realm",
    "level 9: abattoire",
    "level 10: onslaught",
    "level 11: hunted",

    "level 12: speed",
    "level 13: the crypt",
    "level 14: genesis",
    "level 15: the twilight",
    "level 16: the omen",
    "level 17: compound",
    "level 18: neurosphere",
    "level 19: nme",
    "level 20: the death domain",

    "level 21: slayer",
    "level 22: impossible mission",
    "level 23: tombstone",
    "level 24: the final frontier",
    "level 25: the temple of darkness",
    "level 26: bunker",
    "level 27: anti-christ",
    "level 28: the sewers",
    "level 29: odyssey of noises",
    "level 30: the gateway of hell",

    "level 31: cyberden",
    "level 32: go 2 it",

    "level 1: system control",
    "level 2: human bbq",
    "level 3: power control",
    "level 4: wormhole",
    "level 5: hanger",
    "level 6: open season",
    "level 7: prison",
    "level 8: metal",
    "level 9: stronghold",
    "level 10: redemption",
    "level 11: storage facility",

    "level 12: crater",
    "level 13: nukage processing",
    "level 14: steel works",
    "level 15: dead zone",
    "level 16: deepest reaches",
    "level 17: processing area",
    "level 18: mill",
    "level 19: shipping/respawning",
    "level 20: central processing",

    "level 21: administration center",
    "level 22: habitat",
    "level 23: lunar mining project",
    "level 24: quarry",
    "level 25: baron's den",
    "level 26: ballistyx",
    "level 27: mount pain",
    "level 28: heck",
    "level 29: river styx",
    "level 30: last call",

    "level 31: pharaoh",
    "level 32: caribbean",
    "I'm ready to kick butt!",
    "I'm OK.",
    "I'm not looking too good!",
    "Help!",
    "You suck!",
    "Next time, scumbag...",
    "Come here!",
    "I'll take care of it.",
    "Yes",
    "No",

    "You mumble to yourself",
    "Who's there?",
    "You scare yourself",
    "You start to rave",
    "You've lost it...",

    "[Message Sent]",
    "Follow Mode ON",
    "Follow Mode OFF",

    "Grid ON",
    "Grid OFF",
    "Marked Spot",
    "All Marks Cleared",

    "Music Change",
    "IMPOSSIBLE SELECTION",
    "Degreelessness Mode On",
    "Degreelessness Mode Off",

    "Very Happy Ammo Added",
    "Ammo (no keys) Added",

    "No Clipping Mode ON",
    "No Clipping Mode OFF",

    "inVuln, Str, Inviso, Rad, Allmap, or Lite-amp",
    "Power-up Toggled",

    "... doesn't suck - GM",
    "Changing Level...",

    "Once you beat the big badasses and\n" "clean out the moon base you're supposed\n" "to win, aren't you? Aren't you? Where's\n" "your fat reward and ticket home? What\n"
        "the hell is this? It's not supposed to\n" "end this way!\n" "\n" "It stinks like rotten meat, but looks\n" "like the lost Deimos base.  Looks like\n" "you're stuck on The Shores of Hell.\n"
        "The only way out is through.\n" "\n" "To continue the DOOM experience, play\n" "The Shores of Hell and its amazing\n" "sequel, Inferno!\n",

    "You've done it! The hideous cyber-\n" "demon lord that ruled the lost Deimos\n" "moon base has been slain and you\n" "are triumphant! But ... where are\n" "you? You clamber to the edge of the\n"
        "moon and look down to see the awful\n" "truth.\n" "\n" "Deimos floats above Hell itself!\n" "You've never heard of anyone escaping\n" "from Hell, but you'll make the bastards\n"
        "sorry they ever heard of you! Quickly,\n" "you rappel down to  the surface of\n" "Hell.\n" "\n" "Now, it's on to the final chapter of\n" "DOOM! -- Inferno.",

    "The loathsome spiderdemon that\n" "masterminded the invasion of the moon\n" "bases and caused so much death has had\n" "its ass kicked for all time.\n" "\n"
        "A hidden doorway opens and you enter.\n" "You've proven too tough for Hell to\n" "contain, and now Hell at last plays\n" "fair -- for you emerge from the door\n"
        "to see the green fields of Earth!\n" "Home at last.\n" "\n" "You wonder what's been happening on\n" "Earth while you were battling evil\n" "unleashed. It's good that no Hell-\n"
        "spawn could have come through that\n" "door with you ...",

    "the spider mastermind must have sent forth\n" "its legions of hellspawn before your\n" "final confrontation with that terrible\n" "beast from hell.  but you stepped forward\n"
        "and brought forth eternal damnation and\n" "suffering upon the horde as a true hero\n" "would in the face of something so evil.\n" "\n" "besides, someone was gonna pay for what\n"
        "happened to daisy, your pet rabbit.\n" "\n" "but now, you see spread before you more\n" "potential pain and gibbitude as a nation\n" "of demons run amok among our cities.\n" "\n"
        "next stop, hell on earth!",

    "YOU HAVE ENTERED DEEPLY INTO THE INFESTED\n" "STARPORT. BUT SOMETHING IS WRONG. THE\n" "MONSTERS HAVE BROUGHT THEIR OWN REALITY\n" "WITH THEM, AND THE STARPORT'S TECHNOLOGY\n"
        "IS BEING SUBVERTED BY THEIR PRESENCE.\n" "\n" "AHEAD, YOU SEE AN OUTPOST OF HELL, A\n" "FORTIFIED ZONE. IF YOU CAN GET PAST IT,\n" "YOU CAN PENETRATE INTO THE HAUNTED HEART\n"
        "OF THE STARBASE AND FIND THE CONTROLLING\n" "SWITCH WHICH HOLDS EARTH'S POPULATION\n" "HOSTAGE.",

    "YOU HAVE WON! YOUR VICTORY HAS ENABLED\n" "HUMANKIND TO EVACUATE EARTH AND ESCAPE\n" "THE NIGHTMARE.  NOW YOU ARE THE ONLY\n" "HUMAN LEFT ON THE FACE OF THE PLANET.\n"
        "CANNIBAL MUTATIONS, CARNIVOROUS ALIENS,\n" "AND EVIL SPIRITS ARE YOUR ONLY NEIGHBORS.\n" "YOU SIT BACK AND WAIT FOR DEATH, CONTENT\n" "THAT YOU HAVE SAVED YOUR SPECIES.\n" "\n"
        "BUT THEN, EARTH CONTROL BEAMS DOWN A\n" "MESSAGE FROM SPACE: \"SENSORS HAVE LOCATED\n" "THE SOURCE OF THE ALIEN INVASION. IF YOU\n" "GO THERE, YOU MAY BE ABLE TO BLOCK THEIR\n"
        "ENTRY.  THE ALIEN BASE IS IN THE HEART OF\n" "YOUR OWN HOME CITY, NOT FAR FROM THE\n" "STARPORT.\" SLOWLY AND PAINFULLY YOU GET\n" "UP AND RETURN TO THE FRAY.",

    "YOU ARE AT THE CORRUPT HEART OF THE CITY,\n" "SURROUNDED BY THE CORPSES OF YOUR ENEMIES.\n" "YOU SEE NO WAY TO DESTROY THE CREATURES'\n" "ENTRYWAY ON THIS SIDE, SO YOU CLENCH YOUR\n"
        "TEETH AND PLUNGE THROUGH IT.\n" "\n" "THERE MUST BE A WAY TO CLOSE IT ON THE\n" "OTHER SIDE. WHAT DO YOU CARE IF YOU'VE\n" "GOT TO GO THROUGH HELL TO GET TO IT?",

    "THE HORRENDOUS VISAGE OF THE BIGGEST\n" "DEMON YOU'VE EVER SEEN CRUMBLES BEFORE\n" "YOU, AFTER YOU PUMP YOUR ROCKETS INTO\n" "HIS EXPOSED BRAIN. THE MONSTER SHRIVELS\n"
        "UP AND DIES, ITS THRASHING LIMBS\n" "DEVASTATING UNTOLD MILES OF HELL'S\n" "SURFACE.\n" "\n" "YOU'VE DONE IT. THE INVASION IS OVER.\n" "EARTH IS SAVED. HELL IS A WRECK. YOU\n"
        "WONDER WHERE BAD FOLKS WILL GO WHEN THEY\n" "DIE, NOW. WIPING THE SWEAT FROM YOUR\n" "FOREHEAD YOU BEGIN THE LONG TREK BACK\n" "HOME. REBUILDING EARTH OUGHT TO BE A\n"
        "LOT MORE FUN THAN RUINING IT WAS.\n",

    "CONGRATULATIONS, YOU'VE FOUND THE SECRET\n" "LEVEL! LOOKS LIKE IT'S BEEN BUILT BY\n" "HUMANS, RATHER THAN DEMONS. YOU WONDER\n" "WHO THE INMATES OF THIS CORNER OF HELL\n" "WILL BE.",

    "CONGRATULATIONS, YOU'VE FOUND THE\n" "SUPER SECRET LEVEL!  YOU'D BETTER\n" "BLAZE THROUGH THIS ONE!\n",

    "You've fought your way out of the infested\n" "experimental labs.   It seems that UAC has\n" "once again gulped it down.  With their\n" "high turnover, it must be hard for poor\n"
        "old UAC to buy corporate health insurance\n" "nowadays..\n" "\n" "Ahead lies the military complex, now\n" "swarming with diseased horrors hot to get\n"
        "their teeth into you. With luck, the\n" "complex still has some warlike ordnance\n" "laying around.",

    "You hear the grinding of heavy machinery\n" "ahead.  You sure hope they're not stamping\n" "out new hellspawn, but you're ready to\n" "ream out a whole herd if you have to.\n"
        "They might be planning a blood feast, but\n" "you feel about as mean as two thousand\n" "maniacs packed into one mad killer.\n" "\n" "You don't plan to go down easy.",

    "The vista opening ahead looks real damn\n" "familiar. Smells familiar, too -- like\n" "fried excrement. You didn't like this\n" "place before, and you sure as hell ain't\n"
        "planning to like it now. The more you\n" "brood on it, the madder you get.\n" "Hefting your gun, an evil grin trickles\n" "onto your face. Time to take some names.",

    "Suddenly, all is silent, from one horizon\n" "to the other. The agonizing echo of Hell\n" "fades away, the nightmare sky turns to\n" "blue, the heaps of monster corpses start \n"
        "to evaporate along with the evil stench \n" "that filled the air. Jeeze, maybe you've\n" "done it. Have you really won?\n" "\n" "Something rumbles in the distance.\n"
        "A blue light begins to glow inside the\n" "ruined skull of the demon-spitter.",

    "What now? Looks totally different. Kind\n" "of like King Tut's condo. Well,\n" "whatever's here can't be any worse\n" "than usual. Can it?  Or maybe it's best\n" "to let sleeping gods lie..",

    "Time for a vacation. You've burst the\n" "bowels of hell and by golly you're ready\n" "for a break. You mutter to yourself,\n" "Maybe someone else can kick Hell's ass\n"
        "next time around. Ahead lies a quiet town,\n" "with peaceful flowing water, quaint\n" "buildings, and presumably no Hellspawn.\n" "\n" "As you step off the transport, you hear\n"
        "the stomp of a cyberdemon's iron shoe.",

    "ZOMBIEMAN",
    "SHOTGUN GUY",
    "HEAVY WEAPON DUDE",
    "IMP",
    "DEMON",
    "LOST SOUL",
    "CACODEMON",
    "HELL KNIGHT",
    "BARON OF HELL",
    "ARACHNOTRON",
    "PAIN ELEMENTAL",
    "REVENANT",
    "MANCUBUS",
    "ARCH-VILE",
    "THE SPIDER MASTERMIND",
    "THE CYBERDEMON",
    "OUR HERO",

    // DOOM1
    "are you sure you want to\nquit this great game?",
    "please don't leave, there's more\ndemons to toast!",
    "let's beat it -- this is turning\ninto a bloodbath!",
    "i wouldn't leave if i were you.\ndos is much worse.",
    "you're trying to say you like dos\nbetter than me, right?",
    "don't leave yet -- there's a\ndemon around that corner!",
    "ya know, next time you come in here\ni'm gonna toast ya.",
    "go ahead and leave. see if i care.",

    // QuitDOOM II messages
    "you want to quit?\nthen, thou hast lost an eighth!",
    "don't go now, there's a \ndimensional shambler waiting\nat the dos prompt!",
    "get outta here and go back\nto your boring programs.",
    "if i were your boss, i'd \n deathmatch ya in a minute!",
    "look, bud. you leave now\nand you forfeit your body count!",
    "just leave. when you come\nback, i'll be waiting with a bat.",
    "you're lucky i don't smack\nyou for thinking about leaving.",

    "FLOOR4_8",
    "SFLR6_1",
    "MFLR8_4",
    "MFLR8_3",
    "SLIME16",
    "RROCK14",
    "RROCK07",
    "RROCK17",
    "RROCK13",
    "RROCK19",

    "CREDIT",
    "HELP2",
    "VICTORY2",
    "ENDPIC",

    "===========================================================================\n"
    "ATTENTION:  This version of DOOM has been modified.  If you would like to\n"
    "get a copy of the original game, call 1-800-IDGAMES or see the readme file.\n"
    "        You will not receive technical support for modified games.\n"
    "                      press enter to continue\n"
    "===========================================================================\n",

    "===========================================================================\n"
    "                     This program is Free Software!\n"
    "===========================================================================\n",

    "===========================================================================\n"
    "                     This program is Free Software!\n"
    "             See the terms of the GNU General Public License\n" 
    "===========================================================================\n",

    "Austin Virtual Gaming: Levels will end after 20 minutes\n",
    "M_LoadDefaults: Load system defaults.\n",
    "Z_Init: Init zone memory allocation daemon. \n",
    "W_Init: Init WADfiles.\n",
    "M_Init: Init miscellaneous info.\n",
    "R_Init: Init DOOM refresh daemon - ",
    "\nP_Init: Init Playloop state.\n",
    "I_Init: Setting up machine state.\n",
    "D_CheckNetGame: Checking network game status.\n",
    "S_Init: Setting up sound.\n",
    "HU_Init: Setting up heads up display.\n",
    "ST_Init: Init status bar.\n",
    "External statistics registered.\n",

    "doom2.wad",
    "doomu.wad",
    "doom.wad",
    "doom1.wad",

    "c:\\doomdata",     //UNUSED
    "c:/doomdata/default.cfg",  //UNUSED
    "c:\\doomdata\\" SAVEGAMENAME "%c.dsg",     //UNUSED
    SAVEGAMENAME "%c.dsg",      //UNUSED
    "c:\\doomdata\\" SAVEGAMENAME "%d.dsg",
    SAVEGAMENAME "%d.dsg",

    //SoM: 3/9/2000: Boom generic key messages:
    "You need a blue card to open this door",
    "You need a red card to open this door",
    "You need a yellow card to open this door",
    "You need a blue skull to open this door",
    "You need a red skull to open this door",
    "You need a yellow skull to open this door",
    "Any key will open this door",
    "You need all three keys to open this door",
    "You need all six keys to open this door",

    // heretic strings
    "QUARTZ FLASK",
    "WINGS OF WRATH",
    "RING OF INVINCIBILITY",
    "TOME OF POWER",
    "SHADOWSPHERE",
    "MORPH OVUM",
    "MYSTIC URN",
    "TORCH",
    "TIME BOMB OF THE ANCIENTS",
    "CHAOS DEVICE",

    "WAND CRYSTAL",
    "CRYSTAL GEODE",
    "MACE SPHERES",
    "PILE OF MACE SPHERES",
    "ETHEREAL ARROWS",
    "QUIVER OF ETHEREAL ARROWS",
    "CLAW ORB",
    "ENERGY ORB",
    "LESSER RUNES",
    "GREATER RUNES",
    "FLAME ORB",
    "INFERNO ORB",

    "FIREMACE",
    "ETHEREAL CROSSBOW",
    "DRAGON CLAW",
    "HELLSTAFF",
    "PHOENIX ROD",
    "GAUNTLETS OF THE NECROMANCER",

    "BAG OF HOLDING",

    "GOD MODE ON",
    "GOD MODE OFF",
    "NO CLIPPING ON",
    "NO CLIPPING OFF",
    "ALL WEAPONS",
    "FLIGHT ON",
    "FLIGHT OFF",
    "POWER ON",
    "POWER OFF",
    "FULL HEALTH",
    "ALL KEYS",
    "SOUND DEBUG ON",
    "SOUND DEBUG OFF",
    "TICKER ON",
    "TICKER OFF",
    "CHOOSE AN ARTIFACT ( A - J )",
    "HOW MANY ( 1 - 9 )",
    "YOU GOT IT",
    "BAD INPUT",
    "LEVEL WARP",
    "SCREENSHOT",
    "CHICKEN ON",
    "CHICKEN OFF",
    "MASSACRE",
    "TRYING TO CHEAT, EH?  NOW YOU DIE!",
    "CHEATER - YOU DON'T DESERVE WEAPONS",

    // EPISODE 1 - THE CITY OF THE DAMNED
    "E1M1:  THE DOCKS",
    "E1M2:  THE DUNGEONS",
    "E1M3:  THE GATEHOUSE",
    "E1M4:  THE GUARD TOWER",
    "E1M5:  THE CITADEL",
    "E1M6:  THE CATHEDRAL",
    "E1M7:  THE CRYPTS",
    "E1M8:  HELL'S MAW",
    "E1M9:  THE GRAVEYARD",
    // EPISODE 2 - HELL'S MAW
    "E2M1:  THE CRATER",
    "E2M2:  THE LAVA PITS",
    "E2M3:  THE RIVER OF FIRE",
    "E2M4:  THE ICE GROTTO",
    "E2M5:  THE CATACOMBS",
    "E2M6:  THE LABYRINTH",
    "E2M7:  THE GREAT HALL",
    "E2M8:  THE PORTALS OF CHAOS",
    "E2M9:  THE GLACIER",
    // EPISODE 3 - THE DOME OF D'SPARIL
    "E3M1:  THE STOREHOUSE",
    "E3M2:  THE CESSPOOL",
    "E3M3:  THE CONFLUENCE",
    "E3M4:  THE AZURE FORTRESS",
    "E3M5:  THE OPHIDIAN LAIR",
    "E3M6:  THE HALLS OF FEAR",
    "E3M7:  THE CHASM",
    "E3M8:  D'SPARIL'S KEEP",
    "E3M9:  THE AQUIFER",
    // EPISODE 4: THE OSSUARY
    "E4M1:  CATAFALQUE",
    "E4M2:  BLOCKHOUSE",
    "E4M3:  AMBULATORY",
    "E4M4:  SEPULCHER",
    "E4M5:  GREAT STAIR",
    "E4M6:  HALLS OF THE APOSTATE",
    "E4M7:  RAMPARTS OF PERDITION",
    "E4M8:  SHATTERED BRIDGE",
    "E4M9:  MAUSOLEUM",
    // EPISODE 5: THE STAGNANT DEMESNE
    "E5M1:  OCHRE CLIFFS",
    "E5M2:  RAPIDS",
    "E5M3:  QUAY",
    "E5M4:  COURTYARD",
    "E5M5:  HYDRATYR",
    "E5M6:  COLONNADE",
    "E5M7:  FOETID MANSE",
    "E5M8:  FIELD OF JUDGEMENT",
    "E5M9:  SKEIN OF D'SPARIL",

// Heretic E1TEXT
    "with the destruction of the iron\n" "liches and their minions, the last\n" "of the undead are cleared from this\n" "plane of existence.\n\n" "those creatures had to come from\n"
        "somewhere, though, and you have the\n" "sneaky suspicion that the fiery\n" "portal of hell's maw opens onto\n" "their home dimension.\n\n" "to make sure that more undead\n"
        "(or even worse things) don't come\n" "through, you'll have to seal hell's\n" "maw from the other side. of course\n" "this means you may get stuck in a\n" "very unfriendly world, but no one\n"
        "ever said being a Heretic was easy!",

// Heretic E2TEXT
    "the mighty maulotaurs have proved\n" "to be no match for you, and as\n" "their steaming corpses slide to the\n" "ground you feel a sense of grim\n" "satisfaction that they have been\n"
        "destroyed.\n\n" "the gateways which they guarded\n" "have opened, revealing what you\n" "hope is the way home. but as you\n" "step through, mocking laughter\n" "rings in your ears.\n\n"
        "was some other force controlling\n" "the maulotaurs? could there be even\n" "more horrific beings through this\n" "gate? the sweep of a crystal dome\n" "overhead where the sky should be is\n"
        "certainly not a good sign....",

// Heretic E3TEXT
    "the death of d'sparil has loosed\n" "the magical bonds holding his\n" "creatures on this plane, their\n" "dying screams overwhelming his own\n" "cries of agony.\n\n"
        "your oath of vengeance fulfilled,\n" "you enter the portal to your own\n" "world, mere moments before the dome\n" "shatters into a million pieces.\n\n" "but if d'sparil's power is broken\n"
        "forever, why don't you feel safe?\n" "was it that last shout just before\n" "his death, the one that sounded\n" "like a curse? or a summoning? you\n" "can't really be sure, but it might\n"
        "just have been a scream.\n\n" "then again, what about the other\n" "serpent riders?",

// Heretic E4TEXT
    "you thought you would return to your\n" "own world after d'sparil died, but\n" "his final act banished you to his\n" "own plane. here you entered the\n" "shattered remnants of lands\n"
        "conquered by d'sparil. you defeated\n" "the last guardians of these lands,\n" "but now you stand before the gates\n" "to d'sparil's stronghold. until this\n"
        "moment you had no doubts about your\n" "ability to face anything you might\n" "encounter, but beyond this portal\n" "lies the very heart of the evil\n" "which invaded your world. d'sparil\n"
        "might be dead, but the pit where he\n" "was spawned remains. now you must\n" "enter that pit in the hopes of\n" "finding a way out. and somewhere,\n" "in the darkest corner of d'sparil's\n"
        "demesne, his personal bodyguards\n" "await your arrival ...",

// Heretic E5TEXT
    "as the final maulotaur bellows his\n" "death-agony, you realize that you\n" "have never come so close to your own\n" "destruction. not even the fight with\n"
        "d'sparil and his disciples had been\n" "this desperate. grimly you stare at\n" "the gates which open before you,\n" "wondering if they lead home, or if\n" "they open onto some undreamed-of\n"
        "horror. you find yourself wondering\n" "if you have the strength to go on,\n" "if nothing but death and pain await\n" "you. but what else can you do, if\n"
        "the will to fight is gone? can you\n" "force yourself to continue in the\n" "face of such despair? do you have\n" "the courage? you find, in the end,\n" "that it is not within you to\n"
        "surrender without a fight. eyes\n" "wide, you go to meet your fate.",

    "%s suicides\n",
    "%s was telefraged by %s\n",
    "%s was beaten to a pulp by %s\n",
    "%s was gunned by %s\n",
    "%s was shot down by %s\n",
    "%s was machine-gunned by %s\n",
    "%s was caught by %s's rocket\n",
    "%s was gibbed by %s's rocket\n",
    "%s eats %s's toaster\n",
    "%s enjoys %s's big fraggin' gun\n",
    "%s was divided up into little pieces by %s's chainsaw\n",
    "%s ate 2 loads of %s's buckshot\n",
    "%s was killed by %s\n",
    "%s dies in hellslime\n",
    "%s gulped a load of nukage\n",
    "%s dies in super hellslime/strobe hurt\n",
    "%s dies in special sector\n",
    "%s was barrel-fragged by %s\n",
    "%s dies from a barrel explosion\n",
    "%s was shot by a possessed\n",
    "%s was shot down by a shotguy\n",
    "%s was blasted by an Arch-vile\n",
    "%s was exploded by a Mancubus\n",
    "%s was punctured by a Chainguy\n",
    "%s was fried by an Imp\n",
    "%s was eviscerated by a Demon\n",
    "%s was mauled by a Shadow Demon\n",
    "%s was fried by a Caco-demon\n",
    "%s was slain by a Baron of Hell\n",
    "%s was smashed by a Revenant\n",
    "%s was slain by a Hell-Knight\n",
    "%s was killed by a Lost Soul\n",
    "%s was killed by a The Spider Mastermind\n",
    "%s was killed by a Arachnotron\n",
    "%s was crushed by the Cyber-demon\n",
    "%s was killed by a Pain Elemental\n",
    "%s was killed by a WolfSS\n",
    "%s died\n",

    //BP: here is special dehacked handling, include centring and version

    "DOOM 2: Hell on Earth",
    "The Ultimate DOOM Startup",
    "DOOM Registered Startup",
    "DOOM Shareware Startup",

};

char savegamename[256];
