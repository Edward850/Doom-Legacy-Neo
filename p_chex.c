#include "doomdef.h"
#include "g_game.h"
#include "p_local.h"
#include "r_main.h"
#include "r_state.h"
#include "s_sound.h"
#include "m_random.h"
#include "m_cheat.h"
#include "dstrings.h"
#include "p_chex.h"

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

#ifdef HWRENDER
		extern light_t lspr[];
#endif

void Chex1PatchEngine(void)
{

	//patch new text
	text[LOADNET_NUM] = "only the server can do a load net quest!\n\npress a key.";
	text[QLOADNET_NUM] = "you can't quickload during a netquest!\n\npress a key.";
	text[QSPROMPT_NUM] = "quicksave over your quest named\n\n'%s'?\n\npress y or n.";
	text[QLPROMPT_NUM] = "do you want to quickload the quest named\n\n'%s'?\n\npress y or n.";
	text[NEWGAME_NUM] = "you can't start a new quest\n" "while in a network quest.\n\n";
	text[NIGHTMARE_NUM] = "Careful, this will be tough.\nDo you wish to continue?\n\npress y or n.";
	text[SWSTRING_NUM] = "this is Chex(R) Quest. look for\n\nfuture levels at www.chexquest.com.\n\npress a key.";
	text[NETEND_NUM] = "you can't end a netquest!\n\npress a key.";
	text[ENDGAME_NUM] = "are you sure you want to end the quest?\n\npress y or n.";

	text[GOTARMOR_NUM] = "Picked up the Chex(R) Armor.";
	text[GOTMEGA_NUM] = "Picked up the Super Chex(R) Armor!";
	text[GOTHTHBONUS_NUM] = "Picked up a glass of water.";
	text[GOTARMBONUS_NUM] = "Picked up slime repellent.";
	text[GOTSTIM_NUM] = "Picked up a bowl of fruit.";
	text[GOTMEDINEED_NUM] = "Picked up some needed vegetables!";
	text[GOTMEDIKIT_NUM] = "Picked up a bowl of vegetables.";
	text[GOTSUPER_NUM] = "Supercharge Breakfast!";

	text[GOTBLUECARD_NUM] = "Picked up a blue key.";
	text[GOTYELWCARD_NUM] = "Picked up a yellow key.";
	text[GOTREDCARD_NUM] = "Picked up a red key.";
	text[GOTBLUESKUL_NUM] = "Picked up a blue key.";
	text[GOTYELWSKUL_NUM] = "Picked up a yellow key.";
	text[GOTREDSKULL_NUM] = "Picked up a red key.";

	text[GOTSUIT_NUM] = "Slimeproof Suit";

	text[GOTCLIP_NUM] = "Picked up a mini zorch recharge.";
	text[GOTCLIPBOX_NUM] = "Picked up a mini zorch pack.";
	text[GOTROCKET_NUM] = "Picked up a zorch propulsor recharge.";
	text[GOTROCKBOX_NUM] = "Picked up a zorch propulsor pack.";
	text[GOTCELL_NUM] = "Picked up a phasing zorcher recharge.";
	text[GOTCELLBOX_NUM] = "Picked up a phasing zorcher pack.";
	text[GOTSHELLS_NUM] = "Picked up a large zorcher recharge.";
	text[GOTSHELLBOX_NUM] = "Picked up a large zorcher pack.";
	text[GOTBACKPACK_NUM] = "Picked up a Zorchpak!";

	text[GOTBFG9000_NUM] = "You got the LAZ Device!";
	text[GOTCHAINGUN_NUM] = "You got the Rapid Zorcher!";
	text[GOTCHAINSAW_NUM] = "You got the Super Bootspork!";
	text[GOTLAUNCHER_NUM] = "You got the Zorch Propulsor!";
	text[GOTPLASMA_NUM] = "You got the Phasing Zorcher!";
	text[GOTSHOTGUN_NUM] = "You got the Large Zorcher!";
	text[GOTSHOTGUN2_NUM] = "You got the Super Large Zorcher!";

	text[HUSTR_E1M1_NUM] = "E1M1: Landing Zone";
	text[HUSTR_E1M2_NUM] = "E1M2: Storage Facility";
	text[HUSTR_E1M3_NUM] = "E1M3: Experimental Lab";
	text[HUSTR_E1M4_NUM] = "E1M4: Arboretum";
	text[HUSTR_E1M5_NUM] = "E1M5: Caverns of Bazoik";

	text[HUSTR_CHATMACRO1_NUM] = "I'm ready to zorch!";
	text[HUSTR_CHATMACRO2_NUM] = "I'm feeling great!";
	text[HUSTR_CHATMACRO3_NUM] = "I'm getting pretty gooed up!";
	text[HUSTR_CHATMACRO4_NUM] = "Somebody help me!";
	text[HUSTR_CHATMACRO5_NUM] = "Go back to your own dimension!";
	text[HUSTR_CHATMACRO6_NUM] = "Stop that Flemoid";
	text[HUSTR_CHATMACRO7_NUM] = "I think I'm lost!";
	text[HUSTR_CHATMACRO8_NUM] = "I'll get you out of this gunk.";

	text[HUSTR_TALKTOSELF1_NUM] = "I'm feeling great.";
	text[HUSTR_TALKTOSELF2_NUM] = "I think I'm lost.";
	text[HUSTR_TALKTOSELF3_NUM] = "Oh No...";
	text[HUSTR_TALKTOSELF4_NUM] = "Gotta break free.";
	text[HUSTR_TALKTOSELF5_NUM] = "Hurry!";

	text[STSTR_DQDON_NUM] = "Invincible Mode On";
	text[STSTR_DQDOFF_NUM] = "Invincible Mode Off";

	text[STSTR_KFAADDED_NUM] = "Super Zorch Added";
	text[STSTR_FAADDED_NUM] = "Zorch Added";

	text[STSTR_CHOPPERS_NUM] = "... Eat Chex(R)!";

	text[E1TEXT_NUM] = "Mission accomplished.\n\nAre you prepared for the next mission?\n\n\n\n\n\n\nPress the escape key to continue...\n";
	text[E2TEXT_NUM] = "You've done it!";
	text[E3TEXT_NUM] = "Wonderful Job!";
	text[E4TEXT_NUM] = "Fantastic";
	text[C1TEXT_NUM] = "Great!";
	text[C2TEXT_NUM] = "Way to go!";
	text[C3TEXT_NUM] = "Thanks for the help!";
	text[C4TEXT_NUM] = "Great!\n";
	text[C5TEXT_NUM] = "Fabulous!";
	text[C6TEXT_NUM] = "CONGRATULATIONS!\n";
	text[T1TEXT_NUM] = "There's more to come...";
	text[T2TEXT_NUM] = "Keep up the good work!";
	text[T3TEXT_NUM] = "Get ready!.";
	text[T4TEXT_NUM] = "Be Proud.";
	text[T5TEXT_NUM] = "Wow!";
	text[T6TEXT_NUM] = "Great.";

	text[CC_ZOMBIE_NUM] = "FLEMOIDUS COMMONUS";
	text[CC_SHOTGUN_NUM] = "FLEMOIDUS BIPEDICUS";
	text[CC_IMP_NUM] = "FLEMOIDUS BIPEDICUS WITH ARMOR";
	text[CC_DEMON_NUM] = "FLEMOIDUS CYCLOPTIS";
	text[CC_BARON_NUM] = "THE FLEMBRANE";

	char* NEW_QUIT1 = "Don't give up now...do\nyou still wish to quit?";
	char* NEW_QUIT2 = "please don't leave we\nneed your help!";
	
	text[QUITMSG_NUM] = NEW_QUIT1;
	text[QUITMSG1_NUM] = NEW_QUIT2;
	text[QUITMSG2_NUM] = NEW_QUIT2;
	text[QUITMSG3_NUM] = NEW_QUIT2;
	text[QUITMSG4_NUM] = NEW_QUIT2;
	text[QUITMSG5_NUM] = NEW_QUIT2;
	text[QUITMSG6_NUM] = NEW_QUIT2;
	text[QUITMSG7_NUM] = NEW_QUIT2;

	text[QUIT2MSG_NUM] = NEW_QUIT1;
	text[QUIT2MSG1_NUM] = NEW_QUIT2;
	text[QUIT2MSG2_NUM] = NEW_QUIT2;
	text[QUIT2MSG3_NUM] = NEW_QUIT2;
	text[QUIT2MSG4_NUM] = NEW_QUIT2;
	text[QUIT2MSG5_NUM] = NEW_QUIT2;
	text[QUIT2MSG6_NUM] = NEW_QUIT2;

	text[R_INIT_NUM] = "R_Init: Init Chex(R) Quest refresh daemon - ";

	text[DEATHMSG_SUICIDE] = "%s was hit by inter-dimensional slime!\n";
	text[DEATHMSG_TELEFRAG] = "%s was telezorched by %s.\n";
	text[DEATHMSG_FIST] = "%s was spoon fed by %s.\n";
	text[DEATHMSG_GUN] = "%s was popped by %s's Mini-zorcher.\n";
	text[DEATHMSG_SHOTGUN] = "%s was zapped by %s's clip.\n";
	text[DEATHMSG_MACHGUN] = "%s was spun by %s's Rapid Zorcher.\n";
	text[DEATHMSG_ROCKET] = "%s was hit by %s's Propulsor.\n";
	text[DEATHMSG_PLASMA] = "%s was mowed by %s's Phasing Zorcher.\n";
	text[DEATHMSG_BFGBALL] = "%s melted in %s's lasagna pan.\n";
	text[DEATHMSG_CHAINSAW] = "%s was mixed by %s's Bootspork.\n";
	text[DEATHMSG_SUPSHOTGUN] = "%s was pelted by two of %s's clips.\n";
	text[DEATHMSG_HELLSLIME] = "%s fell into a goo pit!\n";
	text[DEATHMSG_NUKE] = "%s sinks into some slime.\n";
	text[DEATHMSG_SUPHELLSLIME] = "%s fell into quickslime!\n";
	text[DEATHMSG_SPECUNKNOW] = "%s was hit by inter-dimensional slime!\n";
	text[DEATHMSG_POSSESSED] = "%s was slimed by a Commonus.\n";
	text[DEATHMSG_SHOTGUY] = "%s was slimed by a Bipedicus.\n";
	text[DEATHMSG_TROOP] = "%s was slimed by a BWA.\n";
	text[DEATHMSG_SERGEANT] = "%s was slimed by a Cycloptis.\n";
	text[DEATHMSG_BRUISER] = "%s was defeated by the Flembrane.\n";
	text[DEATHMSG_DEAD] = "%s was slimed.\n";

	//patch monster changes
	mobjinfo[MT_POSSESSED].missilestate = 0;
	mobjinfo[MT_POSSESSED].meleestate = S_POSS_ATK1;

	mobjinfo[MT_SHOTGUY].missilestate = 0;
	mobjinfo[MT_SHOTGUY].meleestate = S_SPOS_ATK1;

	//coronas
	#ifdef HWRENDER
		lspr[1].dynamic_color = 0xff000050;
		lspr[2].dynamic_color = 0xff000050;
		lspr[3].dynamic_color = 0xff4040f7;
		lspr[4].dynamic_color = 0xff4040f7;
		lspr[5].dynamic_color = 0xff4040f7;
		lspr[6].dynamic_color = 0xff4040a0;
		lspr[7].type = 0;
		lspr[8].type = 0;
		lspr[9].type = 0;
		lspr[10].type = 0;
		lspr[11].type = 0;
		lspr[12].type = 0;
		lspr[15].light_yoffset = 3.0f;	//light
		lspr[16].type = 0;
		lspr[17].type = 0;
	#endif

	//cheat codes
	
}