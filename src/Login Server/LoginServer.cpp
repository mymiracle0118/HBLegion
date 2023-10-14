#include "LoginServer.h"
#include "..\shared\buffer.h"

//=============================================================================
CLoginServer::CLoginServer()
{
 WORD w;
 DWORD dw;

		MainSocket        = NULL;
        GateServerSocket  = NULL;
		for(w=0; w<MAXGAMESERVERSOCKETS; w++) GameServerSocket[w] = NULL;
        for(w=0; w<MAXCLIENTS; w++) ClientSocket[w] = NULL;
        for(w=0; w<MAXGAMESERVERS; w++){
			ZeroMemory(PermittedAddress[w], sizeof(PermittedAddress[w]));
			GameServer[w] = NULL;
		}
        for(w = 0; w < MAXCLIENTS; w++) Client[w] = NULL;
		for(dw = 0; dw < MSGQUENESIZE; dw++) MsgQuene[dw] = NULL;
        ListenToAllAddresses = TRUE;
        mySQLAutoFixProcess = FALSE;
		IsThreadMysqlBeingUsed = FALSE;
		IsOnCloseProcess = FALSE;
		QueneHead = 0;
		QueneTail = 0;
		bIsF1pressed = bIsF4pressed = bIsF5pressed = FALSE;
		bServersBeingShutdown = FALSE;
		bShutDownMsgIndex = 0;
		dwShutdownInterval = 0;
		bConfigsUpdated = FALSE;
		ItemCfg = Item2Cfg = Item3Cfg = BuildItemCfg = DupItemIDCfg = MagicCfg = NULL;
		NoticementTxt = NPCCfg = PotionCfg = QuestCfg = SkillCfg = CraftingCfg = TeleportCfg = NULL;
}
//=============================================================================
CLoginServer::~CLoginServer()
{
 WORD w;
 DWORD dw;
	
		if(Timer != NULL) _StopTimer(Timer);
		mysql_close(&mySQL);
        for(w = 0; w < MAXCLIENTS; w++) SAFEDELETE(ClientSocket[w]);
        for(w = 0; w < MAXGAMESERVERS; w++) SAFEDELETE(GameServer[w]);
        for(w = 0; w < MAXGAMESERVERSOCKETS; w++) SAFEDELETE(GameServerSocket[w]);
        for(w = 0; w < MAXCLIENTS; w++) SAFEDELETE(Client[w]);
		for(dw = 0; dw < MSGQUENESIZE; dw++) SAFEDELETE(MsgQuene[dw]);
		SAFEDELETE(MainSocket);
        SAFEDELETE(GateServerSocket);
        _TermWinsock();
		SAFEDELETE(ItemCfg);
		SAFEDELETE(Item2Cfg);
		SAFEDELETE(Item3Cfg);
		SAFEDELETE(BuildItemCfg);
		SAFEDELETE(DupItemIDCfg);
		SAFEDELETE(MagicCfg);
		SAFEDELETE(NoticementTxt);
		SAFEDELETE(NPCCfg);
		SAFEDELETE(PotionCfg);
		SAFEDELETE(QuestCfg);
		SAFEDELETE(SkillCfg);
		SAFEDELETE(CraftingCfg);
		SAFEDELETE(TeleportCfg);
}
//=============================================================================
BOOL CLoginServer::DoInitialSetup()
{
 		if(ReadProgramConfigFile("LServer.cfg") == FALSE) return FALSE;
        PutLogList("(!) Connecting to mySql database...");
        ZeroMemory(mySqlUser, sizeof(mySqlUser));
        ZeroMemory(mySqlPwd, sizeof(mySqlPwd));
		DialogBox (hInst, MAKEINTRESOURCE(IDD_MYSQL_LOGIN), hWnd, LoginDlgProc );
		return true;
}
//=============================================================================
BOOL CLoginServer::InitServer()
{
 DWORD Time;
 	
		if (_InitWinsock() == FALSE){
			MessageBox(hWnd, "Socket 1.1 not found! Cannot execute program.","ERROR", MB_ICONEXCLAMATION | MB_OK);
			PostQuitMessage(0);
			return FALSE;
		}
        if(!bReadAllConfig()) return FALSE;
        PutLogList("(!) Done!");

        MainSocket = new XSocket(hWnd, XSOCKBLOCKLIMIT);
        MainSocket->bListen(ListenAddress, ListenPort, WM_USER_ACCEPT);

        GateServerSocket = new XSocket(hWnd, XSOCKBLOCKLIMIT);
        GateServerSocket->bListen(ListenAddress, GateServerPort, WM_GATESERVER_ACCEPT);

        if(ListenToAllAddresses == TRUE) PutLogList("(!) permitted-address line not found on config., server will be listening to all IPs!", WARN_MSG);
        PutLogList("-Login server sucessfully started!");
        Time = timeGetTime();
#ifndef _DEBUG
        OptimizeDatabase(Time);
#endif
        RepairDatabase(Time);
        CheckActiveAccountsNumber(Time);
        Timer = _StartTimer(MAINTIMERSIZE);
		return TRUE;
}
//=============================================================================
BOOL CLoginServer::bReadAllConfig()
{
		SAFEDELETE(ItemCfg);
		if(ReadConfig("Item.cfg")			== FALSE) return FALSE;
		SAFEDELETE(Item2Cfg);
		if(ReadConfig("Item2.cfg")			== FALSE) return FALSE;
		SAFEDELETE(Item3Cfg);
		if(ReadConfig("Item3.cfg")			== FALSE) return FALSE;
		SAFEDELETE(BuildItemCfg);
		if(ReadConfig("BuildItem.cfg")		== FALSE) return FALSE;
		SAFEDELETE(DupItemIDCfg);
		if(ReadConfig("DupItemID.cfg")		== FALSE) return FALSE;
		SAFEDELETE(MagicCfg);
		if(ReadConfig("Magic.cfg")			== FALSE) return FALSE;
		SAFEDELETE(NoticementTxt);
		if(ReadConfig("noticement.txt")		== FALSE) return FALSE;
		SAFEDELETE(NPCCfg);
		if(ReadConfig("NPC.cfg")			== FALSE) return FALSE;
		SAFEDELETE(PotionCfg);
		if(ReadConfig("potion.cfg")			== FALSE) return FALSE;
		SAFEDELETE(QuestCfg);
		if(ReadConfig("Quest.cfg")			== FALSE) return FALSE;
		SAFEDELETE(SkillCfg);
		if(ReadConfig("Skill.cfg")			== FALSE) return FALSE;
		SAFEDELETE(CraftingCfg);
		if(ReadConfig("CraftItem.cfg")			== FALSE) return FALSE;
		SAFEDELETE(TeleportCfg);
		if(ReadConfig("Teleport.cfg")			== FALSE) return FALSE;
		return TRUE;
}
//=============================================================================
void CLoginServer::SendUpdatedConfigToAllServers()
{
 char SendBuff[5];
 WORD w;
 DWORD *dwp;
 	
		PutLogList("(*) Sending updated configuration to all servers...", INFO_MSG);
		ZeroMemory(SendBuff, sizeof(SendBuff));
		dwp = (DWORD*)SendBuff;
		*dwp = MSGID_UPDATECONFIGFILES;
		for(w = 0; w < MAXGAMESERVERS; w++)
			if(GameServer[w] != NULL){
				GameServer[w]->SendMsg(SendBuff, 4, NULL, FALSE);				
				SendConfigToGS((BYTE)GameServer[w]->SocketIndex[0]);
			}
		PutLogList("(*) All servers are updated!", INFO_MSG);
}
//=============================================================================
void CLoginServer::SendConfigToGS(BYTE ID)
{
 CONFIG SendCfgData;
 DWORD *dwp;

		ZeroMemory(SendCfgData, sizeof(SendCfgData));
        dwp  = (DWORD*)SendCfgData;
		*dwp = MSGID_ITEMCONFIGURATIONCONTENTS;
        SafeCopy(SendCfgData+6, ItemCfg);
        SendMsgToGS(ID, SendCfgData, strlen(ItemCfg)+7);

        ZeroMemory(SendCfgData, sizeof(SendCfgData));
        dwp  = (DWORD*)SendCfgData;
        *dwp = MSGID_ITEMCONFIGURATIONCONTENTS;
        SafeCopy(SendCfgData+6, Item2Cfg);
        SendMsgToGS(ID, SendCfgData, strlen(Item2Cfg)+7);

        ZeroMemory(SendCfgData, sizeof(SendCfgData));
        dwp  = (DWORD*)SendCfgData;
        *dwp = MSGID_ITEMCONFIGURATIONCONTENTS;
        SafeCopy(SendCfgData+6, Item3Cfg);
        SendMsgToGS(ID, SendCfgData, strlen(Item3Cfg)+7);

        ZeroMemory(SendCfgData, sizeof(SendCfgData));
        dwp  = (DWORD*)SendCfgData;
        *dwp = MSGID_BUILDITEMCONFIGURATIONCONTENTS;
        SafeCopy(SendCfgData+6, BuildItemCfg);
        SendMsgToGS(ID, SendCfgData, strlen(BuildItemCfg)+7);

        ZeroMemory(SendCfgData, sizeof(SendCfgData));
        dwp  = (DWORD*)SendCfgData;
        *dwp = MSGID_DUPITEMIDFILECONTENTS;
        SafeCopy(SendCfgData+6, DupItemIDCfg);
        SendMsgToGS(ID, SendCfgData, strlen(DupItemIDCfg)+7);

        ZeroMemory(SendCfgData, sizeof(SendCfgData));
        dwp  = (DWORD*)SendCfgData;
        *dwp = MSGID_MAGICCONFIGURATIONCONTENTS;
        SafeCopy(SendCfgData+6, MagicCfg);
        SendMsgToGS(ID, SendCfgData, strlen(MagicCfg)+7);

        ZeroMemory(SendCfgData, sizeof(SendCfgData));
        dwp  = (DWORD*)SendCfgData;
        *dwp = MSGID_NOTICEMENTFILECONTENTS;
        SafeCopy(SendCfgData+6, NoticementTxt);
        SendMsgToGS(ID, SendCfgData, strlen(NoticementTxt)+7);

        ZeroMemory(SendCfgData, sizeof(SendCfgData));
        dwp  = (DWORD*)SendCfgData;
        *dwp = MSGID_NPCCONFIGURATIONCONTENTS;
        SafeCopy(SendCfgData+6, NPCCfg);
        SendMsgToGS(ID, SendCfgData, strlen(NPCCfg)+7);

        ZeroMemory(SendCfgData, sizeof(SendCfgData));
        dwp  = (DWORD*)SendCfgData;
        *dwp = MSGID_PORTIONCONFIGURATIONCONTENTS;
        SafeCopy(SendCfgData+6, PotionCfg);
        SendMsgToGS(ID, SendCfgData, strlen(PotionCfg)+7);

        ZeroMemory(SendCfgData, sizeof(SendCfgData));
        dwp  = (DWORD*)SendCfgData;
        *dwp = MSGID_QUESTCONFIGURATIONCONTENTS;
        SafeCopy(SendCfgData+6, QuestCfg);
        SendMsgToGS(ID, SendCfgData, strlen(QuestCfg)+7);

        ZeroMemory(SendCfgData, sizeof(SendCfgData));
        dwp  = (DWORD*)SendCfgData;
        *dwp = MSGID_SKILLCONFIGURATIONCONTENTS;
        SafeCopy(SendCfgData+6, SkillCfg);
        SendMsgToGS(ID, SendCfgData, strlen(SkillCfg)+7);

		  *dwp = MSGID_CRAFTINGCONFIGURATIONCONTENTS;
		  SafeCopy(SendCfgData+6, CraftingCfg);
		  SendMsgToGS(ID, SendCfgData, strlen(CraftingCfg)+7);

		  *dwp = MSGID_TELEPORTLISTCONTENTS;
		  SafeCopy(SendCfgData+6, TeleportCfg);
		  SendMsgToGS(ID, SendCfgData, strlen(TeleportCfg)+7);

}
//=============================================================================
DWORD CLoginServer::filesize(FILE *stream)
{
 long curpos, length;

        curpos = ftell(stream);
        fseek(stream, 0L, SEEK_END);
        length = ftell(stream);
        fseek(stream, curpos, SEEK_SET);
        return length;
}
//=============================================================================
BOOL CLoginServer::ReadProgramConfigFile(char * cFn)
{
 FILE *pFile;
 DWORD  dwFileSize;
 CStrTok *pStrTok;
 char *token, *cp, LogBuff[100];
 char seps[] = "= \t\n";

	pFile = fopen(cFn, "rt");
    if (pFile == NULL) {
		PutLogList("(!) Cannot open configuration file.", WARN_MSG);
		return FALSE;
	}
	else {
		dwFileSize = filesize(pFile);
		PutLogList("(!) Reading configuration file...");
		cp = new char[dwFileSize+2];
		ZeroMemory(cp, dwFileSize+2);
		fread(cp, dwFileSize, 1, pFile);

		pStrTok = new CStrTok(cp, seps);
		token = pStrTok->pGet();
		while( token != NULL )
                     {
					  ZeroMemory(LogBuff, sizeof(LogBuff));
                      if (IsSame(token, "login-server-address"))
                         {
                          token = pStrTok->pGet();
                          SafeCopy(ListenAddress, token);
                          sprintf(LogBuff, "(*) Login server address : %s", ListenAddress);
						  PutLogList(LogBuff);
                         }
					  else if (IsSame(token, "login-server-port"))
                         {
                          token = pStrTok->pGet();
                          ListenPort = (WORD)atoi(token);
						  sprintf(LogBuff, "(*) Login server port : %u", ListenPort);
						  PutLogList(LogBuff);
                         }
					  else if (IsSame(token, "gate-server-port"))
                         {
                          token = pStrTok->pGet();
                          GateServerPort = (WORD)atoi(token);
						  sprintf(LogBuff, "(*) Gate Server port : %u", GateServerPort);
						  PutLogList(LogBuff);
                         }
                      else if (IsSame(token, "mysql-address"))
                         {
                          token = pStrTok->pGet();
                          SafeCopy(mySqlAddress, token);
                          sprintf(LogBuff, "(*) mySql server address : %s", mySqlAddress);
						  PutLogList(LogBuff);
                         }
                      else if (IsSame(token, "mysql-server-port"))
                         {
                          token = pStrTok->pGet();
                          mySqlPort = (WORD)atoi(token);
						  sprintf(LogBuff, "(*) mySql Server port : %u", mySqlPort);
						  PutLogList(LogBuff);
                         }
                      else if (IsSame(token, "permitted-address"))
                         {
                          token = pStrTok->pGet();
						  for(BYTE b = 0; b < MAXGAMESERVERS; b++)
                            if(strlen(PermittedAddress[b]) == NULL)
                              {
                               SafeCopy(PermittedAddress[b], token);
                               if(ListenToAllAddresses == TRUE) ListenToAllAddresses = FALSE;
							   ZeroMemory(LogBuff, sizeof(LogBuff));
							   sprintf(LogBuff, "(*) IP [%s] added to permitted addresses list!", PermittedAddress[b]);
							   PutLogList(LogBuff);
                               break;
                              }
                         }
                      token = pStrTok->pGet();
		     }
            SAFEDELETE(pStrTok);
			SAFEDELETE(cp);
	     }
		if (pFile != NULL) fclose(pFile);
        if(strlen(ListenAddress) == 0 || ListenPort <= 0 || mySqlPort <= 0 || GateServerPort <= 0 || strlen(mySqlAddress) == 0)
          {
           PutLogList("(!!!) Info is missing in config file, unable to start server", WARN_MSG);
           return FALSE;
          }
        return TRUE;
}
//=============================================================================
UINT CLoginServer::MyAux_Get_Error(struct st_mysql *pmySql)
{
char ErrMsg[300];
UINT ErrNum;

        ErrNum = mysql_errno(pmySql);
        if(ErrNum != NULL)
          {
           ZeroMemory(ErrMsg, sizeof(ErrMsg));
		   sprintf(ErrMsg, "(!!!) MySql ERROR: [%lu] - %s", ErrNum, mysql_error(pmySql));
           PutLogList(ErrMsg, WARN_MSG, TRUE, MYSQL_ERROR_LOGFILE);
           return ErrNum;
          }
        else return NULL;
}
//=============================================================================
void CLoginServer::MysqlAutoFix()
{
 char Txt100[100];

        mySQLTimer = timeGetTime();
        ZeroMemory(Txt100, sizeof(Txt100));
        if(MySqlAutoFixNum < MAX_MYSQL_RESTART_ATTEMPT)
          {
           if(RestartServer() == FALSE)
             {
              sprintf(Txt100, "(!!!) Server restart attempt %u of %u has failed...", MySqlAutoFixNum, MAX_MYSQL_RESTART_ATTEMPT);
              MySqlAutoFixNum++;
              PutLogList(Txt100, WARN_MSG);
             }
           else
             {
              _StopTimer(Timer);
              Timer = NULL;
              MySqlAutoFixNum = 1;
              SafeCopy(Txt100, "-Server was sucessfully restarted!");
              PutLogList(Txt100);
              mySQLAutoFixProcess = FALSE;
              Timer = _StartTimer(MAINTIMERSIZE);
             }
          }
        else
          {
           _StopTimer(Timer);
           Timer = NULL;
           SafeCopy(Txt100, "(!!!) CRITICAL ERROR! Impossible to restart MySql database.");
           PutLogList(Txt100, WARN_MSG);
          }
		IsThreadMysqlBeingUsed = FALSE;
}
//=============================================================================
BOOL CLoginServer::CheckServerStatus()
{
 
		if(mysql_ping(&mySQL) != 0){
			int i;
			WORD w;
			MyAux_Get_Error(&mySQL);
			mysql_close(&mySQL);
			mySQLAutoFixProcess = TRUE;
			PutLogList("(!!!) MySql is inacessible, trying to restart the server...", WARN_MSG);
			for(i = 0; i < MAXCLIENTS; i++) SAFEDELETE(ClientSocket[i]);
			for(i = 0; i < MAXGAMESERVERS; i++) SAFEDELETE(GameServer[i]);
			for(w = 0; w < MAXGAMESERVERSOCKETS; w++) SAFEDELETE(GameServerSocket[w]);
			for(w = 0; w < MAXCLIENTS; w++) SAFEDELETE(Client[w]);
			for(DWORD dw = 0; dw < MSGQUENESIZE; dw++) SAFEDELETE(MsgQuene[dw]);
			SAFEDELETE(MainSocket);
			SAFEDELETE(GateServerSocket);
			_TermWinsock();
			MysqlAutoFix();
			return FALSE;
		}
		return TRUE;
}
//=============================================================================
BOOL CLoginServer::RestartServer()
{
 		mysql_close(&mySQL);
		mysql_init(&mySQL);
		mysql_real_connect(&mySQL, mySqlAddress, mySqlUser, mySqlPwd, "playerdb2", mySqlPort, NULL, NULL);
				
		if(MyAux_Get_Error(&mySQL) == 0) PutLogList("-Connection to mySQL database was sucessfully established!");
        else{
           PutLogList("(!!!) Failed to connect to mySQL, please check if the mySQL server is online.", WARN_MSG);
           mysql_close(&mySQL);
		   return FALSE;
        }
		if (_InitWinsock() == FALSE){
			MessageBox(hWnd, "Socket 1.1 not found! Cannot execute program.","ERROR", MB_ICONEXCLAMATION | MB_OK);
			mysql_close(&mySQL);
			PostQuitMessage(0);
			return FALSE;
		}
        MainSocket = new XSocket(hWnd, XSOCKBLOCKLIMIT);
        MainSocket->bListen(ListenAddress, ListenPort, WM_USER_ACCEPT);
        GateServerSocket = new XSocket(hWnd, XSOCKBLOCKLIMIT);
        GateServerSocket->bListen(ListenAddress, GateServerPort, WM_GATESERVER_ACCEPT);
        for(int i=0; i<MAXCLIENTS; i++) SAFEDELETE(ClientSocket[i]);
        for(int i=0; i<MAXGAMESERVERS; i++) SAFEDELETE(GameServer[i]);
		return TRUE;
}
//=============================================================================
void CLoginServer::OnClientSocketEvent(UINT RcvMsg, WPARAM wParam, LPARAM lParam)
{
	UINT iTmp;
	int Result;
	WORD user;

	iTmp = WM_ONCLIENTSOCKETEVENT;
	user = (WORD)(RcvMsg - iTmp);
	if(ClientSocket[user] == NULL) return;
	Result = ClientSocket[user]->iOnSocketEvent(wParam, lParam);
	switch (Result){
	case XSOCKEVENT_SOCKETMISMATCH:
	case XSOCKEVENT_NOTINITIALIZED:
	case XSOCKEVENT_SOCKETERROR:
	case XSOCKEVENT_BLOCK:
	case XSOCKEVENT_CONFIRMCODENOTMATCH:
	case XSOCKEVENT_MSGSIZETOOLARGE:
		PutLogList("(!) An error occured in a client socket!", WARN_MSG);
		CloseClientSocket(user);
		break;

	case XSOCKEVENT_SOCKETCLOSED:
		CloseClientSocket(user);
		break;
	case XSOCKEVENT_READCOMPLETE:
		OnClientRead(user);
		break;
	}
}
//=============================================================================
void CLoginServer::OnGameServerSocketEvent(UINT RcvMsg, WPARAM wParam, LPARAM lParam)
{
 UINT iTmp;
 int Result;
 WORD user;

        iTmp = WM_ONGAMESERVERSOCKETEVENT;
        user = (WORD)(RcvMsg - iTmp);
        if(GameServerSocket[user] == NULL) return;
        Result = GameServerSocket[user]->iOnSocketEvent(wParam, lParam);
        switch (Result){
          case XSOCKEVENT_SOCKETMISMATCH:
          case XSOCKEVENT_NOTINITIALIZED:
		  case XSOCKEVENT_SOCKETERROR:
          PutLogList("(!) An error occured in a gameserver socket!", WARN_MSG);
		  break;

          case XSOCKEVENT_RETRYINGCONNECTION:
          PutLogList("XSOCKEVENT_RETRYINGCONNECTION");
          break;

          case XSOCKEVENT_CONNECTIONESTABLISH:
          PutLogList("XSOCKEVENT_CONNECTIONESTABLISH");
          break;

          case XSOCKEVENT_SOCKETCLOSED:
          CloseGameServerSocket(user);
          break;

          case XSOCKEVENT_READCOMPLETE:
          OnGameServerRead(user);
          break;
        }
}
//=============================================================================
void CLoginServer::OnClientRead(WORD ClientID)
{
	char  * pData, cKey, *dataBuff;
	DWORD  dwMsgSize, msgID;
	_ADDRESS peerAddr;
	char Txt[120], accName[12], accPass[12], macAddress[24], charName[12];
	MYSQL myConn;
	bool err = false;
	const int headSize = 36;

	if (ClientSocket[ClientID] == NULL) return;

	pData = ClientSocket[ClientID]->pGetRcvDataPointer(&dwMsgSize, &cKey);
	if (dwMsgSize < headSize) {
		SAFEDELETE(ClientSocket[ClientID]);
		return;
	}

	dataBuff = new char[dwMsgSize+2];
	ZeroMemory(dataBuff, dwMsgSize+2);

	SafeCopy(dataBuff, pData, dwMsgSize);
    
	mysql_init(&myConn);
	if(!mysql_real_connect(&myConn, mySqlAddress, mySqlUser, mySqlPwd, "playerdb2", mySqlPort, NULL, NULL)){
		MyAux_Get_Error(&myConn);
		err = true;
		goto CleanUp;
	}

	ClientSocket[ClientID]->iGetPeerAddress(peerAddr);

	if (!IsAddressValid(peerAddr, myConn, true))
	{
		sprintf(Txt, "(!) IP address [%s] is on the block list and tried to login!", peerAddr);
		PutLogList(Txt, WARN_MSG, TRUE, HACK_LOGFILE);
		err = true;
		goto CleanUp;
	}
	
	ZeroMemory(Txt, sizeof(Txt));
	ZeroMemory(macAddress, sizeof(macAddress));
	ZeroMemory(accName, sizeof(accName));
	ZeroMemory(accPass, sizeof(accPass));

	pData = dataBuff;

	Pop(pData, (uint32&) msgID);
	Pop(pData, Txt, 12);
	MakeGoodName(macAddress, Txt);
	Pop(pData, accName, 10);
	Pop(pData, accPass, 10);

	if (!IsAddressValid(macAddress, myConn, false))
	{
		sprintf(Txt, "(!) MAC address [%s] is invalid! Related IP [%s]", macAddress, peerAddr);
		PutLogList(Txt, WARN_MSG, TRUE, HACK_LOGFILE);
		err = true;
		goto CleanUp;
	}

	switch (msgID) {		
		case MSGID_REQUEST_LOGIN:
			if(dwMsgSize < headSize + 30){
				err = true;
				break;
			}
			PutLogList("(!) Processing client login...");
			
			if(ProcessClientLogin(accName, accPass, ClientID, myConn)) {
				SendCharList(accName, ClientID, myConn);
			}
		break;

		case MSGID_REQUEST_CREATENEWCHARACTER:
			if(dwMsgSize < headSize + 51){
				err = true;
				break;
			}
			if(ProcessClientLogin(accName, accPass, ClientID, myConn)) 
				CreateNewCharacter((dataBuff+16), ClientID, myConn);
		break;

		case MSGID_REQUEST_CREATENEWACCOUNT:
			if(dwMsgSize < headSize + 220){
				err = true;
				break;
			}
			CreateNewAccount((dataBuff+16), ClientID, myConn);
		break;
		case MSGID_REQUEST_DELETECHARACTER:
			if(dwMsgSize < headSize + 40){
				err = true;
				break;
			}
			if(ProcessClientLogin(accName, accPass, ClientID, myConn)) 
				DeleteCharacter((dataBuff+16), ClientID, myConn);
		break;

		case MSGID_REQUEST_CHANGEPASSWORD:
			if(dwMsgSize < headSize + 20){
				err = true;
				break;
			}
			if(ProcessClientLogin(accName, accPass, ClientID, myConn)) 
				ChangePassword((dataBuff+16), ClientID, myConn);
		break;

		case MSGID_REQUEST_ENTERGAME:
			if(dwMsgSize < headSize + 160){
				err = true;
				break;
			}
			if(ProcessClientLogin(accName, accPass, ClientID, myConn)) {
				ProcessClientRequestEnterGame((dataBuff+16), ClientID, myConn);
				ZeroMemory(charName, sizeof(charName));
				strncpy(charName, dataBuff +16, 10);
				wsprintf(Txt, "Login: IP(%s) MAC(%s) Acc(%s) Char(%s)", peerAddr, macAddress, accName, charName);
				PutLogFileList(Txt, CONNECTION_LOGFILE);
			}
		break;

		default:
			char cDump[1000];
			wsprintf(cDump, " Unknown packet rcvd from client! message received! (0x%.8X)", msgID);
			PutLogList(cDump);
			PutLogFileList(cDump, CLIENTUNKNOWNMSG_LOGFILE2);
			for(DWORD i = 0; i < dwMsgSize; i++) if(dataBuff[i] == NULL) dataBuff[i] = ' ';
			PutLogFileList(dataBuff, CLIENTUNKNOWNMSG_LOGFILE);
			err = true;
		break;
	
	}
	CleanUp:
	if (err) SAFEDELETE(ClientSocket[ClientID]);
	SAFEDELETEARRAY(dataBuff);
	mysql_close(&myConn);	
}
//=============================================================================
void CLoginServer::OnGameServerRead(WORD GSID)
{
 char  * pData, cKey, *dataBuff, accName[12];
 DWORD  dwMsgSize, *dwpMsgID;
 WORD AccountID;
 MYSQL myConn;

	if (GameServerSocket[GSID] == NULL) return;

	pData = GameServerSocket[GSID]->pGetRcvDataPointer(&dwMsgSize, &cKey);
	dataBuff = new char[dwMsgSize+2];
	ZeroMemory(dataBuff, dwMsgSize+2);

	SafeCopy(dataBuff, pData, dwMsgSize);
    	
	dwpMsgID = (DWORD *)(dataBuff);
	switch (*dwpMsgID) {

		case MSGID_CONFIRMFORCELOGOUT:
			strncpy(accName, dataBuff+6, 10);
			if(IsAccountInUse(accName, &AccountID))
				SAFEDELETE(Client[AccountID]);
		break;

		case MSGID_REQUEST_REGISTERGAMESERVER:
		RegisterGameServer((dataBuff+6), (BYTE)GSID);
		break;

		case MSGID_REQUEST_REGISTERGAMESERVERSOCKET:
		RegisterGameServerSocket((dataBuff+4), (BYTE)GSID);
		break;

		case MSGID_ENTERGAMECONFIRM:
			if(GameServerSocket[GSID]->IsRegistered == FALSE){
				SAFEDELETE(dataBuff);
				return;
			}
			ConfirmCharEnterGame(dataBuff+6, (BYTE)GSID);
		break;

		case MSGID_REQUEST_SETACCOUNTWAITSTATUS:
			if(GameServerSocket[GSID]->IsRegistered == FALSE){
				SAFEDELETE(dataBuff);
				return;
			}
			SetAccountServerChangeStatus(dataBuff+6, TRUE);
		break;

		case MSGID_GAMEMASTERLOG:
			if(GameServerSocket[GSID]->IsRegistered == FALSE){
				SAFEDELETE(dataBuff);
				return;
			}
			PutLogFileList(dataBuff+6, GM_LOGFILE);
		break;
		case MSGID_PLAYERLOG:
			if(GameServerSocket[GSID]->IsRegistered == FALSE){
				SAFEDELETE(dataBuff);
				return;
			}
			PutLogFileList(dataBuff+6, PLAYER_LOGFILE);
		break;
		case MSGID_GAMEITEMLOG:
		case MSGID_ITEMLOG:
			if(GameServerSocket[GSID]->IsRegistered == FALSE){
				SAFEDELETE(dataBuff);
				return;
			}
			PutLogFileList(dataBuff+6, ITEM_LOGFILE);
		break;

		case MSGID_GAMECRUSADELOG:
			if(GameServerSocket[GSID]->IsRegistered == FALSE){
				SAFEDELETE(dataBuff);
				return;
			}
			PutLogFileList(dataBuff+6, CRUSADE_LOGFILE);
		break;

		case MSGID_GAMESERVERALIVE:
			if(GameServerSocket[GSID]->IsRegistered == FALSE){
				SAFEDELETE(dataBuff);
				return;
			}
			if(GameServerSocket[GSID]->GSID >= 0)	GameServer[GameServerSocket[GSID]->GSID]->AliveResponseTime = timeGetTime();
		break;

		case MSGID_GAMESERVERSHUTDOWNED:
			if(GameServerSocket[GSID]->IsRegistered == FALSE){
				SAFEDELETE(dataBuff);
				return;
			}
			if(GameServerSocket[GSID]->GSID >= 0){
				GameServer[GameServerSocket[GSID]->GSID]->IsBeingClosed = TRUE;
				PutLogList("(!!!) A Game server is being closed!", WARN_MSG);
			}
		break;

		case MSGID_SERVERSTOCKMSG:
			if(GameServerSocket[GSID]->IsRegistered == FALSE){
				SAFEDELETE(dataBuff);
				return;
			}
			ServerStockMsgHandler(dataBuff, (BYTE)GSID);
		break;

		case MSGID_SENDSERVERSHUTDOWNMSG:
			if(GameServerSocket[GSID]->IsRegistered == FALSE){
				SAFEDELETE(dataBuff);
				return;
			}
		break;

		case MSGID_REQUEST_PLAYERDATA:
		case MSGID_REQUEST_NOSAVELOGOUT:
		case MSGID_REQUEST_SAVEPLAYERDATALOGOUT:
		case MSGID_REQUEST_SAVEPLAYERDATA_REPLY:
		case MSGID_REQUEST_SAVEPLAYERDATA:
		case MSGID_REQUEST_CREATENEWGUILD:
		case MSGID_REQUEST_DISBANDGUILD:
		case MSGID_REQUEST_UPDATEGUILDINFO_NEWGUILDSMAN:
		case MSGID_REQUEST_UPDATEGUILDINFO_DELGUILDSMAN:
		case MSGID_REQUEST_LGNPTS:
		case MSGID_REQUEST_LGNSVC:
		case MSGID_SUBCASH:
		case MSGID_ADDCASH:
		case MSGID_REQUEST_LOAD_GUILDINFO:
		case MSGID_SAVE_GUILDINFO:
		case MSGID_ADDAUCTIONITEM:
		case MSGID_REQUEST_AUCTIONITEMLIST:
		case MSGID_REQUEST_AUCTION_ACTION:
			if(GameServerSocket[GSID]->IsRegistered == FALSE){
				SAFEDELETE(dataBuff);
				return;
			}
			mysql_init(&myConn);
			if(!mysql_real_connect(&myConn, mySqlAddress, mySqlUser, mySqlPwd, "playerdb2", mySqlPort, NULL, NULL)){
				MyAux_Get_Error(&myConn);
				mysql_close(&myConn);
				if (PutMsgQuene(dataBuff, dwMsgSize, GSID, cKey) == FALSE) {
					PutLogFileList("@@@@@@ CRITICAL ERROR in MsgQuene!!! @@@@@@");
					SAFEDELETE(dataBuff);
				}	
				return;
			}
			ProcessGSMsgWithDBAccess(dataBuff, (BYTE)GSID, myConn);
			mysql_close(&myConn);
		break;

		case MSGID_GAMESERVERINITALIZED:
			if(GameServerSocket[GSID]->IsRegistered == FALSE){
				SAFEDELETE(dataBuff);
				return;
			}
			if(GameServerSocket[GSID]->GSID >= 0)	
				GameServer[GameServerSocket[GSID]->GSID]->IsInitialized = true;			
			break;
					
		default:
		PutLogList("(!) Unknown packet rcvd from Game server socket!", WARN_MSG);

		for(DWORD i = 0; i < dwMsgSize; i++) if(dataBuff[i] == NULL) dataBuff[i] = ' ';
		PutLogFileList(dataBuff, GSUNKNOWNMSG_LOGFILE);
		char cDump[1000];
			wsprintf(cDump, " Unknown packet rcvd from Game server! message received! (0x%.8X)", *dwpMsgID);
			PutLogList(cDump);
			PutLogFileList(cDump, GSUNKNOWNMSG_LOGFILE2);
		break;

	}
	SAFEDELETEARRAY(dataBuff);
}
//=============================================================================
void CLoginServer::ProcessGSMsgWithDBAccess(char *pData, BYTE GSID, MYSQL myConn)
{
 DWORD *dwpMsgID;
	
	dwpMsgID = (DWORD *)(pData);
	switch(*dwpMsgID)
	{
	case MSGID_REQUEST_PLAYERDATA:
		ProcessRequestPlayerData(pData+6, (BYTE)GSID, myConn);
		break;

	case MSGID_REQUEST_NOSAVELOGOUT:
		ProcessClientLogout(pData+6, FALSE, (BYTE)GSID, myConn);
		break;

	case MSGID_REQUEST_SAVEPLAYERDATALOGOUT:
	case MSGID_REQUEST_SAVEPLAYERDATA_REPLY:
		ProcessClientLogout(pData+6, TRUE, (BYTE)GSID, myConn);
		break;

	case MSGID_REQUEST_SAVEPLAYERDATA:
		SaveCharacter(pData+6, myConn);
		break;

	case MSGID_REQUEST_CREATENEWGUILD:
		RequestCreateNewGuildHandler(pData+6, (BYTE)GSID, myConn);
		break;

	case MSGID_REQUEST_DISBANDGUILD:
		RequestDisbandGuildHandler(pData+6, (BYTE)GSID, myConn);
		break;

	case MSGID_REQUEST_UPDATEGUILDINFO_NEWGUILDSMAN:
		AddGuildMember(pData+6, myConn);
		break;

	case MSGID_REQUEST_UPDATEGUILDINFO_DELGUILDSMAN:
		RemoveGuildMember(pData+6, myConn);
		break;

	case MSGID_REQUEST_LOAD_GUILDINFO:
		ProcessRequest_GuildInfo(pData+4, (BYTE)GSID, myConn);
		break;

	case MSGID_SAVE_GUILDINFO:
		Process_SaveGuildInfo(pData+4, myConn);
		break;

	case MSGID_REQUEST_LGNPTS:
		RequestLegionPts(pData+4, GSID, myConn);
		break;
		
	case MSGID_REQUEST_LGNSVC:
		RequestLegionSvc(pData+4, GSID, myConn);
		break;

	case MSGID_SUBCASH:
		SubAccountCash(pData+4, myConn);
		break;
			
	case MSGID_ADDCASH:
		AddAccountCash(pData+4, myConn);
		break;

	case MSGID_ADDAUCTIONITEM:
		AddAuctionItem(pData+4, GSID, myConn);
		break;

	case MSGID_REQUEST_AUCTIONITEMLIST:
		ReqAuctionItemList(pData+4, GSID, myConn);
		break;

	case MSGID_REQUEST_AUCTION_ACTION:
		ReqAuctionAction(pData+4, GSID, myConn);
		break;
	}
}
//=============================================================================
void CLoginServer::CloseGameServerSocket(WORD ID, BOOL log)
{
	char GSID, GSSocketID, Txt100[100];

	if(GameServerSocket[ID]->GSSocketID < 0 || GameServerSocket[ID]->GSID < 0) {
		SAFEDELETE(GameServerSocket[ID]);
		return;
	}
	if(GameServerSocket[ID] != NULL)
	{
		GSID = GameServerSocket[ID]->GSID;
		GSSocketID = GameServerSocket[ID]->GSSocketID;
		if(GSID >= 0 && GSSocketID >= 0) GameServer[GSID]->SocketIndex[GSSocketID] = -1;
		SAFEDELETE(GameServerSocket[ID]);
		if(log)
		{
			ZeroMemory(Txt100, sizeof(Txt100));
			sprintf(Txt100, "(!) A GameServer socket was closed [%u].", ID);
			PutLogList(Txt100, WARN_MSG);
		}
		if(GameServer[GSID] == NULL) return;
		GameServer[GSID]->ConnectedSockets--;
		if(GameServer[GSID]->ConnectedSockets == 0){
			SAFEDELETE(GameServer[GSID]);
		}
	}
}
//=============================================================================
void CLoginServer::CloseClientSocket(WORD ID, BOOL log)
{
 char Txt100[100];

        if(ClientSocket[ID] != NULL)
          {
           if(log)
             {
              ZeroMemory(Txt100, sizeof(Txt100));
              sprintf(Txt100, "A client socket was closed [%u].", ID);
              PutLogList(Txt100, WARN_MSG);
             }
           SAFEDELETE(ClientSocket[ID]);
          }
}
//=============================================================================
BYTE CLoginServer::CheckAccountLogin(char *AccName, char *AccPwd, MYSQL myConn)
{
 char QueryConsult[500], Txt[200], GoodName[25];
 st_mysql_res    *QueryResult = NULL;
 MYSQL_FIELD *field[2];
 MYSQL_ROW myRow;
 BYTE b;
 	 
		ZeroMemory(QueryConsult, sizeof(QueryConsult));
		ZeroMemory(GoodName, sizeof(GoodName));
		MakeGoodName(GoodName, AccName);
		sprintf(QueryConsult, "SELECT `name`, `password` FROM `account_database` WHERE `name` = '%s' LIMIT 1;", GoodName);
        if(ProcessQuery(&myConn, QueryConsult) == -1) return 0;
        QueryResult = mysql_store_result(&myConn);
		if(mysql_num_rows(QueryResult) == 0){
			SAFEFREERESULT(QueryResult);
			return ACCOUNTDONTEXISTS;
		}
		myRow = mysql_fetch_row(QueryResult);
		mysql_field_seek(QueryResult, 0);
		for(b = 0; b < 2; b++){
			field[b] = mysql_fetch_field(QueryResult);
			if(IsSame(field[b]->name, "name") && !IsSame(myRow[b], AccName)){
				SAFEFREERESULT(QueryResult);
				return ACCOUNTDONTEXISTS;
			}
			else if(IsSame(field[b]->name, "password") && !IsSame(myRow[b], AccPwd)){
				ZeroMemory(Txt, sizeof(Txt));
                sprintf(Txt, "(!)Wrong password: Account[ '%s' ] - Correct Password[ '%s' ] - Password received[ '%s' ]", AccName, myRow[b], AccPwd);
                PutLogList(Txt, WARN_MSG);
				SAFEFREERESULT(QueryResult);
				return PASSWORDISWRONG;
			}
		}
		SAFEFREERESULT(QueryResult);
		return LOGINOK;	
}
//=============================================================================
BOOL CLoginServer::ProcessClientLogin(char *AccName, char *AccPwd, WORD ClientID, MYSQL myConn)
{
	char Txt200[200];
	char *cp = Txt200; 

	ZeroMemory(Txt200, sizeof(Txt200));
	Push(cp, (uint32) MSGID_RESPONSE_LOG);

	switch(CheckAccountLogin(AccName, AccPwd, myConn)){
	case ACCOUNTDONTEXISTS:
		Push(cp, (uint16) LOGRESMSGTYPE_NOTEXISTINGACCOUNT);
		SendMsgToClient(ClientID, Txt200, 6);
		return FALSE;	
		break;

	case PASSWORDISWRONG:
		Push(cp, (uint16) LOGRESMSGTYPE_PASSWORDMISMATCH);
		SendMsgToClient(ClientID, Txt200, 6);
		return FALSE;	 
		break;

	case LOGINOK:
		return TRUE;	
		break;
	}
	return FALSE;
}
//=============================================================================
BOOL CLoginServer::IsDatePast(char *Date)
{
 char *token;
 SYSTEMTIME SysTime;
 CStrTok *pStrTok;


		char seps[] = " :-";
		pStrTok = new class CStrTok(Date, seps);
		
		GetLocalTime(&SysTime);
		
		token = pStrTok->pGet();
        if(atoi(token) < SysTime.wYear){
			SAFEDELETE(pStrTok);
			return true;
		}
        else if (atoi(token) > SysTime.wYear){
			SAFEDELETE(pStrTok);
			return false;
		}
        
		token = pStrTok->pGet();
        if(atoi(token) < SysTime.wMonth){
			SAFEDELETE(pStrTok);
			return true;
		}
        else if(atoi(token) > SysTime.wMonth){
			SAFEDELETE(pStrTok);
			return false;
		}
        
		token = pStrTok->pGet();
        if(atoi(token) < SysTime.wDay){
			SAFEDELETE(pStrTok);
			return true;
		}
        else if(atoi(token) > SysTime.wDay){
			SAFEDELETE(pStrTok);
			return false;
		}
        
		token = pStrTok->pGet();
        if(atoi(token) < SysTime.wHour){
			SAFEDELETE(pStrTok);
			return true;
		}
        else if(atoi(token) > SysTime.wHour){
			SAFEDELETE(pStrTok);
			return false;
		}
        
		token = pStrTok->pGet();
        if(atoi(token) < SysTime.wMinute){
			SAFEDELETE(pStrTok);
			return true;
		}
        else if(atoi(token) > SysTime.wMinute){
			SAFEDELETE(pStrTok);
			return false;
		}
        
		token = pStrTok->pGet();
        if(atoi(token) < SysTime.wSecond){
			SAFEDELETE(pStrTok);
			return true;
		}
        else if(atoi(token) > SysTime.wSecond){
			SAFEDELETE(pStrTok);
			return false;
		}

        SAFEDELETE(pStrTok);
		return true;
}
//=============================================================================
void CLoginServer::ChangeDateIntoInt(char *date, WORD *year, BYTE *month, BYTE *day, BYTE *hour, BYTE *minute, BYTE *second)
{
 char seps[] = "-: ";
 CStrTok *pStrTok;

		pStrTok = new class CStrTok(date, seps);
		*year = (WORD)atoi(pStrTok->pGet());
        *month = (BYTE)atoi(pStrTok->pGet());
        *day = (BYTE)atoi(pStrTok->pGet());
        if(hour != NULL && minute != NULL && second != NULL)
          {
           *hour = (BYTE)atoi(pStrTok->pGet());
           *minute = (BYTE)atoi(pStrTok->pGet());
           *second = (BYTE)atoi(pStrTok->pGet());
          }
		SAFEDELETE(pStrTok);
}
//=============================================================================
BOOL CLoginServer::IsAddrPermitted(char *addr)
{
        for(BYTE b = 0; b < MAXGAMESERVERS; b++) if(IsSame(PermittedAddress[b], addr)) return TRUE;
        return false;
}
//=============================================================================
void CLoginServer::RegisterGameServer(char *Data, BYTE ID)
{
 char ServerName[15], SendData[15], Txt100[100];
 _ADDRESS ServerIP;
 WORD ServerPort, *wp, InternalID;
 BYTE NumberOfMaps;
 DWORD *dwp;
 BOOL ReceivedConfig;

        ZeroMemory(ServerName, sizeof(ServerName));
        SafeCopy(ServerName, Data, 10);
		ZeroMemory(ServerIP, sizeof(ServerIP));
        SafeCopy(ServerIP, Data+10, 16);
        ServerPort = wGetOffsetValue(Data, 26);
		ReceivedConfig = bGetOffsetValue(Data, 28);
        NumberOfMaps = bGetOffsetValue(Data, 29);
		InternalID = wGetOffsetValue(Data, 30);
        for(WORD w=0; w<MAXGAMESERVERS; w++)
           {
            if(GameServer[w] == NULL)
              {
               GameServerSocket[ID]->GSID = (sBYTE)w;
			   GameServer[w] = new CGameServer(ID);
               SafeCopy(GameServer[w]->ServerName, ServerName);
               SafeCopy(GameServer[w]->ServerIP, ServerIP);
               GameServer[w]->ServerPort = ServerPort;
               GameServer[w]->NumberOfMaps = NumberOfMaps;
			   GameServer[w]->InternalID = w;
			   PutLogList("(!) Maps registered:");
               for(BYTE b = 0; b < NumberOfMaps; b++){
				   ZeroMemory(GameServer[w]->MapName[b], sizeof(GameServer[w]->MapName[b]));
				   SafeCopy(GameServer[w]->MapName[b], (Data+32+(11*b)), 10);
				   ZeroMemory(Txt100, sizeof(Txt100));
				   sprintf(Txt100, "- %s", GameServer[w]->MapName[b]);
                   PutLogList(Txt100);
			   }               
               if(!ReceivedConfig) SendConfigToGS(ID);

               ZeroMemory(SendData, sizeof(SendData));
               dwp  = (DWORD*)SendData;
               *dwp = MSGID_RESPONSE_REGISTERGAMESERVER;
               wp   = (WORD*)(SendData+4);
               *wp  = MSGTYPE_CONFIRM;
               wp   = (WORD*)(SendData+6);
               *wp  = w;
               ZeroMemory(Txt100, sizeof(Txt100));
               sprintf(Txt100, "(!) Game Server registered at ID[%u]-[%u].", w, GameServer[w]->InternalID);
               PutLogList(Txt100);
               SendMsgToGS(ID, SendData, 8);
               return;
              }
           }
}
//=============================================================================
void CLoginServer::RegisterGameServerSocket(char *Data, BYTE ID)
{
 BYTE GSID;
 char Txt100[100];

        GSID = bGetOffsetValue(Data, 0);
        ZeroMemory(Txt100, sizeof(Txt100));
        sprintf(Txt100, "(!) Trying to register socket on GS[%u].", GSID);
        PutLogList(Txt100);
        if(GameServer[GSID] == NULL) {
			PutLogList("(!) GSID is not registered!", WARN_MSG);
			return; 
		}
		for(BYTE b = 0; b < MAXSOCKETSPERSERVER ; b++)
            if(GameServer[GSID]->SocketIndex[b] < 0)
              {
               GameServer[GSID]->SocketIndex[b] = ID;
               GameServer[GSID]->ConnectedSockets++;
			   GameServerSocket[ID]->GSID = GSID;
               GameServerSocket[ID]->GSSocketID = b;
               GameServerSocket[ID]->IsRegistered = TRUE;
			   ZeroMemory(Txt100, sizeof(Txt100));
			   sprintf(Txt100, "(!) Registered Socket(%d) Index(%d) GSID(%d) SocketID(%d).", b, ID, GSID, b);
               PutLogList(Txt100, INFO_MSG);
			   if(GameServer[GSID]->ConnectedSockets == MAXSOCKETSPERSERVER){
				   GameServer[GSID]->IsSocketConnected = TRUE;
				   ZeroMemory(Txt100, sizeof(Txt100));
				   sprintf(Txt100, "(!) Gameserver(%s) registered!", GameServer[GSID]->ServerName);
				   PutLogList(Txt100, INFO_MSG);
			   }
               return;
              }
}
//=============================================================================
BOOL CLoginServer::ReadConfig(char *FileName)
{
 FILE * pFile;
 DWORD  dwFileSize;
 char   File[50], Txt100[100];

        ZeroMemory(Txt100, sizeof(Txt100));
        ZeroMemory(File, sizeof(File));
        sprintf(File, "Config/%s", FileName);
	pFile = fopen(File, "rt");
	if (pFile == NULL) {
        sprintf(Txt100, "(!) Cannot open configuration file [%s].", File);
		PutLogList(Txt100, WARN_MSG, TRUE, ERROR_LOGFILE);
        PutLogList("(!!!) Stopped!", WARN_MSG);
		return FALSE;
	}
	else {
        sprintf(Txt100, "(!) Reading configuration file [%s]...", File);
		PutLogList(Txt100);
        dwFileSize = filesize(pFile);
		
		if(IsSame(FileName, "Item.cfg")){
			ItemCfg = new char[dwFileSize+2];
			ZeroMemory(ItemCfg, dwFileSize+2);
			fread(ItemCfg, dwFileSize, 1, pFile);
		}
		else if(IsSame(FileName,"Item2.cfg")){
			Item2Cfg = new char[dwFileSize+2];
			ZeroMemory(Item2Cfg, dwFileSize+2);
			fread(Item2Cfg, dwFileSize, 1, pFile);
		}
        else if(IsSame(FileName,"Item3.cfg")){
			Item3Cfg = new char[dwFileSize+2];
			ZeroMemory(Item3Cfg, dwFileSize+2);
			fread(Item3Cfg, dwFileSize, 1, pFile);
		}
        else if(IsSame(FileName,"BuildItem.cfg")){
			BuildItemCfg = new char[dwFileSize+2];
			ZeroMemory(BuildItemCfg, dwFileSize+2);
			fread(BuildItemCfg, dwFileSize, 1, pFile);
		}
        else if(IsSame(FileName,"DupItemID.cfg")){
			DupItemIDCfg = new char[dwFileSize+2];
			ZeroMemory(DupItemIDCfg, dwFileSize+2);
			fread(DupItemIDCfg, dwFileSize, 1, pFile);
		}
        else if(IsSame(FileName,"Magic.cfg")){
			MagicCfg = new char[dwFileSize+2];
			ZeroMemory(MagicCfg, dwFileSize+2);
			fread(MagicCfg, dwFileSize, 1, pFile);
		}
        else if(IsSame(FileName,"noticement.txt")){
			NoticementTxt = new char[dwFileSize+2];
			ZeroMemory(NoticementTxt, dwFileSize+2);
			fread(NoticementTxt, dwFileSize, 1, pFile);
		}
        else if(IsSame(FileName,"NPC.cfg")){
			NPCCfg = new char[dwFileSize+2];
			ZeroMemory(NPCCfg, dwFileSize+2);
			fread(NPCCfg, dwFileSize, 1, pFile);
		}
        else if(IsSame(FileName,"potion.cfg")){
			PotionCfg = new char[dwFileSize+2];
			ZeroMemory(PotionCfg, dwFileSize+2);
			fread(PotionCfg, dwFileSize, 1, pFile);	
		}
        else if(IsSame(FileName,"Quest.cfg")){
			QuestCfg = new char[dwFileSize+2];
			ZeroMemory(QuestCfg, dwFileSize+2);
			fread(QuestCfg, dwFileSize, 1, pFile);	
		}
        else if(IsSame(FileName,"Skill.cfg")){
			SkillCfg = new char[dwFileSize+2];
			ZeroMemory(SkillCfg, dwFileSize+2);
			fread(SkillCfg, dwFileSize, 1, pFile);
		  }
		  else if(IsSame(FileName,"CraftItem.cfg")){
			  CraftingCfg = new char[dwFileSize+2];
			  ZeroMemory(CraftingCfg, dwFileSize+2);
			  fread(CraftingCfg, dwFileSize, 1, pFile);
		  }
		  else if(IsSame(FileName,"Teleport.cfg")){
			  TeleportCfg = new char[dwFileSize+2];
			  ZeroMemory(TeleportCfg, dwFileSize+2);
			  fread(TeleportCfg, dwFileSize, 1, pFile);
		  }
		else{
			sprintf(Txt100, "(!) Cannot handle configuration file [%s].", File);
			PutLogList(Txt100, WARN_MSG, TRUE, ERROR_LOGFILE);
			PutLogList("(!!!) Stopped!", WARN_MSG);
		}

		if (pFile != NULL) fclose(pFile);
        return TRUE;
             }
}
//=============================================================================
void CLoginServer::SendCharList(char* AccountName, WORD ClientID, MYSQL myConn)
{
	DWORD ListSize = 0;
	char Txt100[100], Txt500[500];

	ZeroMemory(Txt100, sizeof(Txt100));
	sprintf(Txt100, "(!) Getting character list for account [%s].", AccountName);
	PutLogList(Txt100);
	ZeroMemory(Txt500, sizeof(Txt500));
	PutOffsetValue(Txt500, 0, DWORDSIZE, MSGID_RESPONSE_LOG);
	PutOffsetValue(Txt500, 4, WORDSIZE, MSGTYPE_CONFIRM);
	PutOffsetValue(Txt500, 6, WORDSIZE, UPPER_VERSION);
	PutOffsetValue(Txt500, 8, WORDSIZE, LOWER_VERSION);
	//PutOffsetValue(Txt500, 10, BYTESIZE, 0x01);
   //wp = (WORD*)(Txt500 +11);
   //*wp = 2003;
   //wp = (WORD*)(Txt500 +13);
   //*wp = 10;
   //wp = (WORD*)(Txt500 +15);
   //*wp = 14;
   //wp = (WORD*)(Txt500 +17);
   //*wp = 2003;
   //wp = (WORD*)(Txt500 +19);
   //*wp = 11;
   //wp = (WORD*)(Txt500 +21);
   //*wp = 15;
	GetCharList(AccountName, Txt500+10, &ListSize, myConn);
	SendMsgToClient(ClientID, Txt500, ListSize+10);
	return;
}
//=============================================================================
void CLoginServer::CreateNewCharacter(char *Data, WORD ClientID, MYSQL myConn)
{
	char CharName[11], AccName[15], GoodAccName[25], GoodCharName[25],  Txt500[500], QueryConsult[6000];
	BYTE STR, VIT, DEX, INT, MAG, AGI;
	DWORD *dwp;
	WORD *wp, NRows;
	cItem *Item;
	st_mysql_res    *QueryResult = NULL;
	WORD AccountID;

	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	ZeroMemory(GoodCharName, sizeof(GoodCharName));
	ZeroMemory(CharName, sizeof(CharName));
	ZeroMemory(AccName, sizeof(AccName));
	ZeroMemory(Txt500, sizeof(Txt500));

	SafeCopy(AccName, Data, 10);

	AccountID = GetAccountID(AccName, myConn);
	if (AccountID == 0) return;

	SafeCopy(CharName, Data+20, 10);
	MakeGoodName(GoodCharName, CharName);

	dwp = (DWORD*)Txt500;
	*dwp = MSGID_RESPONSE_LOG;

	sprintf(QueryConsult, "SELECT * FROM `char_database` WHERE `char_name` = '%s';", GoodCharName);
	if(ProcessQuery(&myConn, QueryConsult) == -1) return;
	QueryResult = mysql_store_result(&myConn);
	NRows = (BYTE)mysql_num_rows(QueryResult);
	if(NRows > 0){
		wp = (WORD*)(Txt500+4);
		*wp = LOGRESMSGTYPE_ALREADYEXISTINGCHARACTER;
		SendMsgToClient(ClientID, Txt500, 6);
	}
	else{
		ZeroMemory(QueryConsult, sizeof(QueryConsult));
		ZeroMemory(GoodAccName, sizeof(GoodAccName));
		MakeGoodName(GoodAccName, AccName);
		sprintf(QueryConsult, "SELECT * FROM `char_database` WHERE `account_name` = '%s';", GoodAccName);
		if(ProcessQuery(&myConn, QueryConsult) == -1) return;
		SAFEFREERESULT(QueryResult);
		QueryResult = mysql_store_result(&myConn);
		NRows = (BYTE)mysql_num_rows(QueryResult);
		STR = bGetOffsetValue(Data, 65);
		VIT = bGetOffsetValue(Data, 66);
		DEX = bGetOffsetValue(Data, 67);
		INT = bGetOffsetValue(Data, 68);
		MAG = bGetOffsetValue(Data, 69);
		AGI = bGetOffsetValue(Data, 70);
		if(STR+VIT+DEX+INT+MAG+AGI != 70 || STR<10 || VIT<10 || DEX<10 || INT<10 || MAG<10 || AGI<10 || STR>14 || VIT>14 || DEX>14 || INT>14 || MAG>14 || AGI>14 || NRows >= 4){
			wp = (WORD*)(Txt500+4);
			*wp = LOGRESMSGTYPE_NEWCHARACTERFAILED;
			SendMsgToClient(ClientID, Txt500, 6);
		}
		else{
			short Appr1value = (short)((bGetOffsetValue(Data,62) << 8) | (bGetOffsetValue(Data,63) << 4) | bGetOffsetValue(Data,64));
			ZeroMemory(QueryConsult, sizeof(QueryConsult));
			sprintf(QueryConsult, "INSERT INTO `char_database` (`account_name`, `char_name`, `Strenght` , `Vitality` , `Dexterity` , `Intelligence` , `Magic` , `Agility` , `Appr1`, `Gender` , `Skin` , `HairStyle` , `HairColor` , `Underwear` , `HP` , `MP` , `SP`)\
										 VALUES ( '%s',				'%s'	 ,	%d	,	%d	,	%d		,	%d		,	%d	,	%d	,%d	,	%d,	%d,		%d	,	%d	,		%d	,	%d,%d,%d);",
										 GoodAccName	, GoodCharName ,	STR		,	VIT		,	DEX			,	INT			,	MAG		,	AGI	,Appr1value,bGetOffsetValue(Data,60), bGetOffsetValue(Data,61), bGetOffsetValue(Data,62), bGetOffsetValue(Data,63), bGetOffsetValue(Data,64), (VIT*3)+(STR/2)+2, (MAG*2)+(INT/2)+2, (STR*2)+2);
			int InsertResult = ProcessQuery(&myConn, QueryConsult);
			if(InsertResult == -1) return;
			QueryResult = mysql_store_result(&myConn);
			SAFEFREERESULT(QueryResult);
			if(InsertResult == 0){
				DWORD CharID = GetCharID(CharName, AccName, myConn);
				for(BYTE m = 0; m < MAXMAGICS; m++){
					if (m == 12) {
						ZeroMemory(QueryConsult, sizeof(QueryConsult));
						sprintf(QueryConsult, "INSERT INTO `magic` ( `CharID` , `MagicID`, `MagicMastery` , `Charge`) VALUES ( %lu, %d, 1, -1);", CharID , m);
						if(ProcessQuery(&myConn, QueryConsult) == -1) return;
						QueryResult = mysql_store_result(&myConn);
						SAFEFREERESULT(QueryResult);
					}else {
						ZeroMemory(QueryConsult, sizeof(QueryConsult));
						sprintf(QueryConsult, "INSERT INTO `magic` ( `CharID` , `MagicID`, `MagicMastery` , `Charge`) VALUES ( %lu, %d, 0, 0);", CharID , m);
						if(ProcessQuery(&myConn, QueryConsult) == -1) return;
						QueryResult = mysql_store_result(&myConn);
						SAFEFREERESULT(QueryResult);
					}
				}
				for(BYTE s = 0; s < MAXSKILLS; s++){
					if(s == 4 || s == 5 || s == 7){
						ZeroMemory(QueryConsult, sizeof(QueryConsult));
						sprintf(QueryConsult, "INSERT INTO `skill` ( `CharID` , `SkillID`, `SkillMastery` , `SkillSSN`)\
													 VALUES (	%lu ,		%d ,		%d	 ,  %u  );",
													 CharID  ,  s	,  20  ,  0 );
						if(ProcessQuery(&myConn, QueryConsult) == -1) return;
						QueryResult = mysql_store_result(&myConn);
						SAFEFREERESULT(QueryResult);
					}
					else if(s == 3){
						ZeroMemory(QueryConsult, sizeof(QueryConsult));
						sprintf(QueryConsult, "INSERT INTO `skill` ( `CharID` , `SkillID`, `SkillMastery` , `SkillSSN`) VALUES ( %lu, %d, %d, %d );", CharID, s, 2, 0 );
						if(ProcessQuery(&myConn, QueryConsult) == -1) return;
						QueryResult = mysql_store_result(&myConn);
						SAFEFREERESULT(QueryResult);
					}
					else{
						ZeroMemory(QueryConsult, sizeof(QueryConsult));
						sprintf(QueryConsult, "INSERT INTO `skill` ( `CharID` , `SkillID`, `SkillMastery` , `SkillSSN`) VALUES ( %lu, %d, %d, %d );", CharID, s, 0, 0);
						if(ProcessQuery(&myConn, QueryConsult) == -1) return;
						QueryResult = mysql_store_result(&myConn);
						SAFEFREERESULT(QueryResult);
					}
				}
				Item = new cItem;

				Item->MakeItemInfo("Dagger", 1, 2, 0, 0, 0, 0, 0, 0, 300, 0, 0, 0, FALSE, FALSE, 30, 30);
				CreateNewItem(Item, AccountID, CharID, myConn);
				Item->ItemUniqueID = 0;
				Item->MakeItemInfo("RedPotion", 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, FALSE, FALSE, 40, 30);
				CreateNewItem(Item, AccountID, CharID, myConn);
				Item->ItemUniqueID = 0;
				Item->MakeItemInfo("RedPotion", 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, FALSE, FALSE, 50, 30);
				CreateNewItem(Item, AccountID, CharID, myConn);
				Item->ItemUniqueID = 0;
				Item->MakeItemInfo("BluePotion", 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, FALSE, FALSE, 60, 30);
				CreateNewItem(Item, AccountID, CharID, myConn);
				Item->ItemUniqueID = 0;
				Item->MakeItemInfo("GreenPotion", 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, FALSE, FALSE, 70, 30);
				CreateNewItem(Item, AccountID, CharID, myConn);
				Item->ItemUniqueID = 0;
				Item->MakeItemInfo("Gold", 1000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, FALSE, FALSE, 80, 30);
				CreateNewItem(Item, AccountID, CharID, myConn);
				SAFEDELETE(Item);
				wp = (WORD*)(Txt500+4);
				*wp = LOGRESMSGTYPE_NEWCHARACTERCREATED;
				SafeCopy(Txt500+6, CharName, 10);
				DWORD ListSize;
				GetCharList(AccName, Txt500+16, &ListSize, myConn);
				SendMsgToClient(ClientID, Txt500, ListSize+16);
			}
			else{
				wp = (WORD*)(Txt500+4);
				*wp = LOGRESMSGTYPE_NEWCHARACTERFAILED;
				SendMsgToClient(ClientID, Txt500, 6);
			}
		}
	}
	SAFEFREERESULT(QueryResult);
}
//=============================================================================
void CLoginServer::SendMsgToClient(WORD ClientID, char * cData, DWORD dwSize, char cKey, BOOL log)
{
 		if(ClientID >= MAXCLIENTS) return;
		if(ClientSocket[ClientID] == NULL) return;
		ClientSocket[ClientID]->iSendMsg(cData, dwSize, cKey, log);
}
//=============================================================================
void CLoginServer::SendMsgToGS(WORD GSID, char * cData, DWORD dwSize, char cKey, BOOL log)
{
		if(GSID >= MAXGAMESERVERSOCKETS) return;
		if(GameServerSocket[GSID] == NULL || GameServer[GameServerSocket[GSID]->GSID] == NULL) return;
        else GameServer[GameServerSocket[GSID]->GSID]->SendMsg(cData, dwSize, cKey, log);
}
//=============================================================================
void CLoginServer::SendMsgToAllGameServers(int iClientH, char *pData, DWORD dwMsgSize, BOOL bIsOwnSend)
{
 int i;

	if (bIsOwnSend == TRUE) {
		for (i = 0; i < MAXGAMESERVERS; i++) if(GameServer[i] != NULL) GameServer[i]->SendMsg(pData, dwMsgSize, NULL, NULL);
	}
	else {
		for (i = 0; i < MAXGAMESERVERS; i++) if (i != iClientH && GameServer[i] != NULL)  GameServer[i]->SendMsg(pData, dwMsgSize, NULL, NULL);
	}
}
//=============================================================================
void CLoginServer::GetCharList(char* AccountName, char *CharList, DWORD * Size, MYSQL myConn)
{
 WORD *wp, NFields;
 DWORD *dwp;
 BYTE *bp;
 st_mysql_res    *QueryResult = NULL;
 char *cp, QueryConsult[500], GoodAccName[25], NRows;
 
	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	ZeroMemory(CharList, sizeof(CharList));
	ZeroMemory(GoodAccName, sizeof(GoodAccName));
	MakeGoodName(GoodAccName, AccountName);
	sprintf(QueryConsult, "SELECT * FROM `char_database` WHERE `account_name` = '%s' LIMIT 4;", GoodAccName);
	if(ProcessQuery(&myConn, QueryConsult) == -1) return;
	QueryResult = mysql_store_result(&myConn);
	NRows = (BYTE)mysql_num_rows(QueryResult);

	cp = (char*)(CharList);
	*cp = NRows;
	if(NRows > 0)
	{
		MYSQL_ROW myRow[4];
		for(WORD w = 0; w < NRows; w++) myRow[w] = mysql_fetch_row(QueryResult);
		NFields = (WORD)mysql_num_fields(QueryResult);
		MYSQL_FIELD *field[100];
		mysql_field_seek(QueryResult, 0);
		for(int w = 0; w < NFields; w++) field[w] = mysql_fetch_field(QueryResult);
		for(BYTE b = 0; b < NRows; b++) {
		for(BYTE f = 0; f < NFields; f++) {
			if(IsSame(field[f]->name, "char_name"))
			{
			SafeCopy(CharList+1+(b*59), myRow[b][f]);
			bp = (BYTE*)(CharList+1+(b*59)+10);
			*bp = 0x01;
			}
			else if(IsSame(field[f]->name, "Appr1"))
			{
			wp = (WORD*)(CharList+1+(b*59)+11);
			*wp = (WORD)atoi(myRow[b][f]);
			}
			else if(IsSame(field[f]->name, "Appr2"))
			{
			wp = (WORD*)(CharList+1+(b*59)+13);
			*wp = (WORD)atoi(myRow[b][f]);
			}
			else if(IsSame(field[f]->name, "Appr3"))
			{
			wp = (WORD*)(CharList+1+(b*59)+15);
			*wp = (WORD)atoi(myRow[b][f]);
			}
			else if(IsSame(field[f]->name, "Appr4"))
			{
			wp = (WORD*)(CharList+1+(b*59)+17);
			*wp = (WORD)atoi(myRow[b][f]);
			}
			else if(IsSame(field[f]->name, "Gender"))
			{
			wp = (WORD*)(CharList+1+(b*59)+19);
			*wp = (WORD)atoi(myRow[b][f]);
			}
			else if(IsSame(field[f]->name, "Skin"))
			{
			wp = (WORD*)(CharList+1+(b*59)+21);
			*wp = (WORD)atoi(myRow[b][f]);
			}
			else if(IsSame(field[f]->name, "Level"))
			{
			wp = (WORD*)(CharList+1+(b*59)+23);
			*wp = (WORD)atoi(myRow[b][f]);
			}
			else if(IsSame(field[f]->name, "Exp"))
			{
			dwp = (DWORD*)(CharList+1+(b*59)+25);
			*dwp = atoul(myRow[b][f]);
			}
			else if(IsSame(field[f]->name, "Strenght"))
			{
			wp = (WORD*)(CharList+1+(b*59)+29);
			*wp = (WORD)atoi(myRow[b][f]);
			}
			else if(IsSame(field[f]->name, "Vitality"))
			{
			wp = (WORD*)(CharList+1+(b*59)+31);
			*wp = (WORD)atoi(myRow[b][f]);
			}
			else if(IsSame(field[f]->name, "Dexterity"))
			{
			wp = (WORD*)(CharList+1+(b*59)+33);
			*wp = (WORD)atoi(myRow[b][f]);
			}
			else if(IsSame(field[f]->name, "Intelligence"))
			{
			wp = (WORD*)(CharList+1+(b*59)+35);
			*wp = (WORD)atoi(myRow[b][f]);
			}
			else if(IsSame(field[f]->name, "Magic"))
			{
			wp = (WORD*)(CharList+1+(b*59)+37);
			*wp = (WORD)atoi(myRow[b][f]);
			}
			else if(IsSame(field[f]->name, "Agility"))
			{
			wp = (WORD*)(CharList+1+(b*59)+39);
			*wp = (WORD)atoi(myRow[b][f]);
			}
			else if(IsSame(field[f]->name, "ApprColor1"))
			{
			dwp = (DWORD*)(CharList+1+(b*59)+41);
			*dwp = atoi(myRow[b][f]);
			}
			else if(IsSame(field[f]->name, "ApprColor2"))
			{
			dwp = (DWORD*)(CharList+1+(b*59)+45);
			*dwp = atoi(myRow[b][f]);
			}
            else if(IsSame(field[f]->name, "MapLoc")) SafeCopy(CharList+1+(b*59)+49, myRow[b][f]);
		}
		}
	}
    *Size = 1+(NRows*59);
	SAFEFREERESULT(QueryResult);
 }
