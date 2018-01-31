#include "DMC.h"
#include <atlstr.h>
extern "C"{
#include "Components/StringUtils/MimeTypes.h"
};

void ShowDebug(LPCTSTR pcszFormat, ... )
{
	TCHAR szBuffer[1024];

	va_list vl;
	va_start(vl, pcszFormat);
	wvsprintf(szBuffer, pcszFormat, vl);
	va_end(vl);

	::OutputDebugString(szBuffer);
}


std::string wstring2string(std::wstring wstr)
{
	std::string result;
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	if (len > 0)
	{
		char* buffer = new char[len + 1];   
		if (buffer != NULL)
		{
			WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);   
			buffer[len] = '\0';	
			result.append(buffer);   
			delete[] buffer;
		}
	}
	return result;
}

std::wstring string2wstring(std::string str)
{
	std::wstring result;	
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	if (len > 0)
	{
		wchar_t* buffer = new wchar_t[len + 1];
		if (buffer != NULL)
		{
			MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
			buffer[len] = '\0';
			result.append(buffer);
			delete[] buffer;
		}
	}
	return result;   
}


//*********************************************************************
// Thread Pool Size
//*********************************************************************
#define THREAD_POOL_SIZE	1
//*********************************************************************

CDMC*	g_This = NULL;
FileIO* g_FileIO = NULL;

CDMC::CDMC()
{
}

CDMC::~CDMC()
{

}

BOOL CDMC::DMC_Initialize(FileIO* pIO,
						  LPDLNA_EVENT_ROUTINE pfnEventCallBack,
						  LPVOID pCallBackParameter)
{
	ShowDebug(_T("DLNA: DMC_Initialize"));

	g_This = this;

	m_chain = NULL;
	m_lifeTime = NULL;
	m_chainRunning = FALSE;
	m_rendererToken = NULL;
	m_currentPFSession = NULL;
	m_currentRenderer = NULL;
	m_microstackThread = NULL;
	m_threadPool = NULL;
	m_ipMonitorThread = NULL;
	m_mediaServerToken = NULL;
	m_pushController = NULL;
	m_nLastTickCount = 0;

	if (pIO==NULL)
		g_FileIO = &StdFileIO;
	else
		g_FileIO = pIO;

	m_pfnOnDlnaEvent = pfnEventCallBack;
	m_pCallBackParameter = pCallBackParameter;
	memset(m_szPlayUrl, 0 ,sizeof(m_szPlayUrl));
	memset(m_szSelectedUDN, 0, sizeof(m_szSelectedUDN));
	memset(&m_DlnaEventInfo, 0, sizeof(m_DlnaEventInfo));

	m_nCurrentTimePosition = 0;

	return DMC_SearchAVRenderer();
}

BOOL CDMC::DMC_UnInitialize()
{
	ShowDebug(_T("DLNA: DMC_UnInitialize"));

	// >>> Shutdown here!
	DMC_CancelSearch();

	g_This = NULL;

	m_AVRendererList.clear();
	// <<<

	return 0;
}

