#include "StdAfx.h"
#include "libRpc.h"
#include "Deamon.h"
#include "DllCore/Utils/ErrorInfo.h"
#include "DllCore/Log/LogHelper.h"

#include <Rpc.h>

#ifdef UNICODE
#define RPC_TSTR RPC_WSTR
#else /* UNICODE */
#define RPC_TSTR RPC_CSTR
#endif /* UNICODE */

#pragma comment(lib,"Rpcrt4.lib")

void __RPC_FAR* __RPC_USER midl_user_allocate(size_t len)
{
	return(malloc(len));
}

void __RPC_USER midl_user_free(void __RPC_FAR *ptr)
{
	free(ptr);
}

BOOL InitRpcModule()
{
	BOOL bSuccess = FALSE;

	do 
	{
		RPC_TSTR pszStringBinding = NULL;
		RPC_STATUS bSuccessFlag = RpcStringBindingCompose(NULL, (RPC_TSTR)_T("ncalrpc"), NULL, (RPC_TSTR)_T("DeamonGuard"), NULL, &pszStringBinding);
		if (bSuccessFlag != RPC_S_OK)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("调用系统RpcStringBindingCompose失败"));
			break;
		}

		bSuccessFlag=RpcBindingFromStringBinding(pszStringBinding, &DEAMON_Binding);
		if (bSuccessFlag != RPC_S_OK)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("调用系统RpcBindingFromStringBinding失败"));
			break;
		}

		RpcStringFree(&pszStringBinding);
		bSuccess = TRUE;
	} while (0);

	if (bSuccess == FALSE)
		QLOG_ERR(_T("初始化RPC失败"));

	return bSuccess;
}

void UninitRpcModule()
{
	RpcBindingFree(&DEAMON_Binding);
}

BOOL StartService()
{
	return FALSE;
}

BOOL MyRpcTest()
{
	BOOL bSuccess=FALSE;

	RpcTryExcept
	{
		bSuccess=RpcTest();
	}
	RpcExcept(EXCEPTION_EXECUTE_HANDLER)
	{
		if (RpcExceptionCode()==RPC_S_SERVER_UNAVAILABLE)
		{
			if (StartService()==FALSE)
			{
				SetErrorTitle(_T("调用MyRpcTest函数时启动服务失败"));
			}
		}
		else
		{
			SetErrorInfo(SYSTEM_ERROR,RpcExceptionCode(),_T("调用MyRpcTest函数失败"));
		}
	}
	RpcEndExcept

	return bSuccess;
}

BOOL UserAppExitNotify(DWORD dwProcessId)
{
	BOOL bSuccess=FALSE;

	RpcTryExcept
	{
		MyRpcTest();

		RpcUserAppExitNotify(dwProcessId);

		bSuccess=TRUE;
	}
	RpcExcept(EXCEPTION_EXECUTE_HANDLER)
	{
		SetErrorInfo(SYSTEM_ERROR,RpcExceptionCode(),_T("调用RpcUserAppExitNotify函数失败"));
	}
	RpcEndExcept

		return bSuccess;
}