//=============================================================================
void CLoginServer::DeleteCharacter(char *Data, WORD ClientID, MYSQL myConn)
{
 char CharName[15], GoodCharName[25], AccountName[15], GoodAccName[25], Txt500[500], QueryConsult[500];
 DWORD *dwp;
 WORD *wp, NRows, NFields;
 st_mysql_res    *QueryResult = NULL;
 WORD AccountID;
 
        //if(ProcessClientLogin(Data+10, ClientID, myConn)){
			ZeroMemory(CharName, sizeof(CharName));
			ZeroMemory(AccountName, sizeof(AccountName));
			ZeroMemory(QueryConsult, sizeof(QueryConsult));
			ZeroMemory(Txt500, sizeof(Txt500));

			SafeCopy(AccountName, Data, 10);
			SafeCopy(CharName, Data+20, 10);
			
			AccountID = GetAccountID(AccountName, myConn);
			if (AccountID == 0) return;

			ZeroMemory(GoodAccName, sizeof(GoodAccName));
			MakeGoodName(GoodAccName, AccountName);
			ZeroMemory(GoodCharName, sizeof(GoodCharName));
			MakeGoodName(GoodCharName, CharName);

			if(IsAccountInUse(GoodAccName)) 
			{
				dwp = (DWORD*) Txt500;
				*dwp = MSGID_RESPONSE_LOG;
				wp = (WORD*)(Txt500 +4);
				*wp = ENTERGAMERESTYPE_PLAYING;
				SendMsgToClient(ClientID, Txt500, 6);
				return;
			}

			sprintf(QueryConsult, "SELECT `account_name` ,`char_name` ,`Level` FROM `char_database` WHERE `account_name` = '%s' AND `char_name` = '%s';", GoodAccName, GoodCharName);
			if(ProcessQuery(&myConn, QueryConsult) == -1) return;
			QueryResult = mysql_store_result(&myConn);
			NRows = (WORD)mysql_num_rows(QueryResult);
			NFields = (WORD)mysql_num_fields(QueryResult);
			dwp = (DWORD*)Txt500;
			*dwp = MSGID_RESPONSE_LOG;

			if(NRows == 0){
				wp = (WORD*)(Txt500+4);
				*wp = LOGRESMSGTYPE_NOTEXISTINGCHARACTER;
				SendMsgToClient(ClientID, Txt500, 6);
				SAFEFREERESULT(QueryResult);
				return;
			}
			MYSQL_ROW myRow = mysql_fetch_row(QueryResult);
			MYSQL_FIELD *field[3];
			mysql_field_seek(QueryResult, 0);
			for(BYTE w = 0; w < NFields; w++){
				field[w] = mysql_fetch_field(QueryResult);
				if(IsSame(field[w]->name, "Level") && atoi(myRow[w]) > MAXDELETELEVEL){
					CloseClientSocket(ClientID);
					SAFEFREERESULT(QueryResult);
					return;
                }
			}
			mysql_field_seek(QueryResult, 0);
			DWORD CharID = GetCharID(CharName, AccountName, myConn);
			ZeroMemory(QueryConsult, sizeof(QueryConsult));
			sprintf(QueryConsult, "DELETE FROM `char_database` WHERE `account_name` = '%s' AND `char_name` = '%s' LIMIT 1;", GoodAccName, GoodCharName);
			int InsertResult = ProcessQuery(&myConn, QueryConsult);
			if(InsertResult == -1) return;
			SAFEFREERESULT(QueryResult);
			QueryResult = mysql_store_result(&myConn);
			SAFEFREERESULT(QueryResult);
			DWORD ListSize = 0;
			if(InsertResult == 0){
				ZeroMemory(QueryConsult, sizeof(QueryConsult));
				sprintf(QueryConsult, "DELETE FROM `magic` WHERE `CharID` = %lu;", CharID);
				if(ProcessQuery(&myConn, QueryConsult) == -1) return;
				QueryResult = mysql_store_result(&myConn);
				SAFEFREERESULT(QueryResult);

				//DeleteAllSkillsFromChar(CharID);
				ZeroMemory(QueryConsult, sizeof(QueryConsult));
				sprintf(QueryConsult, "DELETE FROM `skill` WHERE `CharID` = %lu;", CharID);
				if(ProcessQuery(&myConn, QueryConsult) == -1) return;
				QueryResult = mysql_store_result(&myConn);
				SAFEFREERESULT(QueryResult);

				DeleteAllItemsFromChar(AccountID, CharID, myConn, FALSE);
				wp = (WORD*)(Txt500+4);
				*wp = LOGRESMSGTYPE_CHARACTERDELETED;
				GetCharList(AccountName, Txt500+6, &ListSize, myConn);
            }
			else{
				wp = (WORD*)(Txt500+4);
				*wp = LOGRESMSGTYPE_NOTEXISTINGCHARACTER;
            }

			SendMsgToClient(ClientID, Txt500, ListSize+6);
       //} else CloseClientSocket(ClientID);
		SAFEFREERESULT(QueryResult);
}
//=============================================================================
DWORD CLoginServer::GetCharID(char *CharName, char *AccountName, MYSQL myConn)
{
	st_mysql_res    *pQueryResult = NULL;
	MYSQL_ROW       myRow;
	char QueryConsult[500], GoodAccName[25], GoodCharName[25];
	DWORD dwID;

	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	ZeroMemory(GoodAccName, sizeof(GoodAccName));
	MakeGoodName(GoodAccName, AccountName);
	ZeroMemory(GoodCharName, sizeof(GoodCharName));
	MakeGoodName(GoodCharName, CharName);
	sprintf(QueryConsult, "SELECT `CharID` FROM `char_database` WHERE `account_name` = '%s' AND `char_name` = '%s';", GoodAccName, GoodCharName);
	if(ProcessQuery(&myConn, QueryConsult) == -1) return 0;
	pQueryResult = mysql_store_result(&myConn);

	if(mysql_num_rows(pQueryResult) == 0){
		SAFEFREERESULT(pQueryResult);
		return 0;
	}
	myRow = mysql_fetch_row(pQueryResult);
	dwID = atoul(myRow[0]);
	SAFEFREERESULT(pQueryResult);
	return dwID;
}
//=============================================================================
void CLoginServer::ChangePassword(char *Data, WORD ClientID, MYSQL myConn)
{
 char AccName[15], GoodAccName[25], AccPass[15], GoodPass[25], NewPass1[15], NewPass2[15], Txt50[50], QueryConsult[500];
 DWORD *dwp;
 st_mysql_res    *pQueryResult = NULL;
          
        ZeroMemory(Txt50, sizeof(Txt50));
        dwp = (DWORD*)Txt50;
        *dwp = MSGID_RESPONSE_CHANGEPASSWORD;
        dwp = (DWORD*)(Txt50+4);
        SafeCopy(AccName, Data, 10);
        SafeCopy(AccPass, Data+10, 10);
        SafeCopy(NewPass1, Data+20, 10);
        SafeCopy(NewPass2, Data+30, 10);
        if(!IsSame(NewPass1, NewPass2) || strlen(NewPass1)>10) *dwp = LOGRESMSGTYPE_PASSWORDCHANGEFAIL;
        else
          {
           *dwp = LOGRESMSGTYPE_PASSWORDCHANGESUCCESS;
           ZeroMemory(QueryConsult, sizeof(QueryConsult));
		   ZeroMemory(GoodAccName, sizeof(GoodAccName));
		   MakeGoodName(GoodAccName, AccName);
		   ZeroMemory(GoodPass, sizeof(GoodPass));
		   MakeGoodName(GoodPass, NewPass1);
           sprintf(QueryConsult, "UPDATE `account_database` SET `password` = '%s' WHERE `name` = '%s' LIMIT 1;", GoodPass,  GoodAccName);
           if(ProcessQuery(&myConn, QueryConsult) == -1) return;
		   pQueryResult = mysql_store_result(&myConn);
		   SAFEFREERESULT(pQueryResult);
           SafeCopy(Txt50+6, NewPass1, strlen(NewPass1));
          }
        SendMsgToClient(ClientID, Txt50, 16);
}
//=============================================================================
void CLoginServer::DeleteAllItemsFromChar(WORD AccountID, WORD CharID, MYSQL myConn, BOOL bDeleteFromBank)
{
	char QueryConsult[150];
	st_mysql_res    *pQueryResult = NULL;

	ZeroMemory(QueryConsult, sizeof(QueryConsult));

	if(bDeleteFromBank) {
		sprintf(QueryConsult, "DELETE FROM `item` WHERE `AccountID` = %lu AND ItemPos = 'WH';", AccountID);
		if(ProcessQuery(&myConn, QueryConsult) == -1) return;
		pQueryResult = mysql_store_result(&myConn);
		SAFEFREERESULT(pQueryResult);
	}
	
	sprintf(QueryConsult, "DELETE FROM `item` WHERE `CharID` = %lu AND ItemPos = 'BAG';", CharID);
	if(ProcessQuery(&myConn, QueryConsult) == -1) return;
	pQueryResult = mysql_store_result(&myConn);
	SAFEFREERESULT(pQueryResult);
}
//=============================================================================
void CLoginServer::OptimizeTable(char *TableName)
{
 char QueryConsult[100];
 st_mysql_res    *pQueryResult = NULL;
 	 
		if(mySQLAutoFixProcess) return;

		ZeroMemory(QueryConsult, sizeof(QueryConsult));
        sprintf(QueryConsult, "OPTIMIZE TABLE `%s`;", TableName);
        if(ProcessQuery(&mySQL, QueryConsult) == -1) return;
		pQueryResult = mysql_store_result(&mySQL);
		SAFEFREERESULT(pQueryResult);
}
//=============================================================================
void CLoginServer::OptimizeDatabase(DWORD time)
{
        if(mySQLAutoFixProcess) return;

		mySQLdbOptimizeTimer = time;
        OptimizeTable("account_database");
        OptimizeTable("cash_transactions");
        OptimizeTable("char_database");
        OptimizeTable("guild");
        OptimizeTable("magic");
        OptimizeTable("item");
        OptimizeTable("skill");
		OptimizeTable("ipblocked");
}
//=============================================================================
void CLoginServer::RepairTable(char *TableName)
{
 char QueryConsult[100];
 st_mysql_res    *pQueryResult = NULL;
 	 
		if(mySQLAutoFixProcess) return;
		
        ZeroMemory(QueryConsult, sizeof(QueryConsult));
        sprintf(QueryConsult, "REPAIR TABLE `%s`;", TableName);
        if(ProcessQuery(&mySQL, QueryConsult) == -1) return;
		pQueryResult = mysql_store_result(&mySQL);
		SAFEFREERESULT(pQueryResult);
}
//=============================================================================
void CLoginServer::RepairDatabase(DWORD time)
{
        if(mySQLAutoFixProcess) return;

		mySQLdbRepairTimer = time;
        RepairTable("account_database");
        RepairTable("cash_transactions");
        RepairTable("char_database");
        RepairTable("guild");
        RepairTable("magic");
        RepairTable("item");
        RepairTable("skill");
		RepairTable("ipblocked");
}
//=============================================================================
void CLoginServer::OnTimer()
{
 DWORD dwTime, *dwp;
 WORD *wp;
 char log[300];

		if(IsOnCloseProcess || IsThreadMysqlBeingUsed) return;
		IsThreadMysqlBeingUsed = TRUE;
		
		MsgProcess();
		dwTime = timeGetTime();		   

		if(bServersBeingShutdown) ProcessShutdown(dwTime);
		else if(bIsF1pressed && bIsF4pressed){
			bServersBeingShutdown = TRUE;
			ProcessShutdown(dwTime);
		}

		if(bIsF1pressed && bIsF5pressed && !bConfigsUpdated){
			PutLogList("(*) Updating configuration files...", INFO_MSG);
			if(!bReadAllConfig()) PutLogList("(!!!) ERROR! Couldn't read configuration files!", WARN_MSG);
			else{
				PutLogList("(*) Done!", INFO_MSG);
				SendUpdatedConfigToAllServers();
			}
			bConfigsUpdated = TRUE;
		}
		
        if(mySQLAutoFixProcess){if(dwTime - mySQLTimer > MYSQL_AUTOFIX_TIMERSIZE) MysqlAutoFix();}
        else{
			if(!CheckServerStatus()){
				IsThreadMysqlBeingUsed = FALSE;
				return;
			}
			//possibal rollback fix
		/*   if((dwTime - mySQLdbOptimizeTimer) > MYSQL_DBOPTIMIZE_TIMERINTERVAL) OptimizeDatabase(dwTime);
           if((dwTime - mySQLdbRepairTimer) > MYSQL_DBREPAIR_TIMERINTERVAL) RepairDatabase(dwTime);*/
           if((dwTime - CheckAccountsTimer) > CHECKACCSTATUS_TIMERINTERVAL) CheckActiveAccountsNumber(dwTime);
		   for(WORD w = 0; w < MAXCLIENTS; w ++){
               if(Client[w] != NULL){
				   if(Client[w]->IsPlaying == FALSE && (dwTime - Client[w]->Time) > MAXWAITTIMEFORPLAYERENTERGAME){
					    if(Client[w]->ForceDisconnRequestTime == 0)	RequestForceDisconnect(Client[w], 10);
						else if((dwTime - Client[w]->ForceDisconnRequestTime) > MAX_FORCEDISCONN_WAIT_TIME){
							ZeroMemory(log, sizeof(log));
							sprintf(log, "(!) Client(%s) was deleted with no savedata due to no response from gameserver.", Client[w]->AccountName);
							PutLogList(log, WARN_MSG, TRUE, ERROR_LOGFILE);
							SAFEDELETE(Client[w]);
						}						
				   }
				   else if(Client[w]->IsPlaying){
					   if(GameServer[Client[w]->ConnectedServerID] == NULL) {SAFEDELETE(Client[w]);}
					   else if(Client[w]->ForceDisconnRequestTime != 0 && (dwTime - Client[w]->ForceDisconnRequestTime) > MAX_FORCEDISCONN_WAIT_TIME) {SAFEDELETE(Client[w]);}
				   }
			   }
               
           }
		   for(int w = 0; w < MAXGAMESERVERS; w++){
			   if(GameServer[w] != NULL){
				   if((GameServer[w]->AliveResponseTime < dwTime) && (dwTime - GameServer[w]->AliveResponseTime) > MAX_GSALIVE_WAITINTERVAL){
						ZeroMemory(log, sizeof(log));
						sprintf(log, "(!!!) There is no response from GameServer(%s)!", GameServer[w]->ServerName);
						PutLogList(log, WARN_MSG);
				   }
				   else if((dwTime - GameServer[w]->TotalPlayersResponse) > INTERVALTOSEND_TOTALPLAYERS){
						ZeroMemory(log, sizeof(log));
						dwp = (DWORD*)log;
						*dwp = MSGID_TOTALGAMESERVERCLIENTS;
						wp = (WORD*)(log+6);
						*wp = ActiveAccounts;
						GameServer[w]->SendMsg(log, 8, NULL, FALSE);
						GameServer[w]->TotalPlayersResponse = dwTime;
				   }

			   }
		   }

		}
		IsThreadMysqlBeingUsed = FALSE;
}
//=============================================================================
void CLoginServer::ProcessClientRequestEnterGame(char *Data, DWORD ClientID, MYSQL myConn)
{
	char MapName[15], SendBuff[100], AccName[15], GoodAccName[25], AccPwd[15], GoodCharName[25], CharName[15], QueryConsult[300];
	_ADDRESS GameServerIP, ClientIP;
	WORD *wp;
	DWORD *dwp, dwGetResponseTime;
	WORD GameServerPort, *wp2;
	BYTE *bp, GameServerID;
	st_mysql_res    *QueryResult = NULL;
	MYSQL_ROW       myRow;
	WORD AccountID;

	ZeroMemory(SendBuff, sizeof(SendBuff));

	dwp = (DWORD*)SendBuff;
	*dwp = MSGID_RESPONSE_ENTERGAME;
	wp2 = (WORD*)(SendBuff+4);

	ZeroMemory(AccName, sizeof(AccName));
	SafeCopy(AccName, Data, 10);
	ZeroMemory(AccPwd, sizeof(AccPwd));
	SafeCopy(AccPwd, Data+10, 10);
	ZeroMemory(CharName, sizeof(CharName));
	SafeCopy(CharName, Data+20, 10);
	ZeroMemory(GameServerIP, sizeof(GameServerIP));
	ZeroMemory(ClientIP, sizeof(ClientIP));
	ClientSocket[ClientID]->iGetPeerAddress(ClientIP);

	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	ZeroMemory(GoodAccName, sizeof(GoodAccName));
	MakeGoodName(GoodAccName, AccName);
	ZeroMemory(GoodCharName, sizeof(GoodCharName));
	MakeGoodName(GoodCharName, CharName);
	sprintf(QueryConsult, "SELECT `MapLoc` FROM `char_database` WHERE `account_name` = '%s' AND `char_name` = '%s' LIMIT 1;", GoodAccName, GoodCharName);
	if(ProcessQuery(&myConn, QueryConsult) == -1) return;
	QueryResult = mysql_store_result(&myConn);
	if(mysql_num_rows(QueryResult) == 0){
		SAFEDELETE(ClientSocket[ClientID]);
		SAFEFREERESULT(QueryResult);
		return;
	}
	else{
		myRow = mysql_fetch_row(QueryResult);
		SafeCopy(MapName, myRow[0]);
	}

	wp = (WORD*)(Data+30);
	switch(*wp)
	{
	case ENTERGAMEMSGTYPE_NEW:
		if(IsMapAvailable(MapName, GameServerIP, &GameServerPort, &GameServerID))
		{
			if(IsAccountInUse(AccName, &AccountID))
				*wp2 = ENTERGAMERESTYPE_PLAYING;
			else{
				for(WORD w = 0; w < MAXCLIENTS; w++) 
				{
					if(!Client[w])
					{
						Client[w] = new CClient(AccName, AccPwd, CharName, ClientIP, GameServerID);
						break;
					}
				}
				SendLoginConfirmation(AccName, AccPwd, CharName, ClientIP, GameServerID);
				*wp2 = ENTERGAMERESTYPE_CONFIRM;
				SafeCopy(SendBuff+6, GameServerIP);
				wp2 = (WORD*)(SendBuff+22);
				*wp2 = GameServerPort;
			}
		}
		else{
			*wp2 = ENTERGAMERESTYPE_REJECT;
			bp = (BYTE*)(SendBuff+6);
			*bp = REJECTTYPE_GAMESERVEROFFLINE;
		}
		SendMsgToClient((WORD)ClientID, SendBuff, 24);
		break;

	case ENTERGAMEMSGTYPE_NOENTER_FORCEDISCONN:
		if(IsAccountInUse(AccName, &AccountID) && IsSame(AccName, Client[AccountID]->AccountName)
			&& IsSame(AccPwd, Client[AccountID]->AccountPassword) /*&& Client[AccountID]->IsPlaying == TRUE*/)
		{
				if(IsMapAvailable(MapName, GameServerIP, &GameServerPort, &GameServerID)) RequestForceDisconnect(Client[AccountID], 10);
				else SAFEDELETE(Client[AccountID]);
		}
		//else {
		//	SAFEDELETE(ClientSocket[ClientID]);
		//}	
		ZeroMemory(SendBuff, sizeof(SendBuff));
		dwp = (DWORD*)SendBuff;
		*dwp = MSGID_RESPONSE_ENTERGAME;
		wp = (WORD*)(SendBuff+4);
		*wp = ENTERGAMERESTYPE_FORCEDISCONN;
		SendMsgToClient((WORD)ClientID, SendBuff, 7);
		break;

	case ENTERGAMEMSGTYPE_CHANGINGSERVER:
		if(IsAccountInUse(AccName, &AccountID)){
			dwGetResponseTime = timeGetTime();
			while(Client[AccountID] && Client[AccountID]->IsPlaying && ((timeGetTime() - dwGetResponseTime) < MAX_SERVERCHANGERESPONSE)) Delay(500);
		}
		else {
			SAFEDELETE(ClientSocket[ClientID]);
			return;
		}
		if(!Client[AccountID] || ((timeGetTime() - dwGetResponseTime) > MAX_SERVERCHANGERESPONSE)) {
			SAFEDELETE(ClientSocket[ClientID]);
			return;
		}


		if(IsSame(AccName, Client[AccountID]->AccountName)
			&& IsSame(AccPwd, Client[AccountID]->AccountPassword) && IsSame(ClientIP, Client[AccountID]->ClientIP)
			&& IsSame(CharName, Client[AccountID]->CharName) && Client[AccountID]->IsOnServerChange == TRUE)
		{
				if(IsMapAvailable(MapName, GameServerIP, &GameServerPort, &GameServerID)){
					Client[AccountID]->ConnectedServerID = GameServerID;
					Client[AccountID]->Time = timeGetTime();
					Client[AccountID]->IsOnServerChange = FALSE;
					Client[AccountID]->IsPlaying = TRUE;
					*wp2 = ENTERGAMERESTYPE_CONFIRM;
					SafeCopy(SendBuff+6, GameServerIP);
					wp2 = (WORD*)(SendBuff+22);
					*wp2 = GameServerPort;
				}
				else{
					*wp2 = ENTERGAMERESTYPE_REJECT;
					bp = (BYTE*)(SendBuff+6);
					*bp = REJECTTYPE_GAMESERVEROFFLINE;
					SAFEDELETE(Client[AccountID]);
				}
				SendMsgToClient((WORD)ClientID, SendBuff, 24);
		}
		else {SAFEDELETE(ClientSocket[ClientID]);}
		break;

	default:
		SAFEDELETE(ClientSocket[ClientID]);
		break;
	}
	SAFEFREERESULT(QueryResult);
}
//=============================================================================
BOOL CLoginServer::IsMapAvailable(char *MapName, char *GameServerIP, WORD *GameServerPort, BYTE *GSID)
{
        for(BYTE w=0; w<MAXGAMESERVERS; w++)
         if(GameServer[w] != NULL)
          for(BYTE b = 0; b < GameServer[w]->NumberOfMaps; b++)
           if(IsSame(GameServer[w]->MapName[b], MapName))
             {
              if(!GameServer[w]->IsSocketConnected || GameServer[w]->IsBeingClosed || !GameServer[w]->IsInitialized) return FALSE;
			  SafeCopy(GameServerIP, GameServer[w]->ServerIP);
              *GameServerPort = GameServer[w]->ServerPort;
              *GSID = w;
              return TRUE;
             }
        return FALSE;
}
//=============================================================================
void CLoginServer::ProcessRequestPlayerData(char *Data, BYTE GSID, MYSQL myConn)
{
	char SendBuff[20000], AccName[15], AccPwd[15], CharName[11], log[300];
	_ADDRESS ClientIP;
	WORD *wp, CharInfoSize = 0;
	DWORD *dwp;
	WORD AccountID;

	ZeroMemory(SendBuff, sizeof(SendBuff));
	dwp = (DWORD*)SendBuff;
	*dwp = MSGID_RESPONSE_PLAYERDATA;
	wp = (WORD*)(SendBuff+4);
	ZeroMemory(CharName, sizeof(CharName));
	SafeCopy(CharName, Data, 10);
	ZeroMemory(AccName, sizeof(AccName));
	SafeCopy(AccName, Data+10, 10);
	ZeroMemory(AccPwd, sizeof(AccPwd));
	SafeCopy(AccPwd, Data+20, 10);
	ZeroMemory(ClientIP, sizeof(ClientIP));
	SafeCopy(ClientIP, Data+30, 15);
	SafeCopy(SendBuff+6, CharName, strlen(CharName));
	//SendBuff+16 = m_cAccountStatus, not used on hgserver
	if(IsAccountInUse(AccName, &AccountID))
	{
		if(!IsSame(AccPwd, Client[AccountID]->AccountPassword) ||
			!IsSame(CharName, Client[AccountID]->CharName) ||
			Client[AccountID]->ConnectedServerID != GameServerSocket[GSID]->GSID)
		{
			*wp = LOGRESMSGTYPE_REJECT;
			ZeroMemory(log, sizeof(log));
			sprintf(log, "(!!!) Wrong data: Account(%s) pwd[%s/%s] charname[%s/%s] IP[%s/%s] GSID[%d/%d]", AccName, AccPwd, Client[AccountID]->AccountPassword, CharName, Client[AccountID]->CharName, ClientIP, Client[AccountID]->ClientIP, Client[AccountID]->ConnectedServerID, GameServerSocket[GSID]->GSID);
			PutLogList(log, WARN_MSG, TRUE, ERROR_LOGFILE);
			SAFEDELETE(Client[AccountID]);
		}
		/*      else if(!IsSame(Client[AccountID]->ClientIP, "127.0.0.1") && !IsSame(ClientIP, Client[AccountID]->ClientIP))
		{
		*wp = LOGRESMSGTYPE_REJECT;
		ZeroMemory(log, sizeof(log));
		sprintf(log, "(!!!) IP mismatch: Account(%s) IP[%s/%s]", AccName, ClientIP, Client[AccountID]->ClientIP);
		PutLogList(log, WARN_MSG, TRUE, ERROR_LOGFILE);
		SAFEDELETE(Client[AccountID]);
		}*/
		else
		{
			CharInfoSize = GetCharacterInfo(AccName, CharName, (SendBuff+16), myConn);
			if(CharInfoSize == 0)
			{
				*wp = LOGRESMSGTYPE_REJECT;
				SAFEDELETE(Client[AccountID]);
			}
			else{
				*wp = LOGRESMSGTYPE_CONFIRM;
				Client[AccountID]->IsOnServerChange = FALSE;
			}
		}
	}
	else{
		*wp = LOGRESMSGTYPE_REJECT;
		ZeroMemory(log, sizeof(log));
		sprintf(log, "(!) Character(%s) data error: account not initialized!", CharName);
		PutLogList(log, WARN_MSG, TRUE, ERROR_LOGFILE);
	}
	SendMsgToGS(GSID, SendBuff, 16+CharInfoSize);
}

