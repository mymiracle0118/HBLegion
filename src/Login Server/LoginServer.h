#ifndef _LOGINSERVER_H_
#define _LOGINSERVER_H_
#define _CRT_SECURE_NO_DEPRECATE 1


#include "net\XSocket.h"
#include "..\shared\common.h"
#include "main.h"
#include "GameServer.h"
#include "mysql\mysql.h"
#include "defs.hpp"
#include "char\Client.h"
#include "net\MsgCodes.h"
#include "net\Msg.h"
#include "StrTok.h"
#include "res\Resource.h"
#include "char\item\Item.h"
//using namespace std;
//=============================================================================
class CLoginServer
{
public:
	CLoginServer();
	~CLoginServer();
	bool IsAddressValid(char * address, MYSQL myConn, bool isIP);
	void CreateNewAccount(char *Data, WORD ClientID, MYSQL myConn);
	BOOL AccountExists(char *AccountName, MYSQL myConn);
	DWORD filesize(FILE *stream);
	BOOL ReadProgramConfigFile(char * cFn);
	UINT MyAux_Get_Error(struct st_mysql *mySql);
	void MysqlAutoFix();
	BOOL CheckServerStatus();
	BOOL RestartServer();
	void OnClientSocketEvent(UINT RcvMsg, WPARAM wParam, LPARAM lParam);
	void OnGameServerSocketEvent(UINT RcvMsg, WPARAM wParam, LPARAM lParam);
	void OnClientRead(WORD ClientID);
	void OnGameServerRead(WORD GSID);
	void CloseGameServerSocket(WORD ID, BOOL log = TRUE);
	void CloseClientSocket(WORD ID, BOOL log = FALSE);
	BYTE CheckAccountLogin(char *AccName, char *AccPwd, MYSQL myConn);
	BOOL ProcessClientLogin(char *AccName, char *AccPwd, WORD ClientID, MYSQL myConn);
	BOOL IsDatePast(char *Date);
	void ChangeDateIntoInt(char *date, WORD *year, BYTE *month, BYTE *day, BYTE *hour = NULL, BYTE *minute = NULL, BYTE *second = NULL);
	BOOL IsAddrPermitted(char *addr);
	void RegisterGameServer(char *Data, BYTE ID);
	void RegisterGameServerSocket(char *Data, BYTE ID);
	BOOL ReadConfig(char *FileName);
	BOOL bReadAllConfig();
	int iComposeAuctionItemListData(char cType, char *pData, MYSQL myConn, WORD AccountID = 0, WORD CharID = 0, char cPage = 0);
	void SendUpdatedConfigToAllServers();
	void SendConfigToGS(BYTE ID);
	void SendCharList(char* AccountName, WORD ClientID, MYSQL myConn);
	void CreateNewCharacter(char *Data, WORD ClientID, MYSQL myConn);
	void SendMsgToClient(WORD ClientID, char * cData, DWORD dwSize, char cKey = NULL, BOOL log = FALSE);
	void SendMsgToGS(WORD GSID, char * cData, DWORD dwSize, char cKey = NULL, BOOL log = FALSE);
	void SendMsgToAllGameServers(int iClientH, char *pData, DWORD dwMsgSize, BOOL bIsOwnSend);
	void GetCharList(char* AccountName, char *CharList, DWORD * Size, MYSQL myConn);
	void DeleteCharacter(char *Data, WORD ClientID, MYSQL myConn);
	void ChangePassword(char *Data, WORD ClientID, MYSQL myConn);
	void DeleteAllItemsFromChar(WORD AccountID, WORD CharID, MYSQL myConn, BOOL bDeleteFromBank);
	DWORD GetCharID(char *CharName, char *AccountName, MYSQL myConn);
	void OptimizeTable(char *TableName);
	void OptimizeDatabase(DWORD time);
	void RepairTable(char *TableName);
	void RepairDatabase(DWORD time);
	void OnTimer();
	void ProcessClientRequestEnterGame(char *Data, DWORD ClientID, MYSQL myConn);
	BOOL IsMapAvailable(char *MapName, char *GameServerIP, WORD *GameServerPort, BYTE *GSID);
	void ProcessRequestPlayerData(char *Data, BYTE GSID, MYSQL myConn);
	BOOL IsAccountInUse(char *AccountName, WORD *AccountID = NULL);
	WORD GetCharacterInfo(char *AccountName, char *CharName, char *Data, MYSQL myConn);
	void CheckActiveAccountsNumber(DWORD time);
	void OnKeyDown(WPARAM wParam);
	void OnKeyUp(WPARAM wParam);
	void ProcessClientLogout(char *Data, BOOL save, BYTE GSID, MYSQL myConn);
	void ConfirmCharEnterGame(char *Data, BYTE GSID);
	BOOL IsGMAccount(char *AccountName, MYSQL myConn);
	void SaveCharacter(char* Data, MYSQL myConn);
	BOOL NameChange(char *AccountName, char *NewName, MYSQL myConn);
	BYTE CheckDupItem(cItem *Item, MYSQL myConn, BOOL bBank);
	void DeleteItemOnDB(cItem *Item, DWORD CharID, MYSQL myConn);
	void CreateNewItem(cItem *Item, WORD AccountID, DWORD CharID, MYSQL myConn);
	void OnUserAccept(HWND hWnd);
	void OnGateServerAccept(HWND hWnd);
	BOOL DoInitialSetup();
	BOOL InitServer();
	void SetAccountServerChangeStatus(char *Data, BOOL IsOnServerChange);
	BOOL GetMsgQuene(char * pData, DWORD * pMsgSize, int * pIndex, char * pKey);
	BOOL PutMsgQuene(char * pData, DWORD dwMsgSize, int iIndex, char cKey);
	void MsgProcess();
	void RequestForceDisconnect(CClient *pClient, WORD count);
	uint64 GetLastInsertedItemID(MYSQL myConn);
	uint64 GetAccountID(char *AccountName, MYSQL myConn);
	void RequestCreateNewGuildHandler(char *Data, BYTE GSID, MYSQL myConn);
	BOOL GuildExists(char *GuildName, DWORD *GuildID, MYSQL myConn);
	void RequestDisbandGuildHandler(char *Data, BYTE GSID, MYSQL myConn);
	BOOL IsGuildMaster(char *CharName, char *GuildName, MYSQL myConn);
	void AddGuildMember(char *Data, MYSQL myConn);
	void RemoveGuildMember(char *Data, MYSQL myConn);
	void Process_SaveGuildInfo(char *data, MYSQL myConn);
	void ProcessRequest_GuildInfo(char *data, BYTE GSID, MYSQL myConn);
	void ProcessShutdown(DWORD dwTime);
	void ServerStockMsgHandler(char *pData, BYTE ID);
	void ProcessGSMsgWithDBAccess(char *pData, BYTE GSID, MYSQL myConn);
	int  ProcessQuery(MYSQL *myConn, char *cQuery);
	void SendLoginConfirmation(char *IP, char * accName, char * charName, char * accPass, BYTE GSID);

