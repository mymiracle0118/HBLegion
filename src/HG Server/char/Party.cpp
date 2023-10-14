#include <windows.h>
#include "party.h"
#include "..\..\shared\buffer.h"

extern char packet[500];

CParty::CParty(uint32 id) : _id(id)
{
	m_bIsPingEnabled = true;
	m_lBanList.empty();
}

CParty::~CParty()
{	
	while(CClient * member = GetMembers())
	{
		member->Notify(NULL, NOTIFY_PARTY, 2, 0, NULL, NULL);
		member->SetParty( NULL );
	}
}

void CParty::AddPlayer(CClient * player)
{
	if (GetPlayerCnt() <= 0) {
		m_iLeaderID = player->m_handle;
		memcpy(m_cLeaderName, player->m_cCharName, 10);
	}

	_players.push_front( player );

	NotifyMemberList( player );
	NotifyPartyStatus( player );

	player->SetParty( this );

	while(CClient * member = GetMembers())
	{
		member->Notify(NULL, NOTIFY_PARTY, 4, 1, NULL, player->m_cCharName);

		if(member == player || player->m_cMapIndex != member->m_cMapIndex)
			continue;
			
		member->Notify( player->m_handle, NOTIFY_PARTY_COORDS, 
			player->m_sX, player->m_sY, NULL, player->m_cCharName );
	}

	UpdateGuildBonuses();
}

bool CParty::WithdrawPlayer(CClient * player, bool bBan)
{
	if (!player) return _players.empty();

	while(CClient * member = GetMembers())
	{
		if (bBan) member->Notify(NULL, NOTIFY_PARTY, 6, 2, NULL, player->m_cCharName);
		else member->Notify(NULL, NOTIFY_PARTY, 6, 1, NULL, player->m_cCharName);
	}

	if (bBan) m_lBanList.push_back(player->m_cCharName);

	_RemovePlayer(player);

	if(_players.size() == 1)
	{
		ClientListIter iter = _players.begin();
		CClient * solePlayer = *iter;
		_RemovePlayer(solePlayer);

		solePlayer->Notify(NULL, NOTIFY_PARTY, 2, 0, NULL, NULL);
	} else {
		UpdateGuildBonuses();
	}

	return _players.empty();
}

void CParty::_RemovePlayer(CClient * player)
{
	_players.remove( player );
	player->SetParty( NULL );

	if (m_iLeaderID == player->m_handle) {
		if (_players.size() > 1) {
			ClientListCIter iter = _players.end();
			CClient * newLeader = (*iter--);
			m_iLeaderID = newLeader->m_handle;
			memcpy(m_cLeaderName, newLeader->m_cCharName, 10);
			while(CClient * member = GetMembers()) NotifyMemberList( member );
		}
	}
}

void CParty::NotifyPingAlert(CClient * player, short sX, short sY)
{
	if (!m_bIsPingEnabled && player->m_handle != m_iLeaderID) return;

	// Notify party members
	while(CClient * member = GetMembers()) {
		if (player->m_cMapIndex == member->m_cMapIndex) member->Notify(player->m_handle, NOTIFY_PINGALERT, sX, sY, -1);
	}
}

/*void CParty::BanMember(char * pMemberName)
{
	while(CClient * member = GetMembers()) {
		if (strcmp(pMemberName, (char *)member->m_cCharName) == 0) WithdrawPlayer(member, true);
	}
}*/

void CParty::NotifyMemberList(CClient * player) const
{
	char * index = packet;
	char cPartyRank;
	
	Push(index, (char *)m_cLeaderName, 10);

	while(CClient * member = GetMembers())
	{
		/*if(member == player)
			continue;*/
		Push(index, member->m_cCharName, 10);
	}
	player->Notify(NULL, NOTIFY_PARTY, 5, 1, _players.size(), packet);
}


