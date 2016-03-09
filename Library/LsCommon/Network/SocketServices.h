#pragma once

#include <Ws2spi.h>

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif


typedef CList<WSAPROTOCOL_INFOW,const WSAPROTOCOL_INFOW&> CWscProtocolInfoList;

class LS_COMMON_API CSocketServices
{
public:
	CSocketServices();
	~CSocketServices();

	BOOL LsEnumProtocols(CWscProtocolInfoList& WscProtocolInfo);
	BOOL LsGetProviderPath(LPGUID lpGuid,CString& strProviderPath);
	
	BOOL IsProviderConfigChanged();
	void SetCompletionRoutine(LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
protected:
	HANDLE m_NotificationHandle;
	WSAOVERLAPPED m_OverLapped;
	LPWSAOVERLAPPED_COMPLETION_ROUTINE m_lpCompletionRoutine;
};