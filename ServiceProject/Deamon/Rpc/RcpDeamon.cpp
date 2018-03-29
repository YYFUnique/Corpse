#include "StdAfx.h"
#include "RcpDeamon.h"
#include "Rpc/Deamon.h"

#ifdef UNICODE
#define RPC_TSTR RPC_WSTR
#else /* UNICODE */
#define RPC_TSTR RPC_CSTR
#endif /* UNICODE */

HANDLE g_hThreadRpcServer = NULL;

#pragma comment(lib, "Rpcrt4.lib")

void __RPC_FAR* __RPC_USER MIDL_user_allocate(size_t len)
{
	return(malloc(len));
}

void __RPC_USER  MIDL_user_free(void __RPC_FAR *ptr)
{
	free(ptr);
}

boolean RpcTest( void)
{
	return true;
}

BOOL StartRpcServer()
{
	BOOL bRet = FALSE;
	DWORD dwTid = 0;
	g_hThreadRpcServer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadRpcServer, NULL, CREATE_SUSPENDED, &dwTid);
	if (g_hThreadRpcServer != NULL)
	{
		ResumeThread(g_hThreadRpcServer);
		bRet = TRUE;
	}

	return bRet;
}

BOOL StopRpcServer()
{
	BOOL bRet = FALSE;
	if (g_hThreadRpcServer != NULL)
	{
		RpcMgmtStopServerListening(NULL);
		RpcServerUnregisterIf(NULL, NULL, FALSE);
		::WaitForSingleObject(g_hThreadRpcServer, INFINITE);
		g_hThreadRpcServer = NULL;
		bRet = TRUE;
	}
	return bRet;
}

UINT ThreadRpcServer(LPVOID lpParam)
{
	RpcServerUseProtseqEp((RPC_TSTR)_T("ncalrpc"), 
		RPC_C_PROTSEQ_MAX_REQS_DEFAULT, (RPC_TSTR)_T("DeamonGuard"), NULL);
	RpcServerRegisterIf(Deamon_v1_0_s_ifspec, NULL, NULL);
	RpcServerListen(1, 20, FALSE);
	return 0;
}

void RpcUserAppExitNotify( 
						  /* [in] */ unsigned long dwProcessId)
{
	//AppMonitor.SetSelfExitPid(dwProcessId);	
	TCHAR szTip[20];
	_stprintf_s(szTip, _countof(szTip), _T("ProcessID:%d\r\n"), dwProcessId);
	OutputDebugString(szTip);
}

void RpcSetService( 
				   /* [in] */ unsigned long dwType)
{

}