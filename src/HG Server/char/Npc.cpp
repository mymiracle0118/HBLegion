// Npc.cpp: implementation of the CNpc class.
//
//////////////////////////////////////////////////////////////////////

#include "..\HG.h"
#include "..\map\map.h"
#include "..\misc.h"
#include "combat.h"

extern class CGame *   g_game;
extern char            g_cTxt[512];
extern class CMap	**	g_mapList;
extern class CClient ** g_clientList;
extern class CNpc **	g_npcList;
extern class CNpc **   g_npcConfigList;
extern class CMagic ** g_magicConfigList;

extern char _tmp_cTmpDirX[9];
extern char _tmp_cTmpDirY[9];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNpc::CNpc(char * pName5, int NpcH)
{
	int i;

	ZeroMemory(m_cName, sizeof(m_cName));
	memcpy(m_cName, pName5, NPCNAMELENGTH - 1);

	m_handle = NpcH;
	m_ownerType = OWNERTYPE_NPC;

	for (i = 0; i < MAXWAYPOINTS; i++)			  
		m_iWayPointIndex[i] = -1;

	for (i = 0; i < MAXMAGICEFFECTS; i++) 
		m_cMagicEffectStatus[i]	= 0;

	m_bIsSummoned       = FALSE;
	m_bIsPermAttackMode = FALSE;
	m_iFollowOwnerIndex = NULL;

	m_dwRegenTime = NULL;

	m_iSummonControlMode = NULL;

	m_cAttribute = NULL;
	m_iAbsDamage = NULL;
	m_sAppr2     = NULL;

	m_iAttackRange    = 1;
	m_cSpecialAbility = 0;

	m_iBuildCount = 0;

	m_bIsMaster  = FALSE;
	m_iGuildGUID = NULL;

	m_iV1 = 0;

	m_iNpcCrops = 0; 
	m_iCropsSkillLV = 0;

	// 2002-09-17 #1
	m_iNpcitemMax = 0;

	dwGoldDropValue = 0;

	m_rcRandomArea.bottom = NULL;
	m_rcRandomArea.left = NULL;
	m_rcRandomArea.right = NULL;
	m_rcRandomArea.top = NULL;
}

CNpc::~CNpc()
{

}

bool CNpc::initNpcAttr(char * pNpcName, char cSA)
{
	int i, iTemp;
	char cTmpName[21];
	double dV1, dV2, dV3;
	
	ZeroMemory(cTmpName, sizeof(cTmpName));
	strcpy(cTmpName, pNpcName);

	for (i = 0; i < MAXNPCTYPES; i++) 
	{
		if (g_npcConfigList[i] && _strnicmp(cTmpName, g_npcConfigList[i]->m_cNpcName, 20) == 0) {
			ZeroMemory(m_cNpcName, sizeof(m_cNpcName));
			memcpy(m_cNpcName, g_npcConfigList[i]->m_cNpcName, 20);

			m_sType  = g_npcConfigList[i]->m_sType;
				
			m_iExp             = dice(g_npcConfigList[i]->m_iExpDice, 4) + g_npcConfigList[i]->m_iExpDice;
			//
			m_iHitDice         = g_npcConfigList[i]->m_iHitDice;   
			m_iExpDice         = g_npcConfigList[i]->m_iExpDice;   
			m_iDefenseRatio    = g_npcConfigList[i]->m_iDefenseRatio;
			m_iPhysicalHitRatio = g_npcConfigList[i]->m_iPhysicalHitRatio;
			m_iMinBravery      = g_npcConfigList[i]->m_iMinBravery;
			m_cAttackDiceThrow = g_npcConfigList[i]->m_cAttackDiceThrow;
			m_cAttackDiceRange = g_npcConfigList[i]->m_cAttackDiceRange;
			m_cSize            = g_npcConfigList[i]->m_cSize;
			m_side             = g_npcConfigList[i]->m_side;
			m_cActionLimit     = g_npcConfigList[i]->m_cActionLimit;
			m_dwActionTime     = g_npcConfigList[i]->m_dwActionTime;
			m_dwRegenTime      = g_npcConfigList[i]->m_dwRegenTime;
			m_cResistMagic     = g_npcConfigList[i]->m_cResistMagic;
			m_cMagicLevel      = g_npcConfigList[i]->m_cMagicLevel;
			m_iMaxMana         = g_npcConfigList[i]->m_iMaxMana;
			m_iMP            = g_npcConfigList[i]->m_iMaxMana;
			m_cChatMsgPresence = g_npcConfigList[i]->m_cChatMsgPresence;
			m_cDayOfWeekLimit  = g_npcConfigList[i]->m_cDayOfWeekLimit;
			m_cTargetSearchRange = g_npcConfigList[i]->m_cTargetSearchRange;
			m_iAttackStrategy = dice(1,10);
			m_iAILevel		   = dice(1,3);
			m_iAbsDamage         = g_npcConfigList[i]->m_iAbsDamage;
			m_iMagicHitRatio     = g_npcConfigList[i]->m_iMagicHitRatio;
			m_iAttackRange       = g_npcConfigList[i]->m_iAttackRange;
			m_cSpecialAbility    = cSA;
			m_iBuildCount		   = g_npcConfigList[i]->m_iMinBravery;
			m_cAttribute		   = g_npcConfigList[i]->m_cAttribute;
			dwGoldDropValue	   = g_npcConfigList[i]->dwGoldDropValue;
			m_iContPts = g_npcConfigList[i]->m_iContPts;
			switch (m_cSpecialAbility) 
			{
			case 1:
				dV2 = (double)m_iExp;
				dV3 = 25.0f/100.0f;
				dV1 = dV2 * dV3;
				m_iExp += (int)dV1;
				break;

			case 2:
				dV2 = (double)m_iExp;
				dV3 = 30.0f/100.0f;
				dV1 = dV2 * dV3;
				m_iExp += (int)dV1;
				break;

			case 3: // Absorbing Physical Damage
				if (m_iAbsDamage > 0) {
					m_cSpecialAbility = NULL;
					cSA = NULL;
				}
				else {
					iTemp = 20 + dice(1, 60);
					m_iAbsDamage -= iTemp;
					if (m_iAbsDamage < -90) m_iAbsDamage = -90;
				}

				dV2 = (double)m_iExp;
				dV3 = (double)abs(m_iAbsDamage)/100.0f;
				dV1 = dV2 * dV3;
				m_iExp += (int)dV1;
				break;

			case 4: // Absorbing Magical Damage
				if (m_iAbsDamage < 0) {
					m_cSpecialAbility = NULL;
					cSA = NULL;
				}
				else {
					iTemp = 20 + dice(1, 60);
					m_iAbsDamage += iTemp;
					if (m_iAbsDamage > 90) m_iAbsDamage = 90;
				}

				dV2 = (double)m_iExp;
				dV3 = (double)(m_iAbsDamage)/100.0f;
				dV1 = dV2 * dV3;
				m_iExp += (int)dV1;
				break;

			case 5:
				dV2 = (double)m_iExp;
				dV3 = 15.0f/100.0f;
				dV1 = dV2 * dV3;
				m_iExp += (int)dV1;
				break;

			case 6:
			case 7:
				dV2 = (double)m_iExp;
				dV3 = 20.0f/100.0f;
				dV1 = dV2 * dV3;
				m_iExp += (int)dV1;
				break;
					
			case 8:
				dV2 = (double)m_iExp;
				dV3 = 25.0f/100.0f;
				dV1 = dV2 * dV3;
				m_iExp += (int)dV1;
				break;

			case 9:
				uint32 mult = 1;
				switch(m_sType)
				{
					case NPC_LICHE:
						mult = 4;
						break;
				}
				m_iExp *= 4*mult;
				m_iHitDice *= 2*mult;
				m_iDefenseRatio *= 3;
				m_iPhysicalHitRatio *= 2;
				m_iMinBravery = 100;
				m_cAttackDiceThrow += 2;
				m_cAttackDiceRange += 3;
				m_dwActionTime *= 0.85;
				m_cResistMagic *= 1.5 + (mult/10);
				if(m_cResistMagic > 100) m_cResistMagic = 100;
				m_iMaxMana *= 1.5 * mult; 
				m_iMP = m_iMaxMana;
				m_iAttackStrategy = ATTACKAI_NORMAL;
				m_iAILevel = 3;
				if(m_iAbsDamage > 70 && m_iAbsDamage < 90)
					m_iAbsDamage += 10;
				else if(m_iAbsDamage > 50 && m_iAbsDamage < 70)
					m_iAbsDamage += 20;
				else if(m_iAbsDamage < 50)
					m_iAbsDamage += 30;
				m_iMagicHitRatio *= 2 * mult/2;
				dwGoldDropValue *= 4*mult*mult;
				break;
			}

			if (m_iHitDice <= 5) m_iHP  = abs(dice(m_iHitDice, 4) + m_iHitDice);
			else m_iHP  = abs(m_iHitDice * 5 + dice(1, m_iHitDice));

			if (m_iHP == 0) m_iHP = 1; 

			m_iMaxHP = m_iHP;

			m_iNoDieRemainExp  = (m_iExp) - (m_iExp/3);

			SetNibble(m_iStatus, 2, cSA);

			if(m_side == 10) // temporary code
				m_side = HOSTILE;
			SetNibble(m_iStatus, 7, m_side);

			return TRUE;
		}
	}

	return FALSE;
}

void CNpc::RegenHP()
{
	if(IsDead())
		return;

	DWORD now = timeGetTime();

	if ((now - m_dwHPupTime) > HPUPTIME)
	{
		m_dwHPupTime = now;

		AddHP( dice(2, m_iHitDice/2) );
	}
}

void CNpc::RegenMP()
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

void CNpc::Behave()
{
	switch (m_cBehavior)
	{
	case BEHAVIOR_DEAD:
		g_game->NpcBehavior_Dead(m_handle);
		break;
	case BEHAVIOR_STOP:
		behavior_stop();
		break;
	case BEHAVIOR_MOVE:
		Behavior_Move();
		break;
	case BEHAVIOR_ATTACK:
		Behavior_Attack();
		break;
	case BEHAVIOR_FLEE:
		g_game->NpcBehavior_Flee(m_handle);
		break;
	}
}