//=============================================================================
BOOL CLoginServer::IsAccountInUse(char *AccountName, WORD *AccountID)
{
	for(WORD w = 0; w < MAXCLIENTS; w++)
	if(Client[w] != NULL && IsSame(Client[w]->AccountName, AccountName))
	{
		if(AccountID) *AccountID = w;
		return TRUE;
	}

	return FALSE;
}
//=============================================================================
WORD CLoginServer::GetCharacterInfo(char *AccountName2, char *CharName, char *Data, MYSQL myConn)
{
	char QueryConsult[300], CharProfile[260], AccountName[15], GoodCharName[25], log[300];
	WORD *wp, NRows, InfoSize, NBankItems;
	DWORD CharID, *dwp, GuildID;
	BYTE  NTitles, NItems, FISkillID, FISkillMastery, FISkillSSN, FIMagicMastery, FIMagicCharge, NFields, *bp, AdminUserLevel;
	MYSQL_FIELD *field[100];
	MYSQL_ROW myRow, MagicRow[MAXMAGICS], SkillRow[MAXSKILLS], ItemRow[MAXITEMS], BankItemRow[MAXBANKITEMS],TitleRow[MAXTITLES];
	st_mysql_res    *QueryResult = NULL;
	static long charIndexEnd = 462+176+20;
	WORD AccountID;
	WORD Index = 0;

	AccountID = GetAccountID(AccountName2, myConn);
	if (AccountID == 0) return 0;

	PutOffsetValue(Data, 200, BYTESIZE, 3);//lu_pool
	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	ZeroMemory(CharProfile, sizeof(CharProfile));
	ZeroMemory(GoodCharName, sizeof(GoodCharName));
	MakeGoodName(GoodCharName, CharName);
	sprintf(QueryConsult, "SELECT * FROM `char_database` WHERE `char_name` = '%s' LIMIT 1;", GoodCharName);
	if(ProcessQuery(&myConn, QueryConsult) == -1) return 0;
	QueryResult = mysql_store_result(&myConn);
	NFields = (BYTE)mysql_num_fields(QueryResult);
	NRows = (WORD)mysql_num_rows(QueryResult);
	if(NRows == 0){
		ZeroMemory(log, sizeof(log));
		sprintf(log, "(!) Character(%s) data error: character not found in the database!", CharName);
		PutLogList(log, WARN_MSG, TRUE, ERROR_LOGFILE);
		SAFEFREERESULT(QueryResult);
		return 0;
	}

	myRow = mysql_fetch_row(QueryResult);
	mysql_field_seek(QueryResult, 0);

	for(BYTE f = 0; f < NFields; f++)
	{
		field[f] = mysql_fetch_field(QueryResult);
		if(IsSame(field[f]->name, "MapLoc"))                SafeCopy(Data+1, myRow[f], strlen(myRow[f]));
		else if(IsSame(field[f]->name, "account_name"))     SafeCopy(AccountName, myRow[f]);
		else if(IsSame(field[f]->name, "LocX"))             PutOffsetValue(Data, 11, WORDSIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "LocY"))             PutOffsetValue(Data, 13, WORDSIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "Gender"))           PutOffsetValue(Data, 15, BYTESIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "Skin"))             PutOffsetValue(Data, 16, BYTESIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "HairStyle"))        PutOffsetValue(Data, 17, BYTESIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "HairColor"))        PutOffsetValue(Data, 18, BYTESIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "Underwear"))        PutOffsetValue(Data, 19, BYTESIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "GuildName")){
			bp = (BYTE*)(Data);
			SafeCopy(Data+20, myRow[f], strlen(myRow[f]));
			if(!GuildExists(myRow[f], &GuildID, myConn)) *bp = 0;
			else *bp = 1;				
		}
		else if(IsSame(field[f]->name, "GuildRank"))        PutOffsetValue(Data, 40, BYTESIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "HP"))               PutOffsetValue(Data, 41, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "Level"))            PutOffsetValue(Data, 45, WORDSIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "Strenght"))         PutOffsetValue(Data, 47, BYTESIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "Vitality"))         PutOffsetValue(Data, 48, BYTESIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "Dexterity"))        PutOffsetValue(Data, 49, BYTESIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "Intelligence"))     PutOffsetValue(Data, 50, BYTESIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "Magic"))            PutOffsetValue(Data, 51, BYTESIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "Agility"))         PutOffsetValue(Data, 52, BYTESIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "Luck"))             PutOffsetValue(Data, 53, BYTESIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "Exp"))              PutOffsetValue(Data, 54, DWORDSIZE, atoul(myRow[f]));
		//else if(IsSame(field[f]->name, "MagicMastery"))     SafeCopy(Data+58, myRow[f], strlen(myRow[f]));
		else if(IsSame(field[f]->name, "Nation"))           SafeCopy(Data+182+176, myRow[f], strlen(myRow[f]));
		else if(IsSame(field[f]->name, "MP"))               PutOffsetValue(Data, 192+176, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "SP"))               PutOffsetValue(Data, 196+176, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "EK"))               PutOffsetValue(Data, 201+176, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "PK"))               PutOffsetValue(Data, 205+176, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "RewardGold"))       PutOffsetValue(Data, 209+176, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "Hunger"))           PutOffsetValue(Data, 313+176+20, BYTESIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "AdminLevel")){
			AdminUserLevel = (BYTE)atoi(myRow[f]);
			PutOffsetValue(Data, 314+176+20, BYTESIZE, AdminUserLevel);
		}
		else if(IsSame(field[f]->name, "LeftShutupTime"))   PutOffsetValue(Data, 315+176+20, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "LeftPopTime"))      PutOffsetValue(Data, 319+176+20, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "Popularity"))       PutOffsetValue(Data, 323+176+20, DWORDSIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "GuildID"))          PutOffsetValue(Data, 327+176+20, WORDSIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "DownSkillID"))      PutOffsetValue(Data, 331+176+20, BYTESIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "CharID"))
		{
			CharID = atoi(myRow[f]);
			PutOffsetValue(Data, 332+176+20, DWORDSIZE, CharID);
		}
		else if(IsSame(field[f]->name, "ID1"))              PutOffsetValue(Data, 336+176+20, DWORDSIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "ID2"))              PutOffsetValue(Data, 340+176+20, DWORDSIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "ID3"))              PutOffsetValue(Data, 344+176+20, DWORDSIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "BlockDate"))        SafeCopy(Data+348+176+20, myRow[f], strlen(myRow[f]));
		else if(IsSame(field[f]->name, "QuestNum"))         PutOffsetValue(Data, 368+176+20, WORDSIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "QuestCount"))       PutOffsetValue(Data, 370+176+20, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "QuestRewType"))     PutOffsetValue(Data, 374+176+20, WORDSIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "QuestRewAmmount"))  PutOffsetValue(Data, 376+176+20, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "Contribution"))     PutOffsetValue(Data, 380+176+20, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "QuestID"))          PutOffsetValue(Data, 384+176+20, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "QuestCompleted"))   PutOffsetValue(Data, 388+176+20, BYTESIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "LeftForceRecallTime")) PutOffsetValue(Data, 389+176+20, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "LeftFirmStaminarTime")) PutOffsetValue(Data, 393+176+20, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "EventID"))          PutOffsetValue(Data, 397+176+20, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "LeftSAC"))          PutOffsetValue(Data, 401+176+20, WORDSIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "FightNum"))         PutOffsetValue(Data, 403+176+20, BYTESIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "FightDate"))        PutOffsetValue(Data, 404+176+20, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "FightTicket"))      PutOffsetValue(Data, 408+176+20, BYTESIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "LeftSpecTime"))     PutOffsetValue(Data, 409+176+20, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "WarCon"))           PutOffsetValue(Data, 413+176+20, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "LockMapName"))      SafeCopy(Data+417+176+20, myRow[f], strlen(myRow[f]));
		else if(IsSame(field[f]->name, "LockMapTime"))      PutOffsetValue(Data, 427+176+20, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "CruJob"))           PutOffsetValue(Data, 431+176+20, BYTESIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "CruConstructPoint"))PutOffsetValue(Data, 432+176+20, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "CruID"))            PutOffsetValue(Data, 436+176+20, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "LeftDeadPenaltyTime"))PutOffsetValue(Data, 440+176+20, DWORDSIZE, atoul(myRow[f]));
		//else if(IsSame(field[f]->name, "PartyID"))			PutOffsetValue(Data, 444+176+20, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "ContPts"))			PutOffsetValue(Data, 444+176+20, DWORDSIZE, atoul(myRow[f]));
		//else if(IsSame(field[f]->name, "LegionCoins"))		PutOffsetValue(Data, 446+176+20, WORDSIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "DeathCount"))		PutOffsetValue(Data, 448+176+20, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "elo"))				PutOffsetValue(Data, 452+176+20, WORDSIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "MaxEK"))            PutOffsetValue(Data, 454+176+20, DWORDSIZE, atoul(myRow[f]));
		else if(IsSame(field[f]->name, "Title"))			PutOffsetValue(Data, 458+176+20, DWORDSIZE, atoi(myRow[f]));
		else if(IsSame(field[f]->name, "Profile"))          SafeCopy(CharProfile, myRow[f]);
	}
	if(CharID == NULL){
		ZeroMemory(log, sizeof(log));
		sprintf(log, "(!) Character(%s) data error: CharID is null!", CharName);
		PutLogList(log, WARN_MSG, TRUE, ERROR_LOGFILE);
		SAFEFREERESULT(QueryResult);
		return 0;
	}
	if(AdminUserLevel > 0){
		if(!IsGMAccount(AccountName, myConn)){
			char log[200];
			ZeroMemory(log, sizeof(log));
			sprintf(log, "(!!!) Character(%s) tries to enter in game as GM in a non-GM account!!!", CharName);
			PutLogList(log, WARN_MSG, TRUE, HACK_LOGFILE);
			SAFEFREERESULT(QueryResult);
			return 0;
		}
	}
	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	sprintf(QueryConsult, "SELECT `MagicMastery`, `Charge` FROM `magic` WHERE `CharID` = %lu ORDER BY 'MagicID' ASC LIMIT %u;", CharID, MAXMAGICS);
	if(ProcessQuery(&myConn, QueryConsult) == -1) return 0; 
	SAFEFREERESULT(QueryResult);
	QueryResult = mysql_store_result(&myConn);
	NFields = (BYTE)mysql_num_fields(QueryResult);
	NRows = (WORD)mysql_num_rows(QueryResult);
	if(NRows < MAXMAGICS){
		if (NRows == 0) {
			for(int i = 0; i < MAXMAGICS; i++) {
				ZeroMemory(QueryConsult, sizeof(QueryConsult));
				sprintf(QueryConsult, "INSERT INTO `magic` ( `CharID` , `MagicID` , `MagicMastery`, `Charge` ) VALUES (%lu, %d, 0, 0);", CharID, i);
				if(ProcessQuery(&myConn, QueryConsult) == -1) return 0; 
				SAFEFREERESULT(QueryResult);
				QueryResult = mysql_store_result(&myConn);
			}
		}
		ZeroMemory(log, sizeof(log));
		sprintf(log, "(!) Character(%s) data error: number of magics(%d) don't match!", CharName, NRows);
		PutLogList(log, WARN_MSG, TRUE, ERROR_LOGFILE);
		SAFEFREERESULT(QueryResult);
		return 0;
	}
	mysql_field_seek(QueryResult, 0);
	for(BYTE b = 0; b < NFields; b++){
		field[b] = mysql_fetch_field(QueryResult);
		if(IsSame(field[b]->name, "MagicMastery")) FIMagicMastery = b;
		else if(IsSame(field[b]->name, "Charge")) FIMagicCharge = b;
	}
	for(int b = 0; b < NRows; b++){
		MagicRow[b] = mysql_fetch_row(QueryResult);
		PutOffsetValue(Data, 58+(b*3), BYTESIZE, atoi(MagicRow[b][FIMagicMastery]));
		PutOffsetValue(Data, 59+(b*3), WORDSIZE, atoi(MagicRow[b][FIMagicCharge]));
		/*MagicRow[b] = mysql_fetch_row(QueryResult);
		bp = (BYTE*)(Data+57+ atoi(MagicRow[b][FIMagicMastery]));
		*bp = (BYTE)atoi(MagicRow[b][FISkillMastery]);
		wp = (WORD*)(Data+58+ (atoi(MagicRow[b][FIMagicMastery])*2));
		*wp = atoi(MagicRow[b][FISkillSSN]);*/
	}
	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	sprintf(QueryConsult, "SELECT `SkillID`, `SkillMastery`, `SkillSSN` FROM `skill` WHERE `CharID` = %lu ORDER BY 'SkillID' ASC LIMIT %u;", CharID, MAXSKILLS);
	if(ProcessQuery(&myConn, QueryConsult) == -1) return 0; 
	SAFEFREERESULT(QueryResult);
	QueryResult = mysql_store_result(&myConn);
	NFields = (BYTE)mysql_num_fields(QueryResult);
	NRows = (WORD)mysql_num_rows(QueryResult);
	if(NRows < MAXSKILLS){
		if (NRows == 0) {
			for(int i = 0; i < MAXSKILLS; i++) {
				ZeroMemory(QueryConsult, sizeof(QueryConsult));
				sprintf(QueryConsult, "INSERT INTO `skill` ( `CharID` , `SkillID` , `SkillMastery`, `SkillSSN` ) VALUES (%lu, %d, 0, 0);", CharID, i);
				if(ProcessQuery(&myConn, QueryConsult) == -1) return 0; 
				SAFEFREERESULT(QueryResult);
				QueryResult = mysql_store_result(&myConn);
			}
		}
		ZeroMemory(log, sizeof(log));
		sprintf(log, "(!) Character(%s) data error: number of skills(%d) don't match!", CharName, NRows);
		PutLogList(log, WARN_MSG, TRUE, ERROR_LOGFILE);
		SAFEFREERESULT(QueryResult);
		return 0;
	}
	mysql_field_seek(QueryResult, 0);
	for(BYTE b = 0; b < NFields; b++){
		field[b] = mysql_fetch_field(QueryResult);
		if(IsSame(field[b]->name, "SkillID")) FISkillID = b;
		else if(IsSame(field[b]->name, "SkillMastery")) FISkillMastery = b;
		else if(IsSame(field[b]->name, "SkillSSN")) FISkillSSN = b;
	}
	for(int b = 0; b < NRows; b++){
		SkillRow[b] = mysql_fetch_row(QueryResult);
		PutOffsetValue(Data, 213+176+(b*5), BYTESIZE, atoi(SkillRow[b][FISkillMastery]));
		PutOffsetValue(Data, 214+176+(b*5), DWORDSIZE, atoi(SkillRow[b][FISkillSSN]));
	}

	Index += charIndexEnd;

	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	sprintf(QueryConsult, "SELECT * FROM `item` WHERE `CharID` = %lu AND `ItemPos` = 'BAG' LIMIT %u;", CharID, MAXITEMS);
	if(ProcessQuery(&myConn, QueryConsult) == -1) return 0;
	SAFEFREERESULT(QueryResult);
	QueryResult = mysql_store_result(&myConn);
	NFields = (BYTE)mysql_num_fields(QueryResult);
	NRows = (WORD)mysql_num_rows(QueryResult);
	NItems = (BYTE)NRows;
	PutOffsetValue(Data, Index, BYTESIZE, NItems);
	if(NItems > 0)
	{
		BYTE FIItemName, FIItemCount, FIItemType, FIID1, FIID2, FIID3, FIItemColor;
		BYTE FIItemEffect1, FIItemEffect2, FIItemEffect3, FIItemLifeSpan, FIItemAttribute, FITempItemAttr, FISocket1, FISocket2, FISocket3;
		BYTE FIItemEquip, FIItemPosX, FIItemPosY, FIItemID;
		mysql_field_seek(QueryResult, 0);
		for(BYTE b = 0; b < NFields; b++){
			field[b] = mysql_fetch_field(QueryResult);
			if(IsSame(field[b]->name, "ItemName")) FIItemName = b;
			else if(IsSame(field[b]->name, "Count")) FIItemCount = b;
			else if(IsSame(field[b]->name, "ItemType")) FIItemType = b;
			else if(IsSame(field[b]->name, "ID1")) FIID1 = b;
			else if(IsSame(field[b]->name, "ID2")) FIID2 = b;
			else if(IsSame(field[b]->name, "ID3")) FIID3 = b;
			else if(IsSame(field[b]->name, "Color")) FIItemColor = b;
			else if(IsSame(field[b]->name, "Effect1")) FIItemEffect1 = b;
			else if(IsSame(field[b]->name, "Effect2")) FIItemEffect2 = b;
			else if(IsSame(field[b]->name, "Effect3")) FIItemEffect3 = b;
			else if(IsSame(field[b]->name, "LifeSpan")) FIItemLifeSpan = b;
			else if(IsSame(field[b]->name, "Attribute")) FIItemAttribute = b;
			else if(IsSame(field[b]->name, "TempItemAttr")) FITempItemAttr = b;
			else if(IsSame(field[b]->name, "Socket1")) FISocket1 = b;
			else if(IsSame(field[b]->name, "Socket2")) FISocket2 = b;
			else if(IsSame(field[b]->name, "Socket3")) FISocket3 = b;
			else if(IsSame(field[b]->name, "ItemEquip")) FIItemEquip = b;
			else if(IsSame(field[b]->name, "ItemPosX"))  FIItemPosX = b;
			else if(IsSame(field[b]->name, "ItemPosY"))  FIItemPosY = b;
			else if(IsSame(field[b]->name, "ItemID"))  FIItemID = b;
		}
		for(int b = 0; b < NItems; b++)
		{
			WORD IndexForItem = (Index+1 + (b*71));
			ItemRow[b] = mysql_fetch_row(QueryResult);
			SafeCopy((Data + IndexForItem), ItemRow[b][FIItemName], strlen(ItemRow[b][FIItemName]));
			PutOffsetValue(Data, IndexForItem +20, DWORDSIZE, atoul(ItemRow[b][FIItemCount]));
			PutOffsetValue(Data, IndexForItem +24, WORDSIZE, atoi(ItemRow[b][FIItemType]));
			PutOffsetValue(Data, IndexForItem +26, DWORDSIZE, atoi(ItemRow[b][FIID1]));
			PutOffsetValue(Data, IndexForItem +30, DWORDSIZE, atoi(ItemRow[b][FIID2]));
			PutOffsetValue(Data, IndexForItem +34, DWORDSIZE, atoi(ItemRow[b][FIID3]));
			PutOffsetValue(Data, IndexForItem +38, BYTESIZE, atoi(ItemRow[b][FIItemColor]));
			PutOffsetValue(Data, IndexForItem +39, WORDSIZE, atoi(ItemRow[b][FIItemEffect1]));
			PutOffsetValue(Data, IndexForItem +41, WORDSIZE, atoi(ItemRow[b][FIItemEffect2]));
			PutOffsetValue(Data, IndexForItem +43, WORDSIZE, atoi(ItemRow[b][FIItemEffect3]));
			PutOffsetValue(Data, IndexForItem +45, WORDSIZE, atoi(ItemRow[b][FIItemLifeSpan]));
			PutOffsetValue(Data, IndexForItem +47, DWORDSIZE, atoul(ItemRow[b][FIItemAttribute]));
			PutOffsetValue(Data, IndexForItem +51, DWORDSIZE, atoul(ItemRow[b][FITempItemAttr]));
			if(ItemRow[b][FISocket1]) PutOffsetValue(Data, IndexForItem +55, BYTESIZE, atoul(ItemRow[b][FISocket1]));
			else PutOffsetValue(Data, IndexForItem +55, BYTESIZE, SG_NONE);
			if(ItemRow[b][FISocket2]) PutOffsetValue(Data, IndexForItem +56, BYTESIZE, atoul(ItemRow[b][FISocket2]));
			else PutOffsetValue(Data, IndexForItem +56, BYTESIZE, SG_NONE);
			if(ItemRow[b][FISocket3]) PutOffsetValue(Data, IndexForItem +57, BYTESIZE, atoul(ItemRow[b][FISocket3]));
			else PutOffsetValue(Data, IndexForItem +57, BYTESIZE, SG_NONE);
			PutOffsetValue(Data, IndexForItem +58, BYTESIZE, atoi(ItemRow[b][FIItemEquip]));
			PutOffsetValue(Data, IndexForItem +59, WORDSIZE, atoi(ItemRow[b][FIItemPosX]));
			PutOffsetValue(Data, IndexForItem +61, WORDSIZE, atoi(ItemRow[b][FIItemPosY]));
			PutOffsetValue(Data, IndexForItem +63, I64SIZE, atoull(ItemRow[b][FIItemID]));
		}
	}
	Index += 1+(NItems*71);
	//WORD IndexForItem = (charIndexEnd+1 + (NItems*71));

	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	sprintf(QueryConsult, "SELECT * FROM `item` WHERE `AccountID` = %lu AND `ItemPos` = 'WH'  ORDER BY `ItemName` ASC LIMIT %u;", AccountID, MAXBANKITEMS);
	if(ProcessQuery(&myConn, QueryConsult) == -1) return 0;
	SAFEFREERESULT(QueryResult);
	QueryResult = mysql_store_result(&myConn);
	NFields = (BYTE)mysql_num_fields(QueryResult);
	NBankItems = (WORD)mysql_num_rows(QueryResult);
	PutOffsetValue(Data, Index, WORDSIZE, NBankItems);
	if(NBankItems > 0){
		BYTE FIBItemName, FIBItemCount, FIBItemType, FIBID1, FIBID2, FIBID3, FIBItemColor, FIBItemID;
		BYTE FIBItemEffect1, FIBItemEffect2, FIBItemEffect3, FIBItemLifeSpan, FIBItemAttribute, FIBTempItemAttr, FISocket1, FISocket2, FISocket3;
		mysql_field_seek(QueryResult, 0);
		for(BYTE b = 0; b < NFields; b++){
			field[b] = mysql_fetch_field(QueryResult);
			if(IsSame(field[b]->name, "ItemName")) FIBItemName = b;
			else if(IsSame(field[b]->name, "Count")) FIBItemCount = b;
			else if(IsSame(field[b]->name, "ItemType")) FIBItemType = b;
			else if(IsSame(field[b]->name, "ID1")) FIBID1 = b;
			else if(IsSame(field[b]->name, "ID2")) FIBID2 = b;
			else if(IsSame(field[b]->name, "ID3")) FIBID3 = b;
			else if(IsSame(field[b]->name, "Color")) FIBItemColor = b;
			else if(IsSame(field[b]->name, "Effect1")) FIBItemEffect1 = b;
			else if(IsSame(field[b]->name, "Effect2")) FIBItemEffect2 = b;
			else if(IsSame(field[b]->name, "Effect3")) FIBItemEffect3 = b;
			else if(IsSame(field[b]->name, "LifeSpan")) FIBItemLifeSpan = b;
			else if(IsSame(field[b]->name, "Attribute")) FIBItemAttribute = b;
			else if(IsSame(field[b]->name, "TempItemAttr")) FIBTempItemAttr = b;
			else if(IsSame(field[b]->name, "Socket1")) FISocket1 = b;
			else if(IsSame(field[b]->name, "Socket2")) FISocket2 = b;
			else if(IsSame(field[b]->name, "Socket3")) FISocket3 = b;
			else if(IsSame(field[b]->name, "ItemID")) FIBItemID = b;
		}
		for(int b = 0; b < NBankItems; b++){
			WORD BankItemIndex = Index+2 + (b*66);

			BankItemRow[b] = mysql_fetch_row(QueryResult);
			SafeCopy((Data + BankItemIndex), BankItemRow[b][FIBItemName], strlen(BankItemRow[b][FIBItemName]));

			PutOffsetValue(Data, BankItemIndex +20, DWORDSIZE, atoul(BankItemRow[b][FIBItemCount]));
			PutOffsetValue(Data, BankItemIndex +24, WORDSIZE, atoi(BankItemRow[b][FIBItemType]));
			PutOffsetValue(Data, BankItemIndex +26, DWORDSIZE, atoi(BankItemRow[b][FIBID1]));
			PutOffsetValue(Data, BankItemIndex +30, DWORDSIZE, atoi(BankItemRow[b][FIBID2]));
			PutOffsetValue(Data, BankItemIndex +34, DWORDSIZE, atoi(BankItemRow[b][FIBID3]));
			PutOffsetValue(Data, BankItemIndex +38, BYTESIZE, atoi(BankItemRow[b][FIBItemColor]));
			PutOffsetValue(Data, BankItemIndex +39, WORDSIZE, atoi(BankItemRow[b][FIBItemEffect1]));
			PutOffsetValue(Data, BankItemIndex +41, WORDSIZE, atoi(BankItemRow[b][FIBItemEffect2]));
			PutOffsetValue(Data, BankItemIndex +43, WORDSIZE, atoi(BankItemRow[b][FIBItemEffect3]));
			PutOffsetValue(Data, BankItemIndex +45, WORDSIZE, atoi(BankItemRow[b][FIBItemLifeSpan]));
			PutOffsetValue(Data, BankItemIndex +47, DWORDSIZE, atoul(BankItemRow[b][FIBItemAttribute]));
			PutOffsetValue(Data, BankItemIndex +51, DWORDSIZE, atoul(BankItemRow[b][FIBTempItemAttr]));
			if(BankItemRow[b][FISocket1]) PutOffsetValue(Data, BankItemIndex +55, BYTESIZE, atoul(BankItemRow[b][FISocket1]));
			else PutOffsetValue(Data, BankItemIndex +55, BYTESIZE, SG_NONE);
			if(BankItemRow[b][FISocket2]) PutOffsetValue(Data, BankItemIndex +56, BYTESIZE, atoul(BankItemRow[b][FISocket2]));
			else PutOffsetValue(Data, BankItemIndex +56, BYTESIZE, SG_NONE);
			if(BankItemRow[b][FISocket3]) PutOffsetValue(Data, BankItemIndex +57, BYTESIZE, atoul(BankItemRow[b][FISocket3]));
			else PutOffsetValue(Data, BankItemIndex +57, BYTESIZE, SG_NONE);
			PutOffsetValue(Data, BankItemIndex +58, DWORDSIZE, atoull(BankItemRow[b][FIBItemID]));
		}
	}
	Index += 2+(NBankItems*66);
	//WORD BankItemIndex = IndexForItem+2 + (NBankItems*66);
	
	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	sprintf(QueryConsult, "SELECT * FROM `title` WHERE `CharID` = %lu LIMIT %u;", CharID, MAXTITLES);
	if(ProcessQuery(&myConn, QueryConsult) == -1) return 0;
	SAFEFREERESULT(QueryResult);
	QueryResult = mysql_store_result(&myConn);
	NFields = (BYTE)mysql_num_fields(QueryResult);
	NRows = (WORD)mysql_num_rows(QueryResult);
	NTitles = (BYTE)NRows;
	PutOffsetValue(Data, Index, BYTESIZE, NTitles);
	if(NTitles > 0)
	{
		BYTE FTITLE, FUSES;
		mysql_field_seek(QueryResult, 0);
		for(BYTE b = 0; b < NFields; b++){
			field[b] = mysql_fetch_field(QueryResult);
			if(IsSame(field[b]->name, "Title")) FTITLE = b;
			else if(IsSame(field[b]->name, "Uses")) FUSES = b;
		}

		for(int b = 0; b < NTitles; b++)
		{
			WORD IndexForTitle = (Index+1 + (b*5));
			TitleRow[b] = mysql_fetch_row(QueryResult);

			PutOffsetValue(Data, IndexForTitle, BYTESIZE, atoi(TitleRow[b][FTITLE]));
			PutOffsetValue(Data, IndexForTitle +1, DWORDSIZE, atoi(TitleRow[b][FUSES]));
		}
	}
	Index += 1+(NTitles*5);
	//WORD IndexForTitle = (BankItemIndex+1 + (NTitles*5));

	if(strlen(CharProfile) == 0){
		SafeCopy(Data+Index, "__________");
		SAFEFREERESULT(QueryResult);
		return (WORD)(Index + strlen("__________"));
	}
	else{
		SafeCopy(Data+Index, CharProfile, strlen(CharProfile));
		SAFEFREERESULT(QueryResult);
		return (WORD)(Index + strlen(CharProfile)+2);
	}
}
//=============================================================================
void CLoginServer::CheckActiveAccountsNumber(DWORD time)
{
	st_mysql_res *QueryResult = NULL;
	char QueryConsult[100];
	MYSQL_FIELD *field[15];
	MYSQL_ROW myRow;
	BYTE b;

	if(mySQLAutoFixProcess) return;

	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	SafeCopy(QueryConsult, "SHOW TABLE STATUS FROM `playerdb2` LIKE 'account_database';");
	if(ProcessQuery(&mySQL, QueryConsult) == -1) return;
	QueryResult = mysql_store_result(&mySQL);

	if(mysql_num_rows(QueryResult) == 0){
		SAFEFREERESULT(QueryResult);
		return;
	}

	myRow = mysql_fetch_row(QueryResult);
	mysql_field_seek(QueryResult, 0);
	for(b = 0; b < 15; b++){
		field[b] = mysql_fetch_field(QueryResult);
		if(IsSame(field[b]->name, "Rows")){
			TotalAccounts = atoi(myRow[b]);
			break;
		}
	}
	ActiveAccounts = 0;
	for(WORD w = 0; w < MAXCLIENTS; w++) if(Client[w] != NULL) ActiveAccounts++;
	CheckAccountsTimer = time;
	if(ActiveAccounts > PeakPeopleOnline) PeakPeopleOnline = ActiveAccounts;
	SAFEFREERESULT(QueryResult);
}
//=============================================================================
void CLoginServer::OnKeyDown(WPARAM wParam)
{
    switch (wParam) {

	case VK_F1:
		bIsF1pressed = TRUE;
	break;
	
	case VK_F4:	
		bIsF4pressed = TRUE;
	break;
	
	case VK_F5:
		bIsF5pressed = TRUE;
	break;
	}
}
//=============================================================================
void CLoginServer::OnKeyUp(WPARAM wParam)
{
 	switch (wParam) {
        case VK_F1:
			bIsF1pressed = FALSE;
			bConfigsUpdated = FALSE;
		break;
	
		case VK_F4:	
			bIsF4pressed = FALSE;
		break;

		case VK_F5:
			bIsF5pressed = FALSE;
			bConfigsUpdated = FALSE;
		break;
        }
}
//=============================================================================
void CLoginServer::ProcessClientLogout(char *Data, BOOL save, BYTE GSID, MYSQL myConn)
{
 char AccName[15], AccPwd[15], CharName[15], LogTxt[200], SendBuff[50];
 BOOL CountLogout;
 WORD AccountID;

 ZeroMemory(CharName, sizeof(CharName));
 SafeCopy(CharName, Data, 10);
 ZeroMemory(AccName, sizeof(AccName));
 SafeCopy(AccName, Data+10, 10);
 ZeroMemory(AccPwd, sizeof(AccPwd));
 SafeCopy(AccPwd, Data+20, 10);

 if(!strlen(CharName) || !strlen(AccName) || !strlen(AccPwd)) return;

 CountLogout = (BOOL)Data[30];//tell us if the player has logged out(true) or is changing servers(false)?
 if(IsAccountInUse(AccName, &AccountID) && IsSame(AccName, Client[AccountID]->AccountName)
	 && IsSame(AccPwd, Client[AccountID]->AccountPassword) && IsSame(CharName, Client[AccountID]->CharName) )
 {
	 ZeroMemory(LogTxt, sizeof(LogTxt));
	 if(save){
		 SaveCharacter(Data, myConn);
		 sprintf(LogTxt, "(!) Player(%s)[ID:%d] data saved properly.", CharName, AccountID);
		 PutLogList(LogTxt);
	 }
	 else{
		 sprintf(LogTxt, "(!) Player(%s)[ID:%d] logout with no save.", CharName, AccountID);
		 PutLogList(LogTxt);
	 }
	 if(CountLogout) {SAFEDELETE(Client[AccountID]);}
	 else{  
		 Client[AccountID]->IsOnServerChange = TRUE;
		 Client[AccountID]->Time = timeGetTime();
		 ZeroMemory(SendBuff, sizeof(SendBuff));
		 PutOffsetValue(SendBuff, 0, DWORDSIZE, MSGID_RESPONSE_SAVEPLAYERDATA_REPLY);
		 SafeCopy(SendBuff+6, CharName);
		 SendMsgToGS(GSID, SendBuff, strlen(CharName)+6);
	 }
 }
 else{
	 ZeroMemory(LogTxt, sizeof(LogTxt));
	 sprintf(LogTxt, "(!) Server change data-error for character(%s)!", CharName);
	 PutLogList(LogTxt, WARN_MSG, TRUE, ERROR_LOGFILE);
	 ZeroMemory(LogTxt, sizeof(LogTxt));
	 if(!IsAccountInUse(AccName, &AccountID)) PutLogList("(!) Account not even in use by the system. (deleted?)", WARN_MSG, TRUE, ERROR_LOGFILE);
	 else {
		 sprintf(LogTxt, "(!) CharName[%s][%s], AccName[%s][%s], AccPwd[%s][%s]", CharName, Client[AccountID]->CharName, AccName, Client[AccountID]->AccountName, AccPwd, Client[AccountID]->AccountPassword);
		 PutLogList(LogTxt, WARN_MSG, TRUE, ERROR_LOGFILE);
	 }
 }
}
//=============================================================================
void CLoginServer::ConfirmCharEnterGame(char *Data, BYTE GSID)
{
 char AccName[15], AccPwd[15], ServerName[15], log[300];
 _ADDRESS ClientIP;
 WORD AccountID;

        ZeroMemory(AccName, sizeof(AccName));
        SafeCopy(AccName, Data, 10);
        ZeroMemory(AccPwd, sizeof(AccPwd));
        SafeCopy(AccPwd, Data+10, 10);
        ZeroMemory(ServerName, sizeof(ServerName));
        SafeCopy(ServerName, Data+20, 10);
        ZeroMemory(ClientIP, sizeof(ClientIP));
        SafeCopy(ClientIP, Data+30, 16);
        if(IsAccountInUse(AccName, &AccountID))
          {
           if(!IsSame(AccPwd, Client[AccountID]->AccountPassword) || Client[AccountID]->ConnectedServerID != GameServerSocket[GSID]->GSID)
              {
               ZeroMemory(log, sizeof(log));
               sprintf(log, "(!!!) Wrong data: Account(%s) pwd[%s/%s] IP[%s/%s] GSID[%d/%d]", AccName, AccPwd, Client[AccountID]->AccountPassword, ClientIP, Client[AccountID]->ClientIP, Client[AccountID]->ConnectedServerID, GameServerSocket[GSID]->GSID);
               PutLogList(log, WARN_MSG, TRUE, ERROR_LOGFILE);
               RequestForceDisconnect(Client[AccountID], 10);
              }
         /*  else if(!IsSame(Client[AccountID]->ClientIP, "127.0.0.1") && !IsSame(ClientIP, Client[AccountID]->ClientIP))
              {
               ZeroMemory(log, sizeof(log));
               sprintf(log, "(!!!) Client IP mismatch: Account(%s) IP[%s/%s]", AccName, ClientIP, Client[AccountID]->ClientIP);
               PutLogList(log, WARN_MSG, TRUE, ERROR_LOGFILE);
               RequestForceDisconnect(Client[AccountID], 10);
              }*/
           else
              {
               Client[AccountID]->IsPlaying = TRUE;
					Client[AccountID]->Time = timeGetTime();
					ZeroMemory(log, sizeof(log));
               sprintf(log, "(!) Set character(%s)[ID:%d] status: playing.", Client[AccountID]->CharName, AccountID);
               PutLogList(log);               
              }
          }
}
//=============================================================================
BOOL CLoginServer::IsGMAccount(char *AccountName, MYSQL myConn)
{
	char QueryConsult[200], GoodAccName[25];
	MYSQL_ROW Row;
	st_mysql_res *QueryResult = NULL; 
	int InsertResult;

	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	ZeroMemory(GoodAccName, sizeof(GoodAccName));
	MakeGoodName(GoodAccName, AccountName);
	sprintf(QueryConsult, "SELECT `IsGMAccount` FROM `account_database` WHERE `name` = '%s' LIMIT 1;", GoodAccName);
	InsertResult = ProcessQuery(&myConn, QueryConsult);
	if(InsertResult == -1) return FALSE;
	QueryResult = mysql_store_result(&myConn);
	if(InsertResult == 0)
	{
		if(mysql_num_rows(QueryResult) == 0){
			SAFEFREERESULT(QueryResult);
			return FALSE;
		}
		Row = mysql_fetch_row(QueryResult);
		if(atoi(Row[0]) > 0){
			SAFEFREERESULT(QueryResult);
			return TRUE;
		}
	}
	SAFEFREERESULT(QueryResult);
	return FALSE;
}
//=============================================================================
inline void CLoginServer::SaveCharacter(char* Data, MYSQL myConn)
{
	char   *cp, AccName[15], AccPwd[15], CharName[15], Location[15], MapName[15], BlockDate[25], SaveDate[25],
		GuildName[25], GoodGuildName[50], LockedMapName[15], Profile[260], GoodProfile[520], QueryConsult[10000],cTemp22[255];
	sBYTE  DownSkillIndex, MagicMastery;
	BYTE   STR, DEX, VIT, INT, MAG, AGI, Luck, Sex, Skin, HairStyle,
		HairColor, Underwear, HungerStatus, FightzoneNumber,
		FightzoneTicketNumber, CrusadeDuty, NItems, NBankItems, SkillMastery;
	sWORD  MapLocX, MapLocY, GuildRank, GuildID, CharID1, CharID2, CharID3, QuestRewardType, MagicCharge;
	WORD   AccountID, Level, Quest, SuperAttackLeft/*, Gizon*/, w;
	sDWORD Rating;
	long elo;
	DWORD TitleUses, Title, HP, MP, SP, Exp, EK, MaxEK, PK, RewardGold, TimeLeftShutUp, TimeLeftRating, TimeLeftForceRecall,
		TimeLeftFirmStaminar, QuestID, QuestRewardAmmount, Contribution, WarContribution, SpecialEventID, DeathCount,
		ReserveTime, LockedMapTime, CrusadeGUID, ConstructionPoint, DeadPenaltyTime, ContPts,
		/*PartyID, */SpecialAbilityTime, Appr1, Appr2, Appr3, Appr4, ApprColor1, ApprColor2, SkillSSN, CharID,CurQuestCount;
	uint64 CurItemID;
	BOOL   Flag, IsQuestCompleted, bIsBankModified;
	SYSTEMTIME SaveTime;
	st_mysql_res    *pQueryResult = NULL;
	cItem	*ItemInfo;
	string itemQuery;
	static long charIndexEnd = 264;
	WORD Index = 0;

	ZeroMemory(CharName, sizeof(CharName));
	SafeCopy(CharName, Data, 10);
	ZeroMemory(AccName, sizeof(AccName));
	SafeCopy(AccName, Data+10, 10);
	ZeroMemory(AccPwd, sizeof(AccPwd));
	SafeCopy(AccPwd, Data+20, 10);
	AccountID = GetAccountID(AccName, myConn);
	if (AccountID == 0) return;
	CharID = GetCharID(CharName, AccName, myConn);
	Flag = (BOOL)bGetOffsetValue(Data, 30);
	cp = (Data+31);
	SaveTime.wYear   = wGetOffsetValue(cp, 0);
	SaveTime.wMonth  = bGetOffsetValue(cp, 2);
	SaveTime.wDay    = bGetOffsetValue(cp, 3);
	SaveTime.wHour   = bGetOffsetValue(cp, 4);
	SaveTime.wMinute = bGetOffsetValue(cp, 5);
	SaveTime.wSecond = bGetOffsetValue(cp, 6);
	ZeroMemory(SaveDate, sizeof(SaveDate));
	sprintf(SaveDate, "%d-%d-%d %d:%d:%d", SaveTime.wYear, SaveTime.wMonth, SaveTime.wDay, SaveTime.wHour, SaveTime.wMinute, SaveTime.wSecond);
	ZeroMemory(Location, sizeof(Location));
	SafeCopy(Location, cp+7, 10);
	ZeroMemory(MapName, sizeof(MapName));
	SafeCopy(MapName, cp+17, 10);
	MapLocX = (sWORD)wGetOffsetValue(cp, 27);
	MapLocY = (sWORD)wGetOffsetValue(cp, 29);
	ZeroMemory(GuildName, sizeof(GuildName));
	SafeCopy(GuildName, cp+31, 20);
	GuildID = wGetOffsetValue(cp, 51);
	GuildRank = (sWORD)wGetOffsetValue(cp, 53);
	HP = dwGetOffsetValue(cp, 55);
	MP = dwGetOffsetValue(cp, 59);
	SP = dwGetOffsetValue(cp, 63);
	Level = wGetOffsetValue(cp, 67);
	Rating = (sDWORD)dwGetOffsetValue(cp, 69);
	STR = bGetOffsetValue(cp, 73);
	VIT = bGetOffsetValue(cp, 74);
	DEX = bGetOffsetValue(cp, 75);
	INT = bGetOffsetValue(cp, 76);
	MAG = bGetOffsetValue(cp, 77);
	AGI = bGetOffsetValue(cp, 78);
	Luck = bGetOffsetValue(cp, 79);
	Exp = dwGetOffsetValue(cp, 80);
	EK = dwGetOffsetValue(cp, 84);
	PK = dwGetOffsetValue(cp, 88);
	RewardGold = dwGetOffsetValue(cp, 92);
	DownSkillIndex = (sBYTE)bGetOffsetValue(cp, 96);
	CharID1 = (sWORD)dwGetOffsetValue(cp, 97);
	CharID2 = (sWORD)dwGetOffsetValue(cp, 101);
	CharID3 = (sWORD)dwGetOffsetValue(cp, 105);
	Sex = bGetOffsetValue(cp, 109);
	Skin = bGetOffsetValue(cp, 110);
	HairStyle = bGetOffsetValue(cp, 111);
	HairColor = bGetOffsetValue(cp, 112);
	Underwear = bGetOffsetValue(cp, 113);
	HungerStatus = bGetOffsetValue(cp, 114);
	TimeLeftShutUp = dwGetOffsetValue(cp, 115);
	TimeLeftRating = dwGetOffsetValue(cp, 119);
	TimeLeftForceRecall = dwGetOffsetValue(cp, 123);
	TimeLeftFirmStaminar = dwGetOffsetValue(cp, 127);
	bIsBankModified = (BOOL)bGetOffsetValue(cp, 131);
	ZeroMemory(BlockDate, sizeof(BlockDate));
	SafeCopy(BlockDate, cp+132, 20);
	Quest = wGetOffsetValue(cp, 152);
	QuestID = wGetOffsetValue(cp, 154);
	CurQuestCount = dwGetOffsetValue(cp, 156);
	QuestRewardType = (sWORD)wGetOffsetValue(cp, 160);
	QuestRewardAmmount = dwGetOffsetValue(cp, 162);
	Contribution = dwGetOffsetValue(cp, 166);
	WarContribution = dwGetOffsetValue(cp, 170);
	IsQuestCompleted = (BOOL)bGetOffsetValue(cp, 174);
	SpecialEventID = dwGetOffsetValue(cp, 175);
	SuperAttackLeft = wGetOffsetValue(cp, 179);
	FightzoneNumber = bGetOffsetValue(cp, 181);
	ReserveTime = dwGetOffsetValue(cp, 182);
	FightzoneTicketNumber = bGetOffsetValue(cp, 186);
	SpecialAbilityTime = dwGetOffsetValue(cp, 187);
	ZeroMemory(LockedMapName, sizeof(LockedMapName));
	SafeCopy(LockedMapName, cp+191, 10);
	LockedMapTime = dwGetOffsetValue(cp, 201);
	CrusadeDuty = bGetOffsetValue(cp, 205);
	CrusadeGUID = dwGetOffsetValue(cp, 206);
	ConstructionPoint = dwGetOffsetValue(cp, 210);
	DeadPenaltyTime = dwGetOffsetValue(cp, 214);
	//PartyID = dwGetOffsetValue(cp, 218);
	//PartyID = 0;
	//Gizon = wGetOffsetValue(cp, 222);
	//Gizon = 0;
	//LegionCoins = wGetOffsetValue(cp, 220);
	ContPts = dwGetOffsetValue(cp, 218);
	DeathCount = dwGetOffsetValue(cp, 222);
	SpecialAbilityTime = dwGetOffsetValue(cp, 226);
	Appr1 = wGetOffsetValue(cp, 230);
	Appr2 = wGetOffsetValue(cp, 234);
	Appr3 = wGetOffsetValue(cp, 238);
	Appr4 = wGetOffsetValue(cp, 242);
	ApprColor1 = dwGetOffsetValue(cp, 246);
	ApprColor2 = dwGetOffsetValue(cp, 250);
	elo = wGetOffsetValue(cp, 254);
	MaxEK = dwGetOffsetValue(cp, 256);
	Title = dwGetOffsetValue(cp, 260);

	Index += charIndexEnd;

	for (w = 0; w < MAXMAGICS; w++)
	{
		MagicMastery = bGetOffsetValue(cp, Index+(w*3));
		MagicCharge = (sWORD)wGetOffsetValue(cp, Index+1+(w*3));
		ZeroMemory(QueryConsult, sizeof(QueryConsult));
		sprintf(QueryConsult, "UPDATE `magic` SET `MagicMastery` = %d,`Charge` = %d WHERE `CharID` = %lu AND `MagicID` = %d LIMIT 1;", MagicMastery, MagicCharge, CharID, w);
		if(ProcessQuery(&myConn, QueryConsult) == -1) return;
		pQueryResult = mysql_store_result(&myConn);
		SAFEFREERESULT(pQueryResult);
	}

	Index += (MAXMAGICS*3);

	/*char cTxt[128], log[500];
	ZeroMemory(cTxt, sizeof(cTxt));
	ZeroMemory(log, sizeof(log));
	strcat(log, "Received Skill(");
	strcat(log, CharName);
	strcat(log, ")");*/
	for (w = 0; w < MAXSKILLS; w++)
	{
		SkillMastery = bGetOffsetValue(cp, Index+(w*5));
		SkillSSN = dwGetOffsetValue(cp, Index+1+(w*5));
		ZeroMemory(QueryConsult, sizeof(QueryConsult));
		sprintf(QueryConsult, "UPDATE `skill` SET `SkillMastery` = %d,`SkillSSN` = %lu WHERE `CharID` = %lu AND `SkillID` = %d LIMIT 1;", SkillMastery, SkillSSN, CharID, w);
		if(ProcessQuery(&myConn, QueryConsult) == -1) return;
		pQueryResult = mysql_store_result(&myConn);
		SAFEFREERESULT(pQueryResult);
	/*sprintf(cTxt, ", [%d] %d (%d)", w, SkillMastery, SkillSSN);
	strcat(log, cTxt);*/
	}

	Index += (MAXSKILLS*5);

	//PutLogList(log);
	NItems = bGetOffsetValue(cp, Index);
	CurItemID = (GetLastInsertedItemID(myConn)+1);
	DeleteAllItemsFromChar(AccountID, CharID, myConn, bIsBankModified);
	ItemInfo = new cItem;
	if(NItems > 0)
	{
		itemQuery.append("INSERT INTO `item` ( `AccountID` , `CharID` , `ItemName`, `Count` , `ItemType`, `ID1`, `ID2`, `ID3`, `Color`, \
			`Effect1`, `Effect2`, `Effect3`, `LifeSpan`, `Attribute`, `TempItemAttr`, `Socket1`, `Socket2`, `Socket3`, `ItemEquip`, \
			`ItemPosX`, `ItemPosY`, `ItemID`, `ItemPos`) VALUES");

		for(w = 0; w < NItems; w++)
		{
			WORD IndexForItem = (Index+1 + (w*71));
			ZeroMemory(ItemInfo->ItemName, sizeof(ItemInfo->ItemName));
			SafeCopy(ItemInfo->ItemName, cp+IndexForItem, 20);
			if(strlen(ItemInfo->ItemName) == 0) continue;
			ItemInfo->ItemCount = dwGetOffsetValue(cp, (IndexForItem+20));
			ItemInfo->TouchEffectType = (sWORD)wGetOffsetValue(cp, (IndexForItem+24));
			ItemInfo->TouchEffectValue1 = (sWORD)wGetOffsetValue(cp, (IndexForItem+26));
			ItemInfo->TouchEffectValue2 = (sWORD)wGetOffsetValue(cp, (IndexForItem+30));
			ItemInfo->TouchEffectValue3 = (sWORD)dwGetOffsetValue(cp, (IndexForItem+34));
			ItemInfo->ItemColor = bGetOffsetValue(cp, (IndexForItem+38));
			ItemInfo->ItemSpecEffectValue1 = (sWORD)wGetOffsetValue(cp, (IndexForItem+39));
			ItemInfo->ItemSpecEffectValue2 = (sWORD)wGetOffsetValue(cp, (IndexForItem+41));
			ItemInfo->ItemSpecEffectValue3 = (sWORD)wGetOffsetValue(cp, (IndexForItem+43));
			ItemInfo->CurLifeSpan = wGetOffsetValue(cp, (IndexForItem+45));
			ItemInfo->Attribute = dwGetOffsetValue(cp, (IndexForItem+47));
			ItemInfo->TempItemAttr = dwGetOffsetValue(cp, (IndexForItem+51));
			ItemInfo->socket1 = bGetOffsetValue(cp, (IndexForItem+55));
			ItemInfo->socket2 = bGetOffsetValue(cp, (IndexForItem+56));
			ItemInfo->socket3 = bGetOffsetValue(cp, (IndexForItem+57));
			ItemInfo->IsItemEquipped = (BOOL)bGetOffsetValue(cp, (IndexForItem+58));
			ItemInfo->ItemPosX = (sWORD)wGetOffsetValue(cp, (IndexForItem+59));
			ItemInfo->ItemPosY = (sWORD)wGetOffsetValue(cp, (IndexForItem+61));
			ItemInfo->ItemUniqueID = ullGetOffsetValue(cp, (IndexForItem+63));

			if(ItemInfo->ItemUniqueID == 0){
				ItemInfo->ItemUniqueID = CurItemID;
				CurItemID++;
			}
			//ItemInfo->DupItemCode = CheckDupItem(ItemInfo, myConn, FALSE);
			ItemInfo->PutItemInBank = FALSE;
			if(w) itemQuery.append(",");
			sprintf(QueryConsult, "(%lu,%lu,'%s',%lu,%d,%d,%d,%d,%d,%d,%d,%d,%u,%lu,%lu,",
				AccountID, CharID ,ItemInfo->ItemName,ItemInfo->ItemCount, ItemInfo->TouchEffectType, ItemInfo->TouchEffectValue1, ItemInfo->TouchEffectValue2, ItemInfo->TouchEffectValue3, ItemInfo->ItemColor, ItemInfo->ItemSpecEffectValue1, ItemInfo->ItemSpecEffectValue2, ItemInfo->ItemSpecEffectValue3, ItemInfo->CurLifeSpan, ItemInfo->Attribute, ItemInfo->TempItemAttr);
			itemQuery.append(QueryConsult);
			
			if(ItemInfo->socket1 != SG_NONE)
				sprintf(QueryConsult, "%u,", ItemInfo->socket1);
			else
				sprintf(QueryConsult, "NULL,", ItemInfo->socket1);
			itemQuery.append(QueryConsult);

			if(ItemInfo->socket2 != SG_NONE)
				sprintf(QueryConsult, "%u,", ItemInfo->socket2);
			else
				sprintf(QueryConsult, "NULL,", ItemInfo->socket2);
			itemQuery.append(QueryConsult);

			if(ItemInfo->socket3 != SG_NONE)
				sprintf(QueryConsult, "%u,", ItemInfo->socket3);
			else
				sprintf(QueryConsult, "NULL,", ItemInfo->socket3);
			itemQuery.append(QueryConsult);

			sprintf(QueryConsult, "%d,%d,%d,%I64u,'BAG')", ItemInfo->IsItemEquipped, ItemInfo->ItemPosX, ItemInfo->ItemPosY, ItemInfo->ItemUniqueID);
			itemQuery.append(QueryConsult);
			DeleteItemOnDB(ItemInfo, CharID, myConn);
		}
		itemQuery.append(";");
		if(ProcessQuery(&myConn, (char *) itemQuery.c_str()) == -1) return;
		pQueryResult = mysql_store_result(&myConn);
		SAFEFREERESULT(pQueryResult);
	}

	Index += 1+(NItems*71);

	//Index = (charIndexEnd+421+(NItems*71));
	NBankItems = wGetOffsetValue(cp, Index);
	if(NBankItems > 0){
		itemQuery.clear();
		itemQuery.append("INSERT INTO `item` ( `AccountID` , `CharID` , `ItemName`, `Count` , `ItemType`, `ID1`, `ID2`, `ID3`, \
			`Color`, `Effect1`, `Effect2`, `Effect3`, `LifeSpan`, `Attribute`, `TempItemAttr`, `Socket1`, `Socket2`, `Socket3`, `ItemID`, `ItemPos`) VALUES ");
		for(w = 0; w < NBankItems; w++){
			WORD IndexForItem = Index+2+(w*66);
			ZeroMemory(ItemInfo->ItemName, sizeof(ItemInfo->ItemName));
			SafeCopy(ItemInfo->ItemName, cp+IndexForItem, 20);
			if(strlen(ItemInfo->ItemName) == 0) continue;
			ItemInfo->ItemCount = dwGetOffsetValue(cp, (IndexForItem+20));
			ItemInfo->TouchEffectType = (sWORD)wGetOffsetValue(cp, (IndexForItem+24));
			ItemInfo->TouchEffectValue1 = (sWORD)wGetOffsetValue(cp, (IndexForItem+26));
			ItemInfo->TouchEffectValue2 = (sWORD)wGetOffsetValue(cp, (IndexForItem+30));
			ItemInfo->TouchEffectValue3 = (sWORD)dwGetOffsetValue(cp, (IndexForItem+34));
			ItemInfo->ItemColor = bGetOffsetValue(cp, (IndexForItem+38));
			ItemInfo->ItemSpecEffectValue1 = (sWORD)wGetOffsetValue(cp, (IndexForItem+39));
			ItemInfo->ItemSpecEffectValue2 = (sWORD)wGetOffsetValue(cp, (IndexForItem+41));
			ItemInfo->ItemSpecEffectValue3 = (sWORD)wGetOffsetValue(cp, (IndexForItem+43));
			ItemInfo->CurLifeSpan = wGetOffsetValue(cp, (IndexForItem+45));
			ItemInfo->Attribute = dwGetOffsetValue(cp, (IndexForItem+47));
			ItemInfo->TempItemAttr = dwGetOffsetValue(cp, (IndexForItem+51));
			ItemInfo->socket1 = bGetOffsetValue(cp, (IndexForItem+55));
			ItemInfo->socket2 = bGetOffsetValue(cp, (IndexForItem+56));
			ItemInfo->socket3 = bGetOffsetValue(cp, (IndexForItem+57));
			ItemInfo->ItemUniqueID = ullGetOffsetValue(cp, (IndexForItem+58));
			if(ItemInfo->ItemUniqueID == 0){
				ItemInfo->ItemUniqueID = CurItemID;
				CurItemID++;
			}
			//ItemInfo->DupItemCode = CheckDupItem(ItemInfo, myConn, TRUE);
			ItemInfo->PutItemInBank = TRUE;

			if(w) itemQuery.append(",");
			sprintf(QueryConsult, "(%lu,%lu,'%s',%lu,%d,%d,%d,%d,%d,%d,%d,%d,%u,%lu,%lu,",
				AccountID, CharID ,ItemInfo->ItemName,ItemInfo->ItemCount, ItemInfo->TouchEffectType, ItemInfo->TouchEffectValue1, ItemInfo->TouchEffectValue2, ItemInfo->TouchEffectValue3, ItemInfo->ItemColor, ItemInfo->ItemSpecEffectValue1, ItemInfo->ItemSpecEffectValue2, ItemInfo->ItemSpecEffectValue3, ItemInfo->CurLifeSpan, ItemInfo->Attribute, ItemInfo->TempItemAttr);
			itemQuery.append(QueryConsult);
			
			if(ItemInfo->socket1 != SG_NONE)
				sprintf(QueryConsult, "%u,", ItemInfo->socket1);
			else
				sprintf(QueryConsult, "NULL,", ItemInfo->socket1);
			itemQuery.append(QueryConsult);

			if(ItemInfo->socket2 != SG_NONE)
				sprintf(QueryConsult, "%u,", ItemInfo->socket2);
			else
				sprintf(QueryConsult, "NULL,", ItemInfo->socket2);
			itemQuery.append(QueryConsult);

			if(ItemInfo->socket3 != SG_NONE)
				sprintf(QueryConsult, "%u,", ItemInfo->socket3);
			else
				sprintf(QueryConsult, "NULL,", ItemInfo->socket3);
			itemQuery.append(QueryConsult);

			sprintf(QueryConsult, "%I64u, 'WH')", ItemInfo->ItemUniqueID);
			itemQuery.append(QueryConsult);
			DeleteItemOnDB(ItemInfo, CharID, myConn);
		}
		itemQuery.append(";");
		if(ProcessQuery(&myConn, (char *) itemQuery.c_str()) == -1) return;
		pQueryResult = mysql_store_result(&myConn);
		SAFEFREERESULT(pQueryResult);
	}
	SAFEDELETE(ItemInfo);
	//Index += (NBankItems*66);

	Index += 2+(NBankItems*66);

	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	sprintf(QueryConsult, "DELETE FROM `title` WHERE `CharID` = %lu;", CharID);
	if(ProcessQuery(&myConn, QueryConsult) == -1) return;
	pQueryResult = mysql_store_result(&myConn);
	SAFEFREERESULT(pQueryResult);

	BYTE NTitles = bGetOffsetValue(cp, Index);
	for (w = 0; w < NTitles; w++)
	{
		Title = bGetOffsetValue(cp, Index+1+(w*8));
		TitleUses = dwGetOffsetValue(cp, Index+1+4+(w*8));
		ZeroMemory(QueryConsult, sizeof(QueryConsult));
		sprintf(QueryConsult, "INSERT INTO `title` ( `CharID`, `Title` , `Uses`) VALUES ( %lu, %d, %d );", CharID, Title, TitleUses);
		if(ProcessQuery(&myConn, QueryConsult) == -1) return;
		pQueryResult = mysql_store_result(&myConn);
		SAFEFREERESULT(pQueryResult);
	}

	Index += 1+(NTitles*8);

	ZeroMemory(Profile, sizeof(Profile));
	SafeCopy(Profile, (cp+Index), strlen(cp+Index));
	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	ZeroMemory(GoodGuildName, sizeof(GoodGuildName));
	MakeGoodName(GoodGuildName, GuildName);
	ZeroMemory(GoodProfile, sizeof(GoodProfile));
	MakeGoodName(GoodProfile, Profile);
	sprintf(QueryConsult, "UPDATE `char_database` SET `LastSaveDate` = '%s',`ID1` = %d,`ID2` = %d,`ID3` = %d,`Level` = %d,`Strenght` = %d,`Vitality` = %d,`Dexterity` = %d,`Intelligence` = %d,`Magic` = %d,`Agility` = %d,`Luck` = %d,`Exp` = %lu,`Gender` = %d,`Skin` = %d,`HairStyle` = %d,`HairColor` = %d,`Underwear` = %d,`ApprColor1` = %lu,`ApprColor2` = %lu,`Appr1` = %lu,`Appr2` = %lu,`Appr3` = %lu,`Appr4` = %lu,`Nation` = '%s',`MapLoc` = '%s',`LocX` = %d,`LocY` = %d,`Profile` = '%s',`Contribution` = %lu,`LeftSpecTime` = %lu,`LockMapName` = '%s',`LockMapTime` = %lu,`BlockDate` = '%s',`GuildName` = '%s',`GuildID` = %d,`GuildRank` = %d,`FightNum` = %d,`FightDate` = %lu,`FightTicket` = %d,`QuestNum` =%u,`QuestID` =%u,`QuestCount` = %d,`QuestRewType` = %d,`QuestRewAmmount` = %lu,\
								 `QuestCompleted` = %d,`EventID` = %lu,`WarCon` = %lu,`CruJob` = %d,`CruID` = %lu,`CruConstructPoint` = %lu, `Popularity` = '%li' ,`HP` = %lu,`MP` = %lu,`SP` = %lu,`EK` = %lu,`PK` = %lu,`RewardGold` = %lu,`DownSkillID` = %d,`Hunger` = %d,`LeftSAC` =%u,`LeftShutupTime` = %lu,`LeftPopTime` = %lu,`LeftForceRecallTime` = %lu,`LeftFirmStaminarTime` = %lu,`LeftDeadPenaltyTime` = %lu,`DeathCount` = %lu,`ContPts` = %lu,`elo` = %lu,`MaxEK` = %lu WHERE `CharID` = %lu LIMIT 1;",
								 SaveDate, CharID1, CharID2, CharID3, Level, STR, VIT, DEX, INT, MAG, AGI, Luck, Exp, Sex, Skin, HairStyle, HairColor, Underwear, ApprColor1, ApprColor2, Appr1, Appr2, Appr3, Appr4, Location, MapName, MapLocX, MapLocY, GoodProfile, Contribution, SpecialAbilityTime, LockedMapName, LockedMapTime, BlockDate, GoodGuildName, GuildID, GuildRank, FightzoneNumber, ReserveTime, FightzoneTicketNumber, Quest, QuestID, CurQuestCount, QuestRewardType, QuestRewardAmmount, IsQuestCompleted, SpecialEventID, WarContribution, CrusadeDuty, CrusadeGUID, ConstructionPoint, Rating, HP, MP, SP, EK, PK, RewardGold, DownSkillIndex, HungerStatus, SuperAttackLeft, TimeLeftShutUp, TimeLeftRating, TimeLeftForceRecall, TimeLeftFirmStaminar, DeadPenaltyTime, DeathCount, ContPts, elo, MaxEK, CharID);
	//PutLogFileList(QueryConsult, "Logs/SaveCharQuery.txt");
	if(ProcessQuery(&myConn, QueryConsult) == -1) return;
	pQueryResult = mysql_store_result(&myConn);
	SAFEFREERESULT(pQueryResult);
	//sprintf(cTemp22,"Character quest %d",CurQuestCount);
	//PutLogList(cTemp22);
}
//=============================================================================
BYTE CLoginServer::CheckDupItem(cItem *Item, MYSQL myConn, BOOL bBank)
{
	char QueryConsult[500];
	st_mysql_res *QueryResult = NULL;
	BYTE ItemInBag = 0, ItemInBank = 0;
 
	if(Item->TouchEffectType == 1) return NOITEM; //No dupe checking on bound items

	if(Item->TouchEffectValue1 == 0 && Item->TouchEffectValue2 == 0 && Item->TouchEffectValue3 == 0) return NOITEM;

	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	if(bBank){
		sprintf(QueryConsult, "SELECT `ItemName` FROM `item` WHERE `ItemName` = '%s' AND \
			`ItemPos` = 'WH' AND `ID1` = %d AND `ID2` = %d AND `ID3` = %d;",
			Item->ItemName, Item->TouchEffectValue1, Item->TouchEffectValue2, Item->TouchEffectValue3);
		if(ProcessQuery(&myConn, QueryConsult) == -1) return 0;
		QueryResult = mysql_store_result(&myConn);
		ItemInBank = (BYTE)mysql_num_rows(QueryResult);
	} else {
		sprintf(QueryConsult, "SELECT `ItemName` FROM `item` WHERE `ItemName` = '%s' AND \
			`ItemPos` = 'BAG' AND `ID1` = %d AND `ID2` = %d AND `ID3` = %d;",
			Item->ItemName, Item->TouchEffectValue1, Item->TouchEffectValue2, Item->TouchEffectValue3);
		if(ProcessQuery(&myConn, QueryConsult) == -1) return 0;
		QueryResult = mysql_store_result(&myConn);
		ItemInBag = (BYTE)mysql_num_rows(QueryResult);
	}

	SAFEFREERESULT(QueryResult);
	if(ItemInBag == 1 && ItemInBank == 0)           return ONEINBAG;
	else if(ItemInBag == 0 && ItemInBank == 1)      return ONEINBANK;
	else if(ItemInBag == 1 && ItemInBank == 1)      return ONEINBAG_ONEINBANK;
	else if(ItemInBag > 1)                          return DUPITEMINBAG;
	else if(ItemInBank > 1)                         return DUPITEMINBANK;
	else return NOITEM;
}
//=============================================================================
void CLoginServer::DeleteItemOnDB(cItem *Item, DWORD CharID, MYSQL myConn)
{
	char   log[200];
	char QueryConsult[200];
	st_mysql_res    *pQueryResult = NULL;

	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	sprintf(QueryConsult, "DELETE FROM `item` WHERE `ItemID` = %I64u AND \
								 (`ItemPos` = 'BAG' OR `ItemPos` = 'WH');", Item->ItemUniqueID);
	if(ProcessQuery(&myConn, QueryConsult) == -1) return;
	pQueryResult = mysql_store_result(&myConn);
	SAFEFREERESULT(pQueryResult);
}
//=============================================================================
void CLoginServer::CreateNewItem(cItem *Item, WORD AccountID, DWORD CharID, MYSQL myConn)
{
	char QueryConsult[2000];
	st_mysql_res    *pQueryResult = NULL;

	//if(Item->ItemUniqueID == 0) Item->ItemUniqueID = (GetLastInsertedItemID(myConn)+1);

	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	if(Item->PutItemInBank) 
		sprintf(QueryConsult, "INSERT INTO `item` ( `AccountID` , `CharID` , `ItemName`, `Count` , `ItemType`, `ID1`, `ID2`, `ID3`, `Color`, \
									 `Effect1`, `Effect2`, `Effect3`, `LifeSpan`, `Attribute`, `TempItemAttr`, `ItemPos`)\
									 						 VALUES (   %lu  ,   %lu  ,   '%s'    ,  %lu  ,    %d   ,  %d,  %d,  %d,  %d  ,    %d  ,    %d  ,    %d  ,   %u   ,    %lu   ,    %lu  , 'WH');",
														AccountID, CharID ,Item->ItemName,Item->ItemCount, Item->TouchEffectType, Item->TouchEffectValue1, Item->TouchEffectValue2, Item->TouchEffectValue3, Item->ItemColor, Item->ItemSpecEffectValue1, Item->ItemSpecEffectValue2, Item->ItemSpecEffectValue3, Item->CurLifeSpan, Item->Attribute, Item->TempItemAttr);
	else 
		sprintf(QueryConsult, "INSERT INTO `item` ( `AccountID` , `CharID` , `ItemName`, `Count` , `ItemType`, `ID1`, `ID2`, `ID3`, `Color`, `Effect1`, `Effect2`, `Effect3`, `LifeSpan`, `Attribute`, `TempItemAttr`, `ItemEquip`, `ItemPosX`, `ItemPosY`, `ItemPos`)\
									 				  VALUES (   %lu ,   %lu  ,   '%s'    ,  %lu  ,    %d   ,  %d,  %d,  %d,  %d  ,    %d  ,    %d  ,    %d  ,   %u   ,    %lu    ,    %lu    ,    %d    ,    %d   ,    %d, 'BAG');",
													  AccountID, CharID ,Item->ItemName,Item->ItemCount, Item->TouchEffectType, Item->TouchEffectValue1, Item->TouchEffectValue2, Item->TouchEffectValue3, Item->ItemColor, Item->ItemSpecEffectValue1, Item->ItemSpecEffectValue2, Item->ItemSpecEffectValue3, Item->CurLifeSpan, Item->Attribute, Item->TempItemAttr, Item->IsItemEquipped, Item->ItemPosX, Item->ItemPosY);

	if(ProcessQuery(&myConn, QueryConsult) == -1) return;
	pQueryResult = mysql_store_result(&myConn);
	SAFEFREERESULT(pQueryResult);
}
//=============================================================================
void CLoginServer::OnUserAccept(HWND hWnd)
{
 class XSocket * pTmpSock;
 
		for(UINT i = 0; i < MAXCLIENTS; i++){
			if(ClientSocket[i] == NULL){
				ClientSocket[i] = new XSocket(hWnd, XSOCKBLOCKLIMIT);
                MainSocket->bAccept(ClientSocket[i], WM_ONCLIENTSOCKETEVENT + i);
                ClientSocket[i]->bInitBufferSize(MSGCLIENTBUFFERSIZE);
				return;
			}
		}
		pTmpSock = new class XSocket(hWnd, XSOCKBLOCKLIMIT);
		MainSocket->bAccept(pTmpSock, NULL); 
		delete pTmpSock;
}
//=============================================================================
void CLoginServer::OnGateServerAccept(HWND hWnd)
{
 class XSocket * pTmpSock;		
	
		for(WORD w = 0; w < MAXGAMESERVERSOCKETS; w++)
              {
               if(GameServerSocket[w] == NULL)
                 {
                  char Txt100[100];
                  GameServerSocket[w] = new XSocket(hWnd, XSOCKBLOCKLIMIT);
                  GateServerSocket->bAccept(GameServerSocket[w], WM_ONGAMESERVERSOCKETEVENT + w);
                  GameServerSocket[w]->bInitBufferSize(MSGBUFFERSIZE);
                  _ADDRESS peerAddr;
                  GameServerSocket[w]->iGetPeerAddress(peerAddr);
                  ZeroMemory(Txt100, sizeof(Txt100));
                  sprintf(Txt100, "(*) Game-server socket[%u] IP(%s) accepted on gate server socket.", w, peerAddr);
                  PutLogList(Txt100);
                  if(ListenToAllAddresses == FALSE && IsAddrPermitted(peerAddr) == FALSE)
                    {
                        ZeroMemory(Txt100, sizeof(Txt100));
                        sprintf(Txt100, "(!) Game-server connection from non-authorized IP(%s) refused.", peerAddr);
                        PutLogList(Txt100, WARN_MSG, true, HACK_LOGFILE);
                        CloseGameServerSocket(w);
                        return;
                    }
                  return;
                 }
              }
		pTmpSock = new class XSocket(hWnd, XSOCKBLOCKLIMIT);
		GateServerSocket->bAccept(pTmpSock, NULL); 
		delete pTmpSock;
}
//=============================================================================
void CLoginServer::SetAccountServerChangeStatus(char *Data, BOOL IsOnServerChange)
{
 char AccName[15];
 WORD AccountID;

		ZeroMemory(AccName, sizeof(AccName));
		SafeCopy(AccName, Data, 10);
		if(IsAccountInUse(AccName, &AccountID)) Client[AccountID]->IsOnServerChange = IsOnServerChange;
}
//=============================================================================
BOOL CLoginServer::PutMsgQuene(char * pData, DWORD dwMsgSize, int iIndex, char cKey)
{
	if (MsgQuene[QueneTail] != NULL) return FALSE;

	MsgQuene[QueneTail] = new CMsg;
	if (MsgQuene[QueneTail] == NULL) return FALSE;

	if (MsgQuene[QueneTail]->bPut(pData, dwMsgSize, iIndex, cKey) == FALSE) return FALSE;

	QueneTail++;
	if (QueneTail >= MSGQUENESIZE) QueneTail = 0;

	return TRUE;
}
//=============================================================================
BOOL CLoginServer::GetMsgQuene(char * pData, DWORD * pMsgSize, int * pIndex, char * pKey)
{
	if (MsgQuene[QueneHead] == NULL) return FALSE;

	MsgQuene[QueneHead]->Get(pData, pMsgSize, pIndex, pKey);

	SAFEDELETE(MsgQuene[QueneHead]);

	QueneHead++;
	if (QueneHead >= MSGQUENESIZE) QueneHead = 0;

	return TRUE;
}
//=============================================================================
void CLoginServer::MsgProcess()
{
 char   pData[MSGBUFFERSIZE], cKey;
 DWORD    dwMsgSize;
 WORD   w;
 int    ID;
  
	ZeroMemory(pData, sizeof(pData));
	
	while (GetMsgQuene(pData, &dwMsgSize, &ID, &cKey) == TRUE){
		ProcessGSMsgWithDBAccess(pData, (BYTE)ID, mySQL);		
		ZeroMemory(pData, sizeof(pData));
	}
	for(w=0; w<MAXGAMESERVERS; w++)	if(GameServer[w] != NULL) GameServer[w]->SendStockMsgToGameServer();
}
//=============================================================================
void CLoginServer::RequestForceDisconnect(CClient *pClient, WORD count)
{
 char SendBuff[50];
 DWORD *dwp;
 WORD *wp;

		if(pClient == NULL) return;
		if(GameServer[pClient->ConnectedServerID] == NULL){
			pClient->ForceDisconnRequestTime = timeGetTime();
			return;
		}
		ZeroMemory(SendBuff, sizeof(SendBuff));
		dwp = (DWORD*)SendBuff;
		*dwp = MSGID_REQUEST_FORCEDISCONECTACCOUNT;
		wp = (WORD*)(SendBuff+4);
		*wp = count;
		SafeCopy(SendBuff+6, pClient->AccountName);
		if(GameServer[pClient->ConnectedServerID] != NULL) GameServer[pClient->ConnectedServerID]->SendMsg(SendBuff, 16, NULL, true);
		if(pClient->ForceDisconnRequestTime == 0) pClient->ForceDisconnRequestTime = timeGetTime();
}
//=============================================================================
uint64 CLoginServer::GetLastInsertedItemID(MYSQL myConn)
{
	MYSQL_ROW Row;
	st_mysql_res *QueryResult = NULL;
	uint64 LastItemID;

	if(ProcessQuery(&myConn, "SELECT `ItemID` FROM `item` ORDER BY `ItemID` DESC LIMIT 1;") == -1) return 0;
	QueryResult = mysql_store_result(&myConn);
	if(mysql_num_rows(QueryResult) == 0){
		SAFEFREERESULT(QueryResult);
		LastItemID = 0;
	}
	else{
		Row = mysql_fetch_row(QueryResult);
		LastItemID = atoull(Row[0]);
		SAFEFREERESULT(QueryResult);
	}

	return LastItemID;
}
uint64 CLoginServer::GetAccountID(char *AccountName, MYSQL myConn)
{
	MYSQL_ROW Row;
	st_mysql_res *QueryResult = NULL;
	char QueryConsult[200], GoodAccName[25];
	WORD AccountID;
	
	ZeroMemory(GoodAccName, sizeof(GoodAccName));
	MakeGoodName(GoodAccName, AccountName);

	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	sprintf(QueryConsult, "SELECT `AccountID` FROM `account_database` WHERE `name` = '%s' LIMIT 1;", GoodAccName);

	if(ProcessQuery(&myConn, QueryConsult) == -1) return 0;
	QueryResult = mysql_store_result(&myConn);
	if(mysql_num_rows(QueryResult) == 0){
		SAFEFREERESULT(QueryResult);
		AccountID = 0;
	}
	else{
		Row = mysql_fetch_row(QueryResult);
		AccountID = atoull(Row[0]);
		SAFEFREERESULT(QueryResult);
	}

	return AccountID;
}
//=============================================================================
void CLoginServer::RequestCreateNewGuildHandler(char *Data, BYTE GSID, MYSQL myConn)
{
 char CharName[15], AccountName[15], AccountPwd[15], GuildName[25], GuildLoc[15],
	  SendBuff[50], QueryConsult[500], CreationTime[50], GoodGuildName[50], GoodCharName[25];
 DWORD *dwp, GuildID;
 WORD *wp;
 SYSTEMTIME SysTime;
 int InsertResult;
 st_mysql_res    *pQueryResult = NULL;
          			
		ZeroMemory(SendBuff, sizeof(SendBuff));		

		ZeroMemory(CharName, sizeof(CharName));
		SafeCopy(CharName, Data, 10);
		
		ZeroMemory(AccountName, sizeof(AccountName));
		SafeCopy(AccountName, Data+10, 10);
		
		ZeroMemory(AccountPwd, sizeof(AccountPwd));
		SafeCopy(AccountPwd, Data+20, 10);
		
		ZeroMemory(GuildName, sizeof(GuildName));
		SafeCopy(GuildName, Data+30, 20);
		
		ZeroMemory(GuildLoc, sizeof(GuildLoc));
		SafeCopy(GuildLoc, Data+50, 10);

		dwp = (DWORD*)SendBuff;
		*dwp = MSGID_RESPONSE_CREATENEWGUILD;

		if(CheckAccountLogin(AccountName, AccountPwd, myConn) == LOGINOK){
			if(GuildExists(GuildName, &GuildID, myConn)){
				wp = (WORD*)(SendBuff+4);
				*wp = LOGRESMSGTYPE_REJECT;
				SafeCopy(SendBuff+6, CharName);
				SendMsgToGS(GSID, SendBuff, 16);
			}
			else{
				GetLocalTime(&SysTime);
				ZeroMemory(CreationTime, sizeof(CreationTime));
				sprintf(CreationTime, "%d-%d-%d %d:%d:%d", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
				ZeroMemory(QueryConsult, sizeof(QueryConsult));
				ZeroMemory(GoodGuildName, sizeof(GoodGuildName));
				MakeGoodName(GoodGuildName, GuildName);
				ZeroMemory(GoodCharName, sizeof(GoodCharName));
				MakeGoodName(GoodCharName, CharName);
				sprintf(QueryConsult, "INSERT INTO `guild` ( `GuildName` , `MasterName` , `Nation` , `NumberOfMembers` , `CreateDate` ) VALUES ('%s', '%s', '%s', '1', '%s');", GoodGuildName, GoodCharName, GuildLoc, CreationTime);
				InsertResult = ProcessQuery(&myConn, QueryConsult);
				if(InsertResult == -1) return;
				pQueryResult = mysql_store_result(&myConn);
				SAFEFREERESULT(pQueryResult);
				if(InsertResult == 0){
					GuildExists(GuildName, &GuildID, myConn);
					wp = (WORD*)(SendBuff+4);
					*wp = LOGRESMSGTYPE_CONFIRM;
					SafeCopy(SendBuff+6, CharName);
					dwp = (DWORD*)(SendBuff+16);
					*dwp = GuildID;
					SendMsgToGS(GSID, SendBuff, 20);
				}
				else{
					wp = (WORD*)(SendBuff+4);
					*wp = LOGRESMSGTYPE_REJECT;
					SafeCopy(SendBuff+6, CharName);
					SendMsgToGS(GSID, SendBuff, 16);
				}
			}
		}
		else{
			wp = (WORD*)(SendBuff+4);
			*wp = LOGRESMSGTYPE_REJECT;
			SafeCopy(SendBuff+6, CharName);
			SendMsgToGS(GSID, SendBuff, 16);
		}
}
//=============================================================================
BOOL CLoginServer::GuildExists(char *GuildName, DWORD *GuildID, MYSQL myConn)
{
 char QueryConsult[150], GoodGuildName[50];
 st_mysql_res *QueryResult = NULL;
 MYSQL_ROW Row;
   
        ZeroMemory(QueryConsult, sizeof(QueryConsult));
		ZeroMemory(GoodGuildName, sizeof(GoodGuildName));
		MakeGoodName(GoodGuildName, GuildName);
        sprintf(QueryConsult, "SELECT `GuildID` FROM `guild` WHERE `GuildName` = '%s' LIMIT 1;", GoodGuildName);
        if(ProcessQuery(&myConn, QueryConsult) == -1) return FALSE;
        QueryResult = mysql_store_result(&myConn);
        if(mysql_num_rows(QueryResult) == 1){
			Row = mysql_fetch_row(QueryResult);
			*GuildID = atoi(Row[0]);
            SAFEFREERESULT(QueryResult);
			return TRUE;
		}
        SAFEFREERESULT(QueryResult);
        return FALSE;
}
//=============================================================================
void CLoginServer::RequestDisbandGuildHandler(char *Data, BYTE GSID, MYSQL myConn)
{
 char	SendBuff[50], CharName[15], GoodCharName[25], AccName[15], AccPwd[15], GuildName[25], GoodGuildName[50];
 DWORD	*dwp, GuildID;
 WORD	*wp;
 char QueryConsult[150];
 int InsertResult;
 st_mysql_res    *pQueryResult = NULL;
         	
		ZeroMemory(SendBuff, sizeof(SendBuff));
		dwp  = (DWORD *)(SendBuff);
		*dwp = MSGID_RESPONSE_DISBANDGUILD;
				
		ZeroMemory(CharName, sizeof(CharName));
		SafeCopy(CharName, Data, 10);

		ZeroMemory(AccName, sizeof(AccName));
		SafeCopy(AccName, Data+10, 10);

		ZeroMemory(AccPwd, sizeof(AccPwd));
		SafeCopy(AccPwd, Data+20, 10);

		ZeroMemory(GuildName, sizeof(GuildName));
		SafeCopy(GuildName, Data+30, 20);

		if(CheckAccountLogin(AccName, AccPwd, myConn) == LOGINOK){
			if(GuildExists(GuildName, &GuildID, myConn) && IsGuildMaster(CharName, GuildName, myConn)){
				ZeroMemory(QueryConsult, sizeof(QueryConsult));
				ZeroMemory(GoodGuildName, sizeof(GoodGuildName));
				MakeGoodName(GoodGuildName, GuildName);
				ZeroMemory(GoodCharName, sizeof(GoodCharName));
				MakeGoodName(GoodCharName, CharName);
				sprintf(QueryConsult, "DELETE FROM `guild` WHERE `GuildName` = '%s' AND `MasterName` = '%s' LIMIT 1;", GoodGuildName, GoodCharName);
				InsertResult = ProcessQuery(&myConn, QueryConsult);
				if(InsertResult == -1) return;
				pQueryResult = mysql_store_result(&myConn);
				SAFEFREERESULT(pQueryResult);

				wp = (WORD*)(SendBuff+4);
				*wp = (InsertResult == 0) ? LOGRESMSGTYPE_CONFIRM : LOGRESMSGTYPE_REJECT;
				SafeCopy(SendBuff+6, CharName);
				SendMsgToGS(GSID, SendBuff, 16);	
			}
			else{
				wp = (WORD*)(SendBuff+4);
				*wp = LOGRESMSGTYPE_REJECT;
				SafeCopy(SendBuff+6, CharName);
				SendMsgToGS(GSID, SendBuff, 16);	
			}
		}
		else{
			wp = (WORD*)(SendBuff+4);
			*wp = LOGRESMSGTYPE_REJECT;
			SafeCopy(SendBuff+6, CharName);
			SendMsgToGS(GSID, SendBuff, 16);
		}
}
//=============================================================================
BOOL CLoginServer::IsGuildMaster(char *CharName, char *GuildName, MYSQL myConn)
{
	char QueryConsult[150], GoodGuildName[50];
	st_mysql_res *QueryResult = NULL;
	MYSQL_ROW Row;
		 
        ZeroMemory(QueryConsult, sizeof(QueryConsult));
        ZeroMemory(GoodGuildName, sizeof(GoodGuildName));
		MakeGoodName(GoodGuildName, GuildName);
		sprintf(QueryConsult, "SELECT `MasterName` FROM `guild` WHERE `GuildName` = '%s' LIMIT 1;", GoodGuildName);
        if(ProcessQuery(&myConn, QueryConsult) == -1) return FALSE;
        QueryResult = mysql_store_result(&myConn);
        if(mysql_num_rows(QueryResult) == 1){
			Row = mysql_fetch_row(QueryResult);
			if(IsSame(Row[0], CharName)){
				SAFEFREERESULT(QueryResult);
				return TRUE;
			}
			else{
				SAFEFREERESULT(QueryResult);
				return FALSE;
			}
		}
        SAFEFREERESULT(QueryResult);
		return FALSE;
}
//=============================================================================
void CLoginServer::AddGuildMember(char *Data, MYSQL myConn)
{
 char CharName[15], GoodCharName[25], GuildName[25], GoodGuildName[50], QueryConsult[250], CreationTime[50];
 DWORD GuildID;
 st_mysql_res *QueryResult = NULL;
 MYSQL_ROW Row;
 WORD NMembers;
 SYSTEMTIME SysTime;
 
		ZeroMemory(CharName, sizeof(CharName));
		SafeCopy(CharName, Data, 10);
		ZeroMemory(GuildName, sizeof(GuildName));
		SafeCopy(GuildName, Data+10, 20);

		if(GuildExists(GuildName, &GuildID, myConn)){
			GetLocalTime(&SysTime);
			ZeroMemory(CreationTime, sizeof(CreationTime));
			sprintf(CreationTime, "%d-%d-%d %d:%d:%d", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
			
			ZeroMemory(QueryConsult, sizeof(QueryConsult));
			sprintf(QueryConsult, "SELECT `NumberOfMembers` FROM `guild` WHERE `GuildID` = %lu;", GuildID);
			if(ProcessQuery(&myConn, QueryConsult) == -1) return;
			QueryResult = mysql_store_result(&myConn);
			if(mysql_num_rows(QueryResult) == 1){
				Row = mysql_fetch_row(QueryResult);
				NMembers = (WORD)(atoi(Row[0])+1);
			}
			SAFEFREERESULT(QueryResult);
			ZeroMemory(QueryConsult, sizeof(QueryConsult));
			sprintf(QueryConsult, "UPDATE `guild` SET `NumberOfMembers` = %lu WHERE `GuildID` = %lu LIMIT 1;", NMembers,  GuildID);
			if(ProcessQuery(&myConn, QueryConsult) == -1) return;
			QueryResult = mysql_store_result(&myConn);
			SAFEFREERESULT(QueryResult);
		}
}
//=============================================================================
void CLoginServer::RemoveGuildMember(char *Data, MYSQL myConn)
{
 char CharName[15], GoodCharName[25], GuildName[25], QueryConsult[250];
 DWORD GuildID;
 st_mysql_res *QueryResult = NULL;
 MYSQL_ROW Row;
 WORD NMembers;
  
		ZeroMemory(CharName, sizeof(CharName));
		SafeCopy(CharName, Data, 10);
		ZeroMemory(GuildName, sizeof(GuildName));
		SafeCopy(GuildName, Data+10, 20);

		if(GuildExists(GuildName, &GuildID, myConn))
		{
			ZeroMemory(QueryConsult, sizeof(QueryConsult));
			sprintf(QueryConsult, "SELECT `NumberOfMembers` FROM `guild` WHERE `GuildID` = %lu;", GuildID);
			if(ProcessQuery(&myConn, QueryConsult) == -1) return;
			QueryResult = mysql_store_result(&myConn);
			if(mysql_num_rows(QueryResult) == 1){
				Row = mysql_fetch_row(QueryResult);
				NMembers = (WORD)(atoi(Row[0])-1);
				if(NMembers <= 0) NMembers = 1;
			}
			SAFEFREERESULT(QueryResult);
			ZeroMemory(QueryConsult, sizeof(QueryConsult));
			sprintf(QueryConsult, "UPDATE `guild` SET `NumberOfMembers` = %lu WHERE `GuildID` = %lu LIMIT 1;", NMembers,  GuildID);
			if(ProcessQuery(&myConn, QueryConsult) == -1) return;
			QueryResult = mysql_store_result(&myConn);
			SAFEFREERESULT(QueryResult);
		}
}

void CLoginServer::Process_SaveGuildInfo(char * data, MYSQL myConn)
{
	uint16 guildGUID;
	Pop(data, guildGUID);

	char guildName[21];
	ZeroMemory( guildName, sizeof(guildName) );
	Pop(data, guildName, 20);
	
	uint32 guildID;
	if(!GuildExists(guildName, &guildID, myConn))
		return;

	
	uint8 ups[GU_MAX];
	for(int i=0; i < GU_MAX; i++)
		Pop(data, ups[i]);
		
	uint32 gold, maj, cont;
	Pop(data, gold);
	Pop(data, maj);
	Pop(data, cont);

	uint8 captains, raidmasters, huntmasters;
	Pop(data, captains);
	Pop(data, raidmasters);
	Pop(data, huntmasters);

	char query[500];
	st_mysql_res * pQueryResult;
	sprintf(query, "UPDATE `guild` SET `WHLvl`=%u, `SummonsLvl`=%u, `CaptaincyLvl`=%u, \
						`RaidmastersLvl`=%u, `HuntmastersLvl`=%u, `Gold`=%u, `Maj`=%u, `Cont`=%u, \
						`Captains`=%u, `Raidmasters`=%u, `Huntmasters`=%u \
						WHERE `GuildID`=%lu LIMIT 1;", 
						ups[GU_WAREHOUSE], /*ups[GU_SUMMONS]*/0, ups[GU_CAPTAINCY], 
						ups[GU_RAIDMASTERS], ups[GU_HUNTMASTERS], gold, maj, cont, 
						captains, raidmasters, huntmasters, guildID);
	if(ProcessQuery(&myConn, query) == -1) return;
	pQueryResult = mysql_store_result(&myConn);
	SAFEFREERESULT(pQueryResult);

	bool saveWH;
	Pop(data, (uint8&)saveWH);

	if(!saveWH)
		return;

	uint16 nBankItems;
	Pop(data, nBankItems);

	uint64 CurItemID = GetLastInsertedItemID(myConn) + 1;

	sprintf(query, "DELETE FROM `item` WHERE `ItemPos`='GWH' AND `CharID`=%lu", guildGUID);
	
	if(ProcessQuery(&myConn, query) == -1) return;
	pQueryResult = mysql_store_result(&myConn);
	SAFEFREERESULT(pQueryResult);
	
	if(nBankItems == 0) return;
	
	string itemQuery;
	itemQuery.clear();
	itemQuery.append("INSERT INTO `item` ( `CharID` , `ItemName`, `Count` , `ItemType`, `ID1`, `ID2`, `ID3`, \
						  `Color`, `Effect1`, `Effect2`, `Effect3`, `LifeSpan`, `Attribute`, `TempItemAttr`, `ItemID`, `ItemPos`) VALUES ");

	cItem * item = new cItem;
	for(WORD w = 0; w < nBankItems; w++){
		ZeroMemory(item->ItemName, sizeof(item->ItemName));
		SafeCopy(item->ItemName, data, 20);
		data += 20;
		if(strlen(item->ItemName) == 0) continue;

		Pop(data, item->ItemCount);
		Pop(data, (uint16&)item->TouchEffectType);
		Pop(data, (uint16&)item->TouchEffectValue1);
		Pop(data, (uint16&)item->TouchEffectValue2);
		Pop(data, (uint16&)item->TouchEffectValue3);
		Pop(data, (uint8&)item->ItemColor);
		Pop(data, (uint16&)item->ItemSpecEffectValue1);
		Pop(data, (uint16&)item->ItemSpecEffectValue2);
		Pop(data, (uint16&)item->ItemSpecEffectValue3);
		Pop(data, (uint16&)item->CurLifeSpan);
		Pop(data, (uint32&)item->Attribute);
		Pop(data, (uint32&)item->TempItemAttr);
		Pop(data, (uint64&)item->ItemUniqueID);
		if(item->ItemUniqueID == 0){
			item->ItemUniqueID = CurItemID;
			CurItemID++;
		}

		if(w > 0) itemQuery.append(",");
		sprintf(query, "( %lu, '%s',%lu,%d ,%d,%d,%d,%d,%d,%d,%d,%u ,%lu ,%lu ,%I64u, 'GWH')",
			guildGUID ,item->ItemName,item->ItemCount, item->TouchEffectType, item->TouchEffectValue1, item->TouchEffectValue2, item->TouchEffectValue3, 
			item->ItemColor, item->ItemSpecEffectValue1, item->ItemSpecEffectValue2, item->ItemSpecEffectValue3, item->CurLifeSpan, item->Attribute, item->TempItemAttr, 
			item->ItemUniqueID);
		itemQuery.append(query);
		DeleteItemOnDB(item, guildGUID, myConn);
	}
	itemQuery.append(";");
	if(ProcessQuery(&myConn, (char *) itemQuery.c_str()) == -1) return;
	pQueryResult = mysql_store_result(&myConn);
	SAFEFREERESULT(pQueryResult);
}

void CLoginServer::ProcessRequest_GuildInfo(char * data, BYTE GSID, MYSQL myConn)
{
	char sendData[20];

	char GuildName[25], GoodGuildName[50];

	ZeroMemory(GuildName, sizeof(GuildName));
	SafeCopy(GuildName, data, 20);
	ZeroMemory(GoodGuildName, sizeof(GoodGuildName));
	MakeGoodName(GoodGuildName, GuildName);
	data += 20;

	uint16 guildID;

	Pop(data, guildID);


	char * index = (char*)_sendBuff;

	Push(index, (uint32)MSGID_RESPONSE_LOAD_GUILDINFO);
	Push(index, guildID);
	Push(index, GuildName, 20);


	char QueryConsult[300];
	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	sprintf(QueryConsult, "SELECT WHLvl, SummonsLvl, CaptaincyLvl, RaidmastersLvl, HuntmastersLvl, Gold, Maj, Cont, \
								 Captains, Raidmasters, Huntmasters FROM `guild` WHERE `GuildName` = '%s' LIMIT 1;", 
								 GoodGuildName);
	if(ProcessQuery(&myConn, QueryConsult) == -1) return;
	st_mysql_res * QueryResult = mysql_store_result(&myConn);
	uint8 NFields = (uint8)mysql_num_fields(QueryResult);
	if(mysql_num_rows(QueryResult) == 0) return;

	uint32 FIB_WHLevel, FIB_summonsLevel, FIB_captaincyLevel, FIB_raidmastersLevel;
	uint32 FIB_huntmastersLevel, FIB_gold, FIB_maj, FIB_cont, FIB_captains, FIB_raidmasters, FIB_huntmasters;
	MYSQL_FIELD *field[100];
	mysql_field_seek(QueryResult, 0);

	for(BYTE b = 0; b < NFields; b++){
		field[b] = mysql_fetch_field(QueryResult);
		if(IsSame(field[b]->name, "WHLvl")) FIB_WHLevel = b;
		else if(IsSame(field[b]->name, "SummonsLvl")) FIB_summonsLevel = b;
		else if(IsSame(field[b]->name, "CaptaincyLvl")) FIB_captaincyLevel = b;
		else if(IsSame(field[b]->name, "RaidmastersLvl")) FIB_raidmastersLevel = b;
		else if(IsSame(field[b]->name, "HuntmastersLvl")) FIB_huntmastersLevel = b;
		else if(IsSame(field[b]->name, "Gold")) FIB_gold = b;
		else if(IsSame(field[b]->name, "Maj")) FIB_maj = b;
		else if(IsSame(field[b]->name, "Cont")) FIB_cont = b;
		else if(IsSame(field[b]->name, "Captains")) FIB_captains = b;
		else if(IsSame(field[b]->name, "Raidmasters")) FIB_raidmasters = b;
		else if(IsSame(field[b]->name, "Huntmasters")) FIB_huntmasters = b;
	}

	MYSQL_ROW row = mysql_fetch_row(QueryResult);
	
	uint8 WHLevel = atoul(row[FIB_WHLevel]);
	Push(index, WHLevel);
	Push(index, (uint8)atoul(row[FIB_summonsLevel]) );
	Push(index, (uint8)atoul(row[FIB_captaincyLevel]) );
	Push(index, (uint8)atoul(row[FIB_raidmastersLevel]) );
	Push(index, (uint8)atoul(row[FIB_huntmastersLevel]) );
	Push(index, atoul(row[FIB_gold]) );
	Push(index, atoul(row[FIB_maj]) );
	Push(index, atoul(row[FIB_cont]) );
	Push(index, (uint8)atoul(row[FIB_captains]) );
	Push(index, (uint8)atoul(row[FIB_raidmasters]) );
	Push(index, (uint8)atoul(row[FIB_huntmasters]) );


	if(WHLevel > 0 && WHLevel <= gldUps[GU_WAREHOUSE].maxLvl)
	{
		ZeroMemory(QueryConsult, sizeof(QueryConsult));
		sprintf(QueryConsult, "SELECT * FROM `item` WHERE `CharID` = %lu AND `ItemPos` = 'GWH' \
									 ORDER BY `ItemName` ASC LIMIT %u;", guildID, maxGWHItems[WHLevel]);
		if(ProcessQuery(&myConn, QueryConsult) == -1) return;
		QueryResult = mysql_store_result(&myConn);
		NFields = (uint8)mysql_num_fields(QueryResult);
		uint16 NBankItems = (uint16)mysql_num_rows(QueryResult);

		Push(index, NBankItems);

		if(NBankItems > 0)
		{
			uint8 FIBItemName, FIBItemCount, FIBItemType, FIBID1, FIBID2, FIBID3, FIBItemColor, FIBItemID;
			uint8 FIBItemEffect1, FIBItemEffect2, FIBItemEffect3, FIBItemLifeSpan, FIBItemAttribute, FIBTempItemAttr;      //variables for field index
			mysql_field_seek(QueryResult, 0);
			for(BYTE b = 0; b < NFields; b++){
				field[b] = mysql_fetch_field(QueryResult);
				if(IsSame(field[b]->name, "ItemName")) FIBItemName = b;
				else if(IsSame(field[b]->name, "Count")) FIBItemCount = b;
				else if(IsSame(field[b]->name, "ItemType")) FIBItemType = b;
				else if(IsSame(field[b]->name, "ID1")) FIBID1 = b;
				else if(IsSame(field[b]->name, "ID2")) FIBID2 = b;
				else if(IsSame(field[b]->name, "ID3")) FIBID3 = b;
				else if(IsSame(field[b]->name, "Color")) FIBItemColor = b;
				else if(IsSame(field[b]->name, "Effect1")) FIBItemEffect1 = b;
				else if(IsSame(field[b]->name, "Effect2")) FIBItemEffect2 = b;
				else if(IsSame(field[b]->name, "Effect3")) FIBItemEffect3 = b;
				else if(IsSame(field[b]->name, "LifeSpan")) FIBItemLifeSpan = b;
				else if(IsSame(field[b]->name, "Attribute")) FIBItemAttribute = b;
				else if(IsSame(field[b]->name, "TempItemAttr")) FIBTempItemAttr = b;
				else if(IsSame(field[b]->name, "ItemID")) FIBItemID = b;
			}

			MYSQL_ROW BankItemRow[MAXGUILDBANKITEMS];
			for(int b = 0; b < NBankItems; b++){
				MYSQL_ROW bankRow = BankItemRow[b] = mysql_fetch_row(QueryResult);
				SafeCopy(index, bankRow[FIBItemName], strlen(bankRow[FIBItemName]));
				index += 20;

				Push(index, atoul(bankRow[FIBItemCount]));
				Push(index, (uint16)atoi(bankRow[FIBItemType]));
				Push(index, (uint16)atoi(bankRow[FIBID1]));
				Push(index, (uint16)atoi(bankRow[FIBID2]));
				Push(index, (uint16)atoi(bankRow[FIBID3]));
				Push(index, (uint8)atoi(bankRow[FIBItemColor]));
				Push(index, (uint16)atoi(bankRow[FIBItemEffect1]));
				Push(index, (uint16)atoi(bankRow[FIBItemEffect2]));
				Push(index, (uint16)atoi(bankRow[FIBItemEffect3]));
				Push(index, atoul(bankRow[FIBItemLifeSpan]));
				Push(index, atoul(bankRow[FIBItemAttribute]));
				Push(index, atoul(bankRow[FIBTempItemAttr]));
				Push(index, atoull(bankRow[FIBItemID]));
			}
		}
	}

	SendMsgToGS(GSID, _sendBuff, index - _sendBuff);
}
//=============================================================================
void CLoginServer::ProcessShutdown(DWORD dwTime)
{
 char SendBuff[10];
 WORD w;
	
	for(w=0; w<MAXGAMESERVERS; w++) {
		if(GameServer[w] != NULL) break;
		else if(w == MAXGAMESERVERS-1){
			bServersBeingShutdown = FALSE;
			bShutDownMsgIndex = 0;
			return;
		}
	}
	if((dwTime - dwShutdownInterval)>SHUTDOWN_INTERVAL_MSG){
		dwShutdownInterval = dwTime;
		if(bShutDownMsgIndex == 0 || bShutDownMsgIndex == 1){
			PutLogList("(!!!) Sending server shutdown announcement!");
			ZeroMemory(SendBuff, sizeof(SendBuff));
			PutOffsetValue(SendBuff, 0, DWORDSIZE, MSGID_SENDSERVERSHUTDOWNMSG);
			PutOffsetValue(SendBuff, 6, WORDSIZE, bShutDownMsgIndex+1);			
			for(w=0; w<MAXGAMESERVERS; w++) if(GameServer[w] != NULL && GameServer[w]->IsSocketConnected){
				GameServer[w]->SendMsg(SendBuff, 8, NULL, NULL);
			}
		}
		else{
			PutLogList("(!!!) Shutting down all the servers!");
			ZeroMemory(SendBuff, sizeof(SendBuff));
			PutOffsetValue(SendBuff, 0, DWORDSIZE, MSGID_GAMESERVERSHUTDOWNED);
			for(w=0; w<MAXGAMESERVERS; w++) if(GameServer[w] != NULL && GameServer[w]->IsSocketConnected){
				GameServer[w]->SendMsg(SendBuff, 4, NULL, NULL);
			}
		}
		bShutDownMsgIndex++;
	}
}
//=============================================================================
void CLoginServer::ServerStockMsgHandler(char *pData, BYTE ID)
{
 char * cp, cName[12], SendBuff[500];
 WORD InternalID, w, w2;
 BOOL bFlag = FALSE;
 int iTotal;
 
	iTotal = 0;
	cp = (char *)(pData + 6);
	while (bFlag == FALSE) {
		iTotal++;
		switch (*cp) {
		
		case GSM_DISCONNECT:
			cp++;
			ZeroMemory(cName, sizeof(cName));
			SafeCopy(cName, cp, 10);
			cp += 10;
			for(w = 0; w < MAXCLIENTS; w++) if(Client[w] != NULL && IsSame(Client[w]->CharName, cName)){ Client[w]->IsPlaying = FALSE; return;}
			SAFEDELETE(Client[w]);
		break;

		case GSM_REQUEST_FINDCHARACTER:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 25);
			cp++;
			InternalID = wGetOffsetValue(cp, 0);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServer[w] != NULL && GameServer[w]->InternalID != InternalID) GameServer[w]->bStockMsgToGameServer(SendBuff, 25);
			cp += 24;
		break;

		case GSM_RESPONSE_FINDCHARACTER:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 39);
			cp++;
			InternalID = wGetOffsetValue(cp, 0);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServer[w] != NULL && GameServer[w]->InternalID == InternalID){
					GameServer[w]->bStockMsgToGameServer(SendBuff, 39);
					break;
				}
			cp+=38;
		break;

		case GSM_REQUEST_FINDFRIEND:
			w2 = wGetOffsetValue(cp, 15);
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, w2+17);
			cp += w2+17;
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, w2+17);
		break;

		case GSM_RESPONSE_FINDFRIEND:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 25);
			cp++;
			InternalID = wGetOffsetValue(cp, 0);
			for(w = 0; w < MAXGAMESERVERS; w++){
				if(GameServer[w] != NULL && GameServer[w]->InternalID == InternalID){
					GameServer[w]->bStockMsgToGameServer(SendBuff, 25);
					break;
				}
			}
			cp += 24;
		break;

		case GSM_CHATMSG:
			w2 = wGetOffsetValue(cp, 16);
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, w2+18);
			cp += w2+18;
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, w2+18);
		break;

		case GSM_WHISPERMSG:
			w2 = wGetOffsetValue(cp, 11);
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, w2+13);
			cp += w2+13;
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, w2+13);
		break;

		case GSM_REQUEST_SHUTUPPLAYER:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 27);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, 27);
			cp += 27;			
		break;
		
		case GSM_RESPONSE_SHUTUPPLAYER:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 27);
			InternalID = wGetOffsetValue(cp, 1);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServer[w] != NULL && GameServer[w]->InternalID == InternalID){
					GameServer[w]->bStockMsgToGameServer(SendBuff, 27);
					break;
				}
			cp += 27;
		break;

		case GSM_BEGINCRUSADE:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 5);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, 5);
			cp += 5;
		break;

			case GSM_BEGINAPOCALYPSE:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 5);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, 5);
			cp += 5;
		break;
	case GSM_ENDAPOCALYPSE:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 18);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, 18);
			cp += 18;
		break;

		case GSM_ENDCRUSADE:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 18);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, 18);
			cp += 18;
		break;

		case GSM_CONSTRUCTIONPOINT:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 9);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, 9);
			cp += 9;
		break;

		/*case GSM_REQUEST_SUMMONALL:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 25);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, 25);
			cp += 25;
		break;*/

		case GSM_REQUEST_SUMMONPLAYER:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 25);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, 25);
			cp += 25;
		break;
	case GSM_REQUEST_SUMMONGUILD:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 25);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, 25);
			cp += 35;
		break;
		case GSM_SETGUILDTELEPORTLOC:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 23);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, 23);
			cp += 23;
		break;

		case GSM_SETGUILDCONSTRUCTLOC:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 23);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, 23);
			cp += 23;
		break;

		case GSM_MIDDLEMAPSTATUS:
			w2 = wGetOffsetValue(cp, 1);;
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(cp, 3+(w2*6));
			cp += 3+(w2*6);
		break;

		case GSM_REQUEST_SETFORCERECALLTIME:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 3);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, 3);
			cp += 3;
		break;

		case GSM_COLLECTEDMANA:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 5);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, 5);
			cp += 5;
		break;

		case GSM_GRANDMAGICRESULT:
			w2 = wGetOffsetValue(cp, 19);
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 18+((w2+1)*2));
			cp += 18+((w2+1)*2);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, 18+((w2+1)*2));
		break;

		/*case GSM_BEGINAPOCALYPSE:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 5);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, 5);
			cp += 5;
		break;*/
		
		/*case GSM_ENDAPOCALYPSE:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 5);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, 5);
			cp += 5;
		break;*/

		case GSM_GRANDMAGICLAUNCH:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 5);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, 5);
			cp += 5;
		break;
		
		/*case GSM_ENDHELDENIAN:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 1);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, 1);
			cp += 1;
		break;

		case GSM_STARTHELDENIAN:
			ZeroMemory(SendBuff, sizeof(SendBuff));
			SafeCopy(SendBuff, cp, 9);
			for(w = 0; w < MAXGAMESERVERS; w++) 
				if(GameServerSocket[ID] != NULL && GameServerSocket[ID]->GSID != w && GameServer[w] != NULL) GameServer[w]->bStockMsgToGameServer(SendBuff, 9);
			cp += 9;
		break;*/

		default:
			bFlag = TRUE;
			break;
		}
	}
}
//=============================================================================
int CLoginServer::ProcessQuery(MYSQL *myConn, char *cQuery)
{
 BYTE bErrorCount = 0;
 int iQuery = -1;
 UINT uiLastError;

	do {
		if(mysql_ping(myConn) != 0){
			mysql_close(myConn);
			mysql_init(myConn);
			mysql_real_connect(myConn, mySqlAddress, mySqlUser, mySqlPwd, "playerdb2", mySqlPort, NULL, NULL);
		}			
		else iQuery = mysql_query(myConn, cQuery);
		bErrorCount++;
		uiLastError = MyAux_Get_Error(myConn);
		if(uiLastError) Delay(1000);
	}
	while(uiLastError != NULL && bErrorCount < MAXALLOWEDQUERYERROR);
	if(bErrorCount == MAXALLOWEDQUERYERROR){
		PutLogFileList(cQuery, QUERYERROR_LOGFILE);
		return -1;
	}
	return iQuery;
}
//=============================================================================

