#ifndef HTNPC_H
#define HTNPC_H
#pragma once

#include "npc.h"

class CHTNpc : public CNpc
{
public:
	CHTNpc(char * pName5, int NpcH);
	~CHTNpc(void);
	
	void Behavior_Death(Unit * attacker, int16 dmg);
	void Behavior_MinionDeath(CNpc * minion);
	void Behavior_Attack();
	void Behavior_Move();
	bool Behavior_Detect();
	void TargetSearch(short * pTarget, char * pTargetType);

private:
	bool _SummonMinion();

	uint8 _minions;
};

#endif