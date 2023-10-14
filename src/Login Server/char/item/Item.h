#ifndef _ITEM_H_
#define _ITEM_H_
#include <windows.h>
#include "..\..\defs.hpp"
#include "..\..\main.h"
#include "..\..\..\shared\items.h"

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
	SG_MAX
};

//==============================================================================
class cItem
{
 public:	

	cItem();
	~cItem();
	void MakeItemInfo(char *pItemName, DWORD  dwItemCount, sWORD  wTouchEffectType, sWORD wTouchEffectValue1, sWORD wTouchEffectValue2, sWORD wTouchEffectValue3, sWORD wItemSpecEffectValue1, sWORD wItemSpecEffectValue2, sWORD wItemSpecEffectValue3, WORD wCurLifeSpan, DWORD dwAttribute, DWORD dwTempItemAttr, BYTE bItemColor, BOOL bPutItemInBank = TRUE, BOOL bIsItemEquipped = FALSE, sWORD wItemPosX = NULL, sWORD wItemPosY = NULL);
	
	char   ItemName[25];
	BYTE   ItemColor, DupItemCode;
	sWORD  TouchEffectType, TouchEffectValue1, TouchEffectValue2, TouchEffectValue3, ItemSpecEffectValue1,
			ItemSpecEffectValue2, ItemSpecEffectValue3, ItemPosX, ItemPosY;
	WORD   CurLifeSpan;
	DWORD  ItemCount, Attribute, TempItemAttr;
	uint8 socket1, socket2, socket3;
	uint64 ItemUniqueID;
	BOOL   IsItemEquipped, PutItemInBank;
};
//==============================================================================
#endif