void CLoginServer::CreateNewAccount(char *Data, WORD ClientID, MYSQL myConn)
{
	char AccountName[15], NewAccName[25], GoodAccName[25], NewAcc1[15], CreateDate[25], NewEmailAddr[51], safeEmailAddr[91], safeAccQuiz[91], safeAnswer[41];
	char NewAccountQuiz[46], NewAccountAnswer[21], GoodPass[25], NewPass1[15], NewPass2[15], QueryConsult[500], pQueryConsult[500];
	char *pBuff = Data;
	DWORD *dwp, dwID;
	st_mysql_res    *QueryResult = NULL;
	WORD *wp, NRows, w;
	char Txt100[100], Txt500[500];
	MYSQL_FIELD *field[2];
	MYSQL_ROW myRow;
	BYTE b, NFields;
	_ADDRESS ClientIP;
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);

	ZeroMemory(Txt100, sizeof(Txt100));
	ZeroMemory(NewEmailAddr, sizeof(NewEmailAddr));
	ZeroMemory(NewAccountQuiz, sizeof(NewAccountQuiz));
	ZeroMemory(NewAccountAnswer, sizeof(NewAccountAnswer));
	sprintf(Txt100, "(!) Create acc in progress.");
	PutLogList(Txt100);

	SafeCopy(NewAcc1, Data, 10);
	SafeCopy(NewPass1, Data+10, 10);
	SafeCopy(NewEmailAddr, Data+20, 50);
	SafeCopy(NewAccountQuiz, Data+125, 45);	//SafeCopy(NewAccountQuiz, Data+143, 45);
	SafeCopy(NewAccountAnswer, Data+170, 20);	//SafeCopy(NewAccountAnswer, Data+188, 20);

	ZeroMemory(ClientIP, sizeof(ClientIP));
	ClientSocket[ClientID]->iGetPeerAddress(ClientIP);
	ZeroMemory(CreateDate, sizeof(CreateDate));
	sprintf(CreateDate, "%d-%d-%d %d:%d:%d", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);


	ZeroMemory(Txt500, sizeof(Txt500));
	dwp = (DWORD*)Txt500;
	*dwp = MSGID_RESPONSE_LOG;

	if(AccountExists(NewAcc1, myConn)){
		wp = (WORD*)(Txt500+4);
		*wp = LOGRESMSGTYPE_ALREADYEXISTINGACCOUNT;
		SendMsgToClient(ClientID, Txt500, 6);
	} else {
		ZeroMemory(QueryConsult, sizeof(QueryConsult));
		ZeroMemory(NewAccName, sizeof(NewAccName));
		MakeGoodName(NewAccName, NewAcc1);

		ZeroMemory(GoodPass, sizeof(GoodPass));
		MakeGoodName(GoodPass, NewPass1);

		ZeroMemory(safeEmailAddr, sizeof(safeEmailAddr));
		MakeGoodName(safeEmailAddr, NewEmailAddr);

		ZeroMemory(safeAccQuiz, sizeof(safeAccQuiz));
		MakeGoodName(safeAccQuiz, NewAccountQuiz);

		ZeroMemory(safeAnswer, sizeof(safeAnswer));
		MakeGoodName(safeAnswer, NewAccountAnswer);
	    
		sprintf(QueryConsult, "INSERT INTO `account_database` SET `name` = '%s' , `password` = '%s', `LoginIpAddress` = '%s', `CreateDate` = '%s', `Email` = '%s', `Quiz` = '%s', `Answer` = '%s';", NewAccName,  GoodPass, ClientIP, CreateDate, safeEmailAddr, safeAccQuiz, safeAnswer);

		if(ProcessQuery(&myConn, QueryConsult) == -1) return;
		QueryResult = mysql_store_result(&myConn);
		SAFEFREERESULT(QueryResult);
		wp = (WORD*)(Txt500+4);
		*wp = LOGRESMSGTYPE_NEWACCOUNTCREATED;
		SendMsgToClient(ClientID, Txt500, 6);
	}
	SAFEFREERESULT(QueryResult);
}
BOOL CLoginServer::AccountExists(char *AccountName, MYSQL myConn)
{
	char QueryConsult[150], GoodAccountName[50];
	st_mysql_res *QueryResult = NULL;
	MYSQL_ROW Row;
	char Txt100[100];

	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	ZeroMemory(GoodAccountName, sizeof(GoodAccountName));
	MakeGoodName(GoodAccountName, AccountName);
	sprintf(QueryConsult, "SELECT `name` FROM `account_database` WHERE `name` = '%s' LIMIT 1;", GoodAccountName);
	if(ProcessQuery(&myConn, QueryConsult) == -1) return FALSE; 
	QueryResult = mysql_store_result(&myConn);
       
	if(mysql_num_rows(QueryResult) == 1){
		Row = mysql_fetch_row(QueryResult);
		*AccountName = atoi(Row[0]);
		SAFEFREERESULT(QueryResult);
		return TRUE;
	}
	SAFEFREERESULT(QueryResult);
	return FALSE;
}