bool CNpc::behavior_searchMaster()
{
	int ix, iy, rX, rY;
	short sMasterH;
	char cOwnerType;

	rX = m_sX - m_cTargetSearchRange;
	rY = m_sY - m_cTargetSearchRange;

	sMasterH = NULL;

	for (ix = rX; ix < rX + m_cTargetSearchRange*2 + 1; ix++)
		for (iy = rY; iy < rY + m_cTargetSearchRange*2 + 1; iy++) {
			g_mapList[m_cMapIndex]->GetOwner(&sMasterH, &cOwnerType, ix, iy);
			switch (cOwnerType) 
			{
			case OWNERTYPE_NPC:
				if ((sMasterH != m_handle) && (g_npcList[sMasterH] != NULL) && (g_npcList[sMasterH]->m_sType == m_sType) &&
					(g_npcList[sMasterH]->m_cMoveType == MOVETYPE_RANDOM)) {
						g_npcList[sMasterH]->m_bIsMaster = TRUE;
						m_cMoveType = MOVETYPE_FOLLOW;
						m_cFollowOwnerType = OWNERTYPE_NPC; 
						m_iFollowOwnerIndex = sMasterH;
						return TRUE;
				}
				break;
			}
		}

		return FALSE;
}

void CNpc::Behavior_Move()
{
	char  cDir;
	short sX, sY, dX, dY, absX, absY;
	short sTarget, sDistance;
	char  cTargetType;

	if (m_bIsKilled == TRUE) return;

	if ((m_bIsSummoned == TRUE) && 
		(m_iSummonControlMode == 1)) return;
	if (m_cMagicEffectStatus[ MAGICTYPE_HOLDOBJECT ] != 0) return;

	switch (m_cActionLimit) {
	case 2:
	case 3:
	case 5:
		case 8: // Heldenian gates
		m_cBehavior          = BEHAVIOR_STOP;
		m_sBehaviorTurnCount = 0;
		return;
	}

	int iStX, iStY;
	if (g_mapList[m_cMapIndex] != NULL) {
		iStX = m_sX / 20;
		iStY = m_sY / 20;
		g_mapList[m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iMonsterActivity++;  
	}

	m_sBehaviorTurnCount++;
	if (m_sBehaviorTurnCount > 5)
	{
		m_sBehaviorTurnCount = 0;

		absX = abs(m_vX - m_sX);
		absY = abs(m_vY - m_sY);

		if ((absX <= 2)	&& (absY <= 2)) nextWaypointDest();

		m_vX = m_sX;
		m_vY = m_sY;
	}

	/*if (m_sBehaviorTurnCount > 20)
	{
			nextWaypointDest();
	}*/

	TargetSearch(&sTarget, &cTargetType);
	if (sTarget != NULL) {
		if (m_dwActionTime < 1000) {
			if (dice(1,3) == 3) {
				m_cBehavior          = BEHAVIOR_ATTACK;
				m_sBehaviorTurnCount = 0;
				m_iTargetIndex = sTarget;
				m_cTargetType  = cTargetType;
				return;
			}
		}
		else {
			m_cBehavior          = BEHAVIOR_ATTACK;
			m_sBehaviorTurnCount = 0;
			m_iTargetIndex = sTarget;
			m_cTargetType  = cTargetType;
			return;
		}
	}

	if ((m_bIsMaster == TRUE) && (dice(1,3) == 2)) return;

	if (m_cMoveType == MOVETYPE_FOLLOW) {
		sX = m_sX;
		sY = m_sY;
		switch (m_cFollowOwnerType) {
		case OWNERTYPE_PLAYER:
			if (g_clientList[m_iFollowOwnerIndex] == NULL) {
				m_cMoveType = MOVETYPE_RANDOM;
				return;
			}

			dX = g_clientList[m_iFollowOwnerIndex]->m_sX;
			dY = g_clientList[m_iFollowOwnerIndex]->m_sY;
			break;
		case OWNERTYPE_NPC:
			if (g_npcList[m_iFollowOwnerIndex] == NULL) {
				m_cMoveType = MOVETYPE_RANDOM;
				m_iFollowOwnerIndex = NULL;
				//searchMaster(m_handle);
				return;
			}

			dX = g_npcList[m_iFollowOwnerIndex]->m_sX;
			dY = g_npcList[m_iFollowOwnerIndex]->m_sY;
			break;
		}

		if (abs(sX - dX) >= abs(sY - dY)) 
			sDistance = abs(sX - dX);
		else sDistance = abs(sY - dY);

		if (sDistance >= 3) {
			short DOType = 0;
			cDir = g_game->cGetNextMoveDir(sX, sY, dX, dY, m_cMapIndex, m_cTurn, &m_tmp_iError, &DOType, m_cSize, m_handle);

			if (cDir == 0) {
			}
			else {
				if(DOType == DYNAMICOBJECT_SPIKE)
				{
					uint32 dmg = dice(2,4);
					ReduceHP(dmg);

					if(IsDead())
					{
						g_game->NpcKilledHandler(NULL, NULL, m_handle, 0);
						return;
					} else {
						g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTDAMAGE, dmg, 1, NULL);
					}
				}
				dX = m_sX + _tmp_cTmpDirX[cDir];
				dY = m_sY + _tmp_cTmpDirY[cDir];
				g_mapList[m_cMapIndex]->ClearOwner(/*3,*/ m_handle, OWNERTYPE_NPC, m_sX, m_sY);
				g_mapList[m_cMapIndex]->SetOwner(m_handle, OWNERTYPE_NPC, dX, dY);
				m_sX   = dX;
				m_sY   = dY;
				m_cDir = cDir;
				g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTMOVE, NULL, NULL, NULL);
			}
		}
	}
	else 
	{
		short DOType = 0;
		cDir = g_game->cGetNextMoveDir(m_sX, m_sY, m_dX, m_dY, m_cMapIndex, m_cTurn, &m_tmp_iError, &DOType, m_cSize, m_handle);

		if (cDir == 0) {
			if (dice(1,10) == 3) nextWaypointDest();
		}
		else {
			if(DOType == DYNAMICOBJECT_SPIKE)
			{
				uint32 dmg = dice(2,4);
				ReduceHP(dmg);

				if(IsDead())
				{
					g_game->NpcKilledHandler(NULL, NULL, m_handle, 0);
					return;
				} else {
					g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTDAMAGE, dmg, 1, NULL);
				}
			}
			dX = m_sX + _tmp_cTmpDirX[cDir];
			dY = m_sY + _tmp_cTmpDirY[cDir];
			g_mapList[m_cMapIndex]->ClearOwner(/*4,*/ m_handle, OWNERTYPE_NPC, m_sX, m_sY);
			g_mapList[m_cMapIndex]->SetOwner(m_handle, OWNERTYPE_NPC, dX, dY);
			m_sX   = dX;
			m_sY   = dY;
			m_cDir = cDir;
			g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTMOVE, NULL, NULL, NULL);
		}
	}
}