BOOL CDMC::DMC_SearchAVRenderer()
{
	m_chain = ILibCreateChain();
	if(m_chain == NULL)
	{
		ShowDebug(_T("DLNA: ILibCreateChain failed!"));
		return FALSE;
	}

	m_lifeTime = ILibCreateLifeTime(m_chain);
	if(m_lifeTime == NULL)
	{
		ShowDebug(_T("DLNA: ILibCreateLifeTime failed!"));
		return FALSE;
	}

	m_threadPool = ILibThreadPool_Create();
	if(m_threadPool == NULL)
	{
		ShowDebug(_T("DLNA: ILibThreadPool_Create failed!"));
		return FALSE;
	}

	DWORD id;
	HANDLE hThread = NULL;
	for(int i = 0; i < THREAD_POOL_SIZE; i++)
	{
		hThread = CreateThread(NULL, 0, &ReflectThreadPoolCreation, this, 0, &id);
		if(hThread == NULL)
		{
			ShowDebug(_T("DLNA: CreateThread failed! i=%d"), i);
			return FALSE;
		}
	}

	// >>> Stack startup here!
	m_rendererToken = CreateRendererCP(m_chain);
	if(m_rendererToken == NULL)
	{
		ShowDebug(_T("DLNA: CreateRendererCP failed!"));
		return FALSE;
	}
	RCPToken_SetTag(m_rendererToken, this);

	RendererAdded = &CDMC::MSRendererAdded;
	RendererRemoved = &CDMC::MSRendererRemoved;

	m_mediaServerToken = MediaServerCP_CreateControlPoint
		(m_chain, CDMC::ReflectOnAddMSDevice, CDMC::ReflectOnRemoveMSDevice);
	if(m_mediaServerToken == NULL)
	{
		ShowDebug(_T("DLNA: MediaServerCP_CreateControlPoint failed!"));
		return FALSE;
	}

	MediaServerCP_SetUser(m_mediaServerToken, this);
	m_pushController = PushController_Create(m_chain, m_threadPool, this);
	if(m_pushController == NULL)
	{
		ShowDebug(_T("DLNA: PushController_Create failed!"));
		return FALSE;
	}

	StartChain();
	StartIPAddressMonitoring();

	return TRUE;
}

BOOL CDMC::DMC_CancelSearch()
{
	StopIPAddressMonitoring();
	StopChain();
	return TRUE;
}


BOOL CDMC::DMC_Open(LPCTSTR lpszUrl, HANDLE * phSession)
{
	_tcscpy(m_szPlayUrl, lpszUrl);
	//std::string url = wstring2string(m_szPlayUrl);
	CStringA strUrl(lpszUrl);
	//std::string und = wstring2string(m_szSelectedUDN);
	CStringA strUnd(m_szSelectedUDN);
	/*std::string mimeType;*/
	CStringA strMimeType;
	//CStringA
	/*std::string protocolInfo;*/
	CStringA strProtocolInfo;

	TCHAR szError[1024];
	char* ext = FilePathToFileExtension(strUrl, strUrl.GetLength());
	if (ext == NULL)
	{
		_tcscpy(szError, _T("Unknown File Extension"));
		SetError(szError);
		return FALSE;
	}
	if (stricmp(ext, ".ts") == 0)
	{
		strMimeType = "video/vnd.dlna.mpeg-tts";
		strProtocolInfo = "http-get:*:video/vnd.dlna.mpeg-tts:*";/*DLNA.ORG_PN=MPEG_TS_HD_NA_T*/
	}
	else if (stricmp(ext, ".wmv") == 0)
	{
		strMimeType = "video/x-ms-wmv";
		strProtocolInfo = "http-get:*:video/x-ms-wmv:*";
	}
	else if (stricmp(ext, ".mp4") == 0)
	{
		strMimeType = "video/mp4";
		strProtocolInfo = "http-get:*:video/mp4:*";
	}
	else
	{
		m_errorMessage = L"Unsupported format ";
		m_errorMessage += string2wstring(ext);
		NotifyError();
		free(ext);
		return FALSE;
	}

	strProtocolInfo += ":DLNA.ORG_OP=11";

	BOOL bResult = FALSE;
	std::vector<AVRenderer*>::iterator it;
	for(it=m_AVRendererList.begin(); it != m_AVRendererList.end(); it++)
	{
		if (stricmp(/*und.c_str()*/strUnd, (*it)->device->UDN) == 0)
		{
			m_currentRenderer = *it;

			PC_Session s = PushController_PushFile(m_pushController,
																			m_currentRenderer->Connection,
																			/* url.c_str(),*/
																			strUrl,
																			/*  mimeType.c_str(),*/
																			strMimeType,
																			/*  protocolInfo.c_str(),*/
																			strProtocolInfo,
																			&CDMC::ReflectOnPushFileResponse,
																			&CDMC::ReflectOnPushFileStatusUpdate,
																			this);
			if(s != NULL)
			{
				*phSession = s;
				bResult = TRUE;
			}
			break;
		}
	}

	free(ext);
	return bResult;
}


