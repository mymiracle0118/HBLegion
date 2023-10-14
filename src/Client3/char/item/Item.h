// Item.h: interface for the CItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEM_H__211A1360_91B9_11D2_B143_00001C7030A6__INCLUDED_)
#define AFX_ITEM_H__211A1360_91B9_11D2_B143_00001C7030A6__INCLUDED_

#include <windows.h>
#include "..\..\..\shared\typedefs.h"
#include "..\..\..\shared\items.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define MAXITEMTYPES		1500
#define MAXITEMSOCKETS		3

enum ItemStat{
	ITEMSTAT_NONE,
	ITEMSTAT_CRITICAL,
	ITEMSTAT_POISONING,
	ITEMSTAT_RIGHTEOUS,
	ITEMSTAT_UNUSED,		// UNUSED
	ITEMSTAT_AGILE,
	ITEMSTAT_LIGHT,
	ITEMSTAT_SHARP,
	ITEMSTAT_STRONG,
	ITEMSTAT_ANCIENT,
	ITEMSTAT_CASTPROB,
	ITEMSTAT_MANACONV,
	ITEMSTAT_CRITICAL2
};

enum ItemStat2{
	ITEMSTAT2_NONE,
	ITEMSTAT2_PSNRES,
	ITEMSTAT2_HITPROB,
	ITEMSTAT2_DEF,
	ITEMSTAT2_HPREC,
	ITEMSTAT2_SPREC,
	ITEMSTAT2_MPREC,
	ITEMSTAT2_MR,
	ITEMSTAT2_PA,
	ITEMSTAT2_MA,
	ITEMSTAT2_CAD,
	ITEMSTAT2_EXP,
	ITEMSTAT2_GOLD
};

#define MAXITEMEQUIPPOS		15
#define EQUIPPOS_NONE		0
#define EQUIPPOS_HEAD		1
#define EQUIPPOS_BODY		2
#define EQUIPPOS_ARMS		3
#define EQUIPPOS_PANTS		4
#define EQUIPPOS_BOOTS		5
#define EQUIPPOS_NECK		6
#define EQUIPPOS_LHAND		7
#define EQUIPPOS_RHAND		8
#define EQUIPPOS_TWOHAND	9
#define EQUIPPOS_RFINGER	10
#define EQUIPPOS_LFINGER	11
#define EQUIPPOS_BACK		12
#define EQUIPPOS_FULLBODY	13

enum ItemTypes
{
	ITEMTYPE_NOTUSED = -1,
	ITEMTYPE_NONE,
	ITEMTYPE_EQUIP,
	ITEMTYPE_APPLY,
	ITEMTYPE_USE_DEPLETE,
	ITEMTYPE_INSTALL,
	ITEMTYPE_CONSUME,
	ITEMTYPE_ARROW,
	ITEMTYPE_EAT,
	ITEMTYPE_USE_SKILL,
	ITEMTYPE_USE_PERM,
	ITEMTYPE_USE_SKILL_ENABLEDIALOGBOX,
	ITEMTYPE_USE_DEPLETE_DEST,
	ITEMTYPE_MATERIAL,
	ITEMTYPE_RELIC
};

enum ItemEffectTypes
{
	ITEMEFFECTTYPE_NONE = 0,
	ITEMEFFECTTYPE_ATTACK,
	ITEMEFFECTTYPE_DEFENSE,
	ITEMEFFECTTYPE_ATTACK_ARROW,
	ITEMEFFECTTYPE_HP,
	ITEMEFFECTTYPE_MP,
	ITEMEFFECTTYPE_SP,
	ITEMEFFECTTYPE_HPSTOCK,
	ITEMEFFECTTYPE_GET,
	ITEMEFFECTTYPE_STUDYSKILL,
	ITEMEFFECTTYPE_SHOWLOCATION,
	ITEMEFFECTTYPE_MAGIC,
	ITEMEFFECTTYPE_CHANGEATTR,
	ITEMEFFECTTYPE_ATTACK_MANASAVE,
	ITEMEFFECTTYPE_ADDEFFECT,
	ITEMEFFECTTYPE_MAGICDAMAGESAVE,
	ITEMEFFECTTYPE_OCCUPYFLAG,
	ITEMEFFECTTYPE_DYE,
	ITEMEFFECTTYPE_STUDYMAGIC,
	ITEMEFFECTTYPE_ATTACK_MAXHPDOWN,
	ITEMEFFECTTYPE_ATTACK_DEFENSE,
	ITEMEFFECTTYPE_MATERIAL_ATTR,
	ITEMEFFECTTYPE_FIRMSTAMINAR,
	ITEMEFFECTTYPE_LOTTERY,
	ITEMEFFECTTYPE_ATTACK_SPECABLTY,
	ITEMEFFECTTYPE_DEFENSE_SPECABLTY,
	ITEMEFFECTTYPE_ALTERITEMDROP,
	ITEMEFFECTTYPE_CONSTRUCTIONKIT,
	ITEMEFFECTTYPE_WARMNOCURE,
	ITEMEFFECTTYPE_DEFENSE_ANTIMINE,
	ITEMEFFECTTYPE_ITEMFARM,
	ITEMEFFECTTYPE_SLATES,
	ITEMEFFECTTYPE_JEWELRY,
	ITEMEFFECTTYPE_SOCKET,
	ITEMEFFECTTYPE_PANDORA,
	ITEMEFFECTTYPE_CONTRIBUTION,
	ITEMEFFECTTYPE_WARMCURE
};

