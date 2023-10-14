#ifndef _CLIENT_H_
#define _CLIENT_H_
#include "windows.h"
#include "..\main.h"
#include "..\defs.hpp"
//==============================================================================
class CClient
{
public:
        CClient(char *AccName, char *AccPwd, char *CharaName, char *IP, BYTE GSID);
        ~CClient();

        char AccountName[15], AccountPassword[15], CharName[15];
        _ADDRESS ClientIP;
        char ConnectedServerID;
        DWORD Time, ForceDisconnRequestTime;
        BOOL IsPlaying, IsOnServerChange;
};
//==============================================================================
extern WORD ActiveAccounts, PeakPeopleOnline;
//==============================================================================
#endif