void CNpc::behavior_stop()
{
	char  cTargetType;
	short sTarget = NULL;
	bool  bFlag;

	m_sBehaviorTurnCount++;

	switch (m_cActionLimit)
	{
	case 5:
		switch (m_sType)
		{
		case 38: // Mana Collector
			if (m_sBehaviorTurnCount >= 3) {
				m_sBehaviorTurnCount = 0;
				bFlag = behavior_manaCollector();

				if (bFlag == TRUE) {
					g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTATTACK, m_sX, m_sY, 1); 
				}
			}
			break;

		case 39: // Detector
			if (m_sBehaviorTurnCount >= 3) {
				m_sBehaviorTurnCount = 0;
				bFlag = behavior_detector();

				if (bFlag == TRUE) {
					g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTATTACK, m_sX, m_sY, 1); 
				}
			}
			break;

		case 40: // Energy Shield Generator
			break;

		case 41: // Grand Magic Generator
			if (m_sBehaviorTurnCount >= 3) {
				m_sBehaviorTurnCount = 0;
				behavior_grandMagicGenerator();
			}
			break;

		case 42: // ManaStone
			m_sBehaviorTurnCount = 0;
			m_iV1 += 5;
			if (m_iV1 >= 5) m_iV1 = 5;
			break;

		default: 
			TargetSearch(&sTarget, &cTargetType);
			break;
		}
		break;
	}

	if (sTarget != NULL) {

		m_cBehavior          = BEHAVIOR_ATTACK;
		m_sBehaviorTurnCount = 0;
		m_iTargetIndex = sTarget;
		m_cTargetType  = cTargetType;
		return;
	}
}
void CNpc::Behavior_Attack()
{
	int   iMagicType;
	short sX, sY, dX, dY;
	char  cDir;
	DWORD dwTime = timeGetTime();

	if (m_cMagicEffectStatus[ MAGICTYPE_HOLDOBJECT ] != 0) return;
	if (m_bIsKilled == TRUE) return;

	switch (m_cActionLimit) {
	case 1:
	case 2:
	case 3:
	case 4:
			case 6: // moving merchant
	case 8: // Heldenian gate
		return;

	case 5:
		if (m_iBuildCount > 0) return;
	}

	int iStX, iStY;
	if (g_mapList[m_cMapIndex] != NULL) {
		iStX = m_sX / 20;
		iStY = m_sY / 20;
		g_mapList[m_cMapIndex]->m_stTempSectorInfo[iStX][iStY].iMonsterActivity++;  
	}

	if (m_sBehaviorTurnCount == 0) 
		m_iAttackCount = 0;

	m_sBehaviorTurnCount++;
	if (m_sBehaviorTurnCount > 20) {
		m_sBehaviorTurnCount = 0;

		if (m_bIsPermAttackMode == FALSE)
			m_cBehavior    = BEHAVIOR_MOVE;

		return;
	}

	sX = m_sX;
	sY = m_sY;

	Unit * target = NULL;
	if(m_cTargetType == OWNERTYPE_PLAYER)
		target = g_clientList[m_iTargetIndex];
	else if(m_cTargetType == OWNERTYPE_NPC)
		target = g_npcList[m_iTargetIndex];

	switch (m_cTargetType) {
	case OWNERTYPE_PLAYER:
		if (g_clientList[m_iTargetIndex] == NULL) {
			m_sBehaviorTurnCount = 0;
			m_cBehavior    = BEHAVIOR_MOVE;
			return;
		}
		dX = g_clientList[m_iTargetIndex]->m_sX;
		dY = g_clientList[m_iTargetIndex]->m_sY;
		break;

	case OWNERTYPE_NPC:
		if (g_npcList[m_iTargetIndex] == NULL) {
			m_sBehaviorTurnCount = 0;
			m_cBehavior    = BEHAVIOR_MOVE;
			return;
	}
	dX = g_npcList[m_iTargetIndex]->m_sX;
	dY = g_npcList[m_iTargetIndex]->m_sY;
	break;
	}

	if ( (getDangerValue(dX, dY) > m_cBravery) && 
		(m_bIsPermAttackMode == FALSE) &&
		(m_cActionLimit != 5)) {

			m_sBehaviorTurnCount = 0;
			m_cBehavior          = BEHAVIOR_FLEE;
			return;
	}

	if ( (m_iHP <= 2) && (dice(1,m_cBravery) <= 3) &&
		(m_bIsPermAttackMode == FALSE) &&
		(m_cActionLimit != 5)) {

			m_sBehaviorTurnCount = 0;
			m_cBehavior          = BEHAVIOR_FLEE;
			return;
	}

	if ((abs(sX - dX) <= 1) && (abs(sY - dY) <= 1)) {

		cDir = CMisc::cGetNextMoveDir(sX, sY, dX, dY);
		if (cDir == 0) return;
		m_cDir = cDir;				  

		if (m_cActionLimit == 5) {
			switch (m_sType) {
			case 89: // AGT
				if (target) { 
				if(m_cTargetType == OWNERTYPE_PLAYER) {
				g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTATTACK, dX, dY, 1); 
				m_iMagicHitRatio = 1000;
				magicHandler(dX, dY, 61);
					}
				}
				break;
			case 87: // CT
				if (target) { 
				if(m_cTargetType == OWNERTYPE_PLAYER) {
				g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTATTACK, m_sX + _tmp_cTmpDirX[cDir], m_sY + _tmp_cTmpDirY[cDir], 2); 				
				calculateAttackEffect(m_iTargetIndex, m_cTargetType, m_handle, OWNERTYPE_NPC, dX, dY, 2);
					}
				}
				break;	
			case 36: // Crossbow Guard Tower
				g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTATTACK, m_sX + _tmp_cTmpDirX[cDir], m_sY + _tmp_cTmpDirY[cDir], 2); 				
				calculateAttackEffect(m_iTargetIndex, m_cTargetType, m_handle, OWNERTYPE_NPC, dX, dY, 2);
				break;

			case 37: // Cannon Guard Tower: 
				g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTATTACK, dX, dY, 1); 
				m_iMagicHitRatio = 1000;
				magicHandler(dX, dY, 61);
				break;
				
			case 51: // Catapult
				g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTATTACK, dX, dY, 1); 
				m_iMagicHitRatio = 1000;
				magicHandler(dX, dY, 61);
				break;
			}
		}

		else {
			if (m_cMagicLevel == 11) {
				g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTATTACK, m_sX, m_sY, 1); 
				m_iMagicHitRatio = 1000;
				magicHandler(m_sX, m_sY, 75);
			} else 
			{
				g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTATTACK, m_sX + _tmp_cTmpDirX[cDir], m_sY + _tmp_cTmpDirY[cDir], 1); 
				calculateAttackEffect(m_iTargetIndex, m_cTargetType, m_handle, OWNERTYPE_NPC, dX, dY, 1); 
			}
		}
		m_iAttackCount++;

		if ((m_bIsPermAttackMode == FALSE) && (m_cActionLimit == 0)) {
			switch (m_iAttackStrategy) {
			case ATTACKAI_EXCHANGEATTACK:
				m_sBehaviorTurnCount = 0;
				m_cBehavior          = BEHAVIOR_FLEE;
				break;

			case ATTACKAI_TWOBYONEATTACK:
				if (m_iAttackCount >= 2) {
					m_sBehaviorTurnCount = 0;
					m_cBehavior          = BEHAVIOR_FLEE;
				}
				break;
			}
		}
	}
	else {
		cDir = CMisc::cGetNextMoveDir(sX, sY, dX, dY);
		if (cDir == 0) return;
		m_cDir = cDir;				  

		if ((m_cMagicLevel > 0) && (dice(1,2) == 1) &&
			(abs(sX - dX) <= 9) && (abs(sY - dY) <= 7)) {
				iMagicType = -1;
				switch (m_cMagicLevel) 
				{
				case 1:
					if (g_magicConfigList[0]->m_manaCost <= m_iMP) 
						iMagicType = 0;
					break;
				case 2:
					if (g_magicConfigList[10]->m_manaCost <= m_iMP) 
						iMagicType = 10;
					else if (g_magicConfigList[0]->m_manaCost <= m_iMP) 
						iMagicType = 0;
					break;
				case 3:
					if (g_magicConfigList[20]->m_manaCost <= m_iMP) 
						iMagicType = 20;
					else if (g_magicConfigList[10]->m_manaCost <= m_iMP) 
						iMagicType = 10;
					break;
				case 4:
					if (g_magicConfigList[30]->m_manaCost <= m_iMP) 
						iMagicType = 30;
					else if (g_magicConfigList[37]->m_manaCost <= m_iMP) 
						iMagicType = 37;
					else if (g_magicConfigList[20]->m_manaCost <= m_iMP) 
						iMagicType = 20;
					else if (g_magicConfigList[10]->m_manaCost <= m_iMP) 
						iMagicType = 10;
					break;
				case 5:
					if (g_magicConfigList[43]->m_manaCost <= m_iMP) 
						iMagicType = 43;
					else if (g_magicConfigList[30]->m_manaCost <= m_iMP) 
						iMagicType = 30;
					else if (g_magicConfigList[37]->m_manaCost <= m_iMP) 
						iMagicType = 37;
					else if (g_magicConfigList[20]->m_manaCost <= m_iMP) 
						iMagicType = 20;
					else if (g_magicConfigList[10]->m_manaCost <= m_iMP) 
						iMagicType = 10;
					break;
				case 6:
					if (g_magicConfigList[51]->m_manaCost <= m_iMP) 
						iMagicType = 51;
					else if (g_magicConfigList[43]->m_manaCost <= m_iMP) 
						iMagicType = 43;
					else if (g_magicConfigList[30]->m_manaCost <= m_iMP) 
						iMagicType = 30;
					else if (g_magicConfigList[37]->m_manaCost <= m_iMP) 
						iMagicType = 37;
					else if (g_magicConfigList[20]->m_manaCost <= m_iMP) 
						iMagicType = 20;
					else if (g_magicConfigList[10]->m_manaCost <= m_iMP) 
						iMagicType = 10;
					break;
				case 7:
					if ((g_magicConfigList[70]->m_manaCost <= m_iMP) && (dice(1,5) == 3)) 
						iMagicType = 70;
					else if (g_magicConfigList[61]->m_manaCost <= m_iMP) 
						iMagicType = 61;
					else if (g_magicConfigList[60]->m_manaCost <= m_iMP) 
						iMagicType = 60;
					else if (g_magicConfigList[51]->m_manaCost <= m_iMP) 
						iMagicType = 51;
					else if (g_magicConfigList[43]->m_manaCost <= m_iMP) 
						iMagicType = 43;
					break;
				case 8:
					if ((g_magicConfigList[25]->m_manaCost <= m_iMP) && (dice(1,3) == 2)) 
						iMagicType = 25;
					else if (g_magicConfigList[60]->m_manaCost <= m_iMP) 
						iMagicType = 60;
					else if (g_magicConfigList[51]->m_manaCost <= m_iMP) 
						iMagicType = 51;
					else if (g_magicConfigList[43]->m_manaCost <= m_iMP) 
						iMagicType = 43;
					break;

				case 9:
					if ((g_magicConfigList[74]->m_manaCost <= m_iMP) && (dice(1,3) == 2)) 
						iMagicType = 74;
					break;
				case 10:
					if ((g_magicConfigList[57]->m_manaCost <= m_iMP) && (dice(1,3) == 2)) 
						iMagicType = 57;
					break;
				case 11: 
					goto NBA_CHASE;
					break;
				case 12: 
					if ((g_magicConfigList[91]->m_manaCost <= m_iMP) && (dice(1,3) == 2)) 
						iMagicType = 91; 
					else if (g_magicConfigList[63]->m_manaCost <= m_iMP) 
						iMagicType = 63; 
					break;
				case 13:
					if ((g_magicConfigList[96]->m_manaCost <= m_iMP) && (dice(1,3) == 2)) 
						iMagicType = 96; 
					else if (g_magicConfigList[81]->m_manaCost <= m_iMP) 
						iMagicType = 81; 
					break;
				}

				if (iMagicType != -1) {

					if (m_iAILevel >= 2) {
						switch (m_cTargetType) {
						case OWNERTYPE_PLAYER:
							if (g_clientList[m_iTargetIndex]->m_cMagicEffectStatus[MAGICTYPE_PROTECT] == MAGICPROTECT_PFM) {
								if ((abs(sX - dX) > m_iAttackRange) || (abs(sY - dY) > m_iAttackRange)) {
									m_sBehaviorTurnCount = 0;
									m_cBehavior    = BEHAVIOR_MOVE;
									return;
								}
								else goto NBA_CHASE;
							}
							if ((iMagicType == 25) && (g_clientList[m_iTargetIndex]->m_cMagicEffectStatus[ MAGICTYPE_HOLDOBJECT ] != 0)) goto NBA_CHASE;
							break;

						case OWNERTYPE_NPC:
							if (g_npcList[m_iTargetIndex]->m_cMagicEffectStatus[MAGICTYPE_PROTECT] == MAGICPROTECT_PFM) {
								if ((abs(sX - dX) > m_iAttackRange) || (abs(sY - dY) > m_iAttackRange)) {
									m_sBehaviorTurnCount = 0;
									m_cBehavior    = BEHAVIOR_MOVE;
									return;
								}
								else goto NBA_CHASE;
							}
							if ((iMagicType == 25) && (g_npcList[m_iTargetIndex]->m_cMagicEffectStatus[ MAGICTYPE_HOLDOBJECT ] != 0)) goto NBA_CHASE;
							break;
						}
					}

					g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTATTACK, m_sX + _tmp_cTmpDirX[cDir], m_sY + _tmp_cTmpDirY[cDir], 1); 
					magicHandler(dX, dY, iMagicType);
					m_dwTime = dwTime; 
					return;
				}
		}

		if ((m_cMagicLevel < 0) && (dice(1,2) == 1) &&
			(abs(sX - dX) <= 9) && (abs(sY - dY) <= 7)) {
				iMagicType = -1;
				if (g_magicConfigList[43]->m_manaCost <= m_iMP) 
					iMagicType = 43;
				else if (g_magicConfigList[37]->m_manaCost <= m_iMP) 
					iMagicType = 37;
				else if (g_magicConfigList[0]->m_manaCost <= m_iMP) 
					iMagicType = 0;

				if (iMagicType != -1) {
					g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTATTACK, m_sX + _tmp_cTmpDirX[cDir], m_sY + _tmp_cTmpDirY[cDir], 1); 
					magicHandler(dX, dY, iMagicType);
					m_dwTime = dwTime; 
					return;
				}
		}

		if ((m_iAttackRange > 1) && 
			(abs(sX - dX) <= m_iAttackRange) && (abs(sY - dY) <= m_iAttackRange)) {

				cDir = CMisc::cGetNextMoveDir(sX, sY, dX, dY);
				if (cDir == 0) return;
				m_cDir = cDir;				  

				if (m_cActionLimit == 5) {
					switch (m_sType) {
					case 89: // AGT
						if (target) { 
						if(m_cTargetType == OWNERTYPE_PLAYER) {
						g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTATTACK, dX, dY, 1); 
						m_iMagicHitRatio = 1000;
						magicHandler(dX, dY, 61);
							}
						}
						break;
					case 87: // CT
						if (target) { 
						if(m_cTargetType == OWNERTYPE_PLAYER) {
						g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTATTACK, dX, dY, 2); 
						calculateAttackEffect(m_iTargetIndex, m_cTargetType, m_handle, OWNERTYPE_NPC, dX, dY, 2);
							}
						}
						break;
					case 36: // Crossbow Guard Tower
						g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTATTACK, dX, dY, 2); 
						calculateAttackEffect(m_iTargetIndex, m_cTargetType, m_handle, OWNERTYPE_NPC, dX, dY, 2);
						break;

					case 37: // Cannon Guard Tower
						g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTATTACK, dX, dY, 1); 
						m_iMagicHitRatio = 1000;
						magicHandler(dX, dY, 61);
						break;

					case 51: // Catapult
						g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTATTACK, dX, dY, 1); 
						m_iMagicHitRatio = 1000;
						magicHandler(dX, dY, 61);
						break;
					}
				}
				else {
					switch (m_sType) {

					case 54: // Dark Elf
						g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTATTACK, dX, dY, 2); 
						calculateAttackEffect(m_iTargetIndex, m_cTargetType, m_handle, OWNERTYPE_NPC, dX, dY, 2);
						break;
					case 63: //v2.20 2002-12-20 frost
					case 53: //Beholder
					case 79: 
						if (target)
						{
							if (!target->IsDead() &&  checkResistingIceSuccess(m_cDir, m_iTargetIndex, m_cTargetType, m_iMagicHitRatio) == FALSE) 
							{	if (target->m_cMagicEffectStatus[ MAGICTYPE_ICE ] == 0) {
									target->m_cMagicEffectStatus[ MAGICTYPE_ICE ] = 1;
									target->SetStatusFlag(STATUS_FROZEN, TRUE);
									g_game->RegisterDelayEvent(DELAYEVENTTYPE_MAGICRELEASE, MAGICTYPE_ICE, dwTime + (5*1000), target, NULL, NULL, NULL, 1, NULL, NULL);
								}
							}
							g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTATTACK, dX, dY, 20); 
							calculateAttackEffect(m_iTargetIndex, m_cTargetType, m_handle, OWNERTYPE_NPC, dX, dY, 1);
						}
						break;

					default:
						g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTATTACK, dX, dY, 20);
						calculateAttackEffect(m_iTargetIndex, m_cTargetType, m_handle, OWNERTYPE_NPC, dX, dY, 1);
						break;
					}
				}
				m_iAttackCount++;

				if ((m_bIsPermAttackMode == FALSE) && (m_cActionLimit == 0)) {
					switch (m_iAttackStrategy) {
					case ATTACKAI_EXCHANGEATTACK:
						m_sBehaviorTurnCount = 0;
						m_cBehavior          = BEHAVIOR_FLEE;
						break;

					case ATTACKAI_TWOBYONEATTACK:
						if (m_iAttackCount >= 2) {
							m_sBehaviorTurnCount = 0;
							m_cBehavior          = BEHAVIOR_FLEE;
						}
						break;
					}
				}
				return;
		}