void CParty::NotifyPartyStatus(CClient * player)
{
	if (player == NULL) {
		while(CClient * member = GetMembers())
		{
			member->Notify(NULL, NOTIFY_PARTY, 9, m_bIsPingEnabled);
		}
	}else {
		player->Notify(NULL, NOTIFY_PARTY, 9, m_bIsPingEnabled);
	}
}

void CParty::UpdateMemberCoords(CClient * player) const
{
	if(player->m_partyCoordSteps >= 4)
	{
		player->m_partyCoordSteps = 0;
		
		while(CClient * member = GetMembers())
		{
			if(member == player || player->m_cMapIndex != member->m_cMapIndex)
				continue;

			member->Notify(player->m_handle, NOTIFY_PARTY_COORDS, 
					player->m_sX, player->m_sY, NULL, player->m_cCharName);
		}
	}
	
	player->m_partyCoordSteps++;
}

void CParty::UpdateMemberMap(CClient * player) const
{
	while(CClient * member = GetMembers())
	{
		if(member == player)
			continue;

		// debug tests
		ASSERT(member->m_pXSock);
		if(!member->m_pXSock)
			continue;

		if(player->m_cMapIndex == member->m_cMapIndex)
		{
			player->Notify(member->m_handle, NOTIFY_PARTY_COORDS, 
				member->m_sX, member->m_sY, NULL, member->m_cCharName);
			member->Notify(player->m_handle, NOTIFY_PARTY_COORDS, 
				player->m_sX, player->m_sY, NULL, player->m_cCharName);
		}
		else
		{
			member->Notify(player->m_handle, NOTIFY_PARTY_COORDS, 0, 0, NULL, player->m_cCharName);
		}
	}
}

void CParty::UpdateGuildBonuses()
{	
	_gldHuntmasters.clear();
	_gldRaidmasters.clear();

	while(CClient * player = GetMembers())
	{
		if(!player->m_guild) continue;

		switch( player->m_iGuildRank )
		{
		case GUILDRANK_RAIDMASTER:
			_gldRaidmasters[ player->m_guild ].push_front( player );
			break;

		case GUILDRANK_HUNTMASTER:
			_gldHuntmasters[ player->m_guild ].push_front( player );
			break;

		default:
			continue;
		}
	}
}

uint32 CParty::GetAlivePlayerCnt() const
{ 
	uint32 n = 0;

	for(ClientListCIter iter = _players.begin();
		iter != _players.end();
		++iter)
	{
		if( !(*iter)->IsDead() )
			n++;
	}

	return n; 
}

CClient * CParty::GetMembers() const
{
	static bool getting = FALSE;
	static ClientListCIter iter;

	if(getting)
	{
		if(iter == _players.end())
		{
			getting = FALSE;
			return NULL;
		}

		return (*iter++);
	}
	else
	{
		iter = _players.begin();

		if(iter == _players.end())
			return NULL;

		getting = TRUE;
		return (*iter++);
	}
}

CClient * CParty::GetHuntmasters(CGuild * guild)
{
	static bool getting = FALSE;
	static ClientListCIter iter, end;
	
	if(_gldHuntmasters.count(guild) == 0)
		return NULL;

	if(getting)
	{
		if(iter == end)
		{
			getting = FALSE;
			return NULL;
		}

		return (*iter++);
	}
	else
	{
		iter = _gldHuntmasters[guild].begin();
		end = _gldHuntmasters[guild].end();

		if(iter == end)
			return NULL;

		getting = TRUE;
		return (*iter++);
	}
}

CClient * CParty::GetRaidmasters(CGuild * guild)
{
	static bool getting = FALSE;
	static ClientListCIter iter, end;

	if(_gldRaidmasters.count(guild) == 0)
		return NULL;

	if(getting)
	{
		if(iter == end)
		{
			getting = FALSE;
			return NULL;
		}

		return (*iter++);
	}
	else
	{
		iter = _gldRaidmasters[guild].begin();
		end = _gldRaidmasters[guild].end();

		if(iter == end)
			return NULL;

		getting = TRUE;
		return (*iter++);
	}
}