void CLoginServer::RequestLegionPts(char * data, WORD GSID, MYSQL myConn)
{
	char sendData[20];
	long cash = 0;

	WORD clientH = *(WORD *)data;
	char account[11];
	ZeroMemory(account, sizeof(account));
	memcpy(account, data+2,10);
	
	cash = GetAccountCash(myConn, account);

	ZeroMemory(sendData, sizeof(sendData));
	DWORD * dwp  = (DWORD*)sendData;
	*dwp = MSGID_RESPONSE_LGNPTS;
	WORD * wp = (WORD*)(sendData+4);
	*wp = LOGRESMSGTYPE_CONFIRM;
	wp++;
	*wp = clientH;
	dwp = (DWORD*)(sendData+8);
	*dwp = cash;
	SendMsgToGS(GSID, sendData, 12);
}

BOOL CLoginServer::NameChange(char *AccountName, char *NewName, MYSQL myConn)
{	st_mysql_res *QueryResult = NULL;
	char QueryConsult[500], Txt[200], GoodAccount[25], GoodName[25];
	WORD AccountID;
 	 
	if (!IsAccountInUse(AccountName, &AccountID)) return false;

	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	ZeroMemory(GoodAccount, sizeof(GoodAccount));
	MakeGoodName(GoodAccount, AccountName);
	ZeroMemory(GoodName, sizeof(GoodName));
	MakeGoodName(GoodName, NewName);
	sprintf(QueryConsult, "SELECT * FROM `char_database` WHERE `char_name` = '%s';", GoodName);
    if(ProcessQuery(&myConn, QueryConsult) == -1) return false;
    QueryResult = mysql_store_result(&myConn);
	if(mysql_num_rows(QueryResult) > 0){
		SAFEFREERESULT(QueryResult);
		return false;
	}
	SAFEFREERESULT(QueryResult);

	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	sprintf(QueryConsult, "UPDATE `account_database` SET `cash` = cash-25 WHERE `name` = '%s' LIMIT 1;", GoodAccount);
	if(ProcessQuery(&myConn, QueryConsult) == -1) return false;
	QueryResult = mysql_store_result(&myConn);
	SAFEFREERESULT(QueryResult);
	
	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	sprintf(QueryConsult, "UPDATE `char_database` SET `char_name` = '%s' WHERE `account_name` = '%s' AND `char_name` = '%s' LIMIT 1;", GoodName, GoodAccount, Client[AccountID]->CharName);
	if(ProcessQuery(&myConn, QueryConsult) == -1) return false;
	QueryResult = mysql_store_result(&myConn);
	SAFEFREERESULT(QueryResult);
	
	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	sprintf(QueryConsult, "UPDATE `guild` SET `MasterName` = '%s' WHERE `MasterName` = '%s' LIMIT 1;", GoodName, Client[AccountID]->CharName);
	if(ProcessQuery(&myConn, QueryConsult) != -1) {
		QueryResult = mysql_store_result(&myConn);
		SAFEFREERESULT(QueryResult);
	}
	
	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	sprintf(QueryConsult, "UPDATE `guild_member` SET `MemberName` = '%s' WHERE `MemberName` = '%s' LIMIT 1;", GoodName, Client[AccountID]->CharName);
	if(ProcessQuery(&myConn, QueryConsult) != -1) {
		QueryResult = mysql_store_result(&myConn);
		SAFEFREERESULT(QueryResult);
	}
	
	ZeroMemory(Client[AccountID]->CharName, sizeof(Client[AccountID]->CharName));
	memcpy(Client[AccountID]->CharName, NewName, 10);
	
	return true;
}

