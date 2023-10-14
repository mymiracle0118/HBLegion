#include "..\\hg.h"

extern class CGame *   g_game;
extern class CMap	**	g_mapList;
extern class CClient ** g_clientList;
extern class CNpc **	g_npcList;
extern class CMisc  *  g_misc;
extern class CMagic ** g_magicConfigList;

extern char _tmp_cTmpDirX[9];
extern char _tmp_cTmpDirY[9];

CElite::CElite(char * pName5, int NpcH) : CNpc(pName5, NpcH)
{
	_minions = 0;
	_visTargets = 0;
	for(int i = 0; i < maxtimers; i++)
		_timers[i] = 0;
}

CElite::~CElite(void)
{
}

void CElite::RegenHP()
{
	if(IsDead())
		return;

	DWORD now = timeGetTime();

	if ((now - m_dwHPupTime) > HPUPTIME)
	{
		m_dwHPupTime = now;

		AddHP( dice(2, m_iHitDice/6) );
	}
}

void CElite::RegenMP()
{
	if (m_cMagicLevel == 0)
		return;

	DWORD now = timeGetTime();

	if(timeGetTime() - m_dwMPupTime > MPUPTIME) {
		m_dwMPupTime = now;

		m_iMP += dice(1, (m_iMaxMana/5));

		if (m_iMP > m_iMaxMana)
			m_iMP = m_iMaxMana;
	}
}

void CElite::Behavior_Move()
{
	int result = dice(1, 10000);

	switch(m_sType)
	{
	case NPC_SOR:
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

void CElite::Behavior_Attack()
{
	Unit * target = NULL;
	if(m_cTargetType == OWNERTYPE_PLAYER)
		target = g_clientList[m_iTargetIndex];
	else if(m_cTargetType == OWNERTYPE_NPC)
		target = g_npcList[m_iTargetIndex];

	int result = dice(1, 10000);
	short spell = -1;

	switch(m_sType)
	{
	case NPC_SOR:
		if(result <= 3000 && _SummonMinion())
			return;
		
		if(target && abs(m_sX - target->m_sX) <= 9 && abs(m_sY - target->m_sY) <= 7)
		{			
			if(target->m_cMagicEffectStatus[MAGICTYPE_PROTECT] && result <= 500)
			{
				Cast(target, MAGIC_CANCELLATION);
			}
			else if((_visTargets >= 6 || m_iMP > m_iMaxMana/2) && m_iMP >= 3*g_magicConfigList[MAGIC_EARTHSHOCKWAVE]->m_manaCost &&
				time(0) - _timers[0] > 10)
			{
				// Mass ESW
				_timers[0] = time(0);
				m_iMP += 5*g_magicConfigList[MAGIC_EARTHSHOCKWAVE]->m_manaCost;

				magicHandler(m_sX-9, m_sY-7, MAGIC_EARTHSHOCKWAVE); 
				magicHandler(m_sX, m_sY-7, MAGIC_EARTHSHOCKWAVE); 
				magicHandler(m_sX+9, m_sY-7, MAGIC_EARTHSHOCKWAVE); 
				magicHandler(m_sX-9, m_sY, MAGIC_EARTHSHOCKWAVE); 
				magicHandler(m_sX+9, m_sY, MAGIC_EARTHSHOCKWAVE); 
				magicHandler(m_sX-9, m_sY+7, MAGIC_EARTHSHOCKWAVE); 
				magicHandler(m_sX, m_sY+7, MAGIC_EARTHSHOCKWAVE); 
				magicHandler(m_sX+9, m_sY+7, MAGIC_EARTHSHOCKWAVE); 
				g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, 
					OBJECTATTACK, m_sX + _tmp_cTmpDirX[m_cDir], m_sY + _tmp_cTmpDirY[m_cDir], 1); 
			} else if(result <= 800 && target->IsPlayer() && !target->m_cMagicEffectStatus[MAGICTYPE_CONFUSE] &&
				m_iMP >= g_magicConfigList[MAGIC_MASSILLUSIONMOVEMENT]->m_manaCost)
			{
				Cast(target->m_sX, target->m_sY, MAGIC_MASSILLUSIONMOVEMENT);
			} else if(result <= 7000 && m_iMP >= g_magicConfigList[MAGIC_LIGHTNINGSTRIKE]->m_manaCost)
			{
				Cast(target->m_sX, target->m_sY, MAGIC_LIGHTNINGSTRIKE);
			} else if(result <= 9000 && m_iMP >= g_magicConfigList[MAGIC_MASSMAGICMISSILE]->m_manaCost)
			{
				Cast(target->m_sX, target->m_sY, MAGIC_MASSMAGICMISSILE);
			} else if(m_iMP >= g_magicConfigList[MAGIC_LIGHTNING]->m_manaCost)
			{
				Cast(target->m_sX, target->m_sY, MAGIC_LIGHTNING);
			}

			TargetSearch();
		}
		break;

	default:
		CNpc::Behavior_Attack();
		break;
	}
}

void CElite::Behavior_Death(Unit * attacker, int16 dmg)
{
	CNpc::Behavior_Death(attacker, dmg);
	
	switch(m_sType)
	{
	case NPC_SOR:
		m_iMagicHitRatio = 100;
		magicHandler(m_sX, m_sY, 63); 
		magicHandler(m_sX-2, m_sY-2, 63); 
		magicHandler(m_sX+2, m_sY-2, 63); 
		magicHandler(m_sX-2, m_sY+2, 63); 
		magicHandler(m_sX+2, m_sY+2, 63);
		break;
	}
}

bool CElite::_SummonMinion()
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
	case NPC_SOR:
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

		m_cDir = g_misc->cGetNextMoveDir(m_sX, m_sY, x, y);
		g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, 
			OBJECTATTACK, m_sX + _tmp_cTmpDirX[m_cDir], m_sY + _tmp_cTmpDirY[m_cDir], 1); 

		_minions++;
		m_iMP -= reqmp;
		return true;
	}
}

