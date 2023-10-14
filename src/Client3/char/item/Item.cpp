// Item.cpp: implementation of the CItem class.
//
//////////////////////////////////////////////////////////////////////

#include "Item.h"
#include "..\\..\\..\\shared\\common.h"
#include "..\\..\\..\\shared\\items.h"
#include "..\..\lan_eng.h"

extern class CItem ** g_itemConfigList;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItem::CItem()
{
	ZeroMemory(m_cName, sizeof(m_cName));
	m_sSprite = 0;
	m_sSpriteFrame = 0;
	m_dwAttribute = m_dwTempItemAttr = NULL;
	m_cTouchEffectType = 0;
	m_sItemSpecEffectValue1 = 0;
	m_sItemSpecEffectValue2 = 0;
	m_sItemSpecEffectValue3 = 0;
	m_serverPtr = NULL;
	
	for(int i = 0; i < MAXITEMSOCKETS; i++)
		m_sockets[i] = SG_NONE;
}

void CItem::InitItemAttr(char cItemIndex)
{
	m_cIndex = cItemIndex;
	ZeroMemory(m_cName, sizeof(m_cName));
	memcpy(m_cName, g_itemConfigList[m_sIDnum]->m_cName, 20);
	m_cItemType = g_itemConfigList[m_sIDnum]->m_cItemType;
	m_cEquipPos = g_itemConfigList[m_sIDnum]->m_cEquipPos;
	m_sLevelLimit = g_itemConfigList[m_sIDnum]->m_sLevelLimit;
	m_cGenderLimit = g_itemConfigList[m_sIDnum]->m_cGenderLimit;
	m_wWeight = g_itemConfigList[m_sIDnum]->m_wWeight;
	m_sSprite = g_itemConfigList[m_sIDnum]->m_sSprite;
	m_sSpriteFrame = g_itemConfigList[m_sIDnum]->m_sSpriteFrame;
	
	m_sItemEffectType = g_itemConfigList[m_sIDnum]->m_sItemEffectType;
	m_sItemEffectValue1 = g_itemConfigList[m_sIDnum]->m_sItemEffectValue1;
	m_sItemEffectValue2 = g_itemConfigList[m_sIDnum]->m_sItemEffectValue2;
	m_sItemEffectValue3 = g_itemConfigList[m_sIDnum]->m_sItemEffectValue3;
	m_sItemEffectValue4 = g_itemConfigList[m_sIDnum]->m_sItemEffectValue4;
	m_sItemEffectValue5 = g_itemConfigList[m_sIDnum]->m_sItemEffectValue5;
	m_sItemEffectValue6 = g_itemConfigList[m_sIDnum]->m_sItemEffectValue6;
	m_wMaxLifeSpan = g_itemConfigList[m_sIDnum]->m_wMaxLifeSpan;

	m_wPrice = g_itemConfigList[m_sIDnum]->m_wPrice;
	m_wWeight = g_itemConfigList[m_sIDnum]->m_wWeight;
	m_cApprValue = g_itemConfigList[m_sIDnum]->m_cApprValue;
	m_cSpeed = g_itemConfigList[m_sIDnum]->m_cSpeed;
	m_sLevelLimit = g_itemConfigList[m_sIDnum]->m_sLevelLimit;
	m_cGenderLimit = g_itemConfigList[m_sIDnum]->m_cGenderLimit;

	m_sSpecialEffect = g_itemConfigList[m_sIDnum]->m_sSpecialEffect;
	//m_sSpecialEffectValue1 = g_itemConfigList[m_sIDnum]->m_sSpecialEffectValue1; // changed by HG server
	//m_sSpecialEffectValue2 = g_itemConfigList[m_sIDnum]->m_sSpecialEffectValue2; // changed by HG server

	m_sRelatedSkill = g_itemConfigList[m_sIDnum]->m_sRelatedSkill;
	m_cCategory = g_itemConfigList[m_sIDnum]->m_cCategory;

	m_bIsForSale = g_itemConfigList[m_sIDnum]->m_bIsForSale;
}

CItem::~CItem()
{
	
}

uint32 CItem::GetMaxSockets() const
{
	if(m_sockets[0] == SG_VORTEXGEM) return 2;
	else if(!IsManued()) return 0;
	
	if(memcmp(m_cName,"Knight", 6) == 0) return 0;
	if(memcmp(m_cName,"Wizard", 6) == 0) return 0;
	if(memcmp(m_cName,"BarbarianHammer", 15) == 0) return 0;

	switch(m_cEquipPos)
	{
	case EQUIPPOS_BODY:
		if(GetManuCompletion() < 100) return 1;
		else return 2;
		break;
		
	case EQUIPPOS_ARMS:
	case EQUIPPOS_PANTS:
		if(GetManuCompletion() >= 100 && GetManuCompletion() < 150) return 1;
		else if(GetManuCompletion() >= 150) return 2;
		break;

	case EQUIPPOS_HEAD:
	case EQUIPPOS_LHAND:
	case EQUIPPOS_RHAND:
		if(GetManuCompletion() >= 130) return 1;
		break;
	}

	return 0;
}

