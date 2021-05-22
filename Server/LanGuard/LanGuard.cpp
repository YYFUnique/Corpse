#include "StdAfx.h"
#include "TCPServer.h"
#include "ADELLock.h"
#include "TCPHandler.h"
#include "LanGuardConfig.h"

CTCPServer g_TCPSrv; 

size_t g_nMax = 0;

void OnConnectCB(size_t nClientId)
{ 
	CTCPHandler* pTCPHandler = new CTCPHandler;
	pTCPHandler->Init();
	//设置新连接接收数据回调函数 
	g_TCPSrv.SetTCPHandlerCB(nClientId, pTCPHandler); 

	// nClientId 序列从1开始 ，大于目标值就关闭服务器套接字，不在接收连接
	if (g_nMax != 0 && nClientId + 1 > g_nMax)
		g_TCPSrv.CloseServer();
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
	CLanGuardConfig LanGuardConfig;
	LanGuardConfig.Load(_T("LanGuard.ini"));

	g_TCPSrv.SetConnectExtra((FN_Connect)OnConnectCB); 
	g_TCPSrv.Start(_T("0.0.0.0"), LanGuardConfig.GetPort()); 
#endif
	return 0; 
}