void CLoginServer::RequestLegionSvc(char * data, WORD GSID, MYSQL myConn)
{
	char *cp, sendData[20], cNewName[11], type, sex, cSize;
	long cash = 0;
	WORD *wp;

	WORD cmd = *(WORD *)data;
	cp = (char *)(data+2);
	type = *cp;
	cp++;

	if (cmd == CMD_LGNSVC_NAMECHANGE) {
		memcpy(cNewName, cp, 10);
		cp += 10;
	}else {
		sex = *cp;
		cp++;
	}

	if (type >= 4) return;

	int i=0;
	while(lgnPtsSvcs[type][i].price && 
		lgnPtsSvcs[type][i].cmd != cmd) 
	{
		i++;
		if (i >= sizeof(lgnPtsSvcs[type])) return;
	}

	wp = (WORD *)cp;
	WORD clientH = *wp;
	cp += 2;

	char account[11];
	ZeroMemory(account, sizeof(account));
	memcpy(account, cp, 10);
	cp += 10;

	cash = GetAccountCash(myConn, account);
	if(cash < lgnPtsSvcs[type][i].price && (cmd < CMD_LGNSVC_TRADETOKEN1 || cmd > CMD_LGNSVC_TRADETOKEN100))
		return;

	if (cmd == CMD_LGNSVC_NAMECHANGE) {
		if (!NameChange(account, cNewName, myConn)) return;
	}

	ZeroMemory(sendData, sizeof(sendData));
	DWORD * dwp  = (DWORD*)sendData;
	*dwp = MSGID_RESPONSE_LGNSVC;
	wp = (WORD*)(sendData+4);
	*wp = cmd;
	cp = (char *)(sendData+6);
	*cp = type;
	cp++;
	if (cmd == CMD_LGNSVC_NAMECHANGE) {
		memcpy(cp, cNewName, 10);
		cp += 10;
		cSize = 19;
	}else {
		*cp = sex;
		cp++;
		cSize = 10;
	}
	wp = (WORD*)cp;
	*wp = clientH;
	cp += 2;

	SendMsgToGS(GSID, sendData, cSize);
}