uint32 CItem::GetNUsedSockets() const
{
	uint32 n = 0;
	for(int i = 0; i < GetMaxSockets(); i++)
	{
		if(m_sockets[i] != SG_NONE && m_sockets[i] != SG_VORTEXGEM)
		{
			n++;
		}
	}

	return n;
}

void CItem::GetGemAttr(char * txt, uint8 gem) const
{
	switch(gem)
	{
	case SG_REJUGEM7:
		sprintf(txt, GET_ITEM_NAME29, 7);
		break;
	case SG_REJUGEM14:
		sprintf(txt, GET_ITEM_NAME29, 14);
		break;
	case SG_REJUGEM21:
		sprintf(txt, GET_ITEM_NAME29, 21);
		break;
	case SG_BLOODGEM7:
		sprintf(txt, GET_ITEM_NAME27, 7);
		break;
	case SG_BLOODGEM14:
		sprintf(txt, GET_ITEM_NAME27, 14);
		break;
	case SG_BLOODGEM21:
		sprintf(txt, GET_ITEM_NAME27, 21);
		break;
	case SG_MINDGEM7:
		sprintf(txt, GET_ITEM_NAME30, 7);
		break;
	case SG_MINDGEM14:
		sprintf(txt, GET_ITEM_NAME30, 14);
		break;
	case SG_MINDGEM21:
		sprintf(txt, GET_ITEM_NAME30, 21);
		break;
	case SG_ARMORGEM7:
		sprintf(txt, GET_ITEM_NAME26, 7);
		break;
	case SG_ARMORGEM14:
		sprintf(txt, GET_ITEM_NAME26, 14);
		break;
	case SG_ARMORGEM21:
		sprintf(txt, GET_ITEM_NAME26, 21);
		break;
	case SG_ENCHANTEDGEM2:
		sprintf(txt, GET_ITEM_NAME32, 2);
		break;
	case SG_ENCHANTEDGEM4:
		sprintf(txt, GET_ITEM_NAME32, 4);
		break;
	case SG_ENCHANTEDGEM6:
		sprintf(txt, GET_ITEM_NAME32, 6);
		break;
	case SG_TACTICALGEM3:
		sprintf(txt, GET_ITEM_NAME31, 3);
		break;
	case SG_TACTICALGEM5:
		sprintf(txt, GET_ITEM_NAME31, 5);
		break;
	case SG_TACTICALGEM7:
		sprintf(txt, GET_ITEM_NAME31, 7);
		break;
	case SG_PSNGEM10:
		sprintf(txt, GET_ITEM_NAME15, 10);
		break;
	case SG_PSNGEM15:
		sprintf(txt, GET_ITEM_NAME15, 15);
		break;
	case SG_PSNGEM20:
		sprintf(txt, GET_ITEM_NAME15, 20);
		break;
	case SG_CRITGEM1:
		sprintf(txt, GET_ITEM_NAME14, 1);
		break;
	case SG_CRITGEM2:
		sprintf(txt, GET_ITEM_NAME14, 2);
		break;
	case SG_CRITGEM4:
		sprintf(txt, GET_ITEM_NAME14, 4);
		break;
	case SG_VAMPGEM5:
		sprintf(txt, GET_ITEM_NAME40, 5);
		break;
	case SG_VAMPGEM10:
		sprintf(txt, GET_ITEM_NAME40, 10);
		break;
	case SG_VAMPGEM20:
		sprintf(txt, GET_ITEM_NAME40, 20);
		break;
	case SG_DIMNDGEM4:
		sprintf(txt, GET_ITEM_NAME25, 4);
		break;
	case SG_DIMNDGEM8:
		sprintf(txt, GET_ITEM_NAME25, 8);
		break;
	case SG_DIMNDGEM14:
		sprintf(txt, GET_ITEM_NAME25, 14);
		break;
	case SG_REINFRCEGEM1:
		sprintf(txt, GET_ITEM_NAME39, 500);
		break;
	case SG_REINFRCEGEM2:
		sprintf(txt, GET_ITEM_NAME39, 1000);
		break;
	case SG_REINFRCEGEM3:
		sprintf(txt, GET_ITEM_NAME39, 2000);
		break;
	}
}

