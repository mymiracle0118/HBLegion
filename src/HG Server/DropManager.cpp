#include "hg.h"
#include <boost\random\mersenne_twister.hpp>
#include <boost\random\uniform_int_distribution.hpp>

#include "droplist.hxx"

extern class CGame *   g_game;
boost::random::mt19937 gen;

DropManager::DropManager(void)
{
	_dropCount = 0;

	for(int npc=0; npc < MAXNPCTYPES; npc++)
	{
		for(int tier=0; tier < MAXNPCDROPTIERS; tier++)
		{
			_npcDrops[npc].tiersNum = 0;
			_npcDrops[npc].tierProbSum = 0;
			_npcDrops[npc].tiers[tier].prob = 0;
			_npcDrops[npc].tiers[tier].dropsNum = 0;
			_npcDrops[npc].tiers[tier].dropProbSum = 0;;
			for(int drop=0; drop < MAXNPCDROPS; drop++)
			{
				_npcDrops[npc].tiers[tier].drops[drop].SetDrop("",0,0,0,0);
			}


			_npcDeleteDrops[npc].tiersNum = 0;
			_npcDeleteDrops[npc].tierProbSum = 0;
			_npcDeleteDrops[npc].tiers[tier].prob = 0;
			_npcDeleteDrops[npc].tiers[tier].dropsNum = 0;
			_npcDeleteDrops[npc].tiers[tier].dropProbSum = 0;
			for(int drop=0; drop < MAXNPCDROPS; drop++)
			{
				_npcDeleteDrops[npc].tiers[tier].drops[drop].SetDrop("",0,0,0,0);
			}
		}
	}
}

DropManager::~DropManager(void)
{
}

void DropManager::Reset()
{
	_dropCount = 0;

	for(int npc=0; npc < MAXNPCTYPES; npc++)
	{
		for(int tier=0; tier < MAXNPCDROPTIERS; tier++)
		{
			_npcDrops[npc].tiersNum = 0;
			_npcDrops[npc].tierProbSum = 0;
			_npcDrops[npc].tiers[tier].prob = 0;
			_npcDrops[npc].tiers[tier].dropsNum = 0;
			_npcDrops[npc].tiers[tier].dropProbSum = 0;;
			for(int drop=0; drop < MAXNPCDROPS; drop++)
			{
				_npcDrops[npc].tiers[tier].drops[drop].SetDrop("",0,0,0,0);
			}


			_npcDeleteDrops[npc].tiersNum = 0;
			_npcDeleteDrops[npc].tierProbSum = 0;
			_npcDeleteDrops[npc].tiers[tier].prob = 0;
			_npcDeleteDrops[npc].tiers[tier].dropsNum = 0;
			_npcDeleteDrops[npc].tiers[tier].dropProbSum = 0;
			for(int drop=0; drop < MAXNPCDROPS; drop++)
			{
				_npcDeleteDrops[npc].tiers[tier].drops[drop].SetDrop("",0,0,0,0);
			}
		}
	}

	InitDrops();
}

void DropManager::InitDrops(void)
{
	try
	{
		_ParseXML();
	}
	catch (const xml_schema::exception& e)
	{
		string exc = "XML ERROR: ";
		exc += e.what();
		LogError(exc);
		return;
	}

	sstream err;
	err << "Total drop items: " << _dropCount;
	PutLogList(err.str());
}