long CLoginServer::GetAccountCash(MYSQL myConn, char * account)
{
	char QueryConsult[150], GoodAccName[25];
	st_mysql_res *QueryResult = NULL;
	MYSQL_ROW Row;
	long cash;

	ZeroMemory(GoodAccName, sizeof(GoodAccName));
	MakeGoodName(GoodAccName, account);
	sprintf(QueryConsult, "SELECT `cash` FROM `account_database` WHERE `name` = '%s' LIMIT 1;", GoodAccName);
	if(ProcessQuery(&myConn, QueryConsult) == -1) return 0; // assertion
	QueryResult = mysql_store_result(&myConn);

	if(mysql_num_rows(QueryResult) != 1) // assertion
	{
		SAFEFREERESULT(QueryResult);
		return 0;
	}

	Row = mysql_fetch_row(QueryResult);
	cash = atoi(Row[0]);
	SAFEFREERESULT(QueryResult);

	return cash;
}

void CLoginServer::SubAccountCash(char * data, MYSQL myConn)
{
	char QueryConsult[150], GoodAccName[25];
	st_mysql_res *QueryResult = NULL;
	MYSQL_ROW Row;

	char account[11];
	ZeroMemory(account, sizeof(account));
	memcpy(account, data, 10);

	char type = *(char*)(data+10);
	char i = *(char*)(data+11);
	
	if (type >= 4) return;
	//if (type >= 0 && i >= sizeof(lgnPtsSvcs[type])) return;
	//if (type >= 0 && i >= CMD_LGNSVC_MAX) return;
	if (type < -1) return;
	if (type == -1 && i <= 0) return;

	switch(type) {
	case 0: if (i >= CMD_LGNSVC_MAXWEAPONS) return; break;
	case 1: if (i >= CMD_LGNSVC_MAXARMORS) return; break;
	case 2: if (i >= CMD_LGNSVC_MAXMISC) return; break;
	case 3: if (i >= CMD_LGNSVC_MAXSERVICES) return; break;
	}

	ZeroMemory(GoodAccName, sizeof(GoodAccName));
	MakeGoodName(GoodAccName, account);

	if (type == -1)
	sprintf(QueryConsult, "UPDATE `account_database` SET `cash`=(cash-%d) WHERE `name` = '%s' LIMIT 1;", i, GoodAccName);
	else 
	sprintf(QueryConsult, "UPDATE `account_database` SET `cash`=(cash-%lu) WHERE `name` = '%s' LIMIT 1;", lgnPtsSvcs[type][i].price, GoodAccName);

	if(ProcessQuery(&myConn, QueryConsult) == -1) return; // assertion
	QueryResult = mysql_store_result(&myConn);
	SAFEFREERESULT(QueryResult);
	
	if (type != -1) {
		sprintf(QueryConsult, "INSERT INTO `cash_transactions` (account_name, service, date) \
							  VALUES ('%s', '%s', NOW());", GoodAccName, lgnPtsSvcs[type][i].name);

		if(ProcessQuery(&myConn, QueryConsult) == -1) return; // assertion
		QueryResult = mysql_store_result(&myConn);
		SAFEFREERESULT(QueryResult);
	}
}
void CLoginServer::ReqAuctionAction(char *data, WORD GSID, MYSQL myConn)
{
	char *cp, sendData[2000], QueryConsult[200];
	st_mysql_res *pQueryResult = NULL;
	int iSize = 0;
	BYTE NItems, NFields;
	MYSQL_FIELD *field[100];
	MYSQL_ROW ItemRow;

	char cAction = *(char *)data;
	DWORD dwAuctionItemID = *(DWORD *)(data+1);
	DWORD dwBidAmount = *(DWORD *)(data+5);
	WORD clientH = *(WORD *)(data+9);
	
	if (dwAuctionItemID <= 0) return;
	if (dwBidAmount <= 0) return;

	char account[11], character[11];
	memcpy(account, data+11, 10);
	memcpy(character, data+21, 10);

	WORD AccountID = GetAccountID(account, myConn);
	if (AccountID == 0) return;
	
	WORD CharID = GetCharID(character, account, myConn);
	if (CharID == 0) return;

	ZeroMemory(sendData, sizeof(sendData));
	DWORD * dwp = (DWORD*)sendData;
	*dwp = MSGID_RESPONSE_AUCTION_ACTION;
	WORD * wp = (WORD*)(sendData+4);
	memcpy(sendData+6, data, 11);
	cp = (char *)(sendData+17);

	switch(cAction) {
	case 0: // bid
		sprintf(QueryConsult, "SELECT * FROM `auction_items` WHERE `AuctionItemID` = %lu AND `AuctionStatus` = 0 AND (`HighestBidAmount` = 0 OR `HighestBidAmount` < %lu) LIMIT 1;", dwAuctionItemID, dwBidAmount);
		if(ProcessQuery(&myConn, QueryConsult) == -1) {
			SAFEFREERESULT(pQueryResult);
			*wp = LOGRESMSGTYPE_REJECT;
			*cp = 0; // "This item is no longer available for auction."
			cp++;
			break;
		}
		SAFEFREERESULT(pQueryResult);
		pQueryResult = mysql_store_result(&myConn);
		NFields = (BYTE)mysql_num_fields(pQueryResult);
		NItems = (BYTE)mysql_num_rows(pQueryResult);
		if(NItems == 1)
		{
			BYTE FIAuctionItemID;
			mysql_field_seek(pQueryResult, 0);
			ItemRow = mysql_fetch_row(pQueryResult);
			for(BYTE b = 0; b < NFields; b++){
				field[b] = mysql_fetch_field(pQueryResult);
				if(IsSame(field[b]->name, "AuctionItemID")) atoul(ItemRow[b]);
			}
		}else {
			*wp = LOGRESMSGTYPE_REJECT;
			*cp = 0; // "This item is no longer available for auction."
			cp++;
		}
		break;
	case 1: // buyout
		break;
	default:
		return;
	}

	*wp = LOGRESMSGTYPE_CONFIRM;
	*wp = LOGRESMSGTYPE_REJECT;

	SendMsgToGS(GSID, sendData, 17+iSize);
}