enum WandEffectTypes{
	WET_NONE,
	WET_ADDMAGICDMGPCT
};

enum SocketGems : uint8{
	SG_NONE = (uint8)ITEM_NONE,
	SG_REJUGEM7 = 1,
	SG_REJUGEM14,
	SG_REJUGEM21,
	SG_BLOODGEM7,
	SG_BLOODGEM14,
	SG_BLOODGEM21,
	SG_MINDGEM7,
	SG_MINDGEM14,
	SG_MINDGEM21,
	SG_ARMORGEM7,
	SG_ARMORGEM14,
	SG_ARMORGEM21,
	SG_ENCHANTEDGEM2,
	SG_ENCHANTEDGEM4,
	SG_ENCHANTEDGEM6,
	SG_TACTICALGEM3,
	SG_TACTICALGEM5,
	SG_TACTICALGEM7,
	SG_VORTEXGEM,
	SG_PSNGEM10,
	SG_PSNGEM15,
	SG_PSNGEM20,
	SG_CRITGEM1,
	SG_CRITGEM2,
	SG_CRITGEM4,
	SG_VAMPGEM5,
	SG_VAMPGEM10,
	SG_VAMPGEM20,
	SG_DIMNDGEM4,
	SG_DIMNDGEM8,
	SG_DIMNDGEM14,
	SG_REINFRCEGEM1,
	SG_REINFRCEGEM2,
	SG_REINFRCEGEM3,
	SG_MAX
};

static const char * SocketGems[SG_MAX] = {
	"none",
	"RejuGem7",
	"RejuGem14",
	"RejuGem21",
	"BloodGem7",
	"BloodGem14",
	"BloodGem21",
	"MindGem7",
	"MindGem14",
	"MindGem21",
	"ArmorGem7",
	"ArmorGem14",
	"ArmorGem21",
	"EnchantedGem2",
	"EnchantedGem4",
	"EnchantedGem6",
	"TacticalGem3",
	"TacticalGem5",
	"TacticalGem7",
	"VortexGem",
	"PsnGem10",
	"PsnGem15",
	"PsnGem20",
	"CritGem1",
	"CritGem2",
	"CritGem4",
	"VampGem5",
	"VampGem10",
	"VampGem20",
	"DiamondGem4",
	"DiamondGem8",
	"DiamondGem14",
	"ReinforceGem1",
	"ReinforceGem2",
	"ReinforceGem3"
};

#define ITET_UNIQUE_OWNER		1
#define ITET_ID					2
#define ITET_DATE				3
#define ITET_GM_MADE			4

class CItem  
{
public:
	CItem();
	virtual ~CItem();

	bool operator<(CItem * item) const {
		return (strcmp(m_cName, item->m_cName) < 0) ? TRUE : FALSE;
	}

	void InitItemAttr(char cItemIndex = -1);
	bool IsManued()	const { return (m_dwAttribute & 1); }
	uint32 GetMaxSockets() const;
	uint32 GetNUsedSockets() const;
	uint32 GetManuCompletion()	const { return m_sItemSpecEffectValue2 + 100; }
	bool IsVortexed()	const { return m_sockets[0] == SG_VORTEXGEM; }
	void GetGemAttr(char * txt, uint8 gem) const;
	void GetGemAttr(char * txt) const;

	short m_sRelatedSkill;
	BOOL m_bIsForSale;
	char m_cApprValue, m_cCategory;
	short m_sIDnum;
	short m_sSpecialEffect;
	short m_sSpecialEffectValue1, m_sSpecialEffectValue2;
	char  m_cName[21], m_cBuyerName[12];
	char  m_cIndex, m_cItemType, m_cTouchEffectType;
	char  m_cEquipPos;
	char  m_cItemColor;
	char  m_cSpeed;
	char  m_cGenderLimit;
//	short m_sItemEffectType;
	short m_sLevelLimit, m_sItemEffectType;
	short m_sSprite;
	short m_sSpriteFrame;
	short m_sX, m_sY;
	short m_sItemSpecEffectValue1, m_sItemSpecEffectValue2, m_sItemSpecEffectValue3;
	short m_sItemEffectValue1, m_sItemEffectValue2, m_sItemEffectValue3, m_sItemEffectValue4, m_sItemEffectValue5, m_sItemEffectValue6; 
	WORD  m_wCurLifeSpan;
	WORD  m_wMaxLifeSpan;
	WORD  m_wPrice, m_wWeight;
	DWORD m_dwCount;
	DWORD m_dwAttribute, m_dwTempItemAttr;
	DWORD m_dwAuctionItemID, m_dwMinBidAmount, m_dwBuyoutAmount, m_dwEndTime;
	char m_cAuctionStatus;
	uint8 m_sockets[MAXITEMSOCKETS];
	ItemUID ItemUniqueID;
	uint32 m_serverPtr;
};

struct ItemMapComp
{
	bool operator()(CItem * x, CItem * y) const
	{
		return (strcmp(x->m_cName, y->m_cName) < 0) ? TRUE : FALSE;
	}
};

typedef std::multimap<CItem*, uint32, ItemMapComp> ItemMap;
typedef std::multimap<CItem*, uint32, ItemMapComp>::iterator ItemMapIter;

#endif // !defined(AFX_ITEM_H__211A1360_91B9_11D2_B143_00001C7030A6__INCLUDED_)