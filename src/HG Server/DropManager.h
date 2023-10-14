#ifndef DROPMANAGER_H
#define DROPMANAGER_H
#pragma once

#include "..\shared\npcType.h"
#include "..\shared\items.h"
#include "math.h"

#define MAXNPCDROPTIERS		4
#define MAXNPCDROPS			100

#define ONNPCDEATH			0
#define ONNPCDELETE			1

class CNpc;

struct TierDrop
{
	long prob, tempattr, tempcharge;
	long itemID;
	string itemname;

	void SetDrop(string pItemName, long itemid, long probability, long tempattribute, long tempcharges)
	{
		itemname = pItemName;
		itemID = itemid;
		prob = probability;
		tempattr = tempattribute;
		tempcharge = tempcharges;
	}
};

struct Tier
{
	long prob;
	long dropsNum;
	long dropProbSum;
	TierDrop drops[MAXNPCDROPS];

	bool AddDrop(string pItemname, long itemid, long probability, const long tempattribute, const long tempcharges)
	{
		if(dropsNum >= MAXNPCDROPS)
			return FALSE;
		dropProbSum += probability;
		drops[dropsNum].SetDrop(pItemname, itemid, probability, tempattribute, tempcharges);
		++dropsNum;
		return TRUE;
	}
};

struct NpcDrop
{
	long tiersNum;
	long tierProbSum;
	Tier tiers[MAXNPCDROPTIERS];
	uint32 secDropNum;
	string npcname;

	Tier * AddTier(string pNpcName, long probability)
	{
		if(tiersNum >= MAXNPCDROPTIERS)
			return NULL;
		npcname = pNpcName;
		tierProbSum += probability;
		tiers[tiersNum].prob = probability;
		++tiersNum;
		return &tiers[tiersNum-1];
	}
};

class DropManager
{
public:
	DropManager();
	~DropManager();
	void InitDrops();

	void Reset();
	long Roll(const CNpc * npc, short * tempAttr, short * tempCharge, const long dropType = ONNPCDEATH) const;
	bool HasPrimaryDrop(NpcID id) const;
	bool HasPrimaryDrop(const CNpc * id) const;
	bool HasSecondaryDrop(NpcID id) const;
	bool HasSecondaryDrop(const CNpc * id) const;
	uint32 GetSecDropNum(NpcID npcType)	const { return _npcDeleteDrops[npcType].secDropNum; };

protected:
	void _ParseXML();

	uint32 _dropCount;
	NpcDrop _npcDrops[MAXNPCTYPES];
	NpcDrop _npcDeleteDrops[MAXNPCTYPES];
};

#endif