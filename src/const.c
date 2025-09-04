/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*	ROM 2.4 is copyright 1993-1995 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@pacinfo.com)				   *
*	    Gabrielle Taylor (gtaylor@pacinfo.com)			   *
*	    Brian Moore (rom@rom.efn.org)				   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

/*************************************************************************** 
*       ROT 1.4 is copyright 1996-1997 by Russ Walsh                       * 
*       By using this code, you have agreed to follow the terms of the     * 
*       ROT license, in the file doc/rot.license                           * 
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "interp.h"


char *	const	dir_name	[]		=
{
	"{Cnorth{x", "{Reast{x", "{Msouth{x", "{Gwest{x", "{Yup{x", "{Ddown{x",
	"north", "east", "south", "west", "up", "down"
};
char *  const   rev_move        []              =
{
    "the south", "the west", "the north", "the east", "below", "above"
};
const	sh_int	rev_dir		[]		=
{
    2, 3, 0, 1, 5, 4, 2, 3, 0, 1, 5, 4
};

const	sh_int	movement_loss	[SECT_MAX]	=
{
    1, 2, 2, 3, 4, 6, 4, 1, 6, 10, 6
};

/* item type list */
const struct item_type		item_table	[]	=
{
    {	ITEM_LIGHT,	"light"		},
    {	ITEM_SCROLL,	"scroll"	},
    {	ITEM_WAND,	"wand"		},
    {   ITEM_STAFF,	"staff"		},
    {   ITEM_WEAPON,	"weapon"	},
    {   ITEM_TREASURE,	"treasure"	},
    {   ITEM_ARMOR,	"armor"		},
    {	ITEM_POTION,	"potion"	},
    {	ITEM_CLOTHING,	"clothing"	},
    {   ITEM_FURNITURE,	"furniture"	},
    {	ITEM_TRASH,	"trash"		},
    {	ITEM_CONTAINER,	"container"	},
    {	ITEM_DRINK_CON, "drink"		},
    {	ITEM_KEY,	"key"		},
    {	ITEM_FOOD,	"food"		},
    {	ITEM_MONEY,	"money"		},
    {	ITEM_BOAT,	"boat"		},
    {	ITEM_CORPSE_NPC,"npc_corpse"	},
    {	ITEM_CORPSE_PC,	"pc_corpse"	},
    {   ITEM_FOUNTAIN,	"fountain"	},
    {	ITEM_PILL,	"pill"		},
    {	ITEM_PROTECT,	"protect"	},
    {	ITEM_MAP,	"map"		},
    {	ITEM_PORTAL,	"portal"	},
    {	ITEM_WARP_STONE,"warp_stone"	},
    {	ITEM_ROOM_KEY,	"room_key"	},
    {	ITEM_GEM,	"gem"		},
    {	ITEM_JEWELRY,	"jewelry"	},
    {   ITEM_JUKEBOX,	"jukebox"	},
    {	ITEM_DEMON_STONE,"demon_stone"	},
    {	ITEM_EXIT,	"exit"		},
    {	ITEM_PIT,	"pit"		},
    {   0,		NULL		}
};


/* weapon selection table */
const	struct	weapon_type	weapon_table	[]	=
{
   { "sword",	OBJ_VNUM_SCHOOL_SWORD,	WEAPON_SWORD,	&gsn_sword	},
   { "mace",	OBJ_VNUM_SCHOOL_MACE,	WEAPON_MACE,	&gsn_mace 	},
   { "dagger",	OBJ_VNUM_SCHOOL_DAGGER,	WEAPON_DAGGER,	&gsn_dagger	},
   { "axe",	OBJ_VNUM_SCHOOL_AXE,	WEAPON_AXE,	&gsn_axe	},
   { "staff",	OBJ_VNUM_SCHOOL_STAFF,	WEAPON_SPEAR,	&gsn_spear	},
   { "flail",	OBJ_VNUM_SCHOOL_FLAIL,	WEAPON_FLAIL,	&gsn_flail	},
   { "whip",	OBJ_VNUM_SCHOOL_WHIP,	WEAPON_WHIP,	&gsn_whip	},
   { "polearm",	OBJ_VNUM_SCHOOL_POLEARM,WEAPON_POLEARM,	&gsn_polearm	},
   { NULL,	0,				0,	NULL		}
};


 
/* wiznet table and prototype for future flag setting */
const   struct wiznet_type      wiznet_table    []              =
{
   {    "on",           WIZ_ON,         0  },
   {    "prefix",	WIZ_PREFIX,	0  },
   {    "ticks",        WIZ_TICKS,      IM },
   {    "logins",       WIZ_LOGINS,     IM },
   {    "sites",        WIZ_SITES,      IM },
   {    "links",        WIZ_LINKS,      L7 },
   {	"newbies",	WIZ_NEWBIE,	0  },
   {	"spam",		WIZ_SPAM,	L5 },
   {    "deaths",       WIZ_DEATHS,     IM },
   {    "resets",       WIZ_RESETS,     L4 },
   {    "mobdeaths",    WIZ_MOBDEATHS,  L4 },
   {    "flags",	WIZ_FLAGS,	L5 },
   {	"penalties",	WIZ_PENALTIES,	L5 },
   {	"saccing",	WIZ_SACCING,	L5 },
   {	"levels",	WIZ_LEVELS,	0  },
   {	"load",		WIZ_LOAD,	L2 },
   {	"restore",	WIZ_RESTORE,	L2 },
   {	"snoops",	WIZ_SNOOPS,	L2 },
   {	"switches",	WIZ_SWITCHES,	L2 },
   {	"secure",	WIZ_SECURE,	L1 },
   {	NULL,		0,		0  }
};

/* attack table  -- not very organized :(
 * Be sure to keep MAX_DAMAGE_MESSAGE up
 * to date in merc.h
 */
const 	struct attack_type	attack_table	[]		=
{
    { 	"none",		"hit",		-1		},  /*  0 */
    {	"slice",	"slice", 	DAM_SLASH	},	
    {   "stab",		"stab",		DAM_PIERCE	},
    {	"slash",	"slash",	DAM_SLASH	},
    {	"whip",		"whip",		DAM_SLASH	},
    {   "claw",		"claw",		DAM_SLASH	},  /*  5 */
    {	"blast",	"blast",	DAM_BASH	},
    {   "pound",	"pound",	DAM_BASH	},
    {	"crush",	"crush",	DAM_BASH	},
    {   "grep",		"grep",		DAM_SLASH	},
    {	"bite",		"bite",		DAM_PIERCE	},  /* 10 */
    {   "pierce",	"pierce",	DAM_PIERCE	},
    {   "suction",	"suction",	DAM_BASH	},
    {	"beating",	"beating",	DAM_BASH	},
    {   "digestion",	"digestion",	DAM_ACID	},
    {	"charge",	"charge",	DAM_BASH	},  /* 15 */
    { 	"slap",		"slap",		DAM_BASH	},
    {	"punch",	"punch",	DAM_BASH	},
    {	"wrath",	"wrath",	DAM_ENERGY	},
    {	"magic",	"magic",	DAM_ENERGY	},
    {   "divine",	"divine power",	DAM_HOLY	},  /* 20 */
    {	"cleave",	"cleave",	DAM_SLASH	},
    {	"scratch",	"scratch",	DAM_PIERCE	},
    {   "peck",		"peck",		DAM_PIERCE	},
    {   "peckb",	"peck",		DAM_BASH	},
    {   "chop",		"chop",		DAM_SLASH	},  /* 25 */
    {   "sting",	"sting",	DAM_PIERCE	},
    {   "smash",	 "smash",	DAM_BASH	},
    {   "shbite",	"shocking bite",DAM_LIGHTNING	},
    {	"flbite",	"flaming bite", DAM_FIRE	},
    {	"frbite",	"freezing bite", DAM_COLD	},  /* 30 */
    {	"acbite",	"acidic bite", 	DAM_ACID	},
    {	"chomp",	"chomp",	DAM_PIERCE	},
    {  	"drain",	"life drain",	DAM_NEGATIVE	},
    {   "thrust",	"thrust",	DAM_PIERCE	},
    {   "slime",	"slime",	DAM_ACID	},
    {	"shock",	"shock",	DAM_LIGHTNING	},
    {   "thwack",	"thwack",	DAM_BASH	},
    {   "flame",	"flame",	DAM_FIRE	},
    {   "chill",	"chill",	DAM_COLD	},
    {   "typo",		"typo",		DAM_SLASH	}, /* 40 */
    {   NULL,		NULL,		0		}
};