BOOL CDMC::DMC_Close(HANDLE hSession)
{
	if(hSession == NULL)
		return FALSE;

	PC_Session s = (PC_Session)hSession;
	PC_Session_Release(s);
	return TRUE;
}


BOOL CDMC::DMC_SelectDevice(LPCTSTR lpszDeviceID)
{
	_tcscpy(m_szSelectedUDN, lpszDeviceID);
	return TRUE;
}


BOOL CDMC::DMC_Play()
{
	ShowDebug(_T("DLNA: DMC_Play"));
	if (m_currentRenderer && m_currentRenderer->Connection)
		RCP_Play(m_currentRenderer->Connection, this, &CDMC::ReflectOnPlayResponse);
	else
	{
		ShowDebug(_T("DLNA: DMC_Play Fail"));
	}

	return TRUE;
}


BOOL CDMC::DMC_Pause()
{
	if (m_currentRenderer && m_currentRenderer->Connection)
		RCP_Pause(m_currentRenderer->Connection, this, &CDMC::ReflectOnPauseResponse);
	return TRUE;
}


BOOL CDMC::DMC_Stop()
{
	if(m_currentRenderer && m_currentRenderer->Connection)
		RCP_Stop(m_currentRenderer->Connection, this, &CDMC::ReflectOnStopResponse);
	return TRUE;
}


BOOL CDMC::DMC_SetPosition(LONG nTimePosition)
{
	if (m_currentRenderer)
	{
		char reltime[32];
		memset(reltime, 0, sizeof(reltime));
		int ms, s, m, h;
		ms = nTimePosition % 1000;
		s = nTimePosition / 1000 % 60;
		m = nTimePosition / 1000 / 60 % 60;
		h = nTimePosition / 1000 / 60 / 60;
		sprintf(reltime, "%02d:%02d:%02d.%03d", h, m, s, ms);

		RCP_Seek(m_currentRenderer->Connection, this, &CDMC::ReflectOnSeekResponse,
			"REL_TIME", reltime);
	}
	return TRUE;
}


BOOL CDMC::DMC_GetPosition(LONG* pnTimePosition)
{
	DWORD dwTick = ::GetTickCount();
	if (dwTick - m_nLastTickCount > 2000)
	{
		m_nLastTickCount = dwTick;
		if (m_currentRenderer)
		{
			RCP_GetPosition(m_currentRenderer->Connection,
				this,
				&CDMC::ReflectOnGetPositionResponse);
		}
	}

	if (pnTimePosition)
		*pnTimePosition = m_nCurrentTimePosition;
	return TRUE;
}


void CDMC::ReflectOnPlayResponse(struct AVRendererConnection* connection, int ErrorCode, void* Tag)
{
	CDMC* instance = (CDMC*)Tag;
	if(instance != NULL)
	{
		instance->OnPlayResponse(ErrorCode);
	}
}


void CDMC::ReflectOnSeekResponse(struct AVRendererConnection* connection, int ErrorCode, void* Tag)
{
	CDMC* instance = (CDMC*)Tag;
	if(instance != NULL)
	{
		instance->OnSeekResponse(ErrorCode);
	}
}


void CDMC::OnPlayResponse(int ErrorCode)
{
	if(ErrorCode != 0)
	{
		TCHAR tmp[128];
		wsprintf(tmp, _T("Play action failed with error '%d'!"), ErrorCode);
		m_errorMessage.assign(tmp);
		NotifyError();
	}
}

void CDMC::OnSeekResponse(int ErrorCode)
{
	if(ErrorCode != 0)
	{
		TCHAR tmp[128];
		wsprintf(tmp, _T("Seek action failed with error '%d'!"), ErrorCode);
		m_errorMessage.assign(tmp);
		NotifyError();
	}
}



