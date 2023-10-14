#ifndef COMMON_H
#define COMMON_H
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include	<iostream>
#include	<sstream>
#include <memory>
#include	"version.h"
#include	"typedefs.h"
#include	"lgnSvcs.h"
#include "maps.h"

#define MAXPARTYMEMBERS		25
#define MAXSIDES			4
#define MAXBANKITEMS		400
#define MAXTITLES			100
#define MAXSELLLIST		50
static const char caTitleList[MAXTITLES][50] = {"Novice","Administrator","Game Master","The Beta Tester","The Hero of Elvine","The Hero of Aresden","The Contribution Whore","The Neat Freak","The Famous","Chug Chug Chug!!","Wyvern Slayer","Hellclaw Slayer","Tigerworm Slayer","Let Pandora Loose","The One Who Killed Abaddon","The Wizard","The Warrior","Savior","The Manufacturing Master","The Alchemy Master","The Mining Master","The Crafting Master","The Master Farmer","Sock It!","The Treasure Hunter","Earth Bender","The Valkyrie","The Vampire" };

enum Side
{
	NEUTRAL,
	ARESDEN,
	ELVINE,
	ISTRIA,
	HOSTILE
};

static const char * sideName[MAXSIDES] = { "Traveller", "Aresden", "Elvine", "Istria" };
static const char * sideMap[MAXSIDES] = { "default", "aresden", "elvine", /*"istria"*/"default2" };
static const char * sideMapJail[MAXSIDES] = { "default", "arejail", "elvjail", /*"istjail"*/"default2" };
static const char * sideMapFarm[MAXSIDES] = { "default", "arefarm", "elvfarm", /*"istfarm"*/"default2" };
static const char * sideMapRes[MAXSIDES] = { "default", "resurr1", "resurr2", /*"resurr3"*/"default2" };


// ----------------------------------Guild-------------------------------------------------------------------------------------
#define MAXGUILDBANKITEMS		75*4
#define GUILDSUMMONSGOLDCOST	5000
#define GUILDSUMMONSTIME		MINUTE(5)

#define TEMPITEMREDUCTION				0.70

enum GuildUpgrades{
	GU_WAREHOUSE,
	GU_SUMMONS,
	GU_CAPTAINCY,
	GU_RAIDMASTERS,
	GU_HUNTMASTERS,
	GU_MAX
};

const struct GuildUpgrade{
	string name;
	GuildUpgrades type;
	uint8 maxLvl;
	uint32 costGold[5];
	uint32 costMaj[5];
	uint32 costCont[5];
} gldUps[GU_MAX] = {
	{
		"Warehouse", GU_WAREHOUSE, 4,
		{0, 4000000, 8000000, 20000000, 50000000},	// gold
		{0, 300, 750, 1500, 3000},							// maj
		{0, 5000, 12500, 25000, 50000}					// cont
	}/*,{
		"Summons", GU_SUMMONS, 4,
		{0, 5000000, 10000000, 30000000, 60000000},
		{0, 450, 1000, 3000, 6000},
		{0, 5000, 20000, 37500, 100000}
	}*/,{
		"Captaincy", GU_CAPTAINCY, 4,
		{0, 1000000, 3000000, 12000000, 60000000},
		{0, 50, 150, 450, 3000},
		{0, 1000, 3000, 12000, 60000}
	},{
		"Raidmasters", GU_RAIDMASTERS, 4,
		{0, 6000000, 12000000, 40000000, 100000000},
		{0, 500, 1000, 4000, 10000},
		{0, 4000, 12500, 25000, 50000}
	},{
		"Huntmasters", GU_HUNTMASTERS, 4,
		{0, 3000000, 9000000, 30000000, 90000000},
		{0, 500, 1000, 4000, 10000},
		{0, 4000, 15000, 37500, 75000}
	}
};

const uint32 maxGWHItems[5] = { 0, 75, 75*2, 75*3, 75*4 };


enum GuildRank
{
	GUILDRANK_MASTER,
	GUILDRANK_CAPTAIN,
	GUILDRANK_HUNTMASTER,
	GUILDRANK_RAIDMASTER,
	GUILDRANK_GUILDSMAN,
	GUILDRANK_RECRUIT,
	GUILDRANK_MAX,
	GUILDRANK_NONE = -1
};

