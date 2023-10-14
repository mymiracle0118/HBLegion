#include <windows.h>
#include "GuildMgr.h"


CGuildMgr::CGuildMgr(void)
{

}

CGuildMgr::~CGuildMgr(void)
{
}

void CGuildMgr::Empty()
{
	GuildMapIter curr, it = _guildMap.begin();
	while(it != _guildMap.end())
	{
		curr = it++;
		curr->second->Save(TRUE);
		delete curr->second;
		_guildMap.erase(curr);
	}
}

void CGuildMgr::Add(CGuild * guild)
{
	_guildMap[guild->m_guildGUID] = guild;

	
	ClientListIter it = _pendingGldMbrs.begin();
	while(it != _pendingGldMbrs.end())
	{
		ClientListIter curr = it++;
		if((*curr)->m_iGuildGUID == guild->m_guildGUID)
		{
			guild->MemberLogin(*curr);
			_pendingGldMbrs.erase(curr);
		}
	}
}

void CGuildMgr::Remove(CGuild * guild)
{
	_guildMap.erase(guild->m_guildGUID);

	delete guild;
}

void CGuildMgr::_RemoveExpired()
{
	GuildMapIter it = _guildMap.begin();
	while(it != _guildMap.end())
	{
		GuildMapIter curr = it++;
		CGuild * guild = curr->second;
		if(guild->GetOnlineCnt() == 0 && time(NULL) - guild->m_lastActive >= HOUR(1))
		{
			curr->second->Save(TRUE);
			delete curr->second;
			_guildMap.erase(curr);
		}
	}
}

CGuild * CGuildMgr::FindGuild(uint32 guildID) const
{
	GuildMap::const_iterator it = _guildMap.find(guildID);

	if(it != _guildMap.end())
		return it->second;

	return NULL;
}

void CGuildMgr::AddPendingMember(CClient * player)
{
	_pendingGldMbrs.push_front(player);
}

void CGuildMgr::RemovePendingMember(CClient * player)
{
	_pendingGldMbrs.remove(player);
}

void CGuildMgr::Update()
{
	static uint32 updateTime = timeGetTime();
	uint32 curTime = timeGetTime();
	
	if(curTime - updateTime < 5 _m)
		return;
		
	_RemoveExpired();
	_SaveGuilds();

	updateTime = curTime;
}

void CGuildMgr::_SaveGuilds()
{
	GuildMapIter it = _guildMap.begin();
	while(it != _guildMap.end())
	{
		it->second->Save(TRUE);
		++it;
	}
}

CGuild * CGuildMgr::Get() const
{
	static bool getting = FALSE;
	static GuildMap::const_iterator it;

	CGuild * guild = NULL;

	if(getting)
	{
		if(it == _guildMap.end())
		{
			getting = FALSE;
			return NULL;
		}

		guild = (*it++).second;
		return guild;
	}
	else
	{
		it = _guildMap.begin();

		if(it == _guildMap.end())
			return NULL;

		getting = TRUE;
		guild = (*it++).second;
		return guild;
	}
}