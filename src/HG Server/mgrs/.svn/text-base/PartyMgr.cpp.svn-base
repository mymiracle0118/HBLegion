#include <windows.h>
#include "PartyMgr.h"


CPartyMgr::CPartyMgr(void)
{
	_idCnt = timeGetTime();
}

CPartyMgr::~CPartyMgr(void)
{
}

CParty * CPartyMgr::CreateParty(CClient * player, CClient * player2)
{
	CParty * party = new CParty( _GetNewID() );

	_parties[ party->GetID() ] = party;

	if(player2)
	{
		ASSERT(player->m_pXSock);
		party->AddPlayer( player2 );
	}
	
	if(player)
	{
		ASSERT(player->m_pXSock);
		party->AddPlayer( player );
	}

	return party;
}

void CPartyMgr::DeleteParty(uint32 id)
{
	PartyMapCIter iter = _parties.find( id );
	if(iter != _parties.end())
	{
		delete (*iter).second;
		_parties.erase( iter );
	}
}

void CPartyMgr::RemoveFromParty(CClient * player, bool bBan)
{
	if(!player->GetParty()) 
		return;

	PartyMapCIter iter = _parties.find( player->GetParty()->GetID() );
	if(iter != _parties.end() && (*iter).second->WithdrawPlayer( player, bBan ) )
	{
		delete (*iter).second;
		_parties.erase( iter );
	}
}
