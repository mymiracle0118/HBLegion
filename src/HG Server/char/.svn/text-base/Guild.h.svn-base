#ifndef GUILD_H
#define GUILD_H
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <time.h>
#include "..\..\shared\common.h"
#include "client.h"
#include "item\item.h"

class CGuild
{
public:
	CGuild(int16 guildID, char * name, char * data = NULL);
	~CGuild(void);
	
	int16 m_guildGUID;
#define GUILDGUID_NONE		-1
	char m_name[21];
	uint8 m_upgrades[GU_MAX];
	time_t m_lastActive;
	uint32 warehouseVer;
	int m_iBisleScore;
	void NotifyPingAlert(CClient * player, short sX, short sY);
	void Save(bool saveWH = FALSE) const;
	void MemberLogin(CClient * player);
	bool MemberLogout(CClient * player);
	void MemberWithdrawal(CClient * player);
	void MemberMapChanged(CClient * player) const;
	void UpdateMemberCoords(CClient * player) const;
	void UpdateMemberMap(CClient * player) const;
	void WriteInfo(char * &index) const;
	void WriteMemberList(char * &index, CClient const * player) const;
	uint32 GetOnlineCnt()	const { return _onlineList.size(); }
	CClient * GetMaster()	const { return _master; }
	CClient * GetMembers() const;
	
	uint32 GetGold()			const { return _gold; }
	void ReduceGold(uint32 val)	{ _gold -= val;	_GuildInfoChanged(); }
	void ContributeGold(CClient * player, uint32 val);
	uint32 GetMaj()			const { return _maj; }
	void ReduceMaj(uint32 val)		{ _maj -= val;		_GuildInfoChanged(); }
	void ContributeMaj(CClient * player, uint32 val);
	uint32 GetCont()			const { return _cont; }
	void ReduceCont(uint32 val)	{ _cont -= val;	_GuildInfoChanged(); }
	void ContributeCont(CClient * player, uint32 val);
	void AddCont(uint32 val);
	uint8 GetNCaptains()			const { return _captains; }
	uint8 GetNRaidmasters()		const { return _raidmasters; }
	uint8 GetNHuntmasters()		const { return _huntmasters; }
	
	void GuildUpgrade(CClient * player, GuildUpgrades gu);
	void GuildsmanChange(CClient * player, GuildRank gr, string memberName);

	void RequestTeleport(CClient const * master, string memberName) const;
	bool CanSummon(CClient const * master, CClient const * member ) const;
	void LoadWarehouse(char * data);
	void WriteWarehouse_Save(char * &index) const;
	void WriteWarehouse(char * &index) const;
	bool AddToWarehouse(CItem * item, CClient * player = NULL);
	bool RetrieveFromWarehouse(CItem * item, CClient * player);

	void Broadcast(char * data, uint32 length) const;

	CClient * FindPlayer(string name) const {
		for(ClientListCIter it = _onlineList.begin();
			it != _onlineList.end();
			++it)
		{
			if( (*it)->m_cCharName == name )
				return *it;
		}

		return NULL;
	}
	bool operator==(int16 guildID)	const { return (m_guildGUID == guildID); }

private:
	void _GuildInfoChanged() const;
	uint32 _GetAvailableCaptains()		const { return m_upgrades[GU_CAPTAINCY] - _captains; }
	uint32 _GetAvailableRaidmasters()	const { return m_upgrades[GU_RAIDMASTERS] - _raidmasters; }
	uint32 _GetAvailableHuntmasters()	const { return m_upgrades[GU_HUNTMASTERS] - _huntmasters; }

	ClientList _onlineList;
	ItemList _warehouse;
	CClient * _master;
	uint32 _gold;
	uint32 _maj;
	uint32 _cont;
	uint8 _captains, _raidmasters, _huntmasters;
};

typedef HashMap<int16, CGuild*> GuildMap;
typedef HashMap<int16, CGuild*>::iterator GuildMapIter;
typedef HashMap<int16, CGuild*>::const_iterator GuildMapCIter;

#endif