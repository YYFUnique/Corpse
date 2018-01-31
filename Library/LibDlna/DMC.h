#ifndef _DMC_H
#define _DMC_H

#include <winsock2.h>
#include <tchar.h>
#include <string>
#include <vector>
using namespace std;

#include "libdlna.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "Components/GeneratedStacks/ILib/ILibParsers.h"
#include "Components/GeneratedStacks/ILib/ILibThreadPool.h"
#include "Components/GeneratedStacks/ControlPointStacks/MediaRenderer/DMRCP_ControlPoint.h"
#include "Components/MediaRendererCP/MicroAVRCP.h"
#include "Components/GeneratedStacks/ControlPointStacks/MediaServerCP/MediaServerCP_ControlPoint.h"
#include "Components/CdsObjects/CdsDidlSerializer.h"
#include "Components/ProtocolInfoParser/DLNAProtocolInfo.h"
#include "Components/PushController/PushController.h"

#ifdef __cplusplus
};
#endif

extern FileIO* g_FileIO;

#define MAX_URL_LENGTH					1024

class CDMC
{
public:
	CDMC();
	~CDMC();

	BOOL					DMC_Initialize(FileIO* pIO,
										   LPDLNA_EVENT_ROUTINE pfnEventCallBack,
										   LPVOID pCallBackParameter);
	BOOL					DMC_UnInitialize();
	BOOL					DMC_Open(LPCTSTR lpszUrl, HANDLE * phSession);
	BOOL					DMC_Close(HANDLE hSession);
	BOOL					DMC_SelectDevice(LPCTSTR lpszDeviceID);
	BOOL					DMC_Play();
	BOOL					DMC_Pause();
	BOOL					DMC_Stop();
	BOOL					DMC_SetPosition(LONG nTimePosition);
	BOOL					DMC_GetPosition(LONG* pnTimePosition);

protected:
	BOOL					DMC_SearchAVRenderer();
	BOOL					DMC_CancelSearch();
	static void				ReflectOnPlayResponse(struct AVRendererConnection* connection,
												  int ErrorCode, void* Tag);
	static void				ReflectOnSeekResponse(struct AVRendererConnection* connection,
												  int ErrorCode, void* Tag);
	static void				ReflectOnPauseResponse(struct AVRendererConnection* connection,
												   int ErrorCode, void* Tag);
	static void				ReflectOnStopResponse(struct AVRendererConnection* connection,
												  int ErrorCode, void* Tag);
	static void				ReflectOnGetPositionResponse(struct AVRendererConnection* connection,
												     int ErrorCode,
													 int RelativeSeconds,
													 int AbsoluteSeconds,
													 int RelativeCounter,
													 int AbsoluteCounter,
													 void* Tag);
	void					OnPlayResponse(int ErrorCode);
	void					OnSeekResponse(int ErrorCode);
	void					OnPauseResponse(int ErrorCode);
	void					OnStopResponse(int ErrorCode);
	void					OnGetPositionResponse(int ErrorCode);

	static DWORD WINAPI		ReflectThreadPoolCreation(LPVOID args);

	static void				MSRendererAdded(void* token, struct AVRenderer* r);
	static void				MSRendererRemoved(void* token, struct AVRenderer* r);
	void					AddRenderer(struct AVRenderer* renderer);
	void					RemoveRenderer(struct AVRenderer* renderer);

	static void				ReflectOnAddMSDevice(struct UPnPDevice* device);
	static void				ReflectOnRemoveMSDevice(struct UPnPDevice* device);
	void					OnAddMSDevice(struct UPnPDevice* device);
	void					OnRemoveMSDevice(struct UPnPDevice* device);

	static DWORD WINAPI		ReflectIPAddressMonitorLoop(LPVOID args);
	static void CALLBACK	ReflectOnIPAddressChanged(DWORD dwError,
													  DWORD cbTransferred,
													  LPWSAOVERLAPPED lpOverlapped,
													  DWORD dwFlags);
	DWORD					IPAddressMonitorLoop();
	virtual void			OnIPAddressChanged();
	void					StartIPAddressMonitoring();
	void					StopIPAddressMonitoring();
	BOOL					IsIPAddressMonitoringRunning();

	static void				ReflectOnPushFileResponse(PC_Session session,
													  int errorCode,
													  void* sessionTag);
	void					OnPushFileResponse(PC_Session session, int errorCode);

	static void				ReflectOnPushFileStatusUpdate(PC_Session session,
														  int startTransfer,
														  int finishTransfer,
														  long bytesTransfered,
														  long totalBytesToTransfer,
														  void* sessionTag);
	void					OnStartPushFileStatus(PC_Session session,
												  long bytesTransfered, long totalBytesToTransfer);
	void					OnFinishPushFileStatus(PC_Session session, long bytesTransfered);
	void					OnPushFileStatusUpdate(PC_Session session, long bytesTransfered);

	static DWORD WINAPI		ReflectStartChainThread(LPVOID args);
	DWORD					StartChainThread();
	void					StartChain();
	void					StopChain();
	BOOL					IsChainRunning();
	void					SetError(LPCTSTR str);

	void					NotifyRendererChanged();
	void					NotifyError();

protected:
	HANDLE					m_ipMonitorLoopEvent;
	SOCKET					m_ipMonitorSocket;
	HANDLE					m_ipMonitorThread;
	DWORD					m_ipMonitorbytesReturned;
	WSAOVERLAPPED			m_ipMonitorOverlapped;

	void*					m_chain;
	void*					m_lifeTime;
	ILibThreadPool			m_threadPool;
	void*					m_rendererToken;
	BOOL					m_chainRunning;
	struct AVRenderer*		m_currentRenderer;

	std::string				m_currentRendererInfo;

	PC_Token				m_pushController;

	void*					m_mediaServerToken;

	HANDLE					m_microstackThread;

	PC_Session				m_currentPFSession;
	long					m_nBytesSent;
	long					m_nBytesTotal;

	LPDLNA_EVENT_ROUTINE	m_pfnOnDlnaEvent;

	UINT					m_errorCounter;
	std::wstring			m_errorMessage;

	vector<AVRenderer*>		m_AVRendererList;
	vector<string>			m_RendererInfoList;
	wstring					m_RenderersInfo;

	UINT_PTR				m_timer;
	UINT_PTR				m_pfTimer;

	LPVOID					m_pCallBackParameter;
	LONG					m_nCurrentTimePosition;
	TCHAR					m_szSelectedUDN[MAX_PATH];
	TCHAR					m_szPlayUrl[MAX_URL_LENGTH];

	DLNA_EVENT_INFO			m_DlnaEventInfo;
	DWORD					m_nLastTickCount;
};


void* StdOpen (const char* url);
int StdClose (void* handle);
int StdRead (void* handle, unsigned char* buffer, long len, long* read);
int StdSeek (void* handle, long offset, int method);
int StdTell (void* handle, unsigned long long* offset);

static FileIO StdFileIO = {
	StdOpen,
	StdClose,
	StdRead,
	StdSeek,
	StdTell
};

#endif