NBA_CHASE:;

		if (m_cActionLimit != 0) return;

		m_iAttackCount = 0;

		{
			cDir = g_game->cGetNextMoveDir(sX, sY, dX, dY,m_cMapIndex, m_cTurn, &m_tmp_iError, m_cSize, m_handle);
			if (cDir == 0) {
				return;
			}
			dX = m_sX + _tmp_cTmpDirX[cDir];
			dY = m_sY + _tmp_cTmpDirY[cDir];
			g_mapList[m_cMapIndex]->ClearOwner(/*9,*/ m_handle, OWNERTYPE_NPC, m_sX, m_sY);
			g_mapList[m_cMapIndex]->SetOwner(m_handle, OWNERTYPE_NPC, dX, dY);
			m_sX   = dX;
			m_sY   = dY;
			m_cDir = cDir;
			g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTMOVE, NULL, NULL, NULL);
		}
	}
}

bool CNpc::behavior_manaCollector()
{
	int dX, dY, iMaxMP, iTotal;
	short sOwnerH;
	char  cOwnerType;
	double dV1, dV2, dV3;
	bool bRet;

	if (m_sAppr2 != 0) return FALSE;

	bRet = FALSE;
	for (dX = m_sX-5; dX <= m_sX+5; dX++)
		for (dY = m_sY-5; dY <= m_sY+5; dY++) {
			g_mapList[m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);

			if (sOwnerH != NULL) {
				switch (cOwnerType) {
				case OWNERTYPE_PLAYER:
					if (m_side == g_clientList[sOwnerH]->m_side) {
						iMaxMP = g_clientList[sOwnerH]->GetMaxMP();
						if (g_clientList[sOwnerH]->m_iMP < iMaxMP) {
							iTotal = dice(1, (g_clientList[sOwnerH]->GetMag()) ); // Mana Point´Â 1D(Magic)
							if (g_clientList[sOwnerH]->m_iAddMP != 0) {
								dV2 = (double)iTotal;
								dV3 = (double)g_clientList[sOwnerH]->m_iAddMP;
								dV1 = (dV3 / 100.0f)*dV2;
								iTotal += (int)dV1;
							}

							g_clientList[sOwnerH]->m_iMP += iTotal;

							if (g_clientList[sOwnerH]->m_iMP > iMaxMP) 
								g_clientList[sOwnerH]->m_iMP = iMaxMP;

							g_game->SendNotifyMsg(NULL, sOwnerH, NOTIFY_MP, NULL, NULL, NULL, NULL);
						}
					}
					break;

				case OWNERTYPE_NPC:
					if ((g_npcList[sOwnerH]->m_sType == NPC_MANASTONE) && (g_npcList[sOwnerH]->m_iV1 > 0)) {
						if (g_npcList[sOwnerH]->m_iV1 >= 3) {
							g_game->m_iCollectedMana[m_side] += 3;
							g_npcList[sOwnerH]->m_iV1 -= 3;
							bRet = TRUE;
						} else {
							g_game->m_iCollectedMana[m_side] += g_npcList[sOwnerH]->m_iV1; 
							g_npcList[sOwnerH]->m_iV1 = 0;
							bRet = TRUE;
						}
					}
					break;
				}
			}
		}

	return bRet;
}


void CNpc::behavior_grandMagicGenerator()
{
	if(IsNeutral()) return;
	
	if (g_game->m_mana[m_side] > g_game->m_iMaxGMGMana) {
		g_game->_GrandMagicLaunchMsgSend(1, m_side);
		g_game->MeteorStrikeMsgHandler(m_side);
		g_game->m_mana[m_side] = 0;
	}
}



bool CNpc::behavior_detector()
{
	int dX, dY;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
	bool  bFlag = FALSE;

	if (m_sAppr2 != 0) return FALSE;

	Unit * owner = NULL;

	for (dX = m_sX-10; dX <= m_sX+10; dX++)
		for (dY = m_sY-10; dY <= m_sY+10; dY++) {
			owner = g_mapList[m_cMapIndex]->GetOwner(dX, dY);

			if(!owner)	continue;

			if (!owner->IsNeutral() && (owner->m_side != m_side)) {
				if (owner->m_cMagicEffectStatus[ MAGICTYPE_INVISIBILITY ] != NULL) {
					owner->m_cMagicEffectStatus[ MAGICTYPE_INVISIBILITY ] = NULL;
					owner->SetMagicFlag(MAGICTYPE_INVISIBILITY, FALSE);
				}
				bFlag = TRUE;
			}
		}

	return bFlag;
}

void CNpc::nextWaypointDest()
{
	short sRange, sX, sY;
	int i, j, iMapIndex;
	bool bFlag;

	switch (m_cMoveType) 
	{
	case MOVETYPE_GUARD:
		break;

	case MOVETYPE_SEQWAYPOINT: 

		m_cCurWaypoint++;
		if (m_cCurWaypoint >= m_cTotalWaypoint)
			m_cCurWaypoint = 1;
		m_dX  = (short)(g_mapList[m_cMapIndex]->m_WaypointList[ m_iWayPointIndex[ m_cCurWaypoint ] ].x);
		m_dY  = (short)(g_mapList[m_cMapIndex]->m_WaypointList[ m_iWayPointIndex[ m_cCurWaypoint ] ].y);
		break;

	case MOVETYPE_RANDOMWAYPOINT:

		m_cCurWaypoint = ((rand() % (m_cTotalWaypoint - 1)) + 1);
		m_dX  = (short)(g_mapList[m_cMapIndex]->m_WaypointList[ m_iWayPointIndex[ m_cCurWaypoint ] ].x);
		m_dY  = (short)(g_mapList[m_cMapIndex]->m_WaypointList[ m_iWayPointIndex[ m_cCurWaypoint ] ].y);
		break;

	case MOVETYPE_RANDOMAREA:

		sRange = (short)(m_rcRandomArea.right - m_rcRandomArea.left);
		m_dX = (short)((rand() % sRange) + m_rcRandomArea.left);
		sRange = (short)(m_rcRandomArea.bottom - m_rcRandomArea.top);
		m_dY = (short)((rand() % sRange) + m_rcRandomArea.top);
		break;

	case MOVETYPE_RANDOM:
		//m_dX = (rand() % (g_mapList[m_cMapIndex]->m_sSizeX - 50)) + 15;
		//m_dY = (rand() % (g_mapList[m_cMapIndex]->m_sSizeY - 50)) + 15;
		iMapIndex = m_cMapIndex;

		for ( i = 0; i <= 30; i++) {
			sX = (rand() % g_mapList[iMapIndex]->m_sSizeX);
			sY = (rand() % g_mapList[iMapIndex]->m_sSizeY);

			bFlag = TRUE;
			for (j = 0; j < MAXMGAR; j++)
				if (g_mapList[iMapIndex]->m_rcMobGenAvoidRect[j].left != -1) {
					if ((sX >= g_mapList[iMapIndex]->m_rcMobGenAvoidRect[j].left) &&
						(sX <= g_mapList[iMapIndex]->m_rcMobGenAvoidRect[j].right) &&
						(sY >= g_mapList[iMapIndex]->m_rcMobGenAvoidRect[j].top) &&
						(sY <= g_mapList[iMapIndex]->m_rcMobGenAvoidRect[j].bottom)) {
							bFlag = FALSE;
					}
				}
				if (bFlag == TRUE) goto CNW_GET_VALIDLOC_SUCCESS;
				break;
			}
			// Fail! 
			m_tmp_iError  = 0; 
			return;

CNW_GET_VALIDLOC_SUCCESS:;
			m_dX = sX;
			m_dY = sY;
			break;
		}

	m_tmp_iError  = 0;
}

