#include "StdAfx.h"
#include "TCPServer.h"
#include "ADELLock.h"
#include "TCPHandler.h"
#include "LanGuardConfig.h"
#include "DuiLib/UIlib.h"

#include "MemLeakDetect.h"
using namespace DuiLib;

CTCPServer g_TCPSrv; 

int g_nMax = 1;

BOOL NewConnectCB(int nClientId)
{ 
	if (nClientId > g_nMax)
	{
		g_TCPSrv.CloseServer();
		return FALSE;
	}

	CTCPHandler* pTCPHandler = new CTCPHandler;
	pTCPHandler->Init();
	//设置新连接接收数据回调函数 
	g_TCPSrv.SetTCPHandlerCB(nClientId, pTCPHandler); 
	return TRUE;
}

int main() 
{ 
#if _DEBUG2
	CADELLock* pADELLock = new CADELLock();
	//int n= pADELLock->Init();
	LONG lCardNo;
	int n=pADELLock->ReadCardId(&lCardNo);
	pADELLock->Release();
#else

	CMemLeakDetect MemLeakDetect;
	CLanGuardConfig LanGuardConfig;
	LanGuardConfig.Load(_T("LanGuard.ini"));

	g_TCPSrv.SetConnectExtra((NewConnect)NewConnectCB); 
	g_TCPSrv.Start(_T("0.0.0.0"), LanGuardConfig.GetPort()); 
#endif
	return 0; 
}