	void RequestLegionPts(char * data, WORD GSID, MYSQL myConn);
	void RequestLegionSvc(char * data, WORD GSID, MYSQL myConn);
	void RequestLegionSvcNameChange(char * data, WORD GSID, MYSQL myConn);
	long GetAccountCash(MYSQL myConn, char * account);
	void SubAccountCash(char * data, MYSQL myConn);
	void AddAccountCash(char * data, MYSQL myConn);
	void AddAuctionItem(char *data, WORD GSID, MYSQL myConn);
	void ReqAuctionItemList(char *data, WORD GSID, MYSQL myConn);
	void ReqAuctionAction(char *data, WORD GSID, MYSQL myConn);

	bool ValidateAddress(char * address, MYSQL myConn, bool isIP);

	class CGameServer		*GameServer[MAXGAMESERVERS];
	class CClient			*Client[MAXCLIENTS];
	class CMsg				*MsgQuene[MSGQUENESIZE];

	int             QueneHead, QueneTail;
	char            mySqlUser[21], mySqlPwd[21];
	_ADDRESS         ListenAddress, mySqlAddress, PermittedAddress[MAXGAMESERVERS];
	WORD            ListenPort, GateServerPort, mySqlPort;
	BOOL            ListenToAllAddresses, mySQLAutoFixProcess, IsThreadMysqlBeingUsed;
	char			*ItemCfg, *Item2Cfg, *Item3Cfg, *BuildItemCfg, *DupItemIDCfg, *MagicCfg, 
		*NoticementTxt, *NPCCfg, *PotionCfg, *QuestCfg, *SkillCfg, *CraftingCfg, *TeleportCfg;
	BOOL			bIsF1pressed, bIsF4pressed, bIsF5pressed;
	BOOL			bServersBeingShutdown;
	DWORD			dwShutdownInterval;
	BYTE			bShutDownMsgIndex;
	BOOL			bConfigsUpdated;

	//timeGetTime for intervals on the OnTimer function
	DWORD           KeyDownTimer, mySQLTimer, mySQLdbRepairTimer, mySQLdbOptimizeTimer, CheckAccountsTimer;

private:
	char _sendBuff[MSGBUFFERSIZE];
};
//=============================================================================
extern MMRESULT  Timer;
extern HWND      hWnd, LogWindow;
extern XSocket   *ClientSocket[MAXCLIENTS], *GameServerSocket[MAXGAMESERVERSOCKETS];
extern WORD      ActiveAccounts, PeakPeopleOnline;
extern DWORD     TotalAccounts;
extern BOOL		 IsOnCloseProcess;
extern MYSQL	 mySQL;

extern HINSTANCE				hInst;
extern int CALLBACK LoginDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
extern BYTE MySqlAutoFixNum;
//=============================================================================
#endif