/* race table */
struct	race_type	race_table	[]		=
{
/*
    {
	name,		pc_race?,
	act bits,
	aff_by bits,
	off bits,
	imm,
	res,
	vuln,
	shd bits,
	form,		parts 
    },
*/
    { "unique",		FALSE, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    { 
	"human",		TRUE, 
	0,
	0, 
	0,
	0, 
	0,
	0,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    {
	"elf",			TRUE,
	0,
	AFF_INFRARED,
	0,
	0,
	RES_CHARM,
	VULN_IRON,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    {
	"dwarf",		TRUE,
	0,
	AFF_INFRARED,
	0,
	0,
	RES_POISON|RES_DISEASE,
	VULN_DROWNING,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    {
	"giant",		TRUE,
	0,
	0,
	0,
	0,
	RES_FIRE|RES_COLD,
	VULN_MENTAL|VULN_LIGHTNING,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    { 
	"pixie",		TRUE,
	0,
	AFF_FLYING|AFF_DETECT_MAGIC|AFF_INFRARED,
	0,
	0,
	RES_CHARM|RES_MENTAL,
	VULN_IRON,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|P
    },

    { 
	"halfling",		TRUE,
	0,
	AFF_PASS_DOOR,
	0,
	0,
	RES_POISON|RES_DISEASE,
	VULN_LIGHT,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    { 
	"halforc",		TRUE,
	0,
	AFF_BERSERK,
	0,
	0,
	RES_MAGIC|RES_WEAPON,
	VULN_MENTAL,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    { 
	"goblin",		TRUE,
	0,
	AFF_INFRARED,
	0,
	0,
	RES_MENTAL,
	VULN_SILVER|VULN_LIGHT|VULN_WOOD|VULN_HOLY,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    { 
	"halfelf",		TRUE,
	0,
	AFF_FARSIGHT,
	0,
	0,
	0,
	0,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    {
	"avian",		TRUE,
	0,
    AFF_FLYING|AFF_DETECT_INVIS|AFF_DETECT_HIDDEN|AFF_FARSIGHT|AFF_INFRARED,
	0,
	0,
	RES_LIGHTNING,
	VULN_DISEASE|VULN_POISON,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|P
    },

    {
	"gnome",		TRUE,
	0,
	AFF_INFRARED,
	0,
	0,
	RES_MENTAL,
	VULN_DROWNING,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    {
	"draconian",		TRUE,
	0,
	AFF_FLYING,
	0,
	IMM_POISON|IMM_DISEASE,
	RES_FIRE|RES_COLD,
	VULN_SLASH|VULN_PIERCE|VULN_LIGHTNING,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|P|Q
    },

    {
        "centaur",              TRUE,
        0,
	0,
	0,
        0,
	0,
	0,
        0,
        A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
    },

    {
        "gnoll",                TRUE,
        0,
	AFF_DETECT_HIDDEN|AFF_DARK_VISION,
	0,
        0,
	0,
	VULN_SILVER,
        0,
        A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|U|V
    },

    {
        "heucuva",              TRUE,
        0,
	AFF_DARK_VISION,
	0,
        IMM_NEGATIVE|IMM_POISON|IMM_DISEASE,
	RES_WEAPON,
	VULN_HOLY|VULN_LIGHT|VULN_SILVER,
        0,
        A|H|M|V,        A|B|C|G|H|I|J|K
    },

    {
        "kenku",                TRUE,
        0,
	AFF_FARSIGHT|AFF_DETECT_HIDDEN|AFF_INFRARED|AFF_FLYING,
        0,
        0,
	RES_COLD,
	0,
        0,
        A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|P
    },

    {
        "minotaur",             TRUE,
        0,
	AFF_FARSIGHT,
	0,
        IMM_POISON,
	0,
	VULN_BASH,
        0,
        A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|W
    },

    {
        "satyr",                TRUE,
        0,
	AFF_DETECT_HIDDEN|AFF_DETECT_EVIL|AFF_DETECT_GOOD,
        0,
	IMM_FIRE,
	RES_NEGATIVE,
	VULN_HOLY|VULN_LIGHT,
        0,
        A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
    },

    {
        "titan",                TRUE,
        0,
	AFF_DETECT_INVIS|AFF_BERSERK,
	0,
        IMM_CHARM,
	RES_WEAPON,
	0,
        0,
        A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
    },

    {
	"bat",			FALSE,
	0,
	AFF_FLYING|AFF_DARK_VISION,
	OFF_DODGE|OFF_FAST,
	0,
	0,
	VULN_LIGHT,
	0,
	A|G|V,		A|C|D|E|F|H|J|K|P
    },

    {
	"bear",			FALSE,
	0,
	0,
	OFF_CRUSH|OFF_DISARM|OFF_BERSERK,
	0,
	RES_BASH|RES_COLD,
	0,
	0,
	A|G|V,		A|B|C|D|E|F|H|J|K|U|V
    },

    {
	"cat",			FALSE,
	0,
	AFF_DARK_VISION,
	OFF_FAST|OFF_DODGE,
	0,
	0,
	0,
	0,
	A|G|V,		A|C|D|E|F|H|J|K|Q|U|V
    },

    {
	"centipede",		FALSE,
	0,
	AFF_DARK_VISION,
	0,
	0,
	RES_PIERCE|RES_COLD,
	VULN_BASH,
	0,
 	A|B|G|O,		A|C|K	
    },

    {
	"dog",			FALSE,
	0,
	0,
	OFF_FAST,
	0,
	0,
	0,
	0,
	A|G|V,		A|C|D|E|F|H|J|K|U|V
    },
	
    {
	"deer",			FALSE,
	0,
	0,
	OFF_FAST,
	0,
	0,
	0,
	0,
	A|G|V,		A|C|D|E|F|H|J|K|W
    },


    {
	"doll",			FALSE,
	0,
	0,
	0,
 IMM_COLD|IMM_POISON|IMM_HOLY|IMM_NEGATIVE|IMM_MENTAL|IMM_DISEASE|IMM_DROWNING,
	RES_BASH|RES_LIGHT,
	VULN_SLASH|VULN_FIRE|VULN_ACID|VULN_LIGHTNING|VULN_ENERGY,
	0,
	E|J|M|cc,	A|B|C|G|H|K
    },

    { 	"dragon", 		FALSE, 
	0,
	AFF_INFRARED|AFF_FLYING,
	0,
	0,
	RES_FIRE|RES_BASH|RES_CHARM, 
	VULN_PIERCE|VULN_COLD,
	0,
	A|H|Z,		A|C|D|E|F|G|H|I|J|K|P|Q|U|V|X
    },

    {
	"fido",			FALSE,
	0,
	0,
	OFF_DODGE|ASSIST_RACE,
	0,
	0,
	VULN_MAGIC,
	0,
	A|B|G|V,	A|C|D|E|F|H|J|K|Q|V
    },		
   
    {
	"fox",			FALSE,
	0,
	AFF_DARK_VISION,
	OFF_FAST|OFF_DODGE,
	0,
	0,
	0,
	0,
	A|G|V,		A|C|D|E|F|H|J|K|Q|V
    },

    {
	"hobgoblin",		FALSE,
	0,
	AFF_INFRARED,
	0,
	0,
	RES_DISEASE|RES_POISON,
	0,
	0,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|Y
    },

    {
	"kobold",		FALSE,
	0,
	AFF_INFRARED,
	0,
	0,
	RES_POISON,
	VULN_MAGIC,
	0,
	A|B|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Q
    },

    {
	"lizard",		FALSE,
	0,
	0,
	0,
	0,
	RES_POISON,
	VULN_COLD,
	0,
	A|G|X|cc,	A|C|D|E|F|H|K|Q|V
    },

    {
	"modron",		FALSE,
	0,
	AFF_INFRARED,
	ASSIST_RACE|ASSIST_ALIGN,
	IMM_CHARM|IMM_DISEASE|IMM_MENTAL|IMM_HOLY|IMM_NEGATIVE,
	RES_FIRE|RES_COLD|RES_ACID,
	0,
	0,
	H,		A|B|C|G|H|J|K
    },

    {
	"orc",			FALSE,
	0,
	AFF_INFRARED,
	0,
	0,
	RES_DISEASE,
	VULN_LIGHT,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },
	
    {
	"bovine",			FALSE,
	0,
	0,
	OFF_DODGE,
	0,
	RES_DISEASE,
	0,
	0,
	A|G|V,	 	A|C|D|E|F|H|J|K
    },

    {
	"pig",			FALSE,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	A|G|V,	 	A|C|D|E|F|H|J|K
    },	

    {
	"rabbit",		FALSE,
	0,
	0,
	OFF_DODGE|OFF_FAST,
	0,
	0,
	0,
	0,
	A|G|V,		A|C|D|E|F|H|J|K
    },
    
    {
	"rodent",		FALSE,
	0,
	0,
	OFF_DODGE|OFF_FAST,
	0,
	0,
	0,
	0,
	A|G|V,		A|C|D|E|F|H|J|K
    },

    {
	"school monster",	FALSE,
	ACT_NOALIGN,
	0,
	0,
	IMM_CHARM|IMM_SUMMON,
	0,
	VULN_MAGIC,
	0,
	A|M|V,		A|B|C|D|E|F|H|J|K|Q|U
    },	

    {
	"snake",		FALSE,
	0,
	0,
	0,
	0,
	RES_POISON,
	VULN_COLD,
	0,
	A|G|X|Y|cc,	A|D|E|F|K|L|Q|V|X
    },
 
    {
	"song bird",		FALSE,
	0,
	AFF_FLYING,
	OFF_FAST|OFF_DODGE,
	0,
	0,
	0,
	0,
	A|G|W,		A|C|D|E|F|H|K|P
    },

    {
	"troll",		FALSE,
	0,
	AFF_REGENERATION|AFF_INFRARED|AFF_DETECT_HIDDEN,
	OFF_BERSERK,
 	0,
	RES_CHARM|RES_BASH,
	VULN_FIRE|VULN_ACID,
	0,
	A|B|H|M|V,		A|B|C|D|E|F|G|H|I|J|K|U|V
    },

    {
	"water fowl",		FALSE,
	0,
	AFF_SWIM|AFF_FLYING,
	0,
	0,
	RES_DROWNING,
	0,
	0,
	A|G|W,		A|C|D|E|F|H|K|P
    },		
  
    {
	"wolf",			FALSE,
	0,
	AFF_DARK_VISION,
	OFF_FAST|OFF_DODGE,
	0,
	0,
	0,	
	0,
	A|G|V,		A|C|D|E|F|J|K|Q|V
    },

    {
	"wyvern",		FALSE,
	0,
	AFF_FLYING|AFF_DETECT_INVIS|AFF_DETECT_HIDDEN,
	OFF_BASH|OFF_FAST|OFF_DODGE,
	IMM_POISON,
	0,
	VULN_LIGHT,
	0,
	A|B|G|Z,		A|C|D|E|F|H|J|K|Q|V|X
    },

    {
	NULL, 0, 0, 0, 0, 0, 0
    }
};

struct	pc_race_type	pc_race_table	[]	=
{
    {
	"null race",	"",
	0,
	{ 100, 100, 100, 100, 100, 100, 100,
	  100, 100, 100, 100, 100, 100, 100 },
	{ "" },
	{ 13, 13, 13, 13, 13 },
	{ 18, 18, 18, 18, 18 },
	0,
	0
    },
 
/*
    {
	"race name", 	short name,
	points,
	{ class multipliers },
	{ bonus skills },
	{ base stats },
	{ max stats },
	size,
	tiers
    },
*/
    {
	"human",	"Human",
	0,
	{ 100, 100, 100, 100, 100, 100, 100,
	   90,  90,  90,  90,  90,  90,  90,
	   80,  80,  80,  80,  80,  80,  80
	},
	{ "" },
	{ 13, 13, 13, 13, 13 },
	{ 18, 18, 18, 18, 18 },
	SIZE_MEDIUM,
	A|B|C
    },

    { 	
	"elf",		"Elf",
	5,
	{ 100, 125,  100, 120, 120, 105, 115, 
	   90, 113,   90, 108,  95,  95, 104,
	   80,  80,  80,  80,  80,  80,  80 }, 
	{ "sneak", "hide" },
	{ 12, 14, 13, 15, 11 },
	{ 16, 20, 18, 21, 15 },
	SIZE_SMALL,
	A|B|C
    },

    {
	"dwarf",	"Dwarf",
	8,
	{ 150, 100, 125, 100, 110, 110, 110,
	  135, 90, 113, 90, 113, 99, 99,
	   80,  80,  80,  80,  80,  80,  80 },
	{ "berserk" },
	{ 14, 12, 13, 11, 15 },
	{ 20, 16, 19, 15, 21 },
	SIZE_MEDIUM,
	A|B|C
    },

    {
	"giant",	"Giant",
	6,
	{ 200, 125, 150, 105, 125, 150, 120,
	  180, 113, 135, 95, 135, 135, 108,
	   80,  80,  80,  80,  80,  80,  80 },
	{ "bash", "fast healing" },
	{ 16, 11, 13, 11, 14 },
	{ 22, 15, 18, 15, 20 },
	SIZE_LARGE,
	A|B|C
    },

    {
	"pixie",	"Pixie",
	6,
	{ 100, 100, 120, 200, 150, 100, 150,
	  90, 90, 108, 180, 95, 90, 135,
	   80,  80,  80,  80,  80,  80,  80 },
	{ "" },
	{ 10, 15, 15, 15, 10 },
	{ 14, 21, 21, 20, 14 },
	SIZE_TINY,
	A|B|C
    },

    {
	"halfling",	"Hflng",
	5,
	{ 105, 120, 100, 150, 150, 120, 120,
	  95, 108, 90, 135, 108, 108, 108,
	   80,  80,  80,  80,  80,  80,  80 },
	{ "sneak", "hide" },
	{ 11, 14, 12, 15, 13 },
	{ 15, 20, 16, 21, 18 },
	SIZE_SMALL,
	A|B|C
    },

    {
	"halforc",	"HfOrc",
	6,
	{ 200, 200, 120, 100, 125, 150, 105,
	  180, 180, 108, 90, 135, 135, 95,
	   80,  80,  80,  80,  80,  80,  80 },
	{ "fast healing" },
	{ 14, 11, 11, 14, 15 },
	{ 19, 15, 15, 20, 21 },
	SIZE_MEDIUM,
	A|B|C
    },

    {
	"goblin",	"Gobln",
	5,
	{ 105, 125, 110, 125, 120, 120, 110,
	  95, 113, 99, 113, 99, 108, 99,
	   80,  80,  80,  80,  80,  80,  80 },
	{ "sneak", "hide" },
	{ 11, 14, 12, 15, 14 },
	{ 16, 20, 16, 19, 20 },
	SIZE_SMALL,
	A|B|C
    },

    {
	"halfelf",	"HfElf",
	2,
	{ 105, 105, 105, 105, 105, 105, 105,
	  95, 95, 95, 95, 95, 95, 95,
	   80,  80,  80,  80,  80,  80,  80 },
	{ "" },
	{ 12, 13, 14, 13, 13 },
	{ 17, 18, 19, 18, 18 },
	SIZE_MEDIUM,
	A|B|C
    },

    {
	"avian",	"Avian",
	5,
	{ 110, 105, 150, 125, 120, 100, 120,
	  99, 95, 135, 113, 108, 90, 108,
	   80,  80,  80,  80,  80,  80,  80 },
	{ "" },
	{ 12, 14, 15, 11, 12 },
	{ 17, 19, 20, 16, 17 },
	SIZE_LARGE,
	A|B|C
    },

    {
	"gnome",	"Gnome",
	4,
	{ 100, 110, 150, 150, 125, 105, 150,
	  90, 99, 135, 135, 99, 95, 135,
	   80,  80,  80,  80,  80,  80,  80 },
	{ "" },
	{ 11, 15, 14, 12, 12 },
	{ 16, 20, 19, 15, 15 },
	SIZE_SMALL,
	A|B|C
    },

    {
	"draconian",	"Dracn",
	11,
	{ 125, 150, 200, 100, 110, 125, 150,
	  113, 135, 180, 90, 108, 113, 135,
	   80,  80,  80,  80,  80,  80,  80 },
	{ "fast healing" },
	{ 16, 13, 12, 11, 15 },
	{ 22, 18, 16, 15, 21 },
	SIZE_HUGE,
	A|B|C
    },

    {
	"centaur",	"Centr",
	9,
	{ 100, 110, 100, 175, 110, 110, 95,
	  90, 100, 90, 165, 100, 100, 85,
	   80,  80,  80,  80,  80,  80,  80 },
	{ "enhanced damage" },
	{ 15, 12, 10, 8, 16 },
	{ 20, 17, 15, 13, 21 },
	SIZE_LARGE,
	A|B|C
    },

    {
	"gnoll",	"Gnoll",
	7,
	{ 110, 110, 125, 110, 175, 110, 110,
	  100, 100, 115, 100, 165, 100, 100,
	   80,  80,  80,  80,  80,  80,  80 },
	{ "" },
	{ 15, 11, 10, 16, 15 },
	{ 20, 16, 15, 20, 19 },
	SIZE_LARGE,
	A|B|C
    },

    {
	"heucuva",	"Heucv",
	10,
	{ 110, 110, 110, 100, 110, 110, 100,
          100, 100, 100, 90, 100, 100, 90,
	   80,  80,  80,  80,  80,  80,  80 },
	{ "second attack" },
	{ 20,  5,  5, 20, 20 },
	{ 25, 10, 10, 25, 25 },
	SIZE_MEDIUM,
	A|B|C
    },

    {
	"kenku",	"Kenku",
	5,
	{ 125, 110, 150, 150, 110, 125, 180,
	  115, 100, 140, 140, 100, 115, 170,
	   80,  80,  80,  80,  80,  80,  80 },
	{ "meditation" },
	{ 14, 14, 16, 15, 14 },
	{ 19, 19, 21, 20, 19 },
	SIZE_MEDIUM,
	A|B|C
    },

    {
	"minotaur",	"Mintr",
	7,
	{ 110, 110, 110, 95, 110, 110, 110,
	  100, 100, 100, 85, 100, 100, 100,
	   80,  80,  80,  80,  80,  80,  80 },
	{ "enhanced damage" },
	{ 18, 11, 10, 11, 17 },
	{ 23, 16, 15, 16, 22 },
	SIZE_LARGE,
	A|B|C
    },

    {
	"satyr",	"Satyr",
	6,
	{ 110, 110, 110, 175, 110, 110, 150,
	  100, 100, 100, 165, 100, 100, 140,
	   80,  80,  80,  80,  80,  80,  80 },
	{ "" },
	{ 18, 14,  5, 9, 16 },
	{ 23, 19, 10, 14, 21 },
	SIZE_LARGE,
	A|B|C
    },

    {
	"titan",	"Titan",
	11,
	{ 180, 105, 130, 105, 105, 130, 100,
	  170, 93, 115, 95, 95, 115, 98,
	   80,  80,  80,  80,  80,  80,  80 },
	{ "fast healing" },
	{ 20, 13, 13, 10, 20 },
	{ 25, 18, 18, 15, 25 },
	SIZE_GIANT,
	A|B|C
    }
};

	
      	

/*
 * Class table.
 */
struct	class_type	class_table	[MAX_CLASS]	=
{

/*
    {
	name, who, attr_prime, weapon,
	{guild1, guild2, guild3}, 75, thac0_00, thac0_32, hp_min, hp_max, mana?,
	base_group, default_group,
	tier,
	nexttier
    }
*/

	/* Tier 1 */

    {	"mage"    },
    {	"cleric"    },
    {	"thief"    },
    {	"warrior"    },
    {	"ranger"    },
    {	"druid"    },
    {	"vampire"    },
	
	/* Tier 2 */

    {	"wizard"    },
    {	"priest"    },
    {	"mercenary"    },
    {	"gladiator"    },
    {	"strider"    },
    {	"sage"    },
    {	"lich"    },

	/* Tier 3 */

    {	"warlock"    },
    {	"bishop"    },
    {	"assassin"    },
    {	"swordsman"    },
    {	"wanderer"    },
    {	"forestling"    },
    {	"necromancer"    }
};


/*
 * Attribute bonus tables.
 */
const	struct	str_app_type	str_app		[26]		=
{
	/*{tohit, todam, carry, wield},*/

    { -5, -4,   0,  0 },  /* 0  */
    { -5, -4,   3,  1 },  /* 1  */
    { -3, -2,   3,  2 },
    { -3, -1,  10,  3 },  /* 3  */
    { -2, -1,  25,  4 },
    { -2, -1,  55,  5 },  /* 5  */
    { -1,  0,  80,  6 },
    { -1,  0,  90,  7 },
    {  0,  0, 100,  8 },
    {  0,  0, 100,  9 },
    {  0,  0, 115, 10 }, /* 10  */
    {  0,  0, 115, 11 },
    {  0,  0, 130, 12 },
    {  0,  0, 130, 13 }, /* 13  */
    {  0,  1, 140, 14 },
    {  1,  1, 150, 15 }, /* 15  */
    {  1,  2, 165, 16 },
    {  2,  3, 180, 22 },
    {  2,  3, 200, 25 }, /* 18  */
    {  3,  4, 225, 30 },
    {  3,  5, 250, 35 }, /* 20  */
    {  4,  6, 300, 40 },
    {  4,  6, 350, 45 },
    {  5,  7, 400, 50 },
    {  5,  8, 450, 55 },
    {  6,  9, 500, 60 }  /* 25   */
};



const	struct	int_app_type	int_app		[26]		=
{
	/*
	{ learn },
	*/
    {  3 },	/*  0 */
    {  5 },	/*  1 */
    {  7 },
    {  8 },	/*  3 */
    {  9 },
    { 10 },	/*  5 */
    { 11 },
    { 12 },
    { 13 },
    { 15 },
    { 17 },	/* 10 */
    { 19 },
    { 22 },
    { 25 },
    { 28 },
    { 31 },	/* 15 */
    { 34 },
    { 37 },
    { 40 },	/* 18 */
    { 44 },
    { 49 },	/* 20 */
    { 55 },
    { 60 },
    { 70 },
    { 80 },
    { 85 }	/* 25 */
};



const	struct	wis_app_type	wis_app		[26]		=
{
	/*
	{ practice },
	*/
    { 0 },	/*  0 */
    { 0 },	/*  1 */
    { 0 },
    { 0 },	/*  3 */
    { 0 },
    { 1 },	/*  5 */
    { 1 },
    { 1 },
    { 1 },
    { 1 },
    { 1 },	/* 10 */
    { 1 },
    { 1 },
    { 1 },
    { 1 },
    { 2 },	/* 15 */
    { 2 },
    { 2 },
    { 3 },	/* 18 */
    { 3 },
    { 3 },	/* 20 */
    { 3 },
    { 4 },
    { 4 },
    { 4 },
    { 5 }	/* 25 */
};



const	struct	dex_app_type	dex_app		[26]		=
{
	/*
	{ defensive },
	*/
    {   60 },   /* 0 */
    {   50 },   /* 1 */
    {   50 },
    {   40 },
    {   30 },
    {   20 },   /* 5 */
    {   10 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    { - 10 },   /* 15 */
    { - 15 },
    { - 20 },
    { - 30 },
    { - 40 },
    { - 50 },   /* 20 */
    { - 60 },
    { - 75 },
    { - 90 },
    { -105 },
    { -120 }    /* 25 */
};


const	struct	con_app_type	con_app		[26]		=
{
	/*
	{ hp, shock },
	*/
    { -4, 20 },   /*  0 */
    { -3, 25 },   /*  1 */
    { -2, 30 },
    { -2, 35 },	  /*  3 */
    { -1, 40 },
    { -1, 45 },   /*  5 */
    { -1, 50 },
    {  0, 55 },
    {  0, 60 },
    {  0, 65 },
    {  0, 70 },   /* 10 */
    {  0, 75 },
    {  0, 80 },
    {  0, 85 },
    {  0, 88 },
    {  1, 90 },   /* 15 */
    {  2, 95 },
    {  2, 97 },
    {  3, 99 },   /* 18 */
    {  3, 99 },
    {  4, 99 },   /* 20 */
    {  4, 99 },
    {  5, 99 },
    {  6, 99 },
    {  7, 99 },
    {  8, 99 }    /* 25 */
};



/*
 * Liquid properties.
 * Used in world.obj.
 * Be sure MAX_LIQUID in merc.h matches
 * the number of entries below.
 */
const	struct	liq_type	liq_table	[]	=
{
/*    name			color	 { proof, full, thirst, food, ssize } */
    { "water",			"clear",	{   0, 1, 10, 0, 16 }	},
    { "beer",			"amber",	{  12, 1,  8, 1, 12 }	},
    { "red wine",		"burgundy",	{  30, 1,  8, 1,  5 }	},
    { "ale",			"brown",	{  15, 1,  8, 1, 12 }	},
    { "dark ale",		"dark",		{  16, 1,  8, 1, 12 }	},

    { "whisky",			"golden",	{ 120, 1,  5, 0,  2 }	},
    { "lemonade",		"pink",		{   0, 1,  9, 2, 12 }	},
    { "firebreather",		"boiling",	{ 190, 0,  4, 0,  2 }	},
    { "local specialty",	"clear",	{ 151, 1,  3, 0,  2 }	},
    { "slime mold juice",	"green",	{   0, 2, -8, 1,  2 }	},

    { "milk",			"white",	{   0, 2,  9, 3, 12 }	},
    { "tea",			"tan",		{   0, 1,  8, 0,  6 }	},
    { "coffee",			"black",	{   0, 1,  8, 0,  6 }	},
    { "blood",			"red",		{   0, 2, -1, 2,  6 }	},
    { "salt water",		"clear",	{   0, 1, -2, 0,  1 }	},

    { "coke",			"brown",	{   0, 2,  9, 2, 12 }	}, 
    { "root beer",		"brown",	{   0, 2,  9, 2, 12 }   },
    { "elvish wine",		"green",	{  35, 2,  8, 1,  5 }   },
    { "white wine",		"golden",	{  28, 1,  8, 1,  5 }   },
    { "champagne",		"golden",	{  32, 1,  8, 1,  5 }   },

    { "mead",			"honey-colored",{  34, 2,  8, 2, 12 }   },
    { "rose wine",		"pink",		{  26, 1,  8, 1,  5 }	},
    { "benedictine wine",	"burgundy",	{  40, 1,  8, 1,  5 }   },
    { "vodka",			"clear",	{ 130, 1,  5, 0,  2 }   },
    { "cranberry juice",	"red",		{   0, 1,  9, 2, 12 }	},

    { "orange juice",		"orange",	{   0, 2,  9, 3, 12 }   }, 
    { "absinthe",		"green",	{ 200, 1,  4, 0,  2 }	},
    { "brandy",			"golden",	{  80, 1,  5, 0,  4 }	},
    { "aquavit",		"clear",	{ 140, 1,  5, 0,  2 }	},
    { "schnapps",		"clear",	{  90, 1,  5, 0,  2 }   },

    { "icewine",		"purple",	{  50, 2,  6, 1,  5 }	},
    { "amontillado",		"burgundy",	{  35, 2,  8, 1,  5 }	},
    { "sherry",			"red",		{  38, 2,  7, 1,  5 }   },	
    { "framboise",		"red",		{  50, 1,  7, 1,  5 }   },
    { "rum",			"amber",	{ 151, 1,  4, 0,  2 }	},

    { "cordial",		"clear",	{ 100, 1,  5, 0,  2 }   },
    { "dr pepper",		"brown",	{   0, 2,  9, 2, 12 }	},
    { NULL,			NULL,		{   0, 0,  0, 0,  0 }	}
};



/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n)	n

struct	skill_type	skill_table	[MAX_SKILL]	=
{

/*
 * Magic spells.
 */

/*
    {
	"NAME",	{0}, {0},
	SPELL_FUN,		TARGET,		MIN_POSITION,
	GSN,		SOCKET,	SLOT(#),	MIN_MANA,	BEATS,
	"DAMAGE NOUN",	"OFF MESSAGE",	"OFF MESSAGE (OBJ)"
    }

	NAME		Name of Spell/Skill
	SKILL_LEVELS	Levels to obtain spell/skill per class
	DIFFICULTY	Spells: multiplier for base mana
			Skills: charge for gaining skill
	SPELL_FUN	Routine to call for spells
	TARGET		Legal targets
	MIN_POSTITION	Position for caster/user
	GSN		gsn for skills and some spells
	SOCKET		Can spell be cast on person from same socket
			(to limit multiplaying)
	SLOT(#)		A unique slot number for spells
	MIN_MANA	Base mana for spells (multiplied by DIFFICULTY)
	BEATS		Waiting time after use
	DAMAGE NOUN	Damage Message
	OFF MESSAGE	Wear off message
	OFF MESSAGE (OBJ)	Wear off message for objects
*/

    {
	"reserved",	{0}, {0},
	0,			TAR_IGNORE,	POS_STANDING,
	NULL,		TRUE,	SLOT( 0),	0,	0,
	"",		"",		""
	},

    {
	"acid blast",	{0}, {0},
	spell_acid_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(70),	20,	12,
	"acid blast",	"!Acid Blast!",		""
    },

    {
	"animate",	{0}, {0},
	spell_animate,		TAR_OBJ_INV,		POS_STANDING,
	NULL,		TRUE,	SLOT(239),	 20,	12,
	"",			"!Animate!",	""
    },

    {
	"armor",	{0}, {0},
	spell_armor,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT( 1),	 5,	12,
	"",			"You feel less armored.",	""
    },

    {
	"bless",	{0}, {0},
	spell_bless,		TAR_OBJ_CHAR_DEF,	POS_STANDING,
	NULL,		FALSE,	SLOT( 3),	 5,	12,
	"",			"You feel less righteous.", 
	"$p's holy aura fades."
    },

    {
	"blindness",	{0}, {0},
	spell_blindness,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_blindness,	TRUE,	SLOT( 4),	 5,	12,
	"",			"You can see again.",	""
    },

    {
	"burning hands",	{0}, {0},
	spell_burning_hands,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT( 5),	15,	12,
	"burning hands",	"!Burning Hands!", 	""
    },

    {
	"call lightning",	{0}, {0},
	spell_call_lightning,	TAR_IGNORE,		POS_FIGHTING,
	NULL,		TRUE,	SLOT( 6),	15,	12,
	"lightning bolt",	"!Call Lightning!",	""
    },

    {
	"calm",	{0}, {0},
	spell_calm,		TAR_IGNORE,		POS_FIGHTING,
	NULL,		TRUE,	SLOT(509),	30,	12,
	"",			"You have lost your peace of mind.",	""
    },

    {
	"cancellation",	{0}, {0},
	spell_cancellation,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,		FALSE,	SLOT(507),	20,	12,
	""			"!cancellation!",	""
    },

    {
	"cause critical",	{0}, {0},
	spell_cause_critical,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(63),	20,	12,
	"spell",		"!Cause Critical!",	""
    },

    {
	"cause light",	{0}, {0},
	spell_cause_light,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(62),	15,	12,
	"spell",		"!Cause Light!",	""
    },

    {
	"cause serious",	{0}, {0},
	spell_cause_serious,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(64),	17,	12,
	"spell",		"!Cause Serious!",	""
    },

    {   
	"chain lightning",	{0}, {0},
	spell_chain_lightning,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(500),	25,	12,
	"lightning",		"!Chain Lightning!",	""
    }, 

    {
	"change sex",	{0}, {0},
	spell_change_sex,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(82),	15,	12,
	"",			"Your body feels familiar again.",	""
    },

    {
	"charm person",	{0}, {0},
	spell_charm_person,	TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_charm_person,	TRUE,	SLOT( 7),	 5,	12,
	"",			"You feel more self-confident.",	""
    },

    {
	"chill touch",	{0}, {0},
	spell_chill_touch,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT( 8),	15,	12,
	"chilling touch",	"You feel less cold.",	""
    },

    {
	"colour spray",	{0}, {0},
	spell_colour_spray,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(10),	15,	12,
	"colour spray",		"!Colour Spray!",	""
    },

    { 
        "conjure",	{0}, {0},
        spell_conjure,        TAR_IGNORE,             POS_STANDING,
        NULL,          TRUE,         SLOT(280),       100,    12,
        "",                     "!Conjure!",  ""
    },

    {
	"continual light",	{0}, {0},
	spell_continual_light,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(57),	 7,	12,
	"",			"!Continual Light!",	""
    },

    {
	"control weather",	{0}, {0},
	spell_control_weather,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(11),	25,	12,
	"",			"!Control Weather!",	""
    },

    {
	"create food",	{0}, {0},
	spell_create_food,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(12),	 5,	12,
	"",			"!Create Food!",	""
    },

    {
	"create rose",	{0}, {0},
	spell_create_rose,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(511),	30, 	12,
	"",			"!Create Rose!",	""
    },  

    {
	"create spring",	{0}, {0},
	spell_create_spring,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(80),	20,	12,
	"",			"!Create Spring!",	""
    },

    {
	"create water",	{0}, {0},
	spell_create_water,	TAR_OBJ_INV,		POS_STANDING,
	NULL,		TRUE,	SLOT(13),	 5,	12,
	"",			"!Create Water!",	""
    },

    {
	"cure blindness",	{0}, {0},
	spell_cure_blindness,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,		FALSE,	SLOT(14),	 5,	12,
	"",			"!Cure Blindness!",	""
    },

    {
	"cure critical",	{0}, {0},
	spell_cure_critical,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,		FALSE,	SLOT(15),	60,	12,
	"",			"!Cure Critical!",	""
    },

    {
	"cure disease",	{0}, {0},
	spell_cure_disease,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(501),	20,	12,
	"",			"!Cure Disease!",	""
    },

    {
	"cure light",	{0}, {0},
	spell_cure_light,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,		FALSE,	SLOT(16),	20,	12,
	"",			"!Cure Light!",		""
    },

    {
	"cure poison",	{0}, {0},
	spell_cure_poison,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(43),	 5,	12,
	"",			"!Cure Poison!",	""
    },

    {
	"cure serious",	{0}, {0},
	spell_cure_serious,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,		FALSE,	SLOT(61),	40,	12,
	"",			"!Cure Serious!",	""
    },

    {
	"curse",	{0}, {0},
	spell_curse,		TAR_OBJ_CHAR_OFF,	POS_FIGHTING,
	&gsn_curse,	TRUE,	SLOT(17),	20,	12,
	"curse",		"The curse wears off.", 
	"$p is no longer impure."
    },

    {
	"demonfire",	{0}, {0},
	spell_demonfire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(505),	20,	12,
	"torments",		"!Demonfire!",		""
    },	

    {
	"detect evil",	{0}, {0},
	spell_detect_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,		FALSE,	SLOT(18),	 5,	12,
	"",			"The red in your vision disappears.",	""
    },

    {
        "detect good",	{0}, {0},
        spell_detect_good,      TAR_CHAR_SELF,          POS_STANDING,
        NULL,		FALSE,	SLOT(513),        5,     12,
        "",                     "The gold in your vision disappears.",	""
    },

    {
	"detect hidden",	{0}, {0},
	spell_detect_hidden,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,		FALSE,	SLOT(44),	 5,	12,
	"",			"You feel less aware of your surroundings.",	
	""
    },

    {
	"detect invis",	{0}, {0},
	spell_detect_invis,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,		FALSE,	SLOT(19),	 5,	12,
	"",			"You no longer see invisible objects.",
	""
    },

    {
	"detect magic",	{0}, {0},
	spell_detect_magic,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,		FALSE,	SLOT(20),	 5,	12,
	"",			"The detect magic wears off.",	""
    },

    {
	"detect poison",	{0}, {0},
	spell_detect_poison,	TAR_OBJ_INV,		POS_STANDING,
	NULL,		TRUE,	SLOT(21),	 5,	12,
	"",			"!Detect Poison!",	""
    },

    {
	"dispel evil",	{0}, {0},
	spell_dispel_evil,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(22),	15,	12,
	"dispel evil",		"!Dispel Evil!",	""
    },

    {
        "dispel good",	{0}, {0},
        spell_dispel_good,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,		TRUE,	SLOT(512),      15,     12,
        "dispel good",          "!Dispel Good!",	""
    },

    {
	"dispel magic",	{0}, {0},
	spell_dispel_magic,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(59),	15,	12,
	"",			"!Dispel Magic!",	""
    },

    {
	"earthquake",	{0}, {0},
	spell_earthquake,	TAR_IGNORE,		POS_FIGHTING,
	NULL,		TRUE,	SLOT(23),	15,	12,
	"earthquake",		"!Earthquake!",		""
    },

    {
	"empower",	{0}, {0},
	spell_empower,		TAR_IGNORE,	POS_RESTING,
	NULL,		TRUE,	SLOT(234),	 40,	12,
	"",			"!Empower!",	""
    },

    {
	"enchant armor",	{0}, {0},
	spell_enchant_armor,	TAR_OBJ_INV,		POS_STANDING,
	NULL,		TRUE,	SLOT(510),	100,	24,
	"",			"!Enchant Armor!",	""
    },

    {
	"enchant weapon",	{0}, {0},
	spell_enchant_weapon,	TAR_OBJ_INV,		POS_STANDING,
	NULL,		TRUE,	SLOT(24),	100,	24,
	"",			"!Enchant Weapon!",	""
    },

    {
	"energy drain",	{0}, {0},
	spell_energy_drain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(25),	35,	12,
	"energy drain",		"!Energy Drain!",	""
    },

    {
	"faerie fire",	{0}, {0},
	spell_faerie_fire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(72),	 5,	12,
	"faerie fire",		"The pink aura around you fades away.",
	""
    },

    {
	"faerie fog",	{0}, {0},
	spell_faerie_fog,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(73),	12,	12,
	"faerie fog",		"!Faerie Fog!",		""
    },

    {
	"farsight",	{0}, {0},
	spell_farsight,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(521),	5,	12,
	"",			"The green in your vision disappears.",
	""
    },	

    {
	"fireball",	{0}, {0},
	spell_fireball,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(26),	15,	12,
	"fireball",		"!Fireball!",		""
    },
  
    {
	"fireproof",	{0}, {0},
	spell_fireproof,	TAR_OBJ_INV,		POS_STANDING,
	NULL,		TRUE,	SLOT(523),	10,	12,
	"",			"",	"$p's protective aura fades."
    },

    {
	"fireshield",	{0}, {0},
	spell_fireshield,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(411),	75,	12,
	"fireball",		"Your firey shield gutters out.",
	""
    },

    {
	"flamestrike",	{0}, {0},
	spell_flamestrike,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(65),	20,	12,
	"flamestrike",		"!Flamestrike!",		""
    },

    {
	"floating disc",	{0}, {0},
	spell_floating_disc,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(522),	40,	24,
	"",			"!Floating disc!",	""
    },

    {
	"fly",	{0}, {0},
	spell_fly,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(56),	10,	18,
	"",			"You slowly float to the ground.",	""
    },

    {
        "frenzy",	{0}, {0},
        spell_frenzy,           TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,		FALSE,	SLOT(504),      30,     24,
        "",                     "Your rage ebbs.",	""
    },

    {
	"gate",	{0}, {0},
	spell_gate,		TAR_IGNORE,		POS_FIGHTING,
	NULL,		FALSE,	SLOT(83),	80,	12,
	"",			"!Gate!",		""
    },

    {
	"giant strength",	{0}, {0},
	spell_giant_strength,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(39),	20,	12,
	"",			"You feel weaker.",	""
    },

    {
	"harm",	{0}, {0},
	spell_harm,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(27),	35,	12,
	"harm spell",		"!Harm!,		"""
    },
  
    {
	"haste",	{0}, {0},
	spell_haste,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,		FALSE,	SLOT(502),	30,	12,
	"",			"You feel yourself slow down.",	""
    },

    {
	"heal",	{0}, {0},
	spell_heal,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,		FALSE,	SLOT(28),	100,	12,
	"",			"!Heal!",		""
    },
  
    {
	"heat metal",	{0}, {0},
	spell_heat_metal,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(516), 	25,	18,
	"spell",		"!Heat Metal!",		""
    },

    {
	"holy word",	{0}, {0},
	spell_holy_word,	TAR_IGNORE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(506), 	200,	24,
	"divine wrath",		"!Holy Word!",		""
    },

    {
	"iceshield",	{0}, {0},
	spell_iceshield,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(410),	75,	12,
	"chilling touch",	"Your icy shield slowly melts away.",
	""
    },

    {
	"identify",	{0}, {0},
	spell_identify,		TAR_OBJ_INV,		POS_STANDING,
	NULL,		TRUE,	SLOT(53),	12,	24,
	"",			"!Identify!",		""
    },

    {
	"infravision",	{0}, {0},
	spell_infravision,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(77),	 5,	18,
	"",			"You no longer see in the dark.",	""
    },

    {
	"invisibility",	{0}, {0},
	spell_invis,		TAR_OBJ_CHAR_DEF,	POS_STANDING,
	&gsn_invis,	FALSE,	SLOT(29),	 5,	12,
	"",			"You are no longer invisible.",		
	"$p fades into view."
    },

    {
	"know alignment",	{0}, {0},
	spell_know_alignment,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,		FALSE,	SLOT(58),	 9,	12,
	"",			"!Know Alignment!",	""
    },

    {
	"lightning bolt",	{0}, {0},
	spell_lightning_bolt,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(30),	15,	12,
	"lightning bolt",	"!Lightning Bolt!",	""
    },

    {
	"locate object",	{0}, {0},
	spell_locate_object,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(31),	20,	18,
	"",			"!Locate Object!",	""
    },

    {
	"magic missile",	{0}, {0},
	spell_magic_missile,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(32),	15,	12,
	"magic missile",	"!Magic Missile!",	""
    },

    {
	"mass healing",	{0}, {0},
	spell_mass_healing,	TAR_IGNORE,		POS_STANDING,
	NULL,		FALSE,	SLOT(508),	100,	36,
	"",			"!Mass Healing!",	""
    },

    {
	"mass invis",	{0}, {0},
	spell_mass_invis,	TAR_IGNORE,		POS_STANDING,
	&gsn_mass_invis,	FALSE,	SLOT(69),	20,	24,
	"",			"You are no longer invisible.",		""
    },

    {
        "nexus",	{0}, {0},
        spell_nexus,            TAR_IGNORE,             POS_STANDING,
        NULL,		FALSE,	SLOT(520),       150,   36,
        "",                     "!Nexus!",		""
    },

    {
	"pass door",	{0}, {0},
	spell_pass_door,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,		FALSE,	SLOT(74),	20,	12,
	"",			"You feel solid again.",	""
    },

    {
	"plague",	{0}, {0},
	spell_plague,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_plague,	TRUE,	SLOT(503),	20,	12,
	"sickness",		"Your sores vanish.",	""
    },

    {
	"poison",	{0}, {0},
	spell_poison,		TAR_OBJ_CHAR_OFF,	POS_FIGHTING,
	&gsn_poison,	TRUE,	SLOT(33),	10,	12,
	"poison",		"You feel less sick.",	
	"The poison on $p dries up."
    },

    {
        "portal",	{0}, {0},
        spell_portal,           TAR_IGNORE,             POS_STANDING,
        NULL,		FALSE,	SLOT(519),       100,     24,
        "",                     "!Portal!",		""
    },

    {
	"protection evil",	{0}, {0},
	spell_protection_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,		FALSE,	SLOT(34), 	5,	12,
	"",			"You feel less protected.",	""
    },

    {
        "protection good",	{0}, {0},
        spell_protection_good,  TAR_CHAR_SELF,          POS_STANDING,
        NULL,		FALSE,	SLOT(514),	5,	12,
        "",                     "You feel less protected.",	""
    },

    {
	"protection voodoo",	{0}, {0},
	spell_protection_voodoo, TAR_CHAR_SELF,		POS_STANDING,
	NULL,		FALSE,	SLOT(345),	5,	12,
	"",			"",	""
    },

    {
	"quest pill",	{0}, {0},
	spell_quest_pill,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,		TRUE,	SLOT(530),	 5,	18,
	"",			"!Quest Pill!",	""
    },

    {
        "ray of truth",	{0}, {0},
        spell_ray_of_truth,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,		TRUE,	SLOT(518),      20,     12,
        "ray of truth",         "!Ray of Truth!",	""
    },

    {
	"recharge",	{0}, {0},
	spell_recharge,		TAR_OBJ_INV,		POS_STANDING,
	NULL,		TRUE,	SLOT(517),	60,	24,
	"",			"!Recharge!",		""
    },

    {
	"refresh",	{0}, {0},
	spell_refresh,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(81),	50,	18,
	"refresh",		"!Refresh!",		""
    },

    {
	"remove curse",	{0}, {0},
	spell_remove_curse,	TAR_OBJ_CHAR_DEF,	POS_STANDING,
	NULL,		FALSE,	SLOT(35),	 5,	12,
	"",			"!Remove Curse!",	""
    },

    {
	"restore mana",	{0}, {0},
	spell_restore_mana,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(251),	1,	18,
	"restore mana",		"!Restore Mana!",	""
    },

    {
	"resurrect",	{0}, {0},
	spell_resurrect,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(235),	 35,	12,
	"",			"!Resurrect!",	""
    },

    {
	"sanctuary",	{0}, {0},
	spell_sanctuary,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_sanctuary,	FALSE,	SLOT(36),	75,	12,
	"",			"The white aura around your body fades.",
	""
    },

    {
	"shield",	{0}, {0},
	spell_shield,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(67),	12,	18,
	"",			"Your force shield shimmers then fades away.",
	""
    },

    {
	"shocking grasp",	{0}, {0},
	spell_shocking_grasp,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(53),	15,	12,
	"shocking grasp",	"!Shocking Grasp!",	""
    },

    {
	"shockshield",	{0}, {0},
	spell_shockshield,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(412),	75,	12,
	"lightning bolt",	"Your crackling shield sizzles and fades.",
	""
    },

    {
	"sleep",	{0}, {0},
	spell_sleep,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_sleep,	TRUE,	SLOT(38),	15,	12,
	"",			"You feel less tired.",	""
    },

    {
        "slow",	{0}, {0},
        spell_slow,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,		TRUE,	SLOT(515),      30,     12,
        "",                     "You feel yourself speed up.",	""
    },

    {
	"stone skin",	{0}, {0},
	spell_stone_skin,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,		FALSE,	SLOT(66),	12,	18,
	"",			"Your skin feels soft again.",	""
    },

    {
	"summon",	{0}, {0},
	spell_summon,		TAR_IGNORE,		POS_STANDING,
	NULL,		FALSE,	SLOT(40),	50,	12,
	"",			"!Summon!",		""
    },

    {
	"teleport",	{0}, {0},
	spell_teleport,		TAR_CHAR_SELF,		POS_FIGHTING,
	NULL,	 	TRUE,	SLOT( 2),	35,	12,
	"",			"!Teleport!",		""
    },

    {
	"transport",	{0}, {0},
	spell_transport,	TAR_OBJ_TRAN,		POS_STANDING,
	NULL,		FALSE,	SLOT(524),	30,	12,
	"",			"!Transport!",		""
    },

    {
	"ventriloquate",	{0}, {0},
	spell_ventriloquate,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(41),	 5,	12,
	"",			"!Ventriloquate!",	""
    },

    {
	"voodoo",	{0}, {0},
	spell_voodoo,		TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(286),	80,	12,
	"",			"!Voodoo!",		""
    },

    {
	"weaken",	{0}, {0},
	spell_weaken,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(68),	20,	12,
	"spell",		"You feel stronger.",	""
    },

    {
	"word of recall",	{0}, {0},
	spell_word_of_recall,	TAR_CHAR_SELF,		POS_RESTING,
	NULL,		TRUE,	SLOT(42),	 5,	12,
	"",			"!Word of Recall!",	""
    },

/*
 * Dragon breath
 */
    {
	"acid breath",	{0}, {0},
	spell_acid_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(200),	100,	24,
	"blast of acid",	"!Acid Breath!",	""
    },

    {
	"fire breath",	{0}, {0},
	spell_fire_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(201),	200,	24,
	"blast of flame",	"The smoke leaves your eyes.",	""
    },

    {
	"frost breath",	{0}, {0},
	spell_frost_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(202),	125,	24,
	"blast of frost",	"!Frost Breath!",	""
    },

    {
	"gas breath",	{0}, {0},
	spell_gas_breath,	TAR_IGNORE,		POS_FIGHTING,
	NULL,		TRUE,	SLOT(203),	175,	24,
	"blast of gas",		"!Gas Breath!",		""
    },

    {
	"lightning breath",	{0}, {0},
	spell_lightning_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(204),	150,	24,
	"blast of lightning",	"!Lightning Breath!",	""
    },

/*
 * Spells for mega1.are from Glop/Erkenbrand.
 */
    {
        "general purpose",	{0}, {0},
        spell_general_purpose,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,		TRUE,	SLOT(401),      0,      12,
        "general purpose ammo", "!General Purpose Ammo!",	""
    },
 
    {
        "high explosive",	{0}, {0},
        spell_high_explosive,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,		TRUE,	SLOT(402),      0,      12,
        "high explosive ammo",  "!High Explosive Ammo!",	""
    },


/* combat and weapons skills */


    {
	"axe",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_axe,	TRUE,	SLOT( 0),       0,      0,
        "",                     "!Axe!",		""
    },

    {
        "dagger",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_dagger,	TRUE,	SLOT( 0),       0,      0,
        "",                     "!Dagger!",		""
    },
 
    {
	"flail",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_flail,	TRUE,	SLOT( 0),       0,      0,
        "",                     "!Flail!",		""
    },

    {
	"mace",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_mace,	TRUE,	SLOT( 0),       0,      0,
        "",                     "!Mace!",		""
    },

    {
	"polearm",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_polearm,	TRUE,	SLOT( 0),       0,      0,
        "",                     "!Polearm!",		""
    },
    
    {
	"shield block",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_shield_block,	TRUE,	SLOT(0),	0,	0,
	"",			"!Shield!",		""
    },
 
    {
	"spear",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_spear,	TRUE,	SLOT( 0),       0,      0,
        "",                     "!Spear!",		""
    },

    {
	"sword",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_sword,	TRUE,	SLOT( 0),       0,      0,
        "",                     "!sword!",		""
    },

    {
	"whip",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_whip,	TRUE,	SLOT( 0),       0,      0,
        "",                     "!Whip!",	""
    },

    {
        "backstab",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_backstab,	TRUE,	SLOT( 0),        0,     24,
        "backstab",             "!Backstab!",		""
    },

    {
	"bash",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_bash,	TRUE,	SLOT( 0),       0,      24,
        "bash",                 "!Bash!",		""
    },

    {
	"berserk",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_berserk,	TRUE,	SLOT( 0),       0,      24,
        "",                     "You feel your pulse slow down.",	""
    },

    {
        "circle",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_circle,	TRUE,	SLOT( 0),        0,     24,
        "circle",             "!Circle!",		""
    },

    {
	"dirt kicking",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_dirt,	TRUE,	SLOT( 0),	0,	24,
	"kicked dirt",		"You rub the dirt out of your eyes.",	""
    },

    {
        "disarm",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_disarm,	TRUE,	SLOT( 0),        0,     24,
        "",                     "!Disarm!",		""
    },
 
    {
        "dodge",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_dodge,	TRUE,	SLOT( 0),        0,     0,
        "",                     "!Dodge!",		""
    },
 
    {
        "dual wield",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_dual_wield,	TRUE,	SLOT( 0),        0,     0,
        "",			"!Dual Wield!",		""
    },

    {
        "enhanced damage",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_enhanced_damage,   TRUE,	SLOT( 0),        0,     0,
        "",                     "!Enhanced Damage!",	""
    },

    {
	"envenom",	{0}, {0},
	spell_null,		TAR_IGNORE,	  	POS_RESTING,
	&gsn_envenom,	TRUE,	SLOT(0),	0,	36,
	"",			"!Envenom!",		""
    },

    {
        "feed",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_feed,	TRUE,	SLOT( 0),	0,	24,
        "feed",             "!Feed!",		""
    },

    {
	"gouge",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_gouge,	TRUE,	SLOT( 0),	0,	24,
	"gouge",		"Your vision clears.",	""
    },

    {
        "grip",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_grip,	TRUE,	SLOT( 0),        0,     0,
        "",                     "!Grip!",		""
    },

    {
	"hand to hand",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_hand_to_hand,	TRUE,	SLOT( 0),	0,	0,
	"",			"!Hand to Hand!",	""
    },

    {
        "kick",	{0}, {0},
        spell_null,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_kick,	TRUE,	SLOT( 0),        0,     12,
        "kick",                 "!Kick!",		""
    },

    {
        "parry",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_parry,	TRUE,	SLOT( 0),        0,     0,
        "",                     "!Parry!",		""
    },

    {
        "rescue",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_rescue,	TRUE,	SLOT( 0),        0,     12,
        "",                     "!Rescue!",		""
    },

    {
	"trip",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_trip,	TRUE,	SLOT( 0),	0,	24,
	"trip",			"!Trip!",		""
    },

    {
        "stun",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_stun,	TRUE,	SLOT( 0),        0,     0,
        "",			"!Stun!",		""
    },

    {
        "second attack",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_second_attack,	TRUE,	SLOT( 0),        0,     0,
        "",                     "!Second Attack!",	""
    },

    {
        "third attack",	{0}, {0},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_third_attack,	TRUE,	SLOT( 0),        0,     0,
        "",                     "!Third Attack!",	""
    },

    {
	"fourth attack",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_fourth_attack,	TRUE,	SLOT(0),	0,	0,
	"",			"!Fourth Attack!",	""
    },

    {
	"fifth attack",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_fifth_attack,	TRUE,	SLOT(0),	0,	0,
	"",			"!Fifth Attack!",	""
    },

/* non-combat skills */

    { 
	"fast healing",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_SLEEPING,
	&gsn_fast_healing,	TRUE,	SLOT( 0),	0,	0,
	"",			"!Fast Healing!",	""
    },

    {
	"haggle",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_haggle,	TRUE,	SLOT( 0),	0,	0,
	"",			"!Haggle!",		""
    },

    {
	"hide",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_hide,	TRUE,	SLOT( 0),	 0,	12,
	"",			"!Hide!",		""
    },

    {
	"lore",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_lore,	TRUE,	SLOT( 0),	0,	36,
	"",			"!Lore!",		""
    },

    {
	"meditation",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_SLEEPING,
	&gsn_meditation,	TRUE,	SLOT( 0),	0,	0,
	"",			"Meditation",		""
    },

    {
	"peek",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_peek,	TRUE,	SLOT( 0),	 0,	 0,
	"",			"!Peek!",		""
    },

    {
	"pick lock",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_pick_lock,	TRUE,	SLOT( 0),	 0,	12,
	"",			"!Pick!",		""
    },

    {
	"sneak",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_sneak,	TRUE,	SLOT( 0),	 0,	12,
	"",			"You no longer feel stealthy.",	""
    },

    {
	"steal",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_steal,	TRUE,	SLOT( 0),	 0,	24,
	"",			"!Steal!",		""
    },

    {
	"scrolls",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_scrolls,	TRUE,	SLOT( 0),	0,	24,
	"",			"!Scrolls!",		""
    },

    {
	"staves",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_staves,	TRUE,	SLOT( 0),	0,	12,
	"",			"!Staves!",		""
    },
    
    {
	"track",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_track,	TRUE,	SLOT( 0),	 0,	12,
	"",			"!Track!",		""
    },

    {
	"wands",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_wands,	TRUE,	SLOT( 0),	0,	12,
	"",			"!Wands!",		""
    },

    {
	"recall",	{0}, {0},
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_recall,	TRUE,	SLOT( 0),	0,	12,
	"",			"!Recall!",		""
    }
};

