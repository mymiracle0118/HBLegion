#ifndef PARTY_H
#define PARTY_H
#pragma once

#include "..\..\shared\common.h"
#include "client.h"

#define GUILDPARTYBONUS_RANGE_X		FLAGRANGE_X
#define GUILDPARTYBONUS_RANGE_Y		FLAGRANGE_Y

class CParty
{
public:
	CParty(uint32 id);
	~CParty();
	
	uint32 GetPlayerCnt()	const { return _players.size(); }
	uint32 GetAlivePlayerCnt()	const;
	CClient * GetMembers() const;
	CClient * GetBannedMembers() const;
	CClient * GetHuntmasters(CGuild * guild);
	CClient * GetRaidmasters(CGuild * guild);
	void NotifyPartyStatus(CClient * player = NULL);
	//void BanMember(char * pMemberName);
	void AddPlayer(CClient * player);
	bool WithdrawPlayer(CClient * player, bool bBan = false);
	void NotifyMemberList(CClient * player) const;
	void UpdateMemberCoords(CClient * player) const;
	void UpdateMemberMap(CClient * player) const;
	void UpdateGuildBonuses();
	void NotifyPingAlert(CClient * player, short sX, short sY);

	uint32 GetID()	const { return _id; }

	bool m_bIsPingEnabled;
	int m_iLeaderID;
	char m_cLeaderName[11];
	std::vector<string> m_lBanList;

private:
	void _RemovePlayer(CClient * player);

	typedef Map< CGuild*, ClientList > GuildPartyMastersMap;
	GuildPartyMastersMap _gldHuntmasters;
	GuildPartyMastersMap _gldRaidmasters;

	ClientList _players;
	uint32 _id;
};

typedef HashMap< uint32, CParty* > PartyMap;
typedef HashMap< uint32, CParty* >::const_iterator PartyMapCIter;

#endif