void CLoginServer::ReqAuctionItemList(char *data, WORD GSID, MYSQL myConn)
{
	char sendData[2000];

	char cType = *(char *)data;
	char cPage = *(char *)(data+1);
	int clientH = *(int *)(data+2);

	char account[11], character[11];
	memcpy(account, data+6, 10);
	memcpy(character, data+16, 10);
	
	WORD AccountID = GetAccountID(account, myConn);
	if (AccountID == 0) return;
	
	WORD CharID = GetCharID(character, account, myConn);
	if (CharID == 0) return;

	ZeroMemory(sendData, sizeof(sendData));
	DWORD * dwp = (DWORD*)sendData;
	*dwp = MSGID_RESPONSE_AUCTIONITEMLIST;
	WORD * wp = (WORD*)(sendData+4);
	*wp = LOGRESMSGTYPE_CONFIRM;
	dwp = (DWORD*)(sendData+6);
	*dwp = clientH;

	char *cp = (char *)(sendData+10);

	int iSize = iComposeAuctionItemListData(cType, cp, myConn, AccountID, CharID, cPage);

	SendMsgToGS(GSID, sendData, 10+iSize);
}

void CLoginServer::AddAuctionItem(char *data, WORD GSID, MYSQL myConn)
{
	char *cp, QueryConsult[200], sendData[2000];
	st_mysql_res *pQueryResult = NULL;
	MYSQL_ROW Row;
	string itemQuery;

	int iMinBidAmount = *(int *)data;
	int iBuyoutAmount = *(int *)(data+4);
	//int iCount = *(int *)(data+8);

	char account[11], character[11];
	ZeroMemory(account, sizeof(account));
	ZeroMemory(character, sizeof(character));
	int clientH = *(int *)(data+8);
	memcpy(account, data+12, 10);
	memcpy(character, data+22, 10);
	DWORD dwEndTime = timeGetTime()+(24 _h);

	cp = (data+32);

	cItem *ItemInfo = new cItem;
	ZeroMemory(ItemInfo->ItemName, sizeof(ItemInfo->ItemName));
	SafeCopy(ItemInfo->ItemName, cp, 20);
	if(strlen(ItemInfo->ItemName) == 0) return;
	ItemInfo->ItemCount = dwGetOffsetValue(cp, 20);
	ItemInfo->TouchEffectType = (sWORD)wGetOffsetValue(cp, 24);
	ItemInfo->TouchEffectValue1 = (sWORD)wGetOffsetValue(cp, 26);
	ItemInfo->TouchEffectValue2 = (sWORD)wGetOffsetValue(cp, 30);
	ItemInfo->TouchEffectValue3 = (sWORD)dwGetOffsetValue(cp, 34);
	ItemInfo->ItemColor = bGetOffsetValue(cp, 38);
	ItemInfo->ItemSpecEffectValue1 = (sWORD)wGetOffsetValue(cp, 39);
	ItemInfo->ItemSpecEffectValue2 = (sWORD)wGetOffsetValue(cp, 41);
	ItemInfo->ItemSpecEffectValue3 = (sWORD)wGetOffsetValue(cp, 43);
	ItemInfo->CurLifeSpan = wGetOffsetValue(cp, 45);
	ItemInfo->Attribute = dwGetOffsetValue(cp, 47);
	ItemInfo->TempItemAttr = dwGetOffsetValue(cp, 51);
	ItemInfo->socket1 = bGetOffsetValue(cp, 55);
	ItemInfo->socket2 = bGetOffsetValue(cp, 56);
	ItemInfo->socket3 = bGetOffsetValue(cp, 57);
	ItemInfo->ItemUniqueID = ullGetOffsetValue(cp, 58);
	
	WORD AccountID = GetAccountID(account, myConn);
	if (AccountID == 0) return;
	
	WORD CharID = GetCharID(character, account, myConn);
	if (CharID == 0) return;

	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	itemQuery.append("INSERT INTO `auction_items` ( `AccountID` , `CharID` , `ItemName`, `Count` , `ItemType`, `ID1`, `ID2`, `ID3`, `Color`, \
		`Effect1`, `Effect2`, `Effect3`, `LifeSpan`, `Attribute`, `TempItemAttr`, `MinBidAmount`, `BuyoutAmount`, `EndTime`, `Socket1`, `Socket2`, `Socket3`, `ItemID`) VALUES ");
		
	sprintf(QueryConsult, "(%lu,%lu,'%s',%lu,%d,%d,%d,%d,%d,%d,%d,%d,%u,%lu,%lu,%lu,%lu,%lu,", 
		AccountID, CharID ,ItemInfo->ItemName,ItemInfo->ItemCount, ItemInfo->TouchEffectType, ItemInfo->TouchEffectValue1, ItemInfo->TouchEffectValue2, ItemInfo->TouchEffectValue3, ItemInfo->ItemColor, ItemInfo->ItemSpecEffectValue1, ItemInfo->ItemSpecEffectValue2, ItemInfo->ItemSpecEffectValue3, ItemInfo->CurLifeSpan, ItemInfo->Attribute, ItemInfo->TempItemAttr, iMinBidAmount, iBuyoutAmount, dwEndTime);
	itemQuery.append(QueryConsult);

	if(ItemInfo->socket1 != SG_NONE)
		sprintf(QueryConsult, "%u,", ItemInfo->socket1);
	else sprintf(QueryConsult, "NULL,", ItemInfo->socket1);
	itemQuery.append(QueryConsult);

	if(ItemInfo->socket2 != SG_NONE)
		sprintf(QueryConsult, "%u,", ItemInfo->socket2);
	else sprintf(QueryConsult, "NULL,", ItemInfo->socket2);
	itemQuery.append(QueryConsult);

	if(ItemInfo->socket3 != SG_NONE)
		sprintf(QueryConsult, "%u,", ItemInfo->socket3);
	else sprintf(QueryConsult, "NULL,", ItemInfo->socket3);
	itemQuery.append(QueryConsult);

	sprintf(QueryConsult, "%I64u);", ItemInfo->ItemUniqueID);
	itemQuery.append(QueryConsult);

	if(ProcessQuery(&myConn, (char *) itemQuery.c_str()) == -1) return;
	pQueryResult = mysql_store_result(&myConn);
	SAFEFREERESULT(pQueryResult);
	SAFEDELETE(ItemInfo);

	ZeroMemory(sendData, sizeof(sendData));
	DWORD * dwp  = (DWORD*)sendData;
	*dwp = MSGID_RESPONSE_AUCTIONITEMLIST;
	WORD * wp = (WORD*)(sendData+4);
	*wp = LOGRESMSGTYPE_CONFIRM;
	dwp = (DWORD*)(sendData+6);
	*dwp = clientH;
	
	cp = (char *)(sendData+10);

	int iSize = iComposeAuctionItemListData(2, cp, myConn, AccountID, CharID);

	SendMsgToGS(GSID, sendData, 10+iSize);
}

int CLoginServer::iComposeAuctionItemListData(char cType, char *pData, MYSQL myConn, WORD AccountID, WORD CharID, char cPage)
{
	char QueryConsult[200];
	st_mysql_res *pQueryResult = NULL, *pQueryResult2 = NULL;
	MYSQL_ROW Row;
	string itemQuery;
	BYTE NFields, NItems;
	WORD NTotal;
	MYSQL_FIELD *field[100];
	DWORD dwTime = timeGetTime();
	int iSize;
	string searchQuery;
	MYSQL_ROW ItemRow[MAXAUCTIONITEMS];

	ZeroMemory(QueryConsult, sizeof(QueryConsult));

	switch(cType) {
	case 0:
		sprintf(QueryConsult, "SELECT * FROM `auction_items`;");
		if(ProcessQuery(&myConn, QueryConsult) == -1) return 0;
		SAFEFREERESULT(pQueryResult);
		pQueryResult = mysql_store_result(&myConn);
		NTotal = (WORD)mysql_num_rows(pQueryResult);
		sprintf(QueryConsult, "SELECT * FROM `auction_items` LIMIT %d,%d;", cPage*MAXAUCTIONITEMS, MAXAUCTIONITEMS);
		searchQuery.append(QueryConsult);
		break;

	case 1:
		sprintf(QueryConsult, "SELECT `AuctionItemID` FROM `auction_bids` WHERE `CharID` = %lu;", CharID);
		if(ProcessQuery(&myConn, QueryConsult) == -1) return 0;
		SAFEFREERESULT(pQueryResult);
		pQueryResult = mysql_store_result(&myConn);
		NFields = (BYTE)mysql_num_fields(pQueryResult);
		NTotal = (WORD)mysql_num_rows(pQueryResult);
		if(NTotal > 0)
		{
			sprintf(QueryConsult, "SELECT * FROM `auction_items` WHERE ");
			searchQuery.append(QueryConsult);

			BYTE FIAuctionItemID;
			mysql_field_seek(pQueryResult, 0);
			for(BYTE b = 0; b < NFields; b++){
				field[b] = mysql_fetch_field(pQueryResult);
				if(IsSame(field[b]->name, "AuctionItemID")) FIAuctionItemID = b;
			}
			for(int b = 0; b < NTotal; b++){
				ItemRow[b] = mysql_fetch_row(pQueryResult);
				if (b == NTotal-1) sprintf(QueryConsult, "`AuctionItemID` = %lu ", atoul(ItemRow[b][FIAuctionItemID]));
				else sprintf(QueryConsult, "`AuctionItemID` = %lu OR ", atoul(ItemRow[b][FIAuctionItemID]));
				searchQuery.append(QueryConsult);
			}

			sprintf(QueryConsult, "LIMIT %d,%d;", cPage*MAXAUCTIONITEMS, MAXAUCTIONITEMS);
			searchQuery.append(QueryConsult);
		}
		break;

	case 2:
		sprintf(QueryConsult, "SELECT * FROM `auction_items` WHERE `AccountID` = %lu;", AccountID);
		if(ProcessQuery(&myConn, QueryConsult) == -1) return 0;
		SAFEFREERESULT(pQueryResult);
		pQueryResult = mysql_store_result(&myConn);
		NTotal = (WORD)mysql_num_rows(pQueryResult);
		sprintf(QueryConsult, "SELECT * FROM `auction_items` WHERE `AccountID` = %lu LIMIT %d;", AccountID, MAXAUCTIONITEMS);
		searchQuery.append(QueryConsult);
		break;

	default:
		return 0;
	}
	
	if (searchQuery.length() == 0) return 0;
	PutOffsetValue(pData, 0, WORDSIZE, NTotal);
	if(ProcessQuery(&myConn, (char *)searchQuery.c_str()) == -1) return 0;
	SAFEFREERESULT(pQueryResult);
	pQueryResult = mysql_store_result(&myConn);
	NFields = (BYTE)mysql_num_fields(pQueryResult);
	NItems = (BYTE)mysql_num_rows(pQueryResult);
	PutOffsetValue(pData, 2, BYTESIZE, NItems);
	if(NItems > 0)
	{
		DWORD dwEndTime, dwBuyerID;
		BYTE FIItemName, FIItemCount, FIItemType, FIID1, FIID2, FIID3, FIItemColor;
		BYTE FIItemEffect1, FIItemEffect2, FIItemEffect3, FIItemLifeSpan, FIItemAttribute, FITempItemAttr, FISocket1, FISocket2, FISocket3;
		BYTE FIItemID, FIBuyout, FIMinBid, FIEndTime, FIAuctionStatus, FIAuctionItemID, cAuctionStatus, FIBuyerName;
		mysql_field_seek(pQueryResult, 0);
		for(BYTE b = 0; b < NFields; b++){
			field[b] = mysql_fetch_field(pQueryResult);
			if(IsSame(field[b]->name, "ItemName")) FIItemName = b;
			else if(IsSame(field[b]->name, "Count")) FIItemCount = b;
			else if(IsSame(field[b]->name, "ItemType")) FIItemType = b;
			else if(IsSame(field[b]->name, "ID1")) FIID1 = b;
			else if(IsSame(field[b]->name, "ID2")) FIID2 = b;
			else if(IsSame(field[b]->name, "ID3")) FIID3 = b;
			else if(IsSame(field[b]->name, "Color")) FIItemColor = b;
			else if(IsSame(field[b]->name, "Effect1")) FIItemEffect1 = b;
			else if(IsSame(field[b]->name, "Effect2")) FIItemEffect2 = b;
			else if(IsSame(field[b]->name, "Effect3")) FIItemEffect3 = b;
			else if(IsSame(field[b]->name, "LifeSpan")) FIItemLifeSpan = b;
			else if(IsSame(field[b]->name, "Attribute")) FIItemAttribute = b;
			else if(IsSame(field[b]->name, "TempItemAttr")) FITempItemAttr = b;
			else if(IsSame(field[b]->name, "Socket1")) FISocket1 = b;
			else if(IsSame(field[b]->name, "Socket2")) FISocket2 = b;
			else if(IsSame(field[b]->name, "Socket3")) FISocket3 = b;
			else if(IsSame(field[b]->name, "BuyoutAmount")) FIBuyout = b;
			else if(IsSame(field[b]->name, "MinBidAmount")) FIMinBid = b;
			else if(IsSame(field[b]->name, "EndTime")) FIEndTime = b;
			else if(IsSame(field[b]->name, "AuctionStatus"))  FIAuctionStatus = b;
			else if(IsSame(field[b]->name, "AuctionItemID")) FIAuctionItemID = b;
			else if(IsSame(field[b]->name, "BuyerName")) FIBuyerName = b;
		}
		for(int b = 0; b < NItems; b++)
		{
			WORD IndexForItem = (WORD)(3 + (b*85));
			ItemRow[b] = mysql_fetch_row(pQueryResult);
			SafeCopy((pData + IndexForItem), ItemRow[b][FIItemName], strlen(ItemRow[b][FIItemName]));
			PutOffsetValue(pData, IndexForItem +20, DWORDSIZE, atoul(ItemRow[b][FIItemCount]));
			PutOffsetValue(pData, IndexForItem +24, WORDSIZE, atoi(ItemRow[b][FIItemType]));
			PutOffsetValue(pData, IndexForItem +26, DWORDSIZE, atoi(ItemRow[b][FIID1]));
			PutOffsetValue(pData, IndexForItem +30, DWORDSIZE, atoi(ItemRow[b][FIID2]));
			PutOffsetValue(pData, IndexForItem +34, DWORDSIZE, atoi(ItemRow[b][FIID3]));
			PutOffsetValue(pData, IndexForItem +38, BYTESIZE, atoi(ItemRow[b][FIItemColor]));
			PutOffsetValue(pData, IndexForItem +39, WORDSIZE, atoi(ItemRow[b][FIItemEffect1]));
			PutOffsetValue(pData, IndexForItem +41, WORDSIZE, atoi(ItemRow[b][FIItemEffect2]));
			PutOffsetValue(pData, IndexForItem +43, WORDSIZE, atoi(ItemRow[b][FIItemEffect3]));
			PutOffsetValue(pData, IndexForItem +45, WORDSIZE, atoi(ItemRow[b][FIItemLifeSpan]));
			PutOffsetValue(pData, IndexForItem +47, DWORDSIZE, atoul(ItemRow[b][FIItemAttribute]));
			PutOffsetValue(pData, IndexForItem +51, DWORDSIZE, atoul(ItemRow[b][FITempItemAttr]));
			if(ItemRow[b][FISocket1]) PutOffsetValue(pData, IndexForItem +55, BYTESIZE, atoul(ItemRow[b][FISocket1]));
			else PutOffsetValue(pData, IndexForItem +55, BYTESIZE, SG_NONE);
			if(ItemRow[b][FISocket2]) PutOffsetValue(pData, IndexForItem +56, BYTESIZE, atoul(ItemRow[b][FISocket2]));
			else PutOffsetValue(pData, IndexForItem +56, BYTESIZE, SG_NONE);
			if(ItemRow[b][FISocket3]) PutOffsetValue(pData, IndexForItem +57, BYTESIZE, atoul(ItemRow[b][FISocket3]));
			else PutOffsetValue(pData, IndexForItem +57, BYTESIZE, SG_NONE);
			PutOffsetValue(pData, IndexForItem +58, DWORDSIZE, atoul(ItemRow[b][FIAuctionItemID]));
			PutOffsetValue(pData, IndexForItem +62, DWORDSIZE, atoul(ItemRow[b][FIMinBid]));
			PutOffsetValue(pData, IndexForItem +66, DWORDSIZE, atoul(ItemRow[b][FIBuyout]));
			dwEndTime = atoul(ItemRow[b][FIEndTime]);
			cAuctionStatus = atoi(ItemRow[b][FIAuctionStatus]);
			if (dwEndTime < dwTime && cAuctionStatus == 0) {
				sprintf(QueryConsult, "UPDATE `auction_items` SET `AuctionStatus`=2 WHERE `AuctionItemID` = %lu LIMIT 1;", atoul(ItemRow[b][FIAuctionItemID]));
				if(ProcessQuery(&myConn, QueryConsult) == -1) return 0; // assertion
				pQueryResult2 = mysql_store_result(&myConn);
				SAFEFREERESULT(pQueryResult2);
				cAuctionStatus = 2;
			}
			PutOffsetValue(pData, IndexForItem +70, DWORDSIZE, dwEndTime);
			PutOffsetValue(pData, IndexForItem +74, BYTESIZE, cAuctionStatus);
			SafeCopy((pData + IndexForItem +75), ItemRow[b][FIBuyerName], strlen(ItemRow[b][FIBuyerName]));
		}
		return (3 + (NItems*85));
	}

	return 0;
}

void CLoginServer::AddAccountCash(char * data, MYSQL myConn)
{
	char QueryConsult[150], GoodAccName[25];
	st_mysql_res *QueryResult = NULL;
	MYSQL_ROW Row;

	char account[11];
	ZeroMemory(account, sizeof(account));
	memcpy(account, data, 10);
	
	char type = *(char*)(data+10);
	char i = *(char*)(data+11);
	
	if (type >= 4) return;
	//if (type >= 0 && i >= sizeof(lgnPtsSvcs[type])) return;
	//if (type >= 0 && i >= CMD_LGNSVC_MAX) return;
	if (type < -1) return;
	if (type == -1 && i <= 0) return;
	
	switch(type) {
	case 0: if (i >= CMD_LGNSVC_MAXWEAPONS) return; break;
	case 1: if (i >= CMD_LGNSVC_MAXARMORS) return; break;
	case 2: if (i >= CMD_LGNSVC_MAXMISC) return; break;
	case 3: if (i >= CMD_LGNSVC_MAXSERVICES) return; break;
	}

	/*int i=0;
	while(lgnPtsSvcs[type][i].price && 
		lgnPtsSvcs[type][i].cmd != cmd) 
	{i++;}*/

	ZeroMemory(GoodAccName, sizeof(GoodAccName));
	MakeGoodName(GoodAccName, account);

	if (type == -1)
	sprintf(QueryConsult, "UPDATE `account_database` SET `cash`=cash+%d WHERE `name` = '%s' LIMIT 1;", i, GoodAccName);
	else 
	sprintf(QueryConsult, "UPDATE `account_database` SET `cash`=cash+%lu WHERE `name` = '%s' LIMIT 1;", lgnPtsSvcs[type][i].price, GoodAccName);

	if(ProcessQuery(&myConn, QueryConsult) == -1) return; // assertion
	QueryResult = mysql_store_result(&myConn);
	SAFEFREERESULT(QueryResult);

	if (type != -1) {
		sprintf(QueryConsult, "INSERT INTO `cash_transactions` (account_name, service, date) \
							  VALUES ('%s', '%s', NOW());", GoodAccName, lgnPtsSvcs[type][i].name);

		if(ProcessQuery(&myConn, QueryConsult) == -1) return; // assertion
		QueryResult = mysql_store_result(&myConn);
		SAFEFREERESULT(QueryResult);
	}
}

void CLoginServer::SendLoginConfirmation(char * accName, char * accPass, char * charName, char *IP, BYTE GSID)
{
	if(!accName || !accPass || !charName || !IP) return;

	char SendBuff[100];

	char * pData = (char*)SendBuff;

	Push(pData, (uint32) MSGID_CONFIRMEDLOGIN);
	Push(pData, accName, 16);
	Push(pData, accPass, 16);
	Push(pData, charName, 16);
	Push(pData, IP, 16);

	SendMsgToGS(GSID, SendBuff, pData - SendBuff);

	char Txt[120];
	wsprintf(Txt, "Sent confirmed Login: IP(%s) Acc(%s / %s) Char(%s)", IP, accName, accPass, charName);
	PutLogFileList(Txt, CONNECTION_LOGFILE);
	PutLogList(Txt);
}

bool CLoginServer::IsAddressValid(char * address, MYSQL myConn, bool isIP)
{
	char QueryConsult[200];
	st_mysql_res *pQueryResult = NULL;

	ZeroMemory(QueryConsult, sizeof(QueryConsult));
	if(isIP) {
		sprintf(QueryConsult, "SELECT * FROM `ipblocked` WHERE `ipaddress` = '%s';", address);
	}else{
		sprintf(QueryConsult, "SELECT * FROM `macblocked` WHERE `macaddress` = '%s';", address);
	}
	
	if(ProcessQuery(&myConn, QueryConsult) == -1){
		return false;
	}
	pQueryResult = mysql_store_result(&myConn);
	if (mysql_num_rows(pQueryResult) > 0){
		SAFEFREERESULT(pQueryResult);
		return false;
	}
	SAFEFREERESULT(pQueryResult);
	return true;
}