void CNpc::TargetSearch(short * pTarget, char * pTargetType)
{
	int ix, iy, iPKCount;
	short sX, sY, rX, rY;
	short sDistance, sTempDistance;
	int   iSearchType;

	sDistance    = 100;
	
	iSearchType = NULL;
	switch (m_sType) 
	{
	case NPC_CP: iSearchType = 1; break;
	}
	
	Unit ** owners = g_mapList[m_cMapIndex]->GetOwners(
		m_sX - m_cTargetSearchRange, m_sX + m_cTargetSearchRange,
		m_sY - m_cTargetSearchRange, m_sY + m_cTargetSearchRange);

	Unit * owner, *target = NULL;
	for(owner = (*owners); owner; owner = *(++owners))
	{
		if(owner == this)
			continue;

		iPKCount = 0;
		switch (owner->m_ownerType) 
		{
		case OWNERTYPE_PLAYER:
			if(g_clientList[owner->m_handle]->IsNoAggro() || iSearchType == 1) 
				continue;

			iPKCount    = g_clientList[owner->m_handle]->m_iPKCount;
			break;

		case OWNERTYPE_NPC:
			switch (iSearchType) 
			{
			case 1:
				switch (owner->m_sType) 
				{
				case 36:
				case 37:
				case 38:
				case 39:
				case 40:
				case 41: 
					break;
				default:
					continue;
				}
				break;
			case 0:
				switch (owner->m_sType) 
				{
				case NPC_CT:
				case NPC_AGC:
					continue;
				}
				break;

			}

			iPKCount    = 0;

			if (m_sType == 21) {
				if (g_game->getPlayerNum(owner->m_cMapIndex, owner->m_sX, owner->m_sY, 2) != 0) {
					continue;
				}
			}
			break;
		}

		if (m_side < 10) {
			if (owner->IsNeutral()) {
				if (iPKCount == 0) continue;
			}
			else {
				if ((iPKCount == 0 && owner->m_side == m_side) || IsNeutral())
					continue;
			}
		}
		else {
			if ((owner->m_ownerType == OWNERTYPE_NPC && owner->IsNeutral()) || owner->m_side == m_side)
				continue;
		}

		// INVISIBILITY
		if (owner->IsInvisible() && m_cSpecialAbility != 1) continue;

		if (abs(m_sX - owner->m_sX) >= abs(m_sY - owner->m_sY)) 
			sTempDistance = abs(m_sX - owner->m_sX);
		else sTempDistance = abs(m_sY - owner->m_sY);

		if (sTempDistance <	sDistance) {
			sDistance = sTempDistance;
			target = owner;
		}
	}

	if(target)
	{
		*pTarget     = target->m_handle;
		*pTargetType = target->m_ownerType;
	} else {
		*pTarget     = 0;
	}
}

int CNpc::getDangerValue(short dX, short dY)
{
	int ix, iy, iDangerValue;
	short sDOType;
	DWORD dwRegisterTime;
	Unit * owner;

	iDangerValue = 0;

	for (ix = dX - 2; ix <= dX + 2; ix++){
		for (iy = dY - 2; iy <= dY + 2; iy++) {
			owner = g_mapList[m_cMapIndex]->GetOwner(ix, iy);
			g_mapList[m_cMapIndex]->bGetDynamicObject( ix, iy, &sDOType, &dwRegisterTime );

			if (sDOType == 1) iDangerValue++;

			if (!owner) continue;

			if (m_side != owner->m_side) 
				iDangerValue++;
			else iDangerValue--;
		}
	}

	return iDangerValue;
}

