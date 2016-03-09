#pragma once

#ifdef LS_STATIC_LIB_CALL
#define HPSOCKET_API
#elif defined LS_EXPORTS
#define HPSOCKET_API __declspec(dllexport)
#else
#define HPSOCKET_API __declspec(dllimport)
#endif

#include "socket/SocketInterface.h"

#define USER_INFO_MSG			(WM_USER + 100)
#define MAX_LOG_RECORD_LENGT	1000

#define EVT_ON_SEND				_T("OnSend")
#define EVT_ON_RECEIVE			_T("OnReceive")
#define EVT_ON_CLOSE			_T("OnClose")
#define EVT_ON_ERROR			_T("OnError")
#define EVT_ON_PREPARE_CONNECT	_T("OnPrepareConnect")
#define EVT_ON_PREPARE_LISTEN	_T("OnPrepareListen")
#define EVT_ON_ACCEPT			_T("OnAccept")
#define EVT_ON_CONNECT			_T("OnConnect")
#define EVT_ON_SHUTDOWN			_T("OnShutdown")
#define EVT_ON_END_TEST			_T("END TEST")

enum EnAppState
{
	ST_STARTING, ST_STARTED, ST_STOPING, ST_STOPED
};

struct HPSOCKET_API info_msg
{
	CONNID connID;
	LPCTSTR evt;
	int contentLength;
	LPCTSTR content;

	static info_msg* Construct(CONNID dwConnID, LPCTSTR lpszEvent, int iContentLength, LPCTSTR lpszContent);
	static void Destruct(info_msg* pMsg);

private:
	info_msg(CONNID dwConnID, LPCTSTR lpszEvent, int iContentLength, LPCTSTR lpszContent);
	~info_msg();
};

struct TPkgHeader 
{
	DWORD seq;
	int body_len;
};

struct TPkgBody 
{
	char name[30];
	short age;
	char desc[1];
};

struct TPkgInfo
{
	bool is_header;
	int length;

	TPkgInfo(bool header = true, int len = sizeof(TPkgHeader)) : is_header(header), length(len) {}
	void Reset() {is_header = true, length = sizeof(TPkgHeader);}
	~TPkgInfo() {}
};

CBufferPtr* GeneratePkgBuffer(DWORD seq, LPCTSTR lpszName, short age, LPCTSTR lpszDesc);
CBufferPtr* GeneratePkgBuffer(const TPkgHeader& header, const TPkgBody& body);

HPSOCKET_API void SetMainWnd(CWnd* pWnd);
void SetInfoList(CListBox* pInfoList);
void LogServerStart(LPCTSTR lpszAddress, USHORT port);
void LogServerStartFail(DWORD code, LPCTSTR lpszDesc);
void LogServerStop();
void LogServerStopFail(DWORD code, LPCTSTR lpszDesc);
void LogClientStart(LPCTSTR lpszAddress, USHORT port);
HPSOCKET_API void LogClientStarting(LPCTSTR lpszAddress, USHORT port);
HPSOCKET_API void LogClientStartFail(DWORD code, LPCTSTR lpszDesc);
void LogClientStopping(CONNID dwConnID);
void LogClientStopFail(DWORD code, LPCTSTR lpszDesc);
void LogSend(CONNID dwConnID, LPCTSTR lpszContent);
void LogClientSendFail(int iSequence, int iSocketIndex, DWORD code, LPCTSTR lpszDesc);
void LogSendFail(CONNID dwConnID, DWORD code, LPCTSTR lpszDesc);
void LogDisconnect(CONNID dwConnID);
void LogDisconnectFail(CONNID dwConnID);
void LogDetect(CONNID dwConnID);
void LogDetectFail(CONNID dwConnID);
void LogOnConnect(CONNID dwConnID, const CString& strAddress, USHORT usPort);
void LogOnConnect2(CONNID dwConnID);
void LogMsg(const CString& msg);
HPSOCKET_API void PostOnSend(CONNID dwConnID, const BYTE* pData, int iLength);
HPSOCKET_API void PostOnReceive(CONNID dwConnID, const BYTE* pData, int iLength);
HPSOCKET_API void PostOnClose(CONNID dwConnID);
HPSOCKET_API void PostOnError(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
void PostOnAccept(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort, BOOL bPass);
void PostOnAccept2(CONNID dwConnID);
void PostOnPrepareListen(LPCTSTR lpszAddress, USHORT usPort);
void PostOnPrepareConnect(CONNID dwConnID);
HPSOCKET_API void PostOnConnect(CONNID dwConnID, LPCTSTR lpszAddress, USHORT usPort);
void PostOnShutdown();
void PostServerStatics(const LONGLONG& llTotalSent, const LONGLONG& llTotalReceived);
void PostTimeConsuming(DWORD dwTickCount);
void PostInfoMsg(info_msg* msg);
void LogInfoMsg(info_msg* pInfoMsg);