void DropManager::_ParseXML()
{
	std::auto_ptr<droplist_t> dl (droplist ("..\\configs\\droplist.xml"));

	for (droplist_t::npc_t_const_iterator npc (dl->npc_t().begin ());
		npc != dl->npc_t().end ();
		++npc)
	{
		string npcName;
		npcName = npc->name();
		uint32 npcType = g_game->FindNPC(npcName);

		if(npcType == NPC_INVALID)	
		{
			sstream err;
			err << "Unknown NPC in Droplist: "<< npcName;
			LogError(err.str());
			continue;
		}
		else if(HasPrimaryDrop(npcType) || HasSecondaryDrop(npcType))
		{
			sstream err;
			err << "Duplicated NPC in Droplist: "<< npcName;
			LogError(err.str());
			continue;
		}

		for (npc_t::tier_t_const_iterator t (npc->tier_t().begin ());
			t != npc->tier_t().end ();
			++t)
		{
			Tier * tier;
			NpcDrop * drop;

			if(t->type() == "primary")
			{
				drop = &_npcDrops[npcType];
				tier = drop->AddTier(npcName, t->prob());
			}
			else
			{
				drop = &_npcDeleteDrops[npcType];
				tier = _npcDeleteDrops[npcType].AddTier(npcName, t->prob());
			}

			drop->secDropNum = npc->num();

			if(!tier)
			{
				sstream err;
				err << "Too many a Tier in NPC("<< npcName <<')';
				LogError(err.str());	
				continue;
			}

			for (tier_t::item_t_const_iterator item (t->item_t().begin ());
				item != t->item_t().end ();
				++item)
			{
				string itemName = item->name();

				uint32 itemID;
				if(itemName == "nothing")
					itemID = ITEM_NONE;
				else
					itemID = g_game->FindItem(itemName);

				if(itemID == ITEM_INVALID)	
				{
					sstream err;
					err << "Unknown item("<< itemName <<") in NPC("<< npcName << ')';
					LogError(err.str());
					continue;
				}
				if(! tier->AddDrop(itemName, itemID, item->prob(), *item->tempattr(), *item->tempcharge()) )
				{
					sstream err;
					err << "Too many a Drop("<< itemName <<") in NPC("<< npcName <<')';
					LogError(err.str());	
					continue;
				}

				_dropCount++;
			}
			
			/*if (tier->drops[0].itemname == "nothing")
			{	long tempDrop = tier->dropProbSum-tier->drops[0].prob;
				if (tempDrop < 0)
				{	sstream err;
					err << "Insufficient \"nothing\" / fictitious drop space in NPC("<< npcName <<')';
					LogError(err.str());
					continue;
				}
				tier->dropProbSum += tempDrop;
				tier->drops[0].prob += tempDrop;
			}*/
		}
	}
	
	/*const NpcDrop * drop2;
	const Tier * tier2 = 0;
	char output[128];
	long fixeddropProbSum;*/

	// primary drop
	/*for(int l=0; l < MAXNPCTYPES; l++) {
		drop2 = &_npcDrops[l];

		for(int t=0; t < drop2->tiersNum; t++)
		{	tier2 = &drop2->tiers[t];
			sprintf(output, "%s:", drop2->npcname.c_str());
			LogError(output);
			for(int d=0; d < tier2->dropsNum; d++)
			{	sprintf(output, "     %s (%3.5f%%) - Kill About %d", tier2->drops[d].itemname.c_str(), (float)100*((float)6500/(float)10000)*((float)1500/(float)10000)*((float)drop2->tiers[t].prob/(float)drop2->tierProbSum)*((float)tier2->drops[d].prob/(float)tier2->dropProbSum), (int)((float)100/((float)tier2->dropsNum*((float)100*((float)6500/(float)10000)*((float)1500/(float)10000)*((float)drop2->tiers[t].prob/(float)drop2->tierProbSum)*((float)tier2->drops[d].prob/(float)tier2->dropProbSum)))));
				LogError(output);
			}
		}
	}*/
	
	// secondary drop
	/*for(int l=0; l < MAXNPCTYPES; l++) {
		drop2 = &_npcDeleteDrops[l];
		for(int t=0; t < drop2->tiersNum; t++)
		{	tier2 = &drop2->tiers[t];
			fixeddropProbSum = 0;
			sprintf(output, "%s:", drop2->npcname.c_str());
			LogError(output);
			for(int d=0; d < tier2->dropsNum; d++)
			{	if (strcmp(tier2->drops[d].itemname.c_str(), "nothing") != 0) {
					sprintf(output, "     %s (%3.5f%%) - Kill About %d", tier2->drops[d].itemname.c_str(), (float)100*((float)drop2->tiers[t].prob/(float)drop2->tierProbSum)*((float)tier2->drops[d].prob/(float)tier2->dropProbSum), (int)((float)100/((float)100*((float)drop2->tiers[t].prob/(float)drop2->tierProbSum)*((float)tier2->drops[d].prob/(float)fixeddropProbSum))));
					LogError(output);
				}else {
					fixeddropProbSum = tier2->drops[d].prob;
				}
			}
		}
	}*/
}