void CDMC::ReflectOnPauseResponse(struct AVRendererConnection* connection, int ErrorCode, void* Tag)
{
	CDMC* instance = (CDMC*)Tag;
	if(instance != NULL)
	{
		instance->OnPauseResponse(ErrorCode);
	}
}

void CDMC::OnPauseResponse(int ErrorCode)
{
	if(ErrorCode != 0)
	{
		TCHAR tmp[128];
		wsprintf(tmp, _T("Pause action failed with error '%d'!"), ErrorCode);
		m_errorMessage.assign(tmp);
		NotifyError();
	}
}


void CDMC::ReflectOnStopResponse(struct AVRendererConnection* connection, int ErrorCode, void* Tag)
{
	CDMC* instance = (CDMC*)Tag;
	if(instance != NULL)
	{
		instance->OnStopResponse(ErrorCode);
	}
}

void CDMC::OnStopResponse(int ErrorCode)
{
	if(ErrorCode != 0)
	{
		TCHAR tmp[128];
		wsprintf(tmp, _T("Stop action failed with error '%d'!"), ErrorCode);
		m_errorMessage.assign(tmp);
		NotifyError();
	}
}

void CDMC::ReflectOnGetPositionResponse(struct AVRendererConnection* connection,
										 int ErrorCode,
										 int RelativeSeconds,
										 int AbsoluteSeconds,
										 int RelativeCounter,
										 int AbsoluteCounter,
										 void* Tag)
{
	CDMC* instance = (CDMC*)Tag;
	if(instance != NULL)
	{
		if (ErrorCode == 0)
			instance->m_nCurrentTimePosition = RelativeSeconds;
		instance->OnGetPositionResponse(ErrorCode);
	}
}

void CDMC::OnGetPositionResponse(int ErrorCode)
{
	if(ErrorCode != 0)
	{
		TCHAR tmp[128];
		wsprintf(tmp, _T("GetPosition failed with error '%d'!"), ErrorCode);
		m_errorMessage.assign(tmp);
		NotifyError();
	}
}

void CDMC::ReflectOnPushFileResponse(PC_Session session, int errorCode, void* sessionTag)
{
	CDMC* This = (CDMC*)sessionTag;
	if(This != NULL)
	{
		This->OnPushFileResponse(session, errorCode);
	}
}

void CDMC::OnPushFileResponse(PC_Session session, int errorCode)
{
	if(errorCode != 0)
	{
		TCHAR tmp[128];
		wsprintf(tmp, _T("Push File failed with error '%d'!"), errorCode);
		m_errorMessage.assign(tmp);
		NotifyError();
	}
}


void CDMC::SetError(LPCTSTR str)
{
	m_errorMessage = str;
	m_errorCounter = 16;
	//NotifyError();
}

void CDMC::ReflectOnPushFileStatusUpdate(PC_Session session, int startTransfer,
										int finishTransfer, long bytesTransfered,
										long totalBytesToTransfer, void* sessionTag)
{
	CDMC* This = (CDMC*)sessionTag;
	if(This != NULL)
	{ 
		if(startTransfer == 1)
		{
			This->OnStartPushFileStatus(session, bytesTransfered, totalBytesToTransfer);
		}
		else if(finishTransfer == 1 || finishTransfer == -1)
		{
			This->OnFinishPushFileStatus(session, bytesTransfered);
		}
		else
		{
			This->OnPushFileStatusUpdate(session, bytesTransfered);
		}
	}
}

void CDMC::OnStartPushFileStatus(PC_Session session, long bytesTransfered, long totalBytesToTransfer)
{
	m_nBytesTotal = totalBytesToTransfer;
	OnPushFileStatusUpdate(session, bytesTransfered);
	//m_pfTimer = SetTimer(m_pfTimer, 100, NULL);
	m_currentPFSession = session;
}

