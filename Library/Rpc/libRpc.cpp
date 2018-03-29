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
			SetErrorInfo(SYSTEM_ERROR,0,_T("����ϵͳRpcStringBindingComposeʧ��"));
			break;
		}

		bSuccessFlag=RpcBindingFromStringBinding(pszStringBinding, &DEAMON_Binding);
		if (bSuccessFlag != RPC_S_OK)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("����ϵͳRpcBindingFromStringBindingʧ��"));
			break;
		}

		RpcStringFree(&pszStringBinding);
		bSuccess = TRUE;
	} while (0);

	if (bSuccess == FALSE)
		QLOG_ERR(_T("��ʼ��RPCʧ��"));

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
				SetErrorTitle(_T("����MyRpcTest����ʱ��������ʧ��"));
			}
		}
		else
		{
			SetErrorInfo(SYSTEM_ERROR,RpcExceptionCode(),_T("����MyRpcTest����ʧ��"));
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
		SetErrorInfo(SYSTEM_ERROR,RpcExceptionCode(),_T("����RpcUserAppExitNotify����ʧ��"));
	}
	RpcEndExcept

		return bSuccess;
}