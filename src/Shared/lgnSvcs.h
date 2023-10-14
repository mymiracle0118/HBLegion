#ifndef LGNSVCS_H
#define LGNSVCS_H
#include "NetMessages.h"

enum lgnSvcsCmds{
	CMD_LGNSVC_WARRIORPLATE,
	CMD_LGNSVC_BMAGEPLATE,
	CMD_LGNSVC_BMAGECHAIN,
	CMD_LGNSVC_MAGECHAIN,
	CMD_LGNSVC_WARRIORHAUBERK,
	CMD_LGNSVC_MAGEHAUBERK,
	CMD_LGNSVC_WARRIORHOSE,
	CMD_LGNSVC_MAGEHOSE,
	CMD_LGNSVC_WARRIORCAP,
	CMD_LGNSVC_MAGECAP,
	CMD_LGNSVC_WARRIORWINGHELM,
	CMD_LGNSVC_MAGEWINGHELM,
	CMD_LGNSVC_LGNPLATE,
	CMD_LGNSVC_LGNCHAIN,
	CMD_LGNSVC_LGNPLATELEGS,
	CMD_LGNSVC_LGNHAUBERK,
	CMD_LGNSVC_LGNWINGHELM,
	CMD_LGNSVC_LGNWIZCAP,
	CMD_LGNSVC_LGNWIZHAT,
	CMD_LGNSVC_LGNHORNHELM,
	//CMD_LGNSVC_TOARE,
	//CMD_LGNSVC_TOELV,
	CMD_LGNSVC_CONT2,
	CMD_LGNSVC_CONT20,
	CMD_LGNSVC_GOLD10,
	CMD_LGNSVC_GOLD100,
	CMD_LGNSVC_TOKEN1,
	CMD_LGNSVC_TOKEN10,
	CMD_LGNSVC_TOKEN100,
	//CMD_LGNSVC_XPSLATE,
	CMD_LGNSVC_ZERKSLATE,
	CMD_LGNSVC_MPSLATE,
	//CMD_LGNSVC_HPSLATE,
	CMD_LGNSVC_ZEM,
	CMD_LGNSVC_SOM,
	CMD_LGNSVC_SOX,
	CMD_LGNSVC_NAMECHANGE,
	CMD_LGNSVC_TOWNCHANGE,
	CMD_LGNSVC_DYEGOLD,
	CMD_LGNSVC_DYECRIMRED,
	CMD_LGNSVC_DYEPINK,
	CMD_LGNSVC_DYEBLACK,
	CMD_LGNSVC_STATRESET,
	CMD_LGNSVC_EXTRACTGEM,
	CMD_LGNSVC_DESTROYGEM,
	CMD_LGNSVC_SEXCHANGE,
	CMD_LGNSVC_KEY,
	// tokens start
	CMD_LGNSVC_TRADETOKEN1,
	CMD_LGNSVC_TRADETOKEN10,
	CMD_LGNSVC_TRADETOKEN100,
	// tokens end
	// spells start
	CMD_LGNSVC_TRADECANCEL,
	CMD_LGNSVC_TRADEIMC,
	CMD_LGNSVC_TRADEICESTORM,
	CMD_LGNSVC_TRADEMFS,
	CMD_LGNSVC_TRADEBS,
	CMD_LGNSVC_TRADEESW,
	CMD_LGNSVC_TRADEMIM,
	CMD_LGNSVC_TRADEVALK,

	// end
	CMD_LGNSVC_MAX
};

#define CMD_LGNSVC_MAXWEAPONS	0
#define CMD_LGNSVC_MAXARMORS	20
#define CMD_LGNSVC_MAXMISC		13
#define CMD_LGNSVC_MAXSERVICES	21