void CDMC::OnFinishPushFileStatus(PC_Session session, long bytesTransfered)
{
	m_currentPFSession = NULL;
	//KillTimer(m_pfTimer);
	OnPushFileStatusUpdate(session, bytesTransfered);
	if(bytesTransfered < m_nBytesTotal)
	{
		//SetError(_T("Pushed Bytes were interrupted before completion!"));
	}
}

void CDMC::OnPushFileStatusUpdate(PC_Session session, long bytesTransfered)
{
	m_nBytesSent = bytesTransfered;
}

DWORD WINAPI CDMC::ReflectThreadPoolCreation(LPVOID args)
{
	CDMC *pThis = (CDMC *)args;
	ILibThreadPool_AddThread(pThis->m_threadPool);
	return 0;
}

void CDMC::MSRendererAdded(void* token, struct AVRenderer* renderer)
{
	CDMC* pThis = (CDMC*)RCPToken_GetTag(token);
	if(token != NULL)
	{
		pThis->AddRenderer(renderer);
	}
}

void CDMC::MSRendererRemoved(void* token, struct AVRenderer* renderer)
{
	CDMC* pThis = (CDMC*)RCPToken_GetTag(token);
	if(token != NULL)
	{
		pThis->RemoveRenderer(renderer);
	}
}

void CDMC::AddRenderer(struct AVRenderer* renderer)
{
	if (renderer)
	{
		renderer->Tag = this;
		renderer->Connection->Tag = this;
		
		m_AVRendererList.push_back(renderer);

		std::string rendererInfo("RendererInfo:\r\n");
		rendererInfo += renderer->FriendlyName;
		rendererInfo += "\r\n";

		for (int i=0; i<renderer->ProtocolInfoLength; ++i)
		{
			rendererInfo += (renderer->ProtocolInfo)[i];
			rendererInfo += "\r\n";
		}

		m_RendererInfoList.push_back(rendererInfo);

		//PostMessage(ADD_DMR, (WPARAM)0, (LPARAM)renderer);
		NotifyRendererChanged();
	}	
}

void CDMC::RemoveRenderer(struct AVRenderer* renderer)
{
	int index=-1;
	int i=0;

	for (std::vector<AVRenderer*>::iterator it = m_AVRendererList.begin();
		it != m_AVRendererList.end(); i++)
	{
		if (*it == renderer)
		{
			it = m_AVRendererList.erase(it);
			index = i;
			break;
		}
		else
		{
			++it;
		}
	}
	//PostMessage(REMOVE_DMR, (WPARAM)0, (LPARAM)renderer
	NotifyRendererChanged();
}

void CDMC::ReflectOnAddMSDevice(struct UPnPDevice* device)
{
	CDMC* pThis = (CDMC*)MediaServerCP_GetUser(device->CP);
	if(pThis != NULL)
	{
		pThis->OnAddMSDevice(device);
	}
}

void CDMC::ReflectOnRemoveMSDevice(struct UPnPDevice* device)
{
	CDMC* pThis = (CDMC*)MediaServerCP_GetUser(device->CP);
	if(pThis != NULL)
	{
		pThis->OnRemoveMSDevice(device);
	}
}

void CDMC::OnAddMSDevice(struct UPnPDevice* device)
{
	//PostMessage(ADD_SERVER, 0, (LPARAM)device);
	char* dlnaCap = MediaServerCP_GetCustomXML_X_DLNACAP(device);
	char* dlnaDoc = MediaServerCP_GetCustomXML_X_DLNADOC(device);
	if(dlnaDoc != NULL && (strcmp(dlnaDoc, "DMS-1.00") == 0 || strcmp(dlnaDoc, "M-DMS-1.00") == 0))
	{
		//imageIndex = 3;
	}
	else if(dlnaDoc != NULL && (strcmp(dlnaDoc, "DMS-1.50") == 0 || strcmp(dlnaDoc, "M-DMS-1.50") == 0))
	{
		//imageIndex = 4;
	}
}