long DropManager::Roll(const CNpc * npc, short * tempAttr, short * tempCharge, long dropType) const
{
	const NpcDrop * drop;
	const Tier * tier = 0;
	float factor = 1.0f;

	switch(dropType)
	{
	case ONNPCDEATH:
		drop = &_npcDrops[npc->m_sType];
		break;
	case ONNPCDELETE:
		drop = &_npcDeleteDrops[npc->m_sType];
		if(CClient * killer = npc->GetKiller())
			factor = killer->GetDropFactor();

		if(g_game->m_iMiddlelandMapIndex != -1 && g_game->m_iMiddlelandMapIndex == npc->m_cMapIndex)
		{
			factor *= 0.9f;
		}

		if(npc->IsHighlyTrained())
			factor *= 0.7f; 
		break;
	default:
		return ITEM_NONE;
	}

	long tierRoll = dice(1, drop->tierProbSum);
	for(int n=0; n < drop->tiersNum; n++)
	{
		if(tierRoll <= drop->tiers[n].prob){
			tier = &drop->tiers[n];
			break;
		}
		tierRoll -= drop->tiers[n].prob;
	}

	if(!tier || tier->dropsNum == 0) return ITEM_NONE;

	/*long firstDropProb = tier->drops[0].prob * factor;

	long dropRoll = dice(1, tier->dropProbSum - (tier->drops[0].prob - firstDropProb));

	if(dropRoll <= firstDropProb){
		return tier->drops[0].itemID;
	}
	dropRoll -= firstDropProb;*/
	
	/*long dropRoll = 1;
	float diceMultiplier = tier->dropProbSum/RAND_MAX;
	if (tier->dropProbSum > RAND_MAX)
	{	for(int i=0; i < ceil(diceMultiplier); i++)
		{	std::time(0);
			if (i == ceil(diceMultiplier)-1) {
				long lastRoll = rand();
				if (lastRoll > tier->dropProbSum-RAND_MAX*i) lastRoll = tier->dropProbSum-RAND_MAX*i;
				dropRoll += lastRoll;
			}else dropRoll += rand();
		}
	}else dropRoll = dice(1, tier->dropProbSum);*/

	std::time(0);
	boost::random::uniform_int_distribution<> dist(1, tier->dropProbSum);
	long dropRoll = dist(gen);
	/*char cTxt[128];
	sprintf(cTxt, "%d", dropRoll);
	PutLogList(cTxt);*/

	for(int n=0; n < tier->dropsNum; n++) // for(int n=1; n < tier->dropsNum; n++)
	{
		if(dropRoll <= tier->drops[n].prob){
			if (tier->drops[n].itemID == ITEM_NONE && dropType == ONNPCDELETE && npc->m_sType == 81) {
				std::time(0);
				boost::random::uniform_int_distribution<> dist(1, 19);
				long setDropRoll = dist(gen);
				if (tier->drops[setDropRoll].tempattr < 0 || tier->drops[setDropRoll].tempcharge < 0) {
					*tempAttr = NULL;
					*tempCharge = NULL;
				}else {
					*tempAttr = tier->drops[setDropRoll].tempattr;
					*tempCharge = tier->drops[setDropRoll].tempcharge;
				}
				return tier->drops[setDropRoll].itemID;
			}else {
				if (tier->drops[n].tempattr < 0 || tier->drops[n].tempcharge < 0) {
					*tempAttr = NULL;
					*tempCharge = NULL;
				}else {
					*tempAttr = tier->drops[n].tempattr;
					*tempCharge = tier->drops[n].tempcharge;
				}
				return tier->drops[n].itemID;
			}
		}
		dropRoll -= tier->drops[n].prob;
	}

	return ITEM_NONE;
}

bool DropManager::HasPrimaryDrop(NpcID npcType) const
{
	return (_npcDrops[npcType].tiersNum > 0) ? TRUE : FALSE;
}
bool DropManager::HasPrimaryDrop(const CNpc * npc) const
{
	return (_npcDrops[npc->m_sType].tiersNum > 0) ? TRUE : FALSE;
}

bool DropManager::HasSecondaryDrop(NpcID npcType) const
{
	return (_npcDeleteDrops[npcType].tiersNum > 0) ? TRUE : FALSE;
}
bool DropManager::HasSecondaryDrop(const CNpc * npc) const
{
	return (_npcDeleteDrops[npc->m_sType].tiersNum > 0) ? TRUE : FALSE;
}