const struct lgnPtsSvc{
	char * desc;
	char * name;
	unsigned long price;
	unsigned short cmd;
} lgnPtsSvcs[4][CMD_LGNSVC_MAX] = {
	{},
	{
		{"Plate Mail (HP 21% End 21%)", "Plate HP21", 20, CMD_LGNSVC_WARRIORPLATE},
		{"Plate Mail (MP 21% End 21%)", "Plate MP21", 20, CMD_LGNSVC_BMAGEPLATE},
		{"Chain Mail (HP 21% End 21%)", "Chain HP21", 20, CMD_LGNSVC_BMAGECHAIN},
		{"Chain Mail (MP 21% End 21%)", "Chain MP21", 20, CMD_LGNSVC_MAGECHAIN},
		{"Hauberk (HP 21% End 21%)", "Chain MP21", 20, CMD_LGNSVC_WARRIORHAUBERK},
		{"Hauberk (MP 21% End 21%)", "Chain MP21", 20, CMD_LGNSVC_MAGEHAUBERK},
		{"Chain Hose (HP 21% End 21%)", "Chain MP21", 20, CMD_LGNSVC_WARRIORHOSE},
		{"Chain Hose (MP 21% End 21%)", "Chain MP21", 20, CMD_LGNSVC_MAGEHOSE},
		{"Cap (HP 21% End 21%)", "Chain MP21", 20, CMD_LGNSVC_WARRIORCAP},
		{"Cap (MP 21% End 21%)", "Chain MP21", 20, CMD_LGNSVC_MAGECAP},
		{"Wing Helm (HP 21% End 21%)", "Chain MP21", 20, CMD_LGNSVC_WARRIORWINGHELM},
		{"Wing Helm (MP 21% End 21%)", "Chain MP21", 20, CMD_LGNSVC_MAGEWINGHELM},
		{"Golden Plate Mail (Gold 30% End 35%)", "LegionPlate Gold35", 20, CMD_LGNSVC_LGNPLATE},
		{"Golden Chain Mail (Gold 30% End 35%)", "LegionChain Gold35", 20, CMD_LGNSVC_LGNCHAIN},
		{"Golden Plate Legs (Gold 30% End 35%)", "LegionPlateLegs Gold35", 20, CMD_LGNSVC_LGNPLATELEGS},
		{"Golden Hauberk (Gold 30% End 35%)", "LegionHauberk Gold35", 20, CMD_LGNSVC_LGNHAUBERK},
		{"Golden Wing Helmet(Gold 30% End 35%)", "LegionWingHelm Gold35", 20, CMD_LGNSVC_LGNWINGHELM},
		{"Golden Wiz Cap(Gold 30% End 35%)", "LegionWizCap Gold35", 20, CMD_LGNSVC_LGNWIZCAP},
		{"Golden Wiz Hat(Gold 30% End 35%)", "LegionWizHat Gold35", 20, CMD_LGNSVC_LGNWIZHAT},
		{"Golden Horn Helmet(Gold 30% End 35%)", "LegionHornHelm Gold35", 20, CMD_LGNSVC_LGNHORNHELM}
	},{
		//Support items
		{"The Pandora's Key", "Pandoras Key",2,CMD_LGNSVC_KEY},
		{"Sex Change Potion","Sex Pot", 20,CMD_LGNSVC_SEXCHANGE},
		{"Dye (Gold)","Dye Gold",5,CMD_LGNSVC_DYEGOLD},
		{"Dye (Crimson-Red)","Dye Crimson Red",5,CMD_LGNSVC_DYECRIMRED},
		{"Dye (Pink)","Dye Pink",5,CMD_LGNSVC_DYEPINK},
		{"Dye (Black)","Dye Black",5,CMD_LGNSVC_DYEBLACK},
		//{"Experience Slate", "XP Slate", 2, CMD_LGNSVC_XPSLATE},
		{"Berserk Slate", "Zerk Slate", 2, CMD_LGNSVC_ZERKSLATE},
		{"Mana Slate", "MP Slate", 4, CMD_LGNSVC_MPSLATE},
		//{"Health Slate", "HP Slate", 6, CMD_LGNSVC_HPSLATE},
		{"Zemstone of Sacrifice", "Zem", 2, CMD_LGNSVC_ZEM},
		{"Stone of Merien", "SoM", 2, CMD_LGNSVC_SOM},
		{"Stone of Xelima", "SoX", 2, CMD_LGNSVC_SOX},
		{"10,000 Gold", "10k Gold", 1, CMD_LGNSVC_GOLD10},
		{"100,000 Gold", "100k Gold", 5, CMD_LGNSVC_GOLD100}
	},{
		{"Character Name Change", "NC", 10, CMD_LGNSVC_NAMECHANGE},
		{"Town Change", "TC", 25, CMD_LGNSVC_TOWNCHANGE},
		{"Stat Reset", "SR", 10, CMD_LGNSVC_STATRESET},
		{"Extract Gem", "EG", 25, CMD_LGNSVC_EXTRACTGEM},
		{"Destroy Gem", "DG", 10, CMD_LGNSVC_DESTROYGEM},
		{"2 Contribution points", "2 Cont", 1, CMD_LGNSVC_CONT2},
		{"10 Contribution points", "20 Cont", 4, CMD_LGNSVC_CONT20},
		{"Take 1 Legion Coin", "1 Legion Coin", 1, CMD_LGNSVC_TOKEN1},
		{"Take 10 Legion Coins", "10 Legion Coins", 10, CMD_LGNSVC_TOKEN10},
		{"Take 100 Legion Coins", "100 Legion Coins", 100, CMD_LGNSVC_TOKEN100},
		{"Return 1 Legion Coin", "Trade 1 Legion Coin Back", 1, CMD_LGNSVC_TRADETOKEN1},
		{"Return 10 Legion Coins", "Trade 10 Legion Coins Back", 10, CMD_LGNSVC_TRADETOKEN10},
		{"Return 100 Legion Coins", "Trade 100 Legion Coins Back", 100, CMD_LGNSVC_TRADETOKEN100},
		{"unlearn Cancel and convert to manual", "Trade Cancel Manual", 10, CMD_LGNSVC_TRADECANCEL},
		{"unlearn I.M.C and convert to manual", "Trade I.M.C Manual", 10, CMD_LGNSVC_TRADEIMC},
		{"unlearn I.S and convert to manual", "Trade IceStorm Manual", 10, CMD_LGNSVC_TRADEICESTORM},
		{"unlearn M.F.S and convert to manual", "Trade MassFireStrike Manual", 10, CMD_LGNSVC_TRADEMFS},
		{"unlearn B.S.W and convert to manual", "Trade BloodyShockW Manual", 10, CMD_LGNSVC_TRADEBS},
		{"unlearn E.S.W and convert to manual", "Trade E.S.W Manual", 10, CMD_LGNSVC_TRADEESW},
		{"unlearn M.I.M. and convert to manual", "Trade M.I.M. Manual", 10, CMD_LGNSVC_TRADEMIM},
		{"unlearn Valkyrie and convert to manual", "Trade Valkyrie Manual", 10, CMD_LGNSVC_TRADEVALK}
	}
};