void CDMC::OnRemoveMSDevice(struct UPnPDevice* device)
{
	//PostMessage(REMOVE_SERVER, 0, (LPARAM)device);
}

//*********************************************************************
// IP Address Monitoring Callback Reflection Code										 
//*********************************************************************
DWORD WINAPI CDMC::ReflectIPAddressMonitorLoop(LPVOID args)
{
	DWORD rv = 0;
	// Reflect the callback into the CMainFrame instance....
	if(args != NULL)
	{
		CDMC* pThis = (CDMC*)args;
		rv = pThis->IPAddressMonitorLoop();
	}
	return rv;
}

void CALLBACK CDMC::ReflectOnIPAddressChanged(DWORD dwError,
														  DWORD cbTransferred,
														  LPWSAOVERLAPPED lpOverlapped,
														  DWORD dwFlags)
{
	// Reflect the callback into the CMainFrame instance....
	CDMC* pThis = g_This;
	
	if(pThis != NULL)
	{
		pThis->OnIPAddressChanged();
	}
}
//*********************************************************************


//*********************************************************************
// IP Address Monitoring Callback Member Code										 
//*********************************************************************
DWORD CDMC::IPAddressMonitorLoop()
{
	int rv = 0;
	int error = 0;

	// Create the socket to use for monitoring the IP addresses...
	m_ipMonitorSocket = socket(AF_INET, SOCK_DGRAM, 0);
	error = WSAGetLastError();
	if(m_ipMonitorSocket != INVALID_SOCKET)
	{
		// Setup the asyncronous notification of IP Address changes on the system...
		rv = WSAIoctl(m_ipMonitorSocket, 
			SIO_ADDRESS_LIST_CHANGE,
			NULL,
			0,
			NULL,
			0,
			&m_ipMonitorbytesReturned,
			&m_ipMonitorOverlapped,
			&CDMC::ReflectOnIPAddressChanged);
		error = WSAGetLastError();
		if(rv != SOCKET_ERROR || (rv == SOCKET_ERROR && error == WSA_IO_PENDING))
		{
			// Wait until the app signals to stop...
			while(WaitForSingleObjectEx(m_ipMonitorLoopEvent, INFINITE, TRUE) 
				!= WAIT_OBJECT_0);

			return 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

void CDMC::OnIPAddressChanged()
{
	if(IsChainRunning() == TRUE)
	{
		RCP_IPAddressChanged(m_rendererToken);
		MediaServerCP__CP_IPAddressListChanged(m_mediaServerToken);
	}

	WSAIoctl(m_ipMonitorSocket,
		SIO_ADDRESS_LIST_CHANGE,
		NULL,
		0,
		NULL,
		0,
		&m_ipMonitorbytesReturned,
		&m_ipMonitorOverlapped,
		&CDMC::ReflectOnIPAddressChanged);
}
//*********************************************************************


//*********************************************************************
// Start and Stop IP Address Monitoring
//*********************************************************************
void CDMC::StartIPAddressMonitoring()
{
	// Only do this if the previous monitor thread has been closed...
	if(m_ipMonitorThread == NULL)
	{
		DWORD id;
		memset((void*)&m_ipMonitorOverlapped, 0, sizeof(WSAOVERLAPPED));

		// Create the event that will stop the monitoring when signalled...
		m_ipMonitorLoopEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
		ResetEvent(m_ipMonitorLoopEvent);

		// Start the thread for monitoring the IP addresses...
		m_ipMonitorThread = CreateThread(NULL, 0, 
			&CDMC::ReflectIPAddressMonitorLoop, this, 0, &id);
	}
}

void CDMC::StopIPAddressMonitoring()
{
	// Only do this if the monitoring thread is running....
	if(m_ipMonitorThread != NULL)
	{
		// Signal to stop the monitoring thread...
		SetEvent(m_ipMonitorLoopEvent);

		// Wait for the thread to end...
		WaitForSingleObjectEx(m_ipMonitorThread, INFINITE, FALSE);

		// Close event and socket...
		CloseHandle(m_ipMonitorLoopEvent);
		closesocket(m_ipMonitorSocket);

		// Clear values...
		m_ipMonitorSocket = NULL;
		m_ipMonitorThread = NULL;
	}
}

BOOL CDMC::IsIPAddressMonitoringRunning()
{
	return (m_ipMonitorThread!=NULL)?TRUE:FALSE;
}

//*********************************************************************
// Start and Stop Chain Methods
//*********************************************************************
DWORD WINAPI CDMC::ReflectStartChainThread(LPVOID args)
{
	CDMC* instance = (CDMC*)args;
	if(instance != NULL)
	{
		return instance->StartChainThread();
	}
	else
	{
		return -1;
	}
}

DWORD CDMC::StartChainThread()
{
	m_chainRunning = TRUE;
	ILibStartChain(m_chain);
	m_chainRunning = FALSE;

	return 0;
}

void CDMC::StartChain()
{
	if(IsChainRunning() == FALSE)
	{
		DWORD id;
		m_microstackThread = CreateThread(NULL, 0, &CDMC::ReflectStartChainThread, this, 0, &id);
	}
}

void CDMC::StopChain()
{
	if(IsChainRunning() == TRUE)
	{
		ILibStopChain(m_chain);
//  		if (WAIT_TIMEOUT == ::WaitForSingleObject(m_microstackThread, 1000))
//   			TerminateThread(m_microstackThread, 0);
	}
	::WaitForSingleObject(m_microstackThread, INFINITE);
	m_microstackThread = NULL;
}

BOOL CDMC::IsChainRunning()
{
	return (m_chainRunning == TRUE && ILibIsChainBeingDestroyed(m_chain) == 0)?TRUE:FALSE;
}

void CDMC::NotifyRendererChanged()
{
	if (m_pfnOnDlnaEvent)
	{
		m_RenderersInfo.clear();
		for (std::vector<AVRenderer*>::iterator it = m_AVRendererList.begin();
			it != m_AVRendererList.end(); ++it)
		{
			if ((*it)->FriendlyName != NULL && (*it)->device->UDN)
			{
				m_RenderersInfo += string2wstring((*it)->FriendlyName);
				m_RenderersInfo += L";";
				m_RenderersInfo += string2wstring((*it)->device->UDN);
				m_RenderersInfo += L"\r\n";
			}
		}

		m_DlnaEventInfo.dwEventCode = DLNAEVENT_TYPE_DEVICE_CHANGED;
		_tcscpy(m_DlnaEventInfo.szEventParameter, m_RenderersInfo.c_str());
		m_pfnOnDlnaEvent(&m_DlnaEventInfo, m_pCallBackParameter);
	}
}

void CDMC::NotifyError()
{
	if (m_pfnOnDlnaEvent)
	{
		m_DlnaEventInfo.dwEventCode = DLNAEVENT_TYPE_ERROR;
		_tcscpy(m_DlnaEventInfo.szEventParameter, m_errorMessage.c_str());
		m_pfnOnDlnaEvent(&m_DlnaEventInfo, m_pCallBackParameter);
	}
}

void* StdOpen (const char* url)
{
	FILE* f = NULL;
	f = fopen(url, "rb");
	return f;
}

int StdClose (void* handle)
{
	return fclose((FILE*)handle);
}

int StdRead (void* handle, unsigned char* buffer, long len, long* read)
{
	size_t read_size = fread(buffer, sizeof(char), len, (FILE*)handle);
	if (read)
		*read = read_size;
	return (read_size >= 0) ? 0 : -1;
}

int StdSeek (void* handle, long offset, int method)
{
	return fseek((FILE*)handle, offset, method);
}

int StdTell (void* handle, unsigned long long* offset)
{
	long off = ftell((FILE*)handle);
	if (offset)
		*offset = off;
	return (off>=0) ? 0: -1;
}