void CNpc::magicHandler(short dX, short dY, short sType) // magicHandler
{
	short  sOwnerH;
	char   cOwnerType;
	CMagic * spell;
	int i, iErr, ix, iy, sX, sY, tX, tY, iResult, iWhetherBonus, iMagicAttr;
	const int crossPnts[5][2] = {{0,0},{-1,0},{1,0},{0,-1},{0,1}};
	DWORD  dwTime = timeGetTime();

	if ((dX < 0) || (dX >= g_mapList[m_cMapIndex]->m_sSizeX) ||
		(dY < 0) || (dY >= g_mapList[m_cMapIndex]->m_sSizeY)) return;

	if ((sType < 0) || (sType >= 100))     return;
	if (g_magicConfigList[sType] == NULL) return;

	if (g_mapList[ m_cMapIndex ]->m_bIsAttackEnabled == FALSE) return;

	iResult = m_iMagicHitRatio;

	iWhetherBonus = g_game->iGetWhetherMagicBonusEffect(sType, g_mapList[m_cMapIndex]->m_cWhetherStatus);

	spell = g_magicConfigList[sType];
	iMagicAttr = spell->m_iAttribute;

	Unit * target = g_mapList[m_cMapIndex]->GetOwner(dX, dY);

	if (spell->m_dwDelayTime == 0) {
		switch (spell->m_sType) 
		{
		case MAGICTYPE_CANCELLATION:
			if(target){
				target->RemoveMagicEffect(MAGICTYPE_INVISIBILITY);
				target->RemoveMagicEffect(MAGICTYPE_PROTECT);
				target->RemoveMagicEffect(MAGICTYPE_HOLDOBJECT);
				target->RemoveMagicEffect(MAGICTYPE_CONFUSE);
				target->RemoveMagicEffect(MAGICTYPE_BERSERK);
				target->RemoveMagicEffect(MAGICTYPE_ICE);
				//target->RemoveMagicEffect(MAGICTYPE_ICE2);
				target->RemoveMagicEffect(MAGICTYPE_ICE_LINEAR);
				target->RemoveMagicEffect(MAGICTYPE_POLYMORPH);
				target->RemoveMagicEffect(MAGICTYPE_INHIBITION);
			}
			break;

		case MAGICTYPE_INVISIBILITY:
			switch (spell->m_sValue[MAGICV_TYPE]) 
			{
			case 1: // Invis
				if (!target || !target->AddMagicEffect(spell->m_sType, spell->m_dwLastTime)) goto NMH_NOEFFECT;
				break;

			case 2: // Detect Invis
				for (ix = dX - 8; ix <= dX + 8; ix++)
					for (iy = dY - 8; iy <= dY + 8; iy++){
						target = g_mapList[m_cMapIndex]->GetOwner(ix, iy);
						if (target){
							target->RemoveMagicEffect(spell->m_sType);
							g_game->RemoveFromTarget(target);
						}
					}
				break;
			}
			break;

		case MAGICTYPE_HOLDOBJECT:
			if (target && CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult, MAGICTYPE_HOLDOBJECT) == FALSE) {
				if(target->IsNPC() && g_npcList[target->m_handle]->m_cMagicLevel >= 6) 
						break;
				if (!target->AddMagicEffect(spell->m_sType, spell->m_dwLastTime)) 
						break;
			}else if (target && target->IsPlayer()) g_clientList[target->m_handle]->CalcChargeReduct(OBJECTMAGIC, MAGICTYPE_HOLDOBJECT);
			break;

		case MAGICTYPE_DAMAGE_LINEAR:
			sX = m_sX;
			sY = m_sY;

			for (i = 2; i < 10; i++) {
				iErr = 0;
				CMisc::GetPoint2(sX, sY, dX, dY, &tX, &tY, &iErr, i);

				for(int pNo=0; pNo < 5; pNo++){
					g_mapList[m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, tX+crossPnts[pNo][0], tY+crossPnts[pNo][1]);
					if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
						g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS] + iWhetherBonus, FALSE, iMagicAttr);

					g_mapList[m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, tX+crossPnts[pNo][0], tY+crossPnts[pNo][1]);
					if ( (cOwnerType == OWNERTYPE_PLAYER) && (g_clientList[sOwnerH] != NULL) &&
						(g_clientList[sOwnerH]->m_iHP > 0) ) {
							if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
								g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARRANGE] + iWhetherBonus, FALSE, iMagicAttr);
					}
				}
				if ( (abs(tX - dX) <= 1) && (abs(tY - dY) <= 1)) break;
			}

			for (iy = dY - spell->m_vRange; iy <= dY + spell->m_vRange; iy++)
				for (ix = dX - spell->m_hRange; ix <= dX + spell->m_hRange; ix++) {
					g_mapList[m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
					if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
						g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARRANGE] + iWhetherBonus, FALSE, iMagicAttr);

					g_mapList[m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
					if ( (cOwnerType == OWNERTYPE_PLAYER) && (g_clientList[sOwnerH] != NULL) &&
						(g_clientList[sOwnerH]->m_iHP > 0) ) {
							if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
								g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARRANGE] + iWhetherBonus, FALSE, iMagicAttr);
					}
				}

			// dX, dY
			g_mapList[m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
			if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
				g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, FALSE, iMagicAttr);

			g_mapList[m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
			if ( (cOwnerType == OWNERTYPE_PLAYER) && (g_clientList[sOwnerH] != NULL) &&
				(g_clientList[sOwnerH]->m_iHP > 0) ) {
					if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
						g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, FALSE, iMagicAttr);
			}
			break;
		case MAGICTYPE_ICE_LINEAR:
			sX = m_sX;
			sY = m_sY;
			for (i = 2; i < 10; i++) {
				iErr = 0;
				CMisc::GetPoint2(sX, sY, dX, dY, &tX, &tY, &iErr, i);

				for(int pNo=0; pNo < 5; pNo++){
					target = g_mapList[m_cMapIndex]->GetOwner(tX+crossPnts[pNo][0], tY+crossPnts[pNo][1]);
					if (target && CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE) {
						g_game->Effect_Damage_Spot_DamageMove(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, sX, sY, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS] + iWhetherBonus, FALSE, iMagicAttr);
						if(target && !target->IsDead() && !checkResistingIceSuccess(m_cDir, target->m_handle, target->m_ownerType, iResult))
							target->AddMagicEffect(spell->m_sType, spell->m_sValue[MAGICV_ICEDURATION]);
					}

					target = g_mapList[m_cMapIndex]->GetDeadOwner(tX+crossPnts[pNo][0], tY+crossPnts[pNo][1]);
					if (target && !target->IsDead() && target->IsPlayer() && CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE) {
						g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS] + iWhetherBonus, FALSE, iMagicAttr);
						if(target && !target->IsDead() && !checkResistingIceSuccess(m_cDir, target->m_handle, target->m_ownerType, iResult))
							target->AddMagicEffect(spell->m_sType, spell->m_sValue[MAGICV_ICEDURATION]);
					}
				}
				if ((abs(tX - dX) <= 1) && (abs(tY - dY) <= 1)) break;
			}
			
			for (iy = dY - spell->m_vRange; iy <= dY + spell->m_vRange; iy++)
				for (ix = dX - spell->m_hRange; ix <= dX + spell->m_hRange; ix++) {
					target = g_mapList[m_cMapIndex]->GetOwner(ix, iy);
					if (target && CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE) {
						g_game->Effect_Damage_Spot_DamageMove(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, sX, sY, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS] + iWhetherBonus, FALSE, iMagicAttr);
						if(target && !target->IsDead() && !checkResistingIceSuccess(m_cDir, target->m_handle, target->m_ownerType, iResult))
							target->AddMagicEffect(spell->m_sType, spell->m_sValue[MAGICV_ICEDURATION]);
					}

					target = g_mapList[m_cMapIndex]->GetDeadOwner(ix, iy);
					if (target && !target->IsDead() && target->IsPlayer() && CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE) {
						g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS] + iWhetherBonus, FALSE, iMagicAttr);
						if(target && !target->IsDead() && !checkResistingIceSuccess(m_cDir, target->m_handle, target->m_ownerType, iResult))
							target->AddMagicEffect(spell->m_sType, spell->m_sValue[MAGICV_ICEDURATION]);
					}
				}
				
			target = g_mapList[m_cMapIndex]->GetOwner(dX, dY);
			if (target && CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE) {
				g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, TRUE, iMagicAttr); 
				if(target && !target->IsDead() && checkResistingIceSuccess(m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE)
					target->AddMagicEffect(spell->m_sType, spell->m_sValue[MAGICV_ICEDURATION]);
			}

			target = g_mapList[m_cMapIndex]->GetDeadOwner(dX, dY);
			if (target && !target->IsDead() && target->IsPlayer()  && CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE) {
				g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, TRUE, iMagicAttr);
				if(target && !target->IsDead() && checkResistingIceSuccess(m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE)
					target->AddMagicEffect(spell->m_sType, spell->m_sValue[MAGICV_ICEDURATION]);
			}
			break;


		case MAGICTYPE_DAMAGE_SPOT:
			g_mapList[m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
			if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
				g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, TRUE, iMagicAttr);
			
			g_mapList[m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
			if ( (cOwnerType == OWNERTYPE_PLAYER) && (g_clientList[sOwnerH] != NULL) &&
				(g_clientList[sOwnerH]->m_iHP > 0) ) {
					if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
						g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, TRUE, iMagicAttr);
			}
			break;

		case MAGICTYPE_HPUP_SPOT:
			g_mapList[m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
			g_game->Effect_HpUp_Spot(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS]);
			break;

		case MAGICTYPE_DAMAGE_AREA:
			g_mapList[m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
			if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
				g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, TRUE, iMagicAttr);

			g_mapList[m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, dX, dY);
			if ( (cOwnerType == OWNERTYPE_PLAYER) && (g_clientList[sOwnerH] != NULL) &&
				(g_clientList[sOwnerH]->m_iHP > 0) ) {
					if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
						g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, TRUE, iMagicAttr);
			}

			for (iy = dY - spell->m_vRange; iy <= dY + spell->m_vRange; iy++)
				for (ix = dX - spell->m_hRange; ix <= dX + spell->m_hRange; ix++) {
					g_mapList[m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
					if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
						g_game->Effect_Damage_Spot_DamageMove(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, dX, dY, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS] + iWhetherBonus, FALSE, iMagicAttr);

					g_mapList[m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
					if ( (cOwnerType == OWNERTYPE_PLAYER) && (g_clientList[sOwnerH] != NULL) &&
						(g_clientList[sOwnerH]->m_iHP > 0) ) {
							if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
								g_game->Effect_Damage_Spot_DamageMove(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, dX, dY, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS] + iWhetherBonus, FALSE, iMagicAttr);
					}
				}
			break;

		case MAGICTYPE_DAMAGE_AREA_NOSPOT:
			for (iy = dY - spell->m_vRange; iy <= dY + spell->m_vRange; iy++)
				for (ix = dX - spell->m_hRange; ix <= dX + spell->m_hRange; ix++) {
					g_mapList[m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
					if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
						g_game->Effect_Damage_Spot_DamageMove(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, dX, dY, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS] + iWhetherBonus, FALSE, iMagicAttr);

					g_mapList[m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
					if ( (cOwnerType == OWNERTYPE_PLAYER) && (g_clientList[sOwnerH] != NULL) &&
						(g_clientList[sOwnerH]->m_iHP > 0) ) {
							if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
								g_game->Effect_Damage_Spot_DamageMove(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, dX, dY, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS] + iWhetherBonus, FALSE, iMagicAttr);
					}
				}
			break;

		case MAGICTYPE_SPDOWN_AREA:
			g_mapList[m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, dX, dY);
			if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
				g_game->Effect_SpDown_Spot(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS]);
			for (iy = dY - spell->m_vRange; iy <= dY + spell->m_vRange; iy++)
				for (ix = dX - spell->m_hRange; ix <= dX + spell->m_hRange; ix++) {
					g_mapList[m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
					if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, sOwnerH, cOwnerType, iResult) == FALSE)
						g_game->Effect_SpDown_Spot(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS]);
				}
			break;

		case MAGICTYPE_SPUP_AREA:
			spell->Effect_RecSP(this, target, spell->m_sValue[MAGICV_THROW], 
				spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS]);

			for (iy = dY - spell->m_vRange; iy <= dY + spell->m_vRange; iy++)
			{
				for (ix = dX - spell->m_hRange; ix <= dX + spell->m_hRange; ix++) {
					target = g_mapList[m_cMapIndex]->GetOwner(ix, iy);
					spell->Effect_RecSP(this, target, spell->m_sValue[MAGICV_LINEARTHROW], 
						spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS]);
				}
			}
			break;
			
		case MAGICTYPE_DAMAGE_AREA_NOSPOT_SPDOWN:
			for (iy = dY - spell->m_vRange; iy <= dY + spell->m_vRange; iy++)
			{
				for (ix = dX - spell->m_hRange; ix <= dX + spell->m_hRange; ix++)
				{
					g_mapList[m_cMapIndex]->GetOwner(&sOwnerH, &cOwnerType, ix, iy);
					if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, sOwnerH, cOwnerType, iResult) == FALSE) {
						g_game->Effect_Damage_Spot_DamageMove(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, dX, dY, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, FALSE, iMagicAttr, spell);
						g_game->Effect_SpDown_Spot(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS]);
					}

					g_mapList[m_cMapIndex]->GetDeadOwner(&sOwnerH, &cOwnerType, ix, iy);
					if ( (cOwnerType == OWNERTYPE_PLAYER) && (g_clientList[sOwnerH] != NULL) &&
						(g_clientList[sOwnerH]->m_iHP > 0) ) {

							if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, sOwnerH, cOwnerType, iResult) == FALSE) {
								g_game->Effect_Damage_Spot_DamageMove(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, dX, dY, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, FALSE, iMagicAttr, spell);
								g_game->Effect_SpDown_Spot(m_handle, OWNERTYPE_NPC, sOwnerH, cOwnerType, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS]);
							}
					}
				}
			}
			break;

		case MAGICTYPE_ICE:
			for (iy = dY - spell->m_vRange; iy <= dY + spell->m_vRange; iy++)
			for (ix = dX - spell->m_hRange; ix <= dX + spell->m_hRange; ix++) {

				target = g_mapList[m_cMapIndex]->GetOwner(ix, iy);
				if(target && !target->IsDead() && ((target->IsPlayer() && !target->IsNeutral()) || 
					(target->IsNPC() && g_npcList[target->m_handle]->m_bIsSummoned))) {
					if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE) {
						//g_game->Effect_Damage_Spot(iClientH, OWNERTYPE_PLAYER, sOwnerH, cOwnerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, TRUE, iMagicAttr);
							
						g_game->Effect_Damage_Spot_DamageMove(m_handle, OWNERTYPE_NPC,  target->m_handle, target->m_ownerType, dX, dY, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, TRUE, iMagicAttr);
						if (target && !target->IsDead() && !checkResistingIceSuccess(m_cDir, target->m_handle, target->m_ownerType, iResult))
							target->AddMagicEffect(spell->m_sType, spell->m_sValue[MAGICV_ICEDURATION]);
					}
				} // if

				target = g_mapList[m_cMapIndex]->GetDeadOwner(ix, iy);
				if(target && !target->IsDead() && target->IsPlayer() && !target->IsNeutral()){
					if (CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE) {

							//g_game->Effect_Damage_Spot(iClientH, OWNERTYPE_PLAYER, sOwnerH, cOwnerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, TRUE, iMagicAttr);
							g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, TRUE, iMagicAttr);
							if (target && !target->IsDead() && !checkResistingIceSuccess(m_cDir, target->m_handle, target->m_ownerType, iResult))
								target->AddMagicEffect(spell->m_sType, spell->m_sValue[MAGICV_ICEDURATION]);
					}
				} // if
			} // for
			break;
			
		case MAGICTYPE_CREATE_DYNAMIC:
			switch (spell->m_sValue[MAGICV_DYNAMICOBJ])
			{
			case DYNAMICOBJECT_PCLOUD_BEGIN: 
			case DYNAMICOBJECT_FIRE:   
			case DYNAMICOBJECT_SPIKE:  
				switch (spell->m_sValue[MAGICV_DYNAMICTYPE]) 
				{
				case 1: 
					// wall - type
					m_cDir = CMisc::cGetNextMoveDir(m_sX, m_sY, dX, dY);
					
					short rx, ry;
					switch (m_cDir) 
					{
					case 1:	rx = 1; ry = 0;   break;
					case 2: rx = 1; ry = 1;   break;
					case 3: rx = 0; ry = 1;   break;
					case 4: rx = -1; ry = 1;  break;
					case 5: rx = 1; ry = 0;   break;
					case 6: rx = -1; ry = -1; break;
					case 7: rx = 0; ry = -1;  break;
					case 8: rx = 1; ry = -1;  break;
					}

					g_game->iAddDynamicObjectList(m_handle, OWNERTYPE_NPC_INDIRECT, spell->m_sValue[MAGICV_DYNAMICOBJ], m_cMapIndex, 
						dX, dY, spell->m_dwLastTime*1000);

					for (i = 1; i <= spell->m_hRange; i++) {
						g_game->iAddDynamicObjectList(m_handle, OWNERTYPE_NPC_INDIRECT, spell->m_sValue[MAGICV_DYNAMICOBJ], m_cMapIndex, 
							dX + i*rx, dY + i*ry, spell->m_dwLastTime*1000);
						
						g_game->iAddDynamicObjectList(m_handle, OWNERTYPE_NPC_INDIRECT, spell->m_sValue[MAGICV_DYNAMICOBJ], m_cMapIndex, 
							dX - i*rx, dY - i*ry, spell->m_dwLastTime*1000);
					}
					break;

				case 2:
					// Field - Type
					bool bFlag = FALSE;
					int cx, cy;
					for (iy = dY - spell->m_vRange; iy <= dY + spell->m_vRange; iy++) {
						for (ix = dX - spell->m_hRange; ix <= dX + spell->m_hRange; ix++)
							g_game->iAddDynamicObjectList(m_handle, OWNERTYPE_NPC_INDIRECT, spell->m_sValue[MAGICV_DYNAMICOBJ], m_cMapIndex, 
							ix, iy, spell->m_dwLastTime*1000, spell->m_sValue[MAGICV_THROW]);
					}
					break;
				}
				break;

			case DYNAMICOBJECT_ICESTORM:
				g_game->iAddDynamicObjectList(m_handle, OWNERTYPE_NPC_INDIRECT, spell->m_sValue[MAGICV_DYNAMICOBJ], m_cMapIndex, 
					dX, dY, spell->m_dwLastTime*1000, 100);
				break;
			}
			break;

		case MAGICTYPE_DAMAGE_LINEAR_SPDOWN:
			sX = m_sX;
			sY = m_sY;

			for (i = 2; i < 10; i++) {
				iErr = 0;
				CMisc::GetPoint2(sX, sY, dX, dY, &tX, &tY, &iErr, i);
				for(int pNo = 0; pNo < 5; pNo++){
					target = g_mapList[m_cMapIndex]->GetOwner(tX+crossPnts[pNo][0], tY+crossPnts[pNo][1]);
					if (target && CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE) {

						g_game->Effect_Damage_Spot_DamageMove(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, sX, sY, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS] + iWhetherBonus, FALSE, iMagicAttr, spell);
						if (target && !target->IsDead() && CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE) {
							g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, TRUE, iMagicAttr, spell);
							g_game->Effect_SpDown_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS]);
						}
					}

					target = g_mapList[m_cMapIndex]->GetDeadOwner(tX+crossPnts[pNo][0], tY+crossPnts[pNo][1]);
					if (target && !target->IsDead() && target->IsPlayer() && CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE){

						g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS] + iWhetherBonus, FALSE, iMagicAttr, spell);
						if (target && !target->IsDead() && CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE) {
							g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, TRUE, iMagicAttr, spell);
							g_game->Effect_SpDown_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS]);
						}
					}
				}
				if ((abs(tX - dX) <= 1) && (abs(tY - dY) <= 1)) break;
			}


			for (iy = dY - spell->m_vRange; iy <= dY + spell->m_vRange; iy++)
				for (ix = dX - spell->m_hRange; ix <= dX + spell->m_hRange; ix++) {
					target = g_mapList[m_cMapIndex]->GetOwner(ix, iy);
					if (target && CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE) {

						g_game->Effect_Damage_Spot_DamageMove(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, dX, dY, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS] + iWhetherBonus, FALSE, iMagicAttr, spell);
						if (target && !target->IsDead() && CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE) {
							g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, TRUE, iMagicAttr, spell);
							g_game->Effect_SpDown_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS]);
						}
					}

					target = g_mapList[m_cMapIndex]->GetDeadOwner(ix, iy);
					if (target && !target->IsDead() && target->IsPlayer() && CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE){

						g_game->Effect_Damage_Spot_DamageMove(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, dX, dY, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS] + iWhetherBonus, FALSE, iMagicAttr, spell);
						if (target && !target->IsDead() && CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE) {
							g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, TRUE, iMagicAttr, spell);
							g_game->Effect_SpDown_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS]);
						}
					}
				}

				target = g_mapList[m_cMapIndex]->GetOwner(dX, dY);
				if (target && CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE) {
					g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, TRUE, iMagicAttr, spell);

					if (target && !target->IsDead() && CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE) {
						g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, TRUE, iMagicAttr, spell);
						g_game->Effect_SpDown_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS]);
					}
				}

				target = g_mapList[m_cMapIndex]->GetDeadOwner(dX, dY);
				if (target && !target->IsDead() && target->IsPlayer() && CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE){

					g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, FALSE, iMagicAttr, spell);
					if (target && !target->IsDead() && CheckResistingMagicSuccess(m_handle, OWNERTYPE_NPC, m_cDir, target->m_handle, target->m_ownerType, iResult) == FALSE) {
						g_game->Effect_Damage_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_THROW], spell->m_sValue[MAGICV_RANGE], spell->m_sValue[MAGICV_BONUS] + iWhetherBonus, TRUE, iMagicAttr, spell);
						g_game->Effect_SpDown_Spot(m_handle, OWNERTYPE_NPC, target->m_handle, target->m_ownerType, spell->m_sValue[MAGICV_LINEARTHROW], spell->m_sValue[MAGICV_LINEARRANGE], spell->m_sValue[MAGICV_LINEARBONUS]);
					}
				}
			break;
		}
	}
	else {


	}