enum lgnSlayerCmds{
	CMD_LGNSLAYER_DAGGER,
	CMD_LGNSLAYER_GRADIUS,
	CMD_LGNSLAYER_SCIMITAR,
	CMD_LGNSLAYER_FALCHON,
	CMD_LGNSLAYER_BROADSWORD,
	CMD_LGNSLAYER_BASTADSWORD,
	CMD_LGNSLAYER_CLAYMORE,
	CMD_LGNSLAYER_DOUBLEAXE,
	CMD_LGNSLAYER_GREATSWORD,
	CMD_LGNSLAYER_FLAMEBERGE,
	CMD_LGNSLAYER_GIANTSWORD,
	CMD_LGNSLAYER_BSS,
	CMD_LGNSLAYER_ESTERK,
	CMD_LGNSLAYER_RAPIER,
	CMD_LGNSLAYER_HAMMER,
	CMD_LGNSLAYER_BH,
	CMD_LGNSLAYER_BBH,
	CMD_LGNSLAYER_WARAXE,
	CMD_LGNSLAYER_BATTLEAXE,
	CMD_LGNSLAYER_COMPOSITEBOW,
	CMD_LGNSLAYER_MAGICWAND,
	CMD_LGNSLAYER_HAUBERK_M,
	CMD_LGNSLAYER_HAUBERK_W,
	CMD_LGNSLAYER_PLEGGINGS_M,
	CMD_LGNSLAYER_PLEGGINGS_W,
	CMD_LGNSLAYER_FHELM_M,
	CMD_LGNSLAYER_FHELM_W,
	CMD_LGNSLAYER_PLATEMAIL_M,
	CMD_LGNSLAYER_PLATEMAIL_W,
	CMD_LGNSLAYER_CAP_M,
	CMD_LGNSLAYER_CAP_W,
	CMD_LGNSLAYER_ROBE_M,
	CMD_LGNSLAYER_ROBE_W,
	CMD_LGNSLAYER_LEATHER_M,
	CMD_LGNSLAYER_LEATHER_W,
	CMD_LGNSLAYER_CHAINMAIL_M,
	CMD_LGNSLAYER_CHAINMAIL_W,
	CMD_LGNSLAYER_SCALEMAIL_M,
	CMD_LGNSLAYER_SCALEMAIL_W,
	CMD_LGNSLAYER_HAT_M,
	CMD_LGNSLAYER_HAT_W,

	// end
	CMD_LGNSLAYER_MAX
};

#define CMD_LGNSLAYER_MAXWEAPONS	21
#define CMD_LGNSLAYER_MAXARMORS	20

