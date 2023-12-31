// winmain.h
#ifndef __WINMAIN_H_INCLUDED
#define __WINMAIN_H_INCLUDED
#pragma once

#include <windows.h>
#include "..\..\shared\common.h"
#include <assert.h>	

#ifdef BENCHMARK
#define BENCHMARK_BEGIN(name)	{ string _name = #name; uint32 t; sstream ss; t = timeGetTime();
#define BENCHMARK_END		t = timeGetTime() - t; ss << "BENCHMARK " << _name << " = " << t; PutLogList(ss.str().c_str()); }
#else
#define BENCHMARK_BEGIN(name)
#define BENCHMARK_END
#endif

void Assertion(const char * assertion, const char * file, const uint32 line);
#define ASSERT(assertion) { if( !(assertion) ) { Assertion(#assertion, __FILE__, __LINE__); assert( assertion &&0); } }
void Error(const string str, const char * file, const uint32 line);
#define ERR(str)	{ Error(str, __FILE__, __LINE__); }

bool InitApplication( HINSTANCE hInstance);
bool InitInstance( HINSTANCE hInstance, int nCmdShow );
LRESULT CALLBACK WndProc( HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam );
void OnAccept();
void OnKeyUp(WPARAM wParam, LPARAM lParam);
void UpdateScreen();
int  EventLoop();
void OnPaint();
void OnDestroy();
void Initialize();
void CALLBACK _TimerFunc(UINT wID, UINT wUser, DWORD dwUSer, DWORD dw1, DWORD dw2);
MMRESULT _StartTimer(DWORD dwTime);
void _StopTimer(MMRESULT timerid);
int32 idice(int _throw, int range);
int32 iNdice(int _throw, int range);
void PutLogFileList(const char * cStr, char *FileName = NULL);
//void PutLogList(char * cMsg, BYTE MsgLvl = NULL, bool PutOnFile = FALSE, char *FileName = NULL);
void PutLogList(const char * cMsg);
void PutLogList(const string msg);
void LogError(const char * cMsg);
void LogError(const string msg);
void SafeCopy(char *c1, char *c2, DWORD lenght = 0);
bool IsSame(char *c1, char *c2);
BYTE bGetOffsetValue(char * cp, DWORD offset);
WORD wGetOffsetValue(char * cp, DWORD offset);
DWORD dwGetOffsetValue(char * cp, DWORD offset);
uint64 ullGetOffsetValue(char * cp, DWORD offset);
void PutOffsetValue(char * cp, DWORD offset, BYTE size, uint64 value);
void parseCommand(char* pMsg);

#endif
