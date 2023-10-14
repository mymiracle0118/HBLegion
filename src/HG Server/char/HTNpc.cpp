#include "..\\hg.h"

extern class CGame *   g_game;
extern class CMap	**	g_mapList;
extern class CClient ** g_clientList;
extern class CNpc **	g_npcList;
extern class CMagic ** g_magicConfigList;

extern char _tmp_cTmpDirX[9];
extern char _tmp_cTmpDirY[9];

CHTNpc::CHTNpc(char * pName5, int NpcH) : CNpc(pName5, NpcH)
{
	_minions = 0;
}


CHTNpc::~CHTNpc(void)
{
}

void CHTNpc::Behavior_Move()
{
	int result = dice(1, 10000);

	switch(m_sType)
	{
	case NPC_LICHE:
		if(result <= 4000 && _SummonMinion())
		{
			m_sBehaviorTurnCount++;
		} else {
			CNpc::Behavior_Move();
		}
		break;

	default:
		CNpc::Behavior_Move();
		break;
	}
}

void CHTNpc::Behavior_Attack()
{
	Unit * target = NULL;
	if(m_cTargetType == OWNERTYPE_PLAYER)
		target = g_clientList[m_iTargetIndex];
	else if(m_cTargetType == OWNERTYPE_NPC)
		target = g_npcList[m_iTargetIndex];

	int result = dice(1, 10000);
	bool fallback = true;

	switch(m_sType)
	{
	case NPC_LICHE:
		if(result <= 3000 && _SummonMinion())
			return;
		
		if(target && abs(m_sX - target->m_sX) <= 9 && abs(m_sY - target->m_sY) <= 7)
		{
			if(m_iMP >= g_magicConfigList[MAGIC_CANCELLATION]->m_manaCost && target->m_cMagicEffectStatus[MAGICTYPE_PROTECT])
			{
				Cast(target, MAGIC_CANCELLATION);
				fallback = false;
			} else if(result <= 900 && m_iMP >= g_magicConfigList[MAGIC_ICESTORM]->m_manaCost)
			{
				Cast(target, MAGIC_ICESTORM);
				fallback = false;
			} else if(result <= 7000 && m_iMP >= g_magicConfigList[MAGIC_EARTHWORMSTRIKE]->m_manaCost)
			{
				Cast(target, MAGIC_EARTHWORMSTRIKE);
				fallback = false;
			} else if(m_iMP >= g_magicConfigList[MAGIC_MASSICESTRIKE]->m_manaCost)
			{
				Cast(target, MAGIC_MASSICESTRIKE);
				fallback = false;
			}
		}

		if(fallback)
		{
			CNpc::Behavior_Attack();
		}
		break;

	default:
		CNpc::Behavior_Attack();
		break;
	}
}

void CHTNpc::Behavior_Death(Unit * attacker, int16 dmg)
{
	CNpc::Behavior_Death(attacker, dmg);
	
	switch(m_sType)
	{
	case NPC_LICHE:
		m_iMagicHitRatio = 100;
		magicHandler(m_sX, m_sY, 63); 
		magicHandler(m_sX-2, m_sY-2, 63); 
		magicHandler(m_sX+2, m_sY-2, 63); 
		magicHandler(m_sX-2, m_sY+2, 63); 
		magicHandler(m_sX+2, m_sY+2, 63);
		break;
	}
}

bool CHTNpc::_SummonMinion()
{
	char cName[NPCNAMELENGTH];

	int iNamingValue = g_mapList[m_cMapIndex]->GetEmptyNamingValue(cName, m_cMapIndex);
	if(iNamingValue == -1)
		return false;

	char cNpcName[11];
	ZeroMemory(cNpcName, sizeof(cNpcName));

	uint32 reqmp;
		
	int x = m_sX + dice(1,7) - 4;
	int y = m_sY + dice(1,7) - 4;

	switch(m_sType)
	{
	case NPC_LICHE:
		reqmp = 150;
		if(_minions >= 3 || m_iMP < reqmp)
			return false;

		strcpy(cNpcName, "Troll");
		break;

	default:
		return false;
	}

	char cNpcWaypoint[11];
	if (!g_game->bCreateNewNpc(cNpcName, cName, m_cMapIndex, 9, 
		MOVETYPE_RANDOM, &x, &y, cNpcWaypoint, NULL, NULL, m_side, FALSE)) 
	{
		g_mapList[m_cMapIndex]->SetNamingValueEmpty(iNamingValue);
		return false;
	}
	else
	{
		char cName_Master[NPCNAMELENGTH];
		ZeroMemory(cName_Master, sizeof(cName_Master));
		memcpy(cName_Master, m_cName, NPCNAMELENGTH - 1);

		g_game->bSetNpcFollowMode(cName, cName_Master, m_ownerType);

			m_cDir = CMisc::cGetNextMoveDir(m_sX, m_sY, x, y);
		g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, 
			OBJECTATTACK, m_sX + _tmp_cTmpDirX[m_cDir], m_sY + _tmp_cTmpDirY[m_cDir], 1); 

		_minions++;
		m_iMP -= reqmp;
		return true;
	}
}