void CItem::GetGemAttr(char * txt) const
{
	if(!strcmp(m_cName, "RejuGem7"))
	{
		sprintf(txt, GET_ITEM_NAME29, 7);
	}else if(!strcmp(m_cName, "RejuGem14"))
	{
		sprintf(txt, GET_ITEM_NAME29, 14);
	}else if(!strcmp(m_cName, "RejuGem21"))
	{
		sprintf(txt, GET_ITEM_NAME29, 21);
	}else if(!strcmp(m_cName, "BloodGem7"))
	{
		sprintf(txt, GET_ITEM_NAME27, 7);
	}else if(!strcmp(m_cName, "BloodGem14"))
	{
		sprintf(txt, GET_ITEM_NAME27, 14);
	}else if(!strcmp(m_cName, "BloodGem21"))
	{
		sprintf(txt, GET_ITEM_NAME27, 21);
	}else if(!strcmp(m_cName, "MindGem7"))
	{
		sprintf(txt, GET_ITEM_NAME30, 7);
	}else if(!strcmp(m_cName, "MindGem14"))
	{
		sprintf(txt, GET_ITEM_NAME30, 14);
	}else if(!strcmp(m_cName, "MindGem21"))
	{
		sprintf(txt, GET_ITEM_NAME30, 21);
	}else if(!strcmp(m_cName, "ArmorGem7"))
	{
		sprintf(txt, GET_ITEM_NAME26, 7);
	}else if(!strcmp(m_cName, "ArmorGem14"))
	{
		sprintf(txt, GET_ITEM_NAME26, 14);
	}else if(!strcmp(m_cName, "ArmorGem21"))
	{
		sprintf(txt, GET_ITEM_NAME26, 21);
	}else if(!strcmp(m_cName, "EnchantedGem2"))
	{
		sprintf(txt, GET_ITEM_NAME32, 2);
	}else if(!strcmp(m_cName, "EnchantedGem4"))
	{
		sprintf(txt, GET_ITEM_NAME32, 4);
	}else if(!strcmp(m_cName, "EnchantedGem6"))
	{
		sprintf(txt, GET_ITEM_NAME32, 6);
	}else if(!strcmp(m_cName, "TacticalGem3"))
	{
		sprintf(txt, GET_ITEM_NAME31, 3);
	}else if(!strcmp(m_cName, "TacticalGem5"))
	{
		sprintf(txt, GET_ITEM_NAME31, 5);
	}else if(!strcmp(m_cName, "TacticalGem7"))
	{
		sprintf(txt, GET_ITEM_NAME31, 7);
	}else if(!strcmp(m_cName, "PsnGem10"))
	{
		sprintf(txt, GET_ITEM_NAME15, 10);
	}else if(!strcmp(m_cName, "PsnGem15"))
	{
		sprintf(txt, GET_ITEM_NAME15, 15);
	}else if(!strcmp(m_cName, "PsnGem20"))
	{
		sprintf(txt, GET_ITEM_NAME15, 20);
	}else if(!strcmp(m_cName, "CritGem1"))
	{
		sprintf(txt, GET_ITEM_NAME14, 1);
	}else if(!strcmp(m_cName, "CritGem2"))
	{
		sprintf(txt, GET_ITEM_NAME14, 2);
	}else if(!strcmp(m_cName, "CritGem4"))
	{
		sprintf(txt, GET_ITEM_NAME14, 4);
	}else if(!strcmp(m_cName, "VampGem5"))
	{
		sprintf(txt, GET_ITEM_NAME40, 5);
	}else if(!strcmp(m_cName, "VampGem10"))
	{
		sprintf(txt, GET_ITEM_NAME40, 10);
	}else if(!strcmp(m_cName, "VampGem20"))
	{
		sprintf(txt, GET_ITEM_NAME40, 20);
	}else if(!strcmp(m_cName, "DiamondGem4"))
	{
		sprintf(txt, GET_ITEM_NAME25, 4);
	}else if(!strcmp(m_cName, "DiamondGem8"))
	{
		sprintf(txt, GET_ITEM_NAME25, 8);
	}else if(!strcmp(m_cName, "DiamondGem14"))
	{
		sprintf(txt, GET_ITEM_NAME25, 14);
	}else if(!strcmp(m_cName, "ReinforceGem1"))
	{
		sprintf(txt, GET_ITEM_NAME39, 500);
	}else if(!strcmp(m_cName, "ReinforceGem2"))
	{
		sprintf(txt, GET_ITEM_NAME39, 1000);
	}else if(!strcmp(m_cName, "ReinforceGem3"))
	{
		sprintf(txt, GET_ITEM_NAME39, 2000);
	}
}