NMH_NOEFFECT:;

	m_iMP -= spell->m_manaCost;
	if (m_iMP < 0) 
		m_iMP = 0;

	g_game->SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, COMMONTYPE_MAGIC, m_cMapIndex,
		m_sX, m_sY, dX, dY, NULL, (sType+100), m_sType);

}

int CNpc::GetGenLevel()
{
	switch (m_sType) 
	{
	case 10: // Slime
	case 16: // Giant-Ant
	case 22: // Amphis 
	case 55: // Rabbit	
	case 56: // Cat 
		return 1;

	case 14: // Orc
	case 18: // Zombie
	case 17: // Scorpion
	case 11: // Skeleton
		return 2;

	case 23: // clay-golem
	case 12: // stone-golem
		return 3;

	case 61: // rudolph
	case 27: // hellhound
		return 4;

	case 72: // Claw-Turtle
	case 76: // Giant-Plant
	case 74: // Giant-Crayfish
	case 13: // Cyclops
	case 28: // Troll
	case 53: // Beholder
	case 60: // Cannibal-Plant
	case 62: // DireBoar
		return 5;

	case 29: // Orge
	case 33: // WereWolf
	case 48: // Stalker
	case 54: // Dark-Elf
	case 65: // Ice-Golem
	case 78: // Minotaurus
		return 6;

	case 70: // Barlog
	case 71: // Centaurus
	case 30: // Liche
	case 63: // Frost
	case 79: // Nizie
		return 7;

	case 31: // Demon
	case 32: // Unicorn
	case 49: // Hellclaw
	case 50: // Tigerworm
	case 52: // Gagoyle
		return 8;

	case 58: // Mountain-Giant
		return 9;

	case 77: // MasterMage-Orc
	case 59: // Ettin
	case 75: // Lizards
		return 10;
	}

	return 0;
}

void CNpc::AddHP(uint32 value)
{
	//int iMaxHP = dice(m_iHitDice, 8) + m_iHitDice;

	if (m_iHP < m_iMaxHP)
	{
		m_iHP += value; 
		if (m_iHP > m_iMaxHP) m_iHP = m_iMaxHP;
		if (m_iHP <= 0)     m_iHP = 1;
	}

	if(m_sType == NPC_AS && m_cMapIndex == g_game->m_iAstoriaMapIndex &&
		g_game->m_astoria.get() && g_game->m_astoria->GetEventType() == ET_DESTROY_SHIELD)
	{
		g_game->m_astoria->NotifyShieldHP( this );
	}
}

void CNpc::ReduceHP(uint32 value)
{
	switch (m_cActionLimit) 
	{
	case 0: 		
	case 3: 
	case 5: 
	case 8: // gates
		m_iHP -= value;
		break;
	}

	if(m_sType == NPC_AS && m_cMapIndex == g_game->m_iAstoriaMapIndex &&
		g_game->m_astoria.get() && g_game->m_astoria->GetEventType() == ET_DESTROY_SHIELD)
	{
		g_game->m_astoria->NotifyShieldHP( this );
	}
}