const struct GuildRankPermissions
{
	bool canDisband;
	bool canInvite;
	bool canBan;
	bool canSummon;
	bool canWithdrawWH;
} gldRankPerm[GUILDRANK_MAX] = {
	{TRUE, TRUE, TRUE, TRUE, TRUE},		// GUILDRANK_MASTER
	{FALSE, TRUE, TRUE, FALSE, TRUE},	// GUILDRANK_CAPTAIN
	{FALSE, FALSE, FALSE, FALSE, TRUE},	// GUILDRANK_HUNTMASTER
	{FALSE, FALSE, FALSE, FALSE, TRUE},	// GUILDRANK_RAIDMASTER
	{FALSE, FALSE, FALSE, FALSE, TRUE},	// GUILDRANK_GUILDSMAN
	{FALSE, FALSE, FALSE, FALSE, FALSE}	// GUILDRANK_RECRUIT
};

#define GUILDSTARTRANK		GUILDRANK_RECRUIT

//-----------------------------------------------------------------------------------------------------------------------

enum chatTypes
{
	CHAT_NORMAL,
	CHAT_GUILD,
	CHAT_SHOUT,
	CHAT_NATIONSHOUT,
	CHAT_PARTY,
	CHAT_GM = 10,
	CHAT_WHISPER = 20,
	CHAT_MAX
};
/*enum chatTypes
{
	CHAT_NORMAL,
	CHAT_GUILD,
	CHAT_SHOUT,
	CHAT_NATIONSHOUT,
	CHAT_PARTY,
	CHAT_GM = 10,
	CHAT_WHISPER = 20,
	CHAT_MAX
};*/
//-----------------------------------------------------------------------------------------------------------------------

enum StatusFlags
{
	STATUS_INVISIBILITY =		1 << 4,	// 0x00000010
	STATUS_BERSERK =			1 << 5,	// 0x00000020
	STATUS_FROZEN =				1 << 6,	// 0x00000040
	STATUS_POISON =				1 << 7,	// 0x00000080

	STATUS_ANGELSTR =				1 << 12,	// 0x00001000
	STATUS_ANGELDEX =				1 << 13,	// 0x00002000
	STATUS_ANGELINT =				1 << 14,	// 0x00004000
	STATUS_ANGELMAG =				1 << 15,	// 0x00008000

	STATUS_RELICHOLDER = 		1 << 17,	// 0x00020000 - previously STATUS_HEROFLAG
	STATUS_AFK =					1 << 18,	// 0x00040000
	STATUS_GREENSLATE =			1 << 16,	// 0x00010000
	STATUS_REDSLATE =				1 << 22,	// 0x00400000
	STATUS_BLUESLATE =			1 << 23,	// 0x00800000
	STATUS_ILLUSIONMOVEMENT =	1 << 21,	// 0x00200000
	STATUS_ILLUSION =				1 << 24,	// 0x01000000
	STATUS_DEFENSESHIELD =		1 << 25,	// 0x02000000
	STATUS_PFM =					1 << 26,	// 0x04000000
	STATUS_PFA =					1 << 27,	// 0x08000000
	STATUS_PK =						1 << 20,	// 0x00100000
	STATUS_SIDE =					0xF0000000,
	STATUS_ALL =					0xFFFFFFFF,
	STATUS_ENEMYFLAGS =			STATUS_ALL - ( STATUS_PFA |
												STATUS_PFM |
												STATUS_DEFENSESHIELD |
												STATUS_ILLUSION |
												STATUS_POISON ),
	STATUS_MISCFLAGS =			( STATUS_INVISIBILITY |
									STATUS_RELICHOLDER |
									STATUS_FROZEN |
									STATUS_POISON |
									STATUS_ILLUSION |
									STATUS_ILLUSIONMOVEMENT |
									STATUS_PFA |
									STATUS_PFM |
									STATUS_DEFENSESHIELD/* |
									STATUS_BERSERK*/) // removing STATUS_BERSERK upon death will remove berserk slate effect?
};

//-----------------------------------------------------------------------------------------------------------------------

enum GMFlags
{
	GMFLAG_INVINCIBLE =		1,
	GMFLAG_NOAGGRO =				1 << 1,
	GMFLAG_ETHEREAL =				1 << 2
};

//-----------------------------------------------------------------------------------------------------------------------

//#define SECONDS(x)			(x * CLOCKS_PER_SEC)
//#define MINUTES(x)			(x * SECONDS(60))
//#define HOURS(x)			(x * MINUTES(60))
//#define DAYS(x)				(x * HOURS(24))

// Time
#define _ms					_s / 1000
#define _s					* 1000
#define _m					_s * 60
#define _h					_m * 60
#define _d					_h * 24

// time_t
#define MINUTE(x)		x * 60
#define HOUR(x)	 	x * MINUTE(60)
#define DAY(x)		 	x * HOUR(24)

enum DaysOfWeek
{
	SUNDAY,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY
};

//----------------------------------Events-------------------------------------------------------------------------------------

#define RELICVICTORYTIME		(10 _m)

enum EventType{
	ET_NONE,
	ET_CAPTURE,
	ET_DESTROY_SHIELD,
	ET_CRUSADE,
	ET_APOCALYPSE,
	ET_BISLE,
	ET_MAX
};
static const char * eventName[] = { "", "Capture the Relic", "Destroy the Shield", "Crusade", "Apocalypse","Bleeding Island Guild War"};

struct Casualties{
	uint32 deaths;
	uint32 kills;
};

//----------------------------------------------------------------------------------------------------------------------------------------

enum HeroReq{
	HR_CAPE,
	HR_HELM,
	HR_CAP,
	HR_PLATE,
	HR_BMCAP,
	HR_BMPLATE,
	HR_ROBE,
	HR_HAUBERK,
	HR_LEGGINGS,
	HR_MAX
};

static const struct HeroItemCost{
	uint32 EK;
	uint32 contribution;
}heroPrice[HR_MAX] =
{
	{300,0},	// HR_CAPE
	{150,20},	// HR_HELM
	{100,20},	// HR_CAP
	{300,30},	// HR_PLATE
	{100,20},	// HR_BMCAP
	{300,30},	// HR_BMPLATE
	{200,20},	// HR_ROBE
	{100,10},	// HR_HAUBERK
	{150,15}	// HR_LEGGINGS
};

static const uint32 HeroItemID[HR_MAX][MAXSIDES-1][2] =
{
	{	// HR_CAPE
		{400, 400},		// ARESDEN
		{401, 401},		// ELVINE
		{1009, 1009}	// ISTRIA
	},
	{	// HR_HELM
		{403, 404},		// ARESDEN
		{405, 406},		// ELVINE
		{1010, 1011}	// ISTRIA
	},
	{	// HR_CAP
		{407, 408},		// ARESDEN
		{409, 410},		// ELVINE
		{1012, 1013}	// ISTRIA
	},
	{	// HR_PLATE
		{411, 412},		// ARESDEN
		{413, 414},		// ELVINE
		{1014, 1015}	// ISTRIA
	},
	{	// HR_BMCAP
		{431, 432},		// ARESDEN
		{433, 434},		// ELVINE
		{1012, 1013}	// ISTRIA
	},
	{	// HR_BMPLATE
		{435, 436},		// ARESDEN
		{437, 438},		// ELVINE
		{1014, 1015}	// ISTRIA
	},
	{	// HR_ROBE
		{415, 416},		// ARESDEN
		{417, 418},		// ELVINE
		{1016, 1017}	// ISTRIA
	},
	{	// HR_HAUBERK
		{419, 420},		// ARESDEN
		{421, 422},		// ELVINE
		{1018, 1019}	// ISTRIA
	},
	{	// HR_LEGGINGS
		{423, 424},		// ARESDEN
		{425, 426},		// ELVINE
		{1020, 1021}	// ISTRIA
	}
};

//----------------------------------------------------------------------------------------------------------------------------------------

enum Directions{
	CENTER,
	NORTH,
	NORTHEAST,
	EAST,
	SOUTHEAST,
	SOUTH,
	SOUTHWEST,
	WEST,
	NORTHWEST
};

//----------------------------------------------------------------------------------------------------------------------------------------

#define atoul(str)			strtoul(str, NULL, 10)
#define atoull(str)			_strtoui64(str, NULL, 10)
#define strcpyn(dst, src)	strncpy(dst, src, sizeof(dst))

#define logb(n, b)	(log(n)/log(b))

struct Point{
	int32 x,y;

	bool operator ==(Point p)
	{
		return (p.x == x && p.y == y) ? TRUE : FALSE;
	}
};

#define GetBoolText(val)	( val ? "True" : "False" )

#endif //COMMON_H
