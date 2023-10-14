#include "hg.h"

extern class CGame *   g_game;
extern class CMap	**	g_mapList;
extern class CNpc **	g_npcList;
extern class CClient **	g_clientList;

extern CNpc dummyNpc;

CAstoria::CAstoria(const EventType type) : _eventType(type)
{
	for(int i=0; i < MAXSIDES; i++)
	{
		_sideStats[i].deaths = 0;
		_sideStats[i].kills = 0;
	}

	_relic = NULL;
	_relicHolder = NULL;
	_relicOwnedSide = NEUTRAL;
	_relicOwnedTime = 0;
	m_relicHolderSteps = 0;

	for(int i=0; i < MAXALTARS; i++)
	{
		_shields[i] = -1;
	}

	g_game->ShuffleAstoriaBasePos();

	switch(_eventType)
	{
	case ET_CAPTURE:
		_CaptureStart();
		break;
	case ET_DESTROY_SHIELD:
		_DestroyShieldStart();
		break;
	}

	g_game->CreateAstoriaFlags();

	m_beginTime = timeGetTime();
}

CAstoria::~CAstoria(void)
{
	switch(_eventType)
	{
	case ET_CAPTURE:
		_CaptureEnd();
		break;
	case ET_DESTROY_SHIELD:
		_DestroyShieldEnd();
		break;
	}

	g_game->DeleteFlags();
}

void CAstoria::PlayerGetRelic(CClient * player)
{ 
	_relicHolder = player;
	_relicHolder->SetStatusFlag(STATUS_RELICHOLDER, TRUE);
	_relicHolder->RemoveMagicEffect(MAGICTYPE_INVISIBILITY);

	g_game->NotifyRelicGrabbed(_relicHolder);
}

void CAstoria::PlayerDropRelic(CClient * player)
{ 
	player->SetStatusFlag(STATUS_RELICHOLDER, FALSE);
	_relicHolder = NULL;

	_relicPos.x = player->m_sX;
	_relicPos.y = player->m_sY;

	m_relicHolderSteps = 0;

	if(_IsRelicInAltar())
	{
		_relicOwnedTime = timeGetTime();
		g_game->NotifyRelicInAltar(_relicOwnedSide);
	}
}

bool CAstoria::CheckVictory()
{
	switch(_eventType)
	{
	case ET_CAPTURE:
		if(_IsRelicInAltar() && timeGetTime() - _relicOwnedTime > RELICVICTORYTIME)
		{
			return TRUE;
		}
		break;
	case ET_DESTROY_SHIELD:
		uint32 shields = 0;

		for(int i=0; i < MAXALTARS; i++)
		{
			CNpc * shield = g_npcList[ _shields[i] ];
			if(shield && !shield->IsDead() && shield->m_sType == NPC_AS) shields++;
		}
		/*for(int i=0; i < MAXALTARS; i++)
		{
			if (shield && !shield->IsDead()) shields++;
		}*/

		if(shields < 2)
			return true;

		break;
	}
	return FALSE;
}

Side CAstoria::GetVictoriousSide() const
{
	switch(_eventType)
	{
	case ET_CAPTURE:
		return _relicOwnedSide;
	case ET_DESTROY_SHIELD:
		for(int i=0; i < MAXALTARS; i++)
		{
			CNpc * shield = g_npcList[ _shields[i] ];
			if(shield && !shield->IsDead() && shield->m_sType == NPC_AS)
				return (Side)shield->m_side;
		}
		break;
	}
	return NEUTRAL;
}

bool CAstoria::_IsRelicInAltar()
{
	if(_relicHolder) return FALSE;

	for(int i = 0; i < MAXALTARS; i++)
	{
		if(_relicPos == altarPos[i])
		{
			_relicOwnedSide = g_game->m_astoriaBasePos[i];
			return TRUE;
		}
	}
	return FALSE;
}

void CAstoria::_CaptureStart()
{
	_relic = new CItem(); 
	_relic->InitItemAttr(ITEM_RELIC);
	_relicPos = relicStartPos;

	g_mapList[g_game->m_iAstoriaMapIndex]->bSetItem(_relicPos.x, _relicPos.y, _relic);
	g_game->SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, COMMONTYPE_SETITEM, g_game->m_iAstoriaMapIndex,
		_relicPos.x, _relicPos.y, _relic->m_sSprite, _relic->m_sSpriteFrame, _relic->m_cName, _relic->m_cItemColor);
}

