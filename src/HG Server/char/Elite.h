#ifndef ELITE_H
#define ELITE_H
#pragma once

#include "npc.h"

class CElite : public CNpc
{
public:
	CElite(char * pName5, int NpcH);
	~CElite(void);
	
	void RegenHP();
	void RegenMP();
	void Behavior_Death(Unit * attacker, int16 dmg);
	void Behavior_MinionDeath(CNpc * minion);
	void Behavior_Attack();
	void Behavior_Move();
	bool Behavior_Detect();
	void TargetSearch(short * pTarget, char * pTargetType);
	void TargetSearch();

	static const uint32 maxtargets = 3;
	static const uint32 maxtimers = 1;

private:
	bool _SummonMinion();
	Unit * _targets[maxtargets];
	uint8 _visTargets;
	time_t _timers[1];

	uint8 _minions;
};

#endif