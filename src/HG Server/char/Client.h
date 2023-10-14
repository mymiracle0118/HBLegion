// Client.h: interface for the CClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENT_H__39CC7700_789F_11D2_A8E6_00001C7030A6__INCLUDED_)
#define AFX_CLIENT_H__39CC7700_789F_11D2_A8E6_00001C7030A6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>
#include <winbase.h>
#include <stdio.h>
#include <stdlib.h>
#include "..\net\XSocket.h"
#include "item\Item.h"
#include "Magic.h"
#include "..\GlobalDef.h"

class CGuild;
class CParty;

#define CLIENTSOCKETBLOCKLIMIT	15

#define MAXITEMS		50
#define MAXAUCTIONITEMS	14
#define MAXGUILDSMAN	128 

#define	MAXMAGICTYPE	100
#define MAXSKILLTYPE	24


#define MAXTELEPORTLIST	20

#define SPECABLTYTIMESEC	3600

enum PartyStatus
{
	PS_NOTINPARTY,
	PS_PROCESSING,
	PS_INPARTY
};

enum Sex{
	NONE,
	MALE,
	FEMALE
};

#define FLAGRANGE_X		11
#define FLAGRANGE_Y		9

class CClient sealed : public Unit
{
public:
	CClient(HWND hWnd, int clientH);
	~CClient();
	bool CheckNearbyFlags();
	bool IsHeldWinner() const;
	bool IsHeldLoser() const;

	void WithdrawFromGuild();

	void CalcChargeReduct(char cAttackType, short cElementType = -1, char cAttackerType = -1);
	bool bCheckItemChargeEffect(char cAttackType, char cElementType, DWORD dwTemp, char cAttackerType);
	bool CheckTotalSkillMasteryPoints(int iSkill);
	void ValidateSkills(bool logInvalidSkills);
	int GetPlayerRelationship(int iOpponentH) const;
	void KilledHandler(int iAttackerH, char cAttackerType, short sDamage);
	void ApplyCombatKilledPenalty(char cPenaltyLevel, bool bIsSAattacked = FALSE, bool bItemDrop = FALSE);
	void PenaltyItemDrop(int iTotal, bool bIsSAattacked = FALSE, bool bItemDrop = FALSE);
	void ApplyPKPenalty(short sVictimH);
	void ApplyElo(CClient * foe);
	int GetMaxHP() const;
	int GetMaxMP() const;
	int GetMaxSP() const;
	void AddHP(long hp);
	int GetStr()		const { return _str + _angelStr; }
	int GetMag()		const { return _mag + _angelMag; }
	int GetInt()		const { return _int + _angelInt; }
	int GetDex()		const { return _dex + _angelDex; }
	int GetBaseStr()	const { return _str; }
	int GetBaseMag()	const { return _mag; }
	int GetBaseInt()	const { return _int; }
	int GetBaseDex()	const { return _dex; }
	void SetStr(int str, bool check = true);
	void SetMag(int mag);
	void SetInt(int __int, bool check = true);
	void SetDex(int dex);
	int GetAngelStr()	const { return _angelStr;	}
	int GetAngelMag()	const { return _angelMag;	}
	int GetAngelInt()	const { return _angelInt; }
	int GetAngelDex()	const { return _angelDex;	}
	void SetAngelStr(int str);
	void SetAngelInt(int __int);
	void SetAngelDex(int dex);
	void SetAngelMag(int mag);
	
	int GetEffectiveMA() const;

	bool IsInFoeMap();
	bool IsInJail();
	void DecPKCount();
	void IncPKCount();

	float GetDropFactor() const;

	bool IsGM()				{ return (m_iAdminUserLevel == 0)			? FALSE : TRUE; }
	bool IsInvincible()	{ return (m_GMFlags & GMFLAG_INVINCIBLE)	? TRUE : FALSE; }
	bool IsNoAggro()		{ return (m_GMFlags & GMFLAG_NOAGGRO)			? TRUE : FALSE; }
	bool IsEthereal()		{ return (m_GMFlags & GMFLAG_ETHEREAL); }