void CAstoria::_CaptureEnd()
{
	if(_relicHolder)
	{
		uint32 i = _relicHolder->HasItem(ITEM_RELIC);
		if(i != ITEM_NONE)
			g_game->DropItemHandler(_relicHolder->m_handle, i, 1, _relicHolder->m_pItemList[i]->m_cName, FALSE);
	}

	short nextIDnum;
	char  nextItemColor;
	DWORD nextItemAttribute;

	g_mapList[g_game->m_iAstoriaMapIndex]->pGetItem(_relicPos.x, _relicPos.y, &nextIDnum, &nextItemColor, &nextItemAttribute);

	g_game->SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, COMMONTYPE_SETITEM, g_game->m_iAstoriaMapIndex, _relicPos.x, _relicPos.y, 
		nextIDnum, nextItemColor, NULL, nextItemAttribute);

	if(_relic)
		delete _relic;
}

void CAstoria::PlayerKill(CClient * killer, CClient * victim)
{
	if(killer->m_side != victim->m_side)
		_sideStats[killer->m_side].kills++;
	_sideStats[victim->m_side].deaths++;

	g_game->NotifyEventStats(_sideStats);
}

void CAstoria::_DestroyShieldStart()
{
	Point nationAltars[MAXSIDES];

	for(int i=0; i < MAXALTARS; i++)
	{
		nationAltars[ g_game->m_astoriaBasePos[i] ] = altarPos[ i ];
	}

	char cName[NPCNAMELENGTH], cNpcWayPoint[11];
	ZeroMemory(cNpcWayPoint, sizeof(cNpcWayPoint));

	CMap * map = g_mapList[g_game->m_iAstoriaMapIndex];
	int iNamingValue = map->GetEmptyNamingValue(cName, g_game->m_iAstoriaMapIndex);

	int x,y;

	x = nationAltars[ARESDEN].x;
	y = nationAltars[ARESDEN].y;
	if(!g_game->bCreateNewNpc("AG-Aresden", cName, g_game->m_iAstoriaMapIndex, 0, MOVETYPE_RANDOM, 
		&x, &y, cNpcWayPoint, NULL, NULL, -1, FALSE)) 
	{
		map->SetNamingValueEmpty(iNamingValue);
		ERR("Failed to create Astoria Shield");
	}

	x = nationAltars[ELVINE].x;
	y = nationAltars[ELVINE].y;
	if(!g_game->bCreateNewNpc("AG-Elvine", cName, g_game->m_iAstoriaMapIndex, 0, MOVETYPE_RANDOM, 
		&x, &y, cNpcWayPoint, NULL, NULL, -1, FALSE)) 
	{
		map->SetNamingValueEmpty(iNamingValue);
		ERR("Failed to create Astoria Shield");
	}
		
	uint32 shields = 0;
	for(int i = 0; i < MAXNPCS; i++)
	{
		CNpc * npc = g_npcList[i];
		if(npc && npc->m_cMapIndex == g_game->m_iAstoriaMapIndex && npc->m_sType == NPC_AS)
		{
			_shields[ shields ] = i;
			shields++;
		}
	}
}

void CAstoria::_DestroyShieldEnd() const
{
	for (int i = 0; i < MAXALTARS; i++)
	{
		if(_shields[i] != -1)
			g_game->DeleteNpc( g_npcList[ _shields[i] ]->m_handle );
	}
}

void CAstoria::NotifyShieldHP(CNpc * npc) const
{
	for(int i = 1; i < MAXCLIENTS; i++) 
	{	
		if (g_clientList[i] && g_clientList[i]->m_cMapIndex == g_game->m_iAstoriaMapIndex) 
		{	
			g_clientList[i]->Notify(NULL, NOTIFY_SHIELDHP, npc->m_side, npc->m_iHP);
		}	
	}
}

CNpc * CAstoria::GetShield(Side side) const
{
	for(int i = 0; i < MAXALTARS; i++) 
	{	
		if(_shields[i] != -1 && g_npcList[ _shields[i] ]->m_side == side)
			return g_npcList[ _shields[i] ];
	}
	
	return &dummyNpc;
}