void CElite::Behavior_MinionDeath(CNpc * minion)
{
	_minions--;

	switch(m_sType)
	{
	case NPC_SOR:
		m_iMP += 200;

		int16 dX, dY;
		CClient * killer;
		if(minion->m_killerh != -1 && (killer = g_clientList[ minion->m_killerh ]))
		{
			dX = killer->m_sX;
			dY = killer->m_sY;

			if(killer->m_cMagicEffectStatus[MAGICTYPE_PROTECT])
				minion->magicHandler(dX, dY, 76); // CANCELLATION
		} else {
			dX = minion->m_sX;
			dY = minion->m_sY;
		}

		if(abs(m_sX - dX) <= 9 && abs(m_sY - dY) <= 7)
		{
			int8 dir = g_misc->cGetNextMoveDir(m_sX, m_sY, dX, dY);
			magicHandler(dX + _tmp_cTmpDirX[dir], dY + _tmp_cTmpDirY[dir], 91); // BLIZZARD
		}
		break;
	}
}

void CElite::TargetSearch(short * pTarget, char * pTargetType)
{
	static int32 turn = 0;
	
	_visTargets = 0;
	//Cast(m_sX-3, m_sY, MAGIC_MAGICMISSILE);
	
	for(int i = 0; i < maxtargets; i++)
		_targets[i] = NULL;

	for(int ix = m_sX - (m_cTargetSearchRange+1); 
		ix <= m_sX + (m_cTargetSearchRange+1);
		ix++)
	{
		for(int iy = m_sY - m_cTargetSearchRange; 
			iy <= m_sY + m_cTargetSearchRange;
			iy++) 
		{
			Unit * owner = g_mapList[m_cMapIndex]->GetOwner(ix, iy);
			if (!owner || owner == this) continue;
			
			if(owner->IsPlayer() && g_clientList[owner->m_handle]->IsNoAggro()) 
				continue;

			if((owner->IsNPC() && owner->IsNeutral()) || owner->m_side == m_side)
				continue;
			
			_visTargets++;

			if(owner->m_cMagicEffectStatus[MAGICTYPE_PROTECT] == MAGICPROTECT_AMP)
			{
				magicHandler(owner->m_sX, owner->m_sY, MAGIC_CANCELLATION);
				magicHandler(owner->m_sX, owner->m_sY, MAGIC_EARTHSHOCKWAVE);
			}
			
			for(int i = 0; i < maxtargets; i++)
			{
				if(!_targets[i] || owner->m_iHP < _targets[i]->m_iHP)
				{
					if(i + 1 < maxtargets)
						_targets[i+1] = _targets[i];
					
					_targets[i] = owner;
					break;
				}
			}
		}
	}
		
	for(int32 t = turn; t >= 0; t--)
	{
		if(!_targets[t])
			continue;

		turn++;
		turn %= maxtargets;

		*pTarget = m_iTargetIndex = _targets[t]->m_handle;
		*pTargetType = m_cTargetType = _targets[t]->m_ownerType;
		return;
	}

	// no targets found
	*pTarget = NULL;
}

void CElite::TargetSearch()
{
	short index;
	char type;
	TargetSearch(&index, &type);
}