void CHTNpc::Behavior_MinionDeath(CNpc * minion)
{
	_minions--;

	switch(m_sType)
	{
	case NPC_LICHE:
		m_iMP += 200;

		int16 dX, dY;
		CClient * killer;
		if(minion->m_killerh != -1 && (killer = g_clientList[ minion->m_killerh ]))
		{
			dX = killer->m_sX;
			dY = killer->m_sY;

			if(killer->m_cMagicEffectStatus[MAGICTYPE_PROTECT])
				minion->magicHandler(dX, dY, MAGIC_CANCELLATION);
		} else {
			dX = minion->m_sX;
			dY = minion->m_sY;
		}

		if(abs(m_sX - dX) <= 9 && abs(m_sY - dY) <= 7)
		{
			int8 dir = CMisc::cGetNextMoveDir(m_sX, m_sY, dX, dY);
			magicHandler(dX + _tmp_cTmpDirX[dir], dY + _tmp_cTmpDirY[dir], MAGIC_BLIZZARD);
		}
		break;
	}
}

void CHTNpc::TargetSearch(short * pTarget, char * pTargetType)
{
	short sX, sY, rX, rY, dX, dY;
	short sDistance, sTempDistance;

	sDistance    = 100;

	sX = m_sX;
	sY = m_sY;

	rX = m_sX - m_cTargetSearchRange;
	rY = m_sY - m_cTargetSearchRange;
	
	Unit * owner;
	int32 invPlayers = 0, visPlayers = 0;
	Unit * target = NULL, *invTarget = NULL;

	for(int ix = rX; ix < rX + m_cTargetSearchRange*2 + 1; ix++)
	{
		for(int iy = rY; iy < rY + m_cTargetSearchRange*2 + 1; iy++) 
		{
			owner = g_mapList[m_cMapIndex]->GetOwner(ix, iy);
			if (!owner || owner == this) continue;
			
			int iPKCount = 0;
			dX = owner->m_sX;
			dY = owner->m_sY;
			owner->m_side = owner->m_side;

			if(owner->IsPlayer())
			{
				if(g_clientList[owner->m_handle]->IsNoAggro()) 
					continue;

				iPKCount    = g_clientList[owner->m_handle]->m_iPKCount;
			}

			if (m_side < 10) {
				if (owner->m_side == 0) {
					if (iPKCount == 0) continue;
				}
				else {
					if ((iPKCount == 0 && owner->m_side == m_side) || m_side == NEUTRAL)
						continue;
				}
			}
			else {
				if ((owner->IsNPC() && owner->m_side == NEUTRAL) || owner->m_side == m_side)
					continue;
			}

			if(owner->IsInvisible())
			{
				invPlayers++;
				invTarget = owner;
				if(m_sType != NPC_LICHE && m_cSpecialAbility != 1)
				{
					continue;
				}
			} else {
				visPlayers++;
			}

			if (abs(sX - dX) >= abs(sY - dY)) 
				sTempDistance = abs(sX - dX);
			else sTempDistance = abs(sY - dY);

			if (sTempDistance <	sDistance) {
				sDistance = sTempDistance;
				target = owner;
			}
		}
	}

	if((visPlayers == 0 || invPlayers >= 3) && invPlayers > 0 && m_sType == NPC_LICHE)
	{
		if(invPlayers >= 3 || dice(1,3) == 1)
			Cast(invTarget, MAGIC_DETECTINVISIBILITY);
		else
			target = NULL;
	}

	if(target)
	{
		*pTarget     = target->m_handle;
		*pTargetType = target->m_ownerType;
	} else {
		*pTarget = NULL;
	}
}