const struct lgnSlayerSvc{
	char * desc;
	char * name;
	unsigned short cmd;
} lgnSlayerSvcs[2][CMD_LGNSLAYER_MAX] = {
	{
		{"Slayer Dagger", "SlayerDagger", CMD_LGNSLAYER_DAGGER},
		{"Slayer Gradius", "SlayerGradius", CMD_LGNSLAYER_GRADIUS},
		{"Slayer Scimitar", "SlayerScimitar", CMD_LGNSLAYER_SCIMITAR},
		{"Slayer Falchion", "SlayerFalchion", CMD_LGNSLAYER_FALCHON},
		{"Slayer Broad Sword", "SlayerBroadSword", CMD_LGNSLAYER_BROADSWORD},
		{"Slayer Bastad Sword", "SlayerBastadSword", CMD_LGNSLAYER_BASTADSWORD},
		{"Slayer Claymore", "SlayerClaymore", CMD_LGNSLAYER_CLAYMORE},
		{"Slayer Double Axe", "SlayerDoubleAxe", CMD_LGNSLAYER_DOUBLEAXE},
		{"Slayer Great Sword", "SlayerGreatSword", CMD_LGNSLAYER_GREATSWORD},
		{"Slayer Flameberge", "SlayerFlameberge", CMD_LGNSLAYER_FLAMEBERGE},
		{"Slayer Giant Sword", "SlayerGiantSword", CMD_LGNSLAYER_GIANTSWORD},
		{"Slayer Black Shadow Sword", "SlayerShadowSword", CMD_LGNSLAYER_BSS},
		{"Slayer Esterk", "SlayerEsterk", CMD_LGNSLAYER_ESTERK},
		{"Slayer Rapier", "SlayerRapier", CMD_LGNSLAYER_RAPIER},
		{"Slayer Hammer", "SlayerHammer", CMD_LGNSLAYER_HAMMER},
		{"Slayer Battle Hammer", "SlayerBattleHammer", CMD_LGNSLAYER_BH},
		{"Slayer Barbarian Hammer", "SlayerBarbHammer", CMD_LGNSLAYER_BBH},
		{"Slayer War Axe", "SlayerWarAxe", CMD_LGNSLAYER_WARAXE},
		{"Slayer Battle Axe", "SlayerBattleAxe", CMD_LGNSLAYER_BATTLEAXE},
		{"Slayer Composite Bow", "SlayerCompositeBow", CMD_LGNSLAYER_COMPOSITEBOW},
		{"Slayer Magic Wand(MS20)", "SlayerMagicWandMS20", CMD_LGNSLAYER_MAGICWAND}
	},{
		{"Slayer Hauberk (M)", "SlayerHauberk(M)", CMD_LGNSLAYER_HAUBERK_M},
		{"Slayer Hauberk (W)", "SlayerHauberk(W)", CMD_LGNSLAYER_HAUBERK_W},
		{"Slayer Plate Leggings (M)", "SlayerLeggings(M)", CMD_LGNSLAYER_PLEGGINGS_M},
		{"Slayer Plate Leggings (W)", "SlayerLeggings(W)", CMD_LGNSLAYER_PLEGGINGS_W},
		{"Slayer Full Helm (M)", "SlayerFullHelm(M)", CMD_LGNSLAYER_FHELM_M},
		{"Slayer Full Helm (W)", "SlayerFullHelm(W)", CMD_LGNSLAYER_FHELM_W},
		{"Slayer Plate Mail (M)", "SlayerPlateMail(M)", CMD_LGNSLAYER_PLATEMAIL_M},
		{"Slayer Plate Mail (W)", "SlayerPlateMail(W)", CMD_LGNSLAYER_PLATEMAIL_W},
		{"Slayer Cap (M)", "SlayerCap(M)", CMD_LGNSLAYER_CAP_M},
		{"Slayer Cap (W)", "SlayerCap(W)", CMD_LGNSLAYER_CAP_W},
		{"Slayer Hat (M)", "SlayerHat(M)", CMD_LGNSLAYER_CAP_M},
		{"Slayer Hat (W)", "SlayerHat(W)", CMD_LGNSLAYER_CAP_W},
		{"Slayer Robe (M)", "SlayerRobe(M)", CMD_LGNSLAYER_ROBE_M},
		{"Slayer Robe (W)", "SlayerRobe(W)", CMD_LGNSLAYER_ROBE_W},
		{"Slayer Leather (M)", "SlayerLeather(M)", CMD_LGNSLAYER_LEATHER_M},
		{"Slayer Leather (W)", "SlayerLeather(W)", CMD_LGNSLAYER_LEATHER_W},
		{"Slayer Chain Mail (M)", "SlayerChainMail(M)", CMD_LGNSLAYER_CHAINMAIL_M},
		{"Slayer Chain Mail (W)", "SlayerChainMail(W)", CMD_LGNSLAYER_CHAINMAIL_W},
		{"Slayer Scale Mail (M)", "SlayerScaleMail(M)", CMD_LGNSLAYER_SCALEMAIL_M},
		{"Slayer Scale Mail (W)", "SlayerScaleMail(W)", CMD_LGNSLAYER_SCALEMAIL_W}
	}
};

enum lgnContCmds{
	CMD_LGNCONT_GETCONT10,
	CMD_LGNCONT_GETCONT25,
	CMD_LGNCONT_GETCONT50,
	CMD_LGNCONT_GETCONT100,
	CMD_LGNCONT_CONVERTCONT20,
	CMD_LGNCONT_CONVERTCONT50,
	CMD_LGNCONT_CONVERTCONT100,
	CMD_LGNCONT_CONVERTCONT200,
	CMD_LGNCONT_MAGICNECKMS14,
	CMD_LGNCONT_MAGICNECKDM3,
	CMD_LGNCONT_DEMONPOWERRING,
	CMD_LGNCONT_MAGERING,
	CMD_LGNCONT_DRAGONPOWERRING,
	CMD_LGNCONT_GRANDMAGERING,
	CMD_LGNCONT_RESURWANDMS10,
	//CMD_LGNCONT_CORRUPTZERKWANDMS10,
	CMD_LGNCONT_BLOODGEM7,
	CMD_LGNCONT_REJUGEM7,

	// end
	CMD_LGNCONT_MAX
};

const struct lgnContSvc{
	char * desc;
	char * name;
	unsigned long price;
	unsigned short cmd;
} lgnContSvcs[CMD_LGNCONT_MAX] = {
	{"Convert 10 Contribution to Balls", "ContribBall", 10, CMD_LGNCONT_GETCONT10},
	{"Convert 25 Contribution to Balls", "ContribBall", 25, CMD_LGNCONT_GETCONT25},
	{"Convert 50 Contribution to Balls", "ContribBall", 50, CMD_LGNCONT_GETCONT50},
	{"Convert 100 Contribution to Balls", "ContribBall", 100, CMD_LGNCONT_GETCONT100},
	{"Trade 10 EKs for 20 Contribution", "ContribBall", 20, CMD_LGNCONT_CONVERTCONT20},
	{"Trade 25 EKs for 50 Contribution", "ContribBall", 50, CMD_LGNCONT_CONVERTCONT50},
	{"Trade 50 EKs for 100 Contribution", "ContribBall", 100, CMD_LGNCONT_CONVERTCONT100},
	{"Trade 100 EKs for 200 Contribution", "ContribBall", 200, CMD_LGNCONT_CONVERTCONT200},
	{"Magic Necklace (MS.14) (5000 charges)", "MagicNecklace(MS14)", 50, CMD_LGNCONT_MAGICNECKMS14},
	{"Magic Necklace (DM+3) (5000 charges)", "MagicNecklace(DM+3)", 50, CMD_LGNCONT_MAGICNECKDM3},
	{"Ring of Demon Power (5000 charges)", "RingofDemonpower", 75, CMD_LGNCONT_DEMONPOWERRING},
	{"Ring of Mage (5000 charges)", "RingofMage", 75, CMD_LGNCONT_MAGERING},
	{"Ring of Dragon Power (5000 charges)", "RingofDragonpower", 100, CMD_LGNCONT_DRAGONPOWERRING},
	{"Ring of Grand Mage (5000 charges)", "RingofGrandMage", 100, CMD_LGNCONT_GRANDMAGERING},
	{"Resurrection Wand (MS.10) (10 uses)", "ResurWand(MS.10)", 75, CMD_LGNCONT_RESURWANDMS10},
	//{"Berserk Wand (MS.10) (200 charges)", "BerserkWand(MS.10)", 250, CMD_LGNCONT_CORRUPTZERKWANDMS10},
	{"Blood Gem (7)", "BloodGem7", 100, CMD_LGNCONT_BLOODGEM7},
	{"Rejuvenation Gem (7)", "RejuGem7", 100, CMD_LGNCONT_REJUGEM7}
};
#endif // LGNSVCS_H