void CNpc::Behavior_Death(Unit * attacker, int16 dmg)
{
	short  sAttackerWeapon = 1;
	int iContPts;
	CClient * player = (CClient*)attacker;
	CNpc * npc = (CNpc*)attacker;

	if(attacker && attacker->IsPlayer())
	{
		m_killerh = attacker->m_handle;
		sAttackerWeapon = (player->m_sAppr2 & 0x0FF0) >> 4;
	}

	m_bIsKilled = TRUE;
	m_iHP = 0;
	m_iLastDamage = dmg;

	g_mapList[m_cMapIndex]->m_iTotalAliveObject--;

	g_game->RemoveFromTarget(m_handle, OWNERTYPE_NPC);

	g_game->ReleaseFollowMode(m_handle, OWNERTYPE_NPC); 

	m_iTargetIndex = NULL;
	m_cTargetType  = NULL;

	g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, OBJECTDYING, dmg, sAttackerWeapon, NULL);

	g_mapList[m_cMapIndex]->ClearOwner(/*10,*/ m_handle, OWNERTYPE_NPC, m_sX, m_sY);

	g_mapList[m_cMapIndex]->SetDeadOwner(m_handle, OWNERTYPE_NPC, m_sX, m_sY);
	
	if (m_cActionLimit == 8 && m_cSize > 0) // gate death
	{	for(short sx1 = (m_sX - 1); sx1 <= m_sX + 1; sx1++)
		for(short sy1 = (m_sY - 1); sy1 <= m_sY + 1; sy1++)
		{	g_mapList[m_cMapIndex]->SetTempMoveAllowedFlag(sx1, sy1, TRUE); // restore move
	}	}

	m_cBehavior = BEHAVIOR_DEAD;

	m_sBehaviorTurnCount = 0;

	m_dwDeadTime = timeGetTime();

	if (g_mapList[m_cMapIndex]->m_cType == MAPTYPE_NOPENALTY_NOREWARD) return;

	g_game->NpcDeadItemGenerator(m_handle, (attacker) ? attacker->m_handle : NULL, (attacker) ? attacker->m_ownerType : 0 );


	if(!m_bIsSummoned && attacker && attacker->IsPlayer())
	{
	
		//iContPts = m_iContPts;
		int iExp = (m_iExp/3);
		if (m_iNoDieRemainExp > 0)
			iExp += m_iNoDieRemainExp;
			

		if (player->m_iAddExp != NULL) {
			double dTmp1 = (double)player->m_iAddExp;
			double dTmp2 = (double)iExp;
			double dTmp3 = (dTmp1/100.0f)*dTmp2;
			//contpts
		
			iExp += (int)dTmp3;
		}
	

		if (g_game->m_bIsCrusadeMode) {
			if (  (strcmp(player->m_cMapName, sideMap[NEUTRAL]) != 0)  
				|| (strcmp(player->m_cMapName, "aresden") != 0)
				|| (strcmp(player->m_cMapName, "elvine") != 0) )
				if (iExp > 10) iExp = iExp/4;
		}

		player->m_iContPtsStock += m_iContPts/2;
		g_game->GetExp(player->m_handle, iExp);

		int iQuestIndex = player->m_iQuest;
		if (iQuestIndex) 
		{
			if (g_game->m_pQuestConfigList[iQuestIndex])
			{
				switch (g_game->m_pQuestConfigList[iQuestIndex]->m_iType)
				{
				case QUESTTYPE_MONSTERHUNT:
					if ( (player->m_bQuestMatchFlag_Loc == TRUE) &&
						(g_game->m_pQuestConfigList[iQuestIndex]->m_iTargetType == m_sType) ) {
							player->m_iCurQuestCount++;
							int cQuestRemain = g_game->m_pQuestConfigList[player->m_iQuest]->m_iMaxCount - player->m_iCurQuestCount;
							player->Notify(NULL, NOTIFY_QUESTCOUNTER, cQuestRemain, NULL, NULL, NULL);
							//char cTemp22[100];
							//sprintf(cTemp22,"3 = T - %d - Q - %d - P - %d",cQuestRemain,g_game->m_pQuestConfigList[player->m_iQuest]->m_iMaxCount,player->m_iCurQuestCount);
							//PutLogList(cTemp22);
							g_game->_bCheckIsQuestCompleted(player->m_handle);
					}
					break;
				}
			}
		}
	}
	
	if(g_game->m_bHeldenianMode && g_mapList[m_cMapIndex]->m_bIsHeldenianMap) 
	{	
		if(attacker && attacker->IsNPC()) {
			if (npc && npc->m_bIsSummoned && npc->m_cFollowOwnerType == OWNERTYPE_PLAYER &&
			!npc->IsDead() && npc->m_iFollowOwnerIndex != NULL && g_clientList[npc->m_iFollowOwnerIndex] && !g_clientList[npc->m_iFollowOwnerIndex]->IsDead()) 
			{	
				g_clientList[npc->m_iFollowOwnerIndex]->m_iWarContribution += m_iHitDice;
			}
		}
	}

	if(m_cFollowOwnerType == OWNERTYPE_NPC && g_npcList[m_iFollowOwnerIndex] && 
		g_npcList[m_iFollowOwnerIndex]->IsHighlyTrained())
	{
		CHTNpc * htnpc = (CHTNpc*)g_npcList[m_iFollowOwnerIndex];
		htnpc->Behavior_MinionDeath(this);
	}

	if(g_game->m_bIsCrusadeMode)
	{
		int iConstructionPoint = 0;
		int iWarContribution = 0;
		BOOL bFlag = FALSE;

		switch (m_sType) 
		{
		case 1:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 2:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 3:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 4:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 5:  iConstructionPoint = 50; iWarContribution = 100; break;
		case 6:  iConstructionPoint = 50; iWarContribution = 100; break;		
		case 36: iConstructionPoint = 700; iWarContribution = 4000; break;
		case 37: iConstructionPoint = 700; iWarContribution = 4000; break;
		case 38: iConstructionPoint = 500; iWarContribution = 2000; break;		
		case 39: iConstructionPoint = 500; iWarContribution = 2000; break;		
		case 40: iConstructionPoint = 1500; iWarContribution = 5000; break;		
		case 41: iConstructionPoint = 5000; iWarContribution = 10000; break;		
		case 43: iConstructionPoint =  500; iWarContribution = 1000; break;		
		case 44: iConstructionPoint = 1000; iWarContribution = 2000; break;
		case 45: iConstructionPoint = 1500; iWarContribution = 3000; break;
		case 46: iConstructionPoint = 1000; iWarContribution = 2000; break;		
		case 47: iConstructionPoint = 1500; iWarContribution = 3000; break;		
		case 51: iConstructionPoint = 800; iWarContribution = 1500; break;

		case 64:
			g_mapList[m_cMapIndex]->bRemoveCropsTotalSum();
			break;
		}
		
		if (attacker && attacker->IsPlayer()) {
			for (int i = 0; i < g_game->m_pMapList[m_cMapIndex]->m_iTotalCrusadeStructures; i++)
			if (g_game->m_pMapList[m_cMapIndex]->m_stCrusadeStructureInfo[i].cType == NPC_MANASTONE && 
				abs(g_game->m_pMapList[m_cMapIndex]->m_stCrusadeStructureInfo[i].sX-player->m_sX) <= 60 && 
				abs(g_game->m_pMapList[m_cMapIndex]->m_stCrusadeStructureInfo[i].sY-player->m_sY) <= 60) {
					bFlag = TRUE;
			}
		}

		if(iConstructionPoint && attacker && bFlag)
		{
			if(attacker->IsPlayer())
			{
				if (player->m_side != m_side)
				{
					player->m_iConstructionPoint += iConstructionPoint;

					if (player->m_iConstructionPoint > MAXCONSTRUCTIONPOINT) 
						player->m_iConstructionPoint = MAXCONSTRUCTIONPOINT;

					player->m_iWarContribution   += iWarContribution;
					if (player->m_iWarContribution > MAXWARCONTRIBUTION)
						player->m_iWarContribution = MAXWARCONTRIBUTION;

					player->Notify(NULL, NOTIFY_CONSTRUCTIONPOINT, player->m_iConstructionPoint, player->m_iWarContribution, NULL, NULL);
				}
				else
				{
					player->m_iWarContribution = 0;

					wsprintf(g_cTxt, "WarContribution: Friendly Npc Killed by player(%s)! ", player->m_cAccountName);
					PutLogFileList(g_cTxt, EVENT_LOGFILE);

					player->Notify(NULL, NOTIFY_CONSTRUCTIONPOINT, player->m_iConstructionPoint, player->m_iWarContribution, NULL, NULL);
				}
			}
			else if(attacker->IsNPC() && npc->m_iGuildGUID)
			{
				if(npc->m_side != m_side)
				{
					for(int i = 1; i < MAXCLIENTS; i++)
					{
						if(g_clientList[i] && g_clientList[i]->m_iGuildGUID == npc->m_iGuildGUID &&
							g_clientList[i]->m_iCrusadeDuty == 3) 
						{
							g_clientList[i]->m_iConstructionPoint += iConstructionPoint;
							if (g_clientList[i]->m_iConstructionPoint > MAXCONSTRUCTIONPOINT) 
								g_clientList[i]->m_iConstructionPoint = MAXCONSTRUCTIONPOINT;

							g_game->SendNotifyMsg(NULL, i, NOTIFY_CONSTRUCTIONPOINT, g_clientList[i]->m_iConstructionPoint, g_clientList[i]->m_iWarContribution, NULL, NULL);
							goto NKH_GOTOPOINT1;
						}
					}

					char cData[120];
					ZeroMemory(cData, sizeof(cData));
					char * cp = (char *)cData;
					*cp = GSM_CONSTRUCTIONPOINT;
					cp++;
					int * ip = (int*)cp;
					*ip = npc->m_iGuildGUID;
					cp += 4;
					ip = (int*)cp;
					*ip = iConstructionPoint;
					cp += 4;
					g_game->bStockMsgToGateServer(cData, 9);
				}
			}
		}
	}

NKH_GOTOPOINT1:;

	if (m_cSpecialAbility == 7) {
		m_iMP = 100;
		m_iMagicHitRatio = 100;
		magicHandler(m_sX, m_sY, 30); 
	}
	else if (m_cSpecialAbility == 8) {
		m_iMP = 100;
		m_iMagicHitRatio = 100;
		magicHandler(m_sX, m_sY, 61); 
	}

	if(g_game->m_bHeldenianMode && (g_game->m_iHeldenianType == 1) && g_mapList[m_cMapIndex]->m_bIsHeldenianMap) 
	{	
		if ((m_sType == NPC_CT) || (m_sType == NPC_AGC)) 
		{	
			if (m_side == ARESDEN) 
			{	
				g_game->m_iHeldenianAresdenLeftTower--;
				wsprintf(g_cTxt, "Aresden Tower Broken, Left TOWER %d", g_game->m_iHeldenianAresdenLeftTower);
				PutLogList(g_cTxt);
				g_game->UpdateHeldenianStatus();
				if (g_game->m_iHeldenianAresdenLeftTower <= 0)	g_game->HeldenianEndWarNow(g_game->m_iHeldenianType, ELVINE);				
			}
			else if (m_side == ELVINE) 
			{	
				g_game->m_iHeldenianElvineLeftTower--;
				wsprintf(g_cTxt, "Elvine Tower Broken, Left TOWER %d", g_game->m_iHeldenianElvineLeftTower);
				PutLogList(g_cTxt);
				g_game->UpdateHeldenianStatus();
				if (g_game->m_iHeldenianElvineLeftTower <= 0)	g_game->HeldenianEndWarNow(g_game->m_iHeldenianType, ARESDEN);	

			}	
		}	
	}
}

void CNpc::Cast(Unit * target, short spell)
{
	Cast(target->m_sX, target->m_sY, spell);
}

void CNpc::Cast(short x, short y, short spell)
{
	magicHandler(x, y, spell);
	m_cDir = CMisc::cGetNextMoveDir(m_sX, m_sY, x, y);
	g_game->SendEventToNearClient_TypeA(m_handle, OWNERTYPE_NPC, MSGID_EVENT_MOTION, 
		OBJECTATTACK, x + _tmp_cTmpDirX[m_cDir], y + _tmp_cTmpDirY[m_cDir], 1); 
}