	void Save();
	void Notify(int iFromH, WORD wMsgType, DWORD sV1 = NULL, DWORD sV2 = NULL, DWORD sV3 = NULL, char * pString = NULL, 
		DWORD sV4 = NULL, DWORD sV5 = NULL, DWORD sV6 = NULL, DWORD sV7 = NULL, DWORD sV8 = NULL, DWORD sV9 = NULL, 
		char * pString2 = NULL) const;
	void NotifyGuildInfo(bool memberList = FALSE) const;
	void NotifyGuildsmanStatus(CClient const * const player, bool online = true) const;
	void NotifyGuildSummons(CClient const * const player) const;

	int GetWeight()	const { return m_iCurWeightLoad; }
	void UpdateWeight();
	uint32 HasItem(char * name) const;
	uint32 HasItem(ItemID id) const;
	uint32 GetItemCount(ItemID id) const;
	void SetItemCount(ItemID id, uint32 val, bool notify = true);

	bool IsFlooding(uint32 sensitivity);
	bool IsInCombatMode()	const { return (m_sAppr2 & 0xF000) >> 12; }

	int m_iTitle;
	int bTitleList[MAXTITLES];
	int bTitleUsesList[MAXTITLES];
	int m_iTitleCount;

	char m_cCharName[12];
	char m_cAccountName[12];
	char m_cAccountPassword[12];

	bool  m_bIsInitComplete;
	bool  m_bIsMsgSendAvailable;
	bool  m_bIsCheckingWhisperPlayer;

	char  m_cMapName[12];

	char  m_cGuildName[22];
	CGuild * m_guild;
	char  m_cLocation[12];      
	int   m_iGuildRank;
	time_t m_gldSummonsTime;

	DWORD m_dwDeathCount;

	short m_sAppr1;
	short m_sAppr2;
	short m_sAppr3;
	short m_sAppr4;
	int   m_iApprColor1, m_iApprColor2;

	DWORD m_dwTime, m_dwHPTime, m_dwMPTime, m_dwSPTime, m_dwAutoSaveTime, m_dwHungerTime, m_dwLastPingTime, m_lastMoveTime;

	bool m_hasPrecasted;
	uint32 m_timeHack;
	int32 m_timeHackDif;
	uint32 m_timeHackTurn; 
	uint32 m_moveTime[SPEEDCHECKTURNS];
	uint32 m_moveTurn;
	uint32 m_runTime[SPEEDCHECKTURNS];
	uint32 m_runTurn; 

	Sex m_cSex;
	char m_cSkin, m_cHairStyle, m_cHairColor, m_cUnderwear;

	int  m_iHPstock;
	int  m_iHPStatic_stock;  
	bool m_bIsBeingResurrected;
	int  m_iSP;
	int  m_iNextLevelExp;

	int  m_iDefenseRatio;
	int  m_iPhysicalHitRatio, m_iMagicalHitRatio;


	//int  m_iHitRatio_ItemEffect_SM; 
	//int  m_iHitRatio_ItemEffect_L;

	int  m_iDamageAbsorption_Armor[MAXITEMEQUIPPOS];
	int  m_iDamageAbsorption_Shield;

	int  m_iLevel;
	int m_iQuestLevel[250];
	int m_iQuestLevelID[250];
	int  m_iVit, m_iCharisma;
	int  m_iLuck; 
	int  m_iLU_Pool;

	long m_elo;
	int  m_iEnemyKillCount, m_iPKCount, m_iRewardGold;
	int m_iEnemyKillMaxCount;
	int  m_iCurWeightLoad;
	DWORD m_dwLogoutHackCheck;

	char m_cAttackDiceThrow_SM;
	char m_cAttackDiceRange_SM;
	char m_cAttackDiceThrow_L;
	char m_cAttackDiceRange_L;
	char m_cAttackBonus_SM;
	char m_cAttackBonus_L;

	class CItem * m_pItemList[MAXITEMS];
	POINTS m_ItemPosList[MAXITEMS];
	CItem * m_pItemInBankList[MAXBANKITEMS];
	CItem * m_guildBankItemList[MAXGUILDBANKITEMS];

	bool  m_bIsItemEquipped[MAXITEMS];
	short m_sItemEquipmentStatus[MAXITEMEQUIPPOS];
	char  m_cArrowIndex;
	char m_cMagicMastery[MAXMAGICTYPE];
	short m_sMagicMasteryCharge[MAXMAGICTYPE];
	unsigned char  m_cSkillMastery[MAXSKILLTYPE]; 

	int   m_iSkillSSN[MAXSKILLTYPE];
	bool  m_bSkillUsingStatus[MAXSKILLTYPE];
	int   m_iSkillUsingTimeID[MAXSKILLTYPE]; //v1.12

	int   m_iWhisperPlayerIndex;
	char  m_cWhisperPlayerName[12];
	char  m_cProfile[256];

	DWORD m_noAttackTime;
	int   m_iHungerStatus;
	DWORD m_dwWarBeginTime;
	bool  m_bIsWarLocation;
	bool  m_bIsPoisoned;
	int   m_iPoisonLevel;       
	DWORD m_dwPoisonTime;

	int   m_iPenaltyBlockYear, m_iPenaltyBlockMonth, m_iPenaltyBlockDay; 


	int   m_iFightzoneNumber , m_iReserveTime, m_iFightZoneTicketNumber ; 

	class XSocket * m_pXSock;

	int   m_iAdminUserLevel;
	//uint8 m_GMFlags;
	int   m_reputation;

	int   m_iTimeLeft_ShutUp;	 
	int   m_iTimeLeft_Rating;	 
	int   m_iTimeLeft_ForceRecall;  
	int   m_iTimeLeft_FirmStamina; 
	bool  m_bIsOnServerChange;     
	int   m_iExpStock;			 
	DWORD m_dwExpStockTime;	
	DWORD m_lastDamageTime;

	int   m_iAutoExpAmount;		 
	DWORD m_dwAutoExpTime;		 

	DWORD m_dwRecentAttackTime;  
	DWORD m_dwLastActionTime;	 

	int   m_iAllocatedFish;		 
	int   m_iFishChance;		 
	char  m_cIPaddress[21];		 
	bool  m_bIsSafeAttackMode;
	int m_iLastPing;
	int m_iSlayerDamage;
	int m_iSlayerAbsorb;
	bool  m_bIsOnWaitingProcess; 
	int   m_iSuperAttackLeft;	 
	int   m_iSuperAttackCount;   

	short m_sUsingWeaponSkill;	 

	int   m_iMPSaveRatio;		 

	bool  m_bIsLuckyEffect;		 
	int   m_iSideEffect_MaxHPdown; 

	int   m_iComboAttackCount;   
	int   m_iDownSkillIndex;	 

	int   m_iMagicDamageSaveItemIndex; 

	short m_sCharIDnum1, m_sCharIDnum2, m_sCharIDnum3; 
	

	CParty * GetParty()	const { return _party; }
	void SetParty(CParty * party)
	{
		_party = party;
		_partyStatus = (_party) ? PS_INPARTY : PS_NOTINPARTY;
	}
	PartyStatus GetPartyStatus()				const { return _partyStatus; }
	void SetPartyStatus(PartyStatus status)		{ _partyStatus = status; }
	bool HasPartyRaidBonus() const;
	bool HasPartyHuntBonus() const;
	int m_iReqJoinPartyClientH;
	char m_cReqJoinPartyName[12];
	int m_partyCoordSteps;
	int m_guildCoordSteps;


	int   m_iAbuseCount;
	bool  m_bIsBWMonitor;

	bool  m_bIsManager;		    

	bool  m_isExchangeMode;
	bool  m_isExchangeConfirm; 
	char  m_exchangeName[11];
	int   m_exchangeH;
	char  m_exchangeCount;

	struct {
		char  itemIndex; 
		char  itemName[21];
		int   itemAmount;		
	} m_exchangeItems[4];
	
	int   m_iQuest;				 
	int   m_iQuestID;			 
	int   m_iAskedQuest;		 
	int   m_iCurQuestCount;		 
	int   m_iQuestRewardType;	 
	int   m_iQuestRewardAmount;	 

	int	  m_iContPts;
	int   m_iContPtsStock;
	int   m_iContribution;		 
	bool  m_bQuestMatchFlag_Loc; 
	bool  m_bIsQuestCompleted;   

	int   m_iCustomItemValue_Attack;
	int   m_iCustomItemValue_Defense;

	int   m_iMinAP_SM;
	int   m_iMinAP_L;

	int   m_iMaxAP_SM;
	int   m_iMaxAP_L;

	bool  m_bIsNeutral;
	bool  m_bIsObserverMode;

	int   m_iSpecialEventID;

	int   m_iSpecialWeaponEffectType;
	int   m_iSpecialWeaponEffectValue;

	int	m_nextRecallPoint;

	int   m_iAddHP, m_iAddSP, m_iAddMP; 
	int   m_iAddGlobalHitRatio, m_iAddPR, m_iAddDR;
	int   m_iAddMR, m_iAddAbsPD, m_iAddAbsMD; 
	int   m_iAddCD, m_iAddExp, m_iAddCont, m_iAddGold;

	int   m_iAddTransMana, m_iAddChargeCritical;

	int   m_iAddResistMagic;
	int   m_iAddPhysicalDamage;
	int   m_iAddMagicalDamage;
	uint32 m_addMagicDmgPct;

	int m_iAddPsnDmg, m_iAddCritDmg, m_iAddLifeLeech;

	int   m_iAddAbsAir;
	int   m_iAddAbsEarth;
	int   m_iAddAbsFire;
	int   m_iAddAbsWater;

	int   m_iLastDamage;

	int   m_iMoveMsgRecvCount, m_iAttackMsgRecvCount, m_iRunMsgRecvCount, m_iSkillMsgRecvCount;
	DWORD m_dwMoveLAT, m_dwRunLAT, m_dwAttackLAT;

	int   m_iSpecialAbilityTime;
	bool  m_bIsSpecialAbilityEnabled;
	uint32 m_specialAbilityStartTime;
	uint32 m_specialAbilityLastSec;
	short m_specialAbilityItemIndex;
	int   m_iSpecialAbilityType;

	int   m_iSpecialAbilityEquipPos;
	int   m_iAlterItemDropIndex;

	int   m_iWarContribution;
	DWORD m_dwInitCCTimeRcv;
	DWORD m_dwInitCCTime;

	char  m_cLockedMapName[12];
	int   m_iLockedMapTime;
	int   m_iDeadPenaltyTime;

	int   m_iCrusadeDuty;
	DWORD m_dwCrusadeGUID;

	struct {
		char cType;
		char cSide;
		short sX, sY;
	} m_stCrusadeStructureInfo[MAXCRUSADESTRUCTURES];
	int m_iCSIsendPoint;

	char m_cSendingMapName[12];
	bool m_bIsSendingMapStatus;


	int  m_iConstructionPoint;

	char m_cConstructMapName[12];
	int  m_iConstructLocX, m_iConstructLocY;

	DWORD m_dwFightzoneDeadTime;


	bool m_bIsBankModified ;

	char m_rejectedMoves;
	DWORD m_dwCharID ;
	
	int m_iGizonItemUpgradeLeft;

	DWORD m_dwAttackFreqTime, m_dwMagicFreqTime, m_dwMoveFreqTime; 
	bool m_bIsMoveBlocked; 
	bool m_bIsAttackModeChange; 
	int  m_iIsOnTown; 
	bool m_bIsOnShop; 
	bool m_bIsOnTower; 
	bool m_bIsOnWarehouse;
	bool m_bIsOnGuildHall; 
	bool m_bIsOnCath;
	bool m_bIsInBuilding; 

	DWORD m_dwWarmEffectTime; 

	char m_heroArmourBonus;
	bool m_notifedOfApoc;
		
private:
	int _str, _int, _dex, _mag;
	int _angelStr, _angelInt, _angelDex, _angelMag;

	CParty * _party;
	PartyStatus _partyStatus;
};

typedef List<CClient*> ClientList;
typedef List<CClient*>::iterator ClientListIter;
typedef List<CClient*>::const_iterator ClientListCIter;

#endif // !defined(AFX_CLIENT_H__39CC7700_789F_11D2_A8E6_00001C7030A6__INCLUDED_)
