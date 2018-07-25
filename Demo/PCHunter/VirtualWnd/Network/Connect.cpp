#include "StdAfx.h"
#include "Connect.h"
#include "Utils/TextTools.h"
#include "DllCore/Authority/Process.h"

#define TABLE_SIZE	2048

CConnect::CConnect()
{
	m_bLoad = FALSE;
	m_pPaintManager = NULL;
}

CConnect::~CConnect()
{
	m_pPaintManager = NULL;
}

DUI_BEGIN_MESSAGE_MAP(CConnect, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_REFRESH, OnRefresh)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnLoadItem)
DUI_END_MESSAGE_MAP()

void CConnect::SetPaintManager(CPaintManagerUI *pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void SaveIcon(HICON hIconToSave, LPCTSTR sIconFileName)
{
	if(hIconToSave==NULL || sIconFileName==NULL)
		return;
	//warning: this code snippet is not bullet proof.
	//do error check by yourself [masterz]
	PICTDESC picdesc;
	picdesc.cbSizeofstruct = sizeof(PICTDESC);
	picdesc.picType = PICTYPE_ICON ;            
	picdesc.icon.hicon = hIconToSave;
	IPicture* pPicture=NULL;
	OleCreatePictureIndirect(&picdesc, IID_IPicture, TRUE,(VOID**)&pPicture);
	LPSTREAM pStream;
	CreateStreamOnHGlobal(NULL,TRUE,&pStream);
	LONG size;
	HRESULT hr=pPicture->SaveAsFile(pStream,TRUE,&size);

	TCHAR szPath[MAX_PATH];
	_tcscpy_s(szPath,sIconFileName);
// 	CFile iconfile;
// 	iconfile.Open(szPath, CFile::modeCreate|CFile::modeWrite);
	HANDLE hFile = CreateFile(szPath, GENERIC_ALL, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, OPEN_ALWAYS,0,NULL);
	LARGE_INTEGER li;
	li.HighPart =0;
	li.LowPart =0;
	ULARGE_INTEGER ulnewpos;
	pStream->Seek( li,STREAM_SEEK_SET,&ulnewpos);
	ULONG uReadCount = 1;
	DWORD dwWrite = 0;
	while(uReadCount>0)
	{
		BYTE bData[MAX_PATH];
		pStream->Read(bData,sizeof(bData),&uReadCount);
		if(uReadCount>0)
			WriteFile(hFile,bData,uReadCount,&dwWrite,NULL);
	}
	pStream->Release();
	CloseHandle(hFile);
}

void CConnect::OnRefresh(TNotifyUI& msg)
{
	m_bLoad = FALSE;
	OnLoadItem(msg);
}

void CConnect::OnLoadItem(TNotifyUI& msg)
{
	if (m_bLoad != FALSE)
		return;
	m_bLoad = TRUE;

	CConnectList ConnectListInfo;
	GetTcpConnectionTable(ConnectListInfo);
	GetUdpConnectionTable(ConnectListInfo);

	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("Connect"));
	if (pList->GetCount())
		pList->RemoveAll();

	TListInfoUI* pListInfo = pList->GetListInfo();
	POSITION pos = ConnectListInfo.GetHeadPosition();
	CDuiString strTipInfo;
	while (pos)
	{
		const CONNECTION_INFO& ConnectionInfo = ConnectListInfo.GetNext(pos);
		CFileListItemUI* pConnectListInfo = NULL;
		if (m_DialogBuilder.GetMarkup()->IsValid() == false)
			pConnectListInfo = (CFileListItemUI*)m_DialogBuilder.Create(CONNECT_LIST_ITEM_INFO, NULL, &m_RootBuilder, m_pPaintManager);
		else
			pConnectListInfo = (CFileListItemUI*)m_DialogBuilder.Create(&m_RootBuilder, m_pPaintManager);

		pList->Add(pConnectListInfo);
		pConnectListInfo->SetFixedHeight(27);
		
		// 显示协议图标
		CPictureUI* pProtoType = (CPictureUI*)pConnectListInfo->FindSubControl(_T("ProtoType"));
		if (pProtoType != NULL)
		{
			CDuiString strForeImage;
			if (ConnectionInfo.dwProtoclType == IPPROTO_TCP)
				strForeImage.Format(_T("file='icon.png' source='16,0,32,16'"));
			else
				strForeImage.Format(_T("file='icon.png' source='32,0,48,16'"));
			pProtoType->SetForeImage(strForeImage);
		}

		// 显示协议类型
		CLabelUI* pType = (CLabelUI*)pConnectListInfo->FindSubControl(_T("Type"));
		pType->SetText(ConnectionInfo.dwProtoclType != IPPROTO_TCP ? _T("UDP") : _T("TCP"));
		pType->SetFont(pListInfo->nFont);
		pType->SetForeColor(pListInfo->dwTextColor);

		CLabelUI* pLocalAddr = (CLabelUI*)pConnectListInfo->FindSubControl(_T("LocalAddr"));
		pLocalAddr->SetText(GetFormatIPAndPort(ConnectionInfo.dwLocalIp, ntohs((WORD)ConnectionInfo.dwLocalPort)));
		pLocalAddr->SetFont(pListInfo->nFont);
		pLocalAddr->SetForeColor(pListInfo->dwTextColor);
	
		// 根据协议类型，显示不同数据类型
		if (ConnectionInfo.dwProtoclType == IPPROTO_TCP)
		{
			CLabelUI* pRemoteAddr = (CLabelUI*)pConnectListInfo->FindSubControl(_T("RemoteAddr"));
			pRemoteAddr->SetText(GetFormatIPAndPort(ConnectionInfo.dwRemoteIp,ntohs((WORD)ConnectionInfo.dwRemotePort)));
			pRemoteAddr->SetFont(pListInfo->nFont);
			pRemoteAddr->SetForeColor(pListInfo->dwTextColor);

			CLabelUI* pState = (CLabelUI*)pConnectListInfo->FindSubControl(_T("State"));
			pState->SetText(GetConnectionState(ConnectionInfo.dwConnectionState));
			pState->SetFont(pListInfo->nFont);
			pState->SetForeColor(pListInfo->dwTextColor);
		}
		else 
		{
			CTime tm(ConnectionInfo.nCreateTime/10000000);
			strTipInfo.Format(_T("%02d:%02d:%02d"),tm.GetHour(),tm.GetMinute(),tm.GetSecond());

			CLabelUI* pTime = (CLabelUI*)pConnectListInfo->FindSubControl(_T("Time"));
			pTime->SetText(strTipInfo);
			pTime->SetFont(pListInfo->nFont);
			pTime->SetForeColor(pListInfo->dwTextColor);
		}

		// 显示进程ID
		strTipInfo.Format(_T("%d"),ConnectionInfo.nPID);
		CLabelUI* pPID = (CLabelUI*)pConnectListInfo->FindSubControl(_T("PID"));
		pPID->SetText(strTipInfo);
		pPID->SetFont(pListInfo->nFont);
		pPID->SetForeColor(pListInfo->dwTextColor);		

		// 设置进程或服务名称，并显示文件图标
		BOOL bFindIcon = FALSE;
		CPictureUI* pPicIcon = (CPictureUI*)pConnectListInfo->FindSubControl(_T("PicICON"));
		if (ConnectionInfo.strProcessPath.IsEmpty() == FALSE)
		{
			SHFILEINFO SHFileInfo = {0};
			SHGetFileInfo(ConnectionInfo.strProcessPath, 0, &SHFileInfo, sizeof(SHFileInfo), SHGFI_ICON|SHGFI_SMALLICON);
			if (SHFileInfo.hIcon != NULL)
			{
				bFindIcon = TRUE;
				pPicIcon->SetIcon(SHFileInfo.hIcon);
				DestroyIcon(SHFileInfo.hIcon);
			}
		}

		if (bFindIcon == FALSE && ConnectionInfo.nPID != 0)
		{
			CDuiString strForeImage;
			strForeImage.Format(_T("file='icon.png' source='176,0,192,16'"));
			pPicIcon->SetForeImage(strForeImage);
		}

		CLabelUI* pSrvName = (CLabelUI*)pConnectListInfo->FindSubControl(_T("SrvName"));
		pSrvName->SetText(ConnectionInfo.strProcessName);
		pSrvName->SetFont(pListInfo->nFont);
		pSrvName->SetForeColor(pListInfo->dwTextColor);
		
		// 显示文件路径
		CLabelUI* pSrvPath = (CLabelUI*)pConnectListInfo->FindSubControl(_T("Path"));
		pSrvPath->SetText(ConnectionInfo.strProcessPath);
		pSrvPath->SetFont(pListInfo->nFont);
		pSrvPath->SetForeColor(pListInfo->dwTextColor);
	}
}

void CConnect::GetTcpConnectionTable(CConnectList& TcpListInfo)
{
	BYTE pBuf[TABLE_SIZE];
	DWORD dwSize = TABLE_SIZE;
	PMIB_TCPTABLE_OWNER_MODULE pTcpTable = (PMIB_TCPTABLE_OWNER_MODULE)pBuf;
	DWORD dwSizeData=sizeof(PMIB_TCPTABLE_OWNER_MODULE);
	DWORD dwRet = GetExtendedTcpTable(pTcpTable,&dwSizeData,FALSE,AF_INET,TCP_TABLE_OWNER_MODULE_ALL,0);
	if (dwRet == ERROR_INSUFFICIENT_BUFFER)
	{
		pTcpTable = (PMIB_TCPTABLE_OWNER_MODULE) new BYTE[dwSizeData];
		if (pTcpTable != NULL)
			dwRet = GetExtendedTcpTable(pTcpTable,&dwSizeData,TRUE,AF_INET,TCP_TABLE_OWNER_MODULE_ALL,0);
	}
	if (dwRet == NO_ERROR)
	{
		CString strProcessName, strLocalProcessPath;
		for (DWORD dwIndex = 0 ; dwIndex < pTcpTable->dwNumEntries ; dwIndex++)
		{
			DWORD dwConnectionState = pTcpTable->table[dwIndex].dwState;
			DWORD PID = pTcpTable->table[dwIndex].dwOwningPid;
			//获取模块路径
			GetProcessFullPath(PID, strLocalProcessPath);
			
			BYTE pData[TABLE_SIZE];
			TCPIP_OWNER_MODULE_BASIC_INFO* TcpInfo = (PTCPIP_OWNER_MODULE_BASIC_INFO)pData;
			DWORD dwSize = sizeof(TCPIP_OWNER_MODULE_BASIC_INFO);
			DWORD dwRet = GetOwnerModuleFromTcpEntry(&pTcpTable->table[dwIndex],TCPIP_OWNER_MODULE_INFO_BASIC,TcpInfo,&dwSize);
			BOOL bReset = FALSE;
			if (dwRet == ERROR_INSUFFICIENT_BUFFER)
			{
				bReset = TRUE;
				TcpInfo = (PTCPIP_OWNER_MODULE_BASIC_INFO) new BYTE[dwSize];
				if (TcpInfo != NULL)
					dwRet = GetOwnerModuleFromTcpEntry(&pTcpTable->table[dwIndex],TCPIP_OWNER_MODULE_INFO_BASIC,TcpInfo,&dwSize);
			}

			if (dwRet == NO_ERROR)
				strProcessName = TcpInfo->pModuleName;		//进程或服务名
			else
				strProcessName.Empty();
			if (TcpInfo && bReset)
			{
				delete[] TcpInfo;
				TcpInfo = NULL;
			}
			DWORD dwRemoteIP = pTcpTable->table[dwIndex].dwRemoteAddr;           //远程IP
			DWORD dwRemotePort = pTcpTable->table[dwIndex].dwRemotePort;         //远程端口
			if (dwConnectionState == MIB_TCP_STATE_LISTEN)
			{
				dwRemotePort = 0;
				dwRemoteIP   = 0;
			}

			MIB_TCPROW_OWNER_MODULE TcpOwnerModule = pTcpTable->table[dwIndex];

			CONNECTION_INFO ConnectInfo;
			ConnectInfo.dwProtoclType = IPPROTO_TCP;
			ConnectInfo.dwLocalIp = TcpOwnerModule.dwLocalAddr;
			ConnectInfo.dwLocalPort = TcpOwnerModule.dwLocalPort;
			ConnectInfo.dwRemoteIp = TcpOwnerModule.dwRemoteAddr;
			ConnectInfo.dwRemotePort = TcpOwnerModule.dwRemotePort;
			ConnectInfo.dwConnectionState = TcpOwnerModule.dwState;
			ConnectInfo.nCreateTime = TcpOwnerModule.liCreateTimestamp.QuadPart;
			ConnectInfo.nPID = TcpOwnerModule.dwOwningPid;
			ConnectInfo.strProcessName = strProcessName;
			ConnectInfo.strProcessPath = strLocalProcessPath;

			TcpListInfo.AddTail(ConnectInfo);
		}
		if (pTcpTable)
		{
			delete[] pTcpTable;
		}
	}
}

void CConnect::GetUdpConnectionTable(CConnectList& UdpListInfo)
{
	BYTE pBuf[TABLE_SIZE];
	DWORD dwSize = TABLE_SIZE;
	PMIB_UDPTABLE_OWNER_MODULE pTcpTable = (PMIB_UDPTABLE_OWNER_MODULE)pBuf;
	DWORD dwSizeData=sizeof(PMIB_UDPTABLE_OWNER_MODULE);
	DWORD dwRet = GetExtendedUdpTable(pTcpTable,&dwSizeData,FALSE,AF_INET,UDP_TABLE_OWNER_MODULE,0);
	if (dwRet == ERROR_INSUFFICIENT_BUFFER)
	{
		pTcpTable = (PMIB_UDPTABLE_OWNER_MODULE) new BYTE[dwSizeData];
		if (pTcpTable != NULL)
			dwRet = GetExtendedUdpTable(pTcpTable,&dwSizeData,TRUE,AF_INET,UDP_TABLE_OWNER_MODULE,0);
	}
	if (dwRet == NO_ERROR)
	{
		CString strProcessName,strLocalProcessPath;
		for (DWORD dwIndex = 0 ; dwIndex < pTcpTable->dwNumEntries ; dwIndex++)
		{
			DWORD PID = pTcpTable->table[dwIndex].dwOwningPid;
			//获取模块路径
			//GetProcessNameAndPathByPID(PID, strProcessName, strLocalProcessPath);
			GetProcessFullPath(PID, strLocalProcessPath);
			BYTE pData[TABLE_SIZE];
			TCPIP_OWNER_MODULE_BASIC_INFO* TcpInfo = (PTCPIP_OWNER_MODULE_BASIC_INFO)pData;
			DWORD dwSize = sizeof(TCPIP_OWNER_MODULE_BASIC_INFO);
			DWORD dwRet = GetOwnerModuleFromUdpEntry(&pTcpTable->table[dwIndex],TCPIP_OWNER_MODULE_INFO_BASIC,TcpInfo,&dwSize);
			BOOL bReset = FALSE;
			if (dwRet == ERROR_INSUFFICIENT_BUFFER)
			{
				bReset = TRUE;
				TcpInfo = (PTCPIP_OWNER_MODULE_BASIC_INFO) new BYTE[dwSize];
				if (TcpInfo != NULL)
					dwRet = GetOwnerModuleFromUdpEntry(&pTcpTable->table[dwIndex],TCPIP_OWNER_MODULE_INFO_BASIC,TcpInfo,&dwSize);
			}

			if (dwRet == NO_ERROR)
				strProcessName = TcpInfo->pModuleName;		//进程或服务名
			else
				strProcessName.Empty();
			if (TcpInfo && bReset)
			{
				delete[] TcpInfo;
				TcpInfo = NULL;
			}

			MIB_UDPROW_OWNER_MODULE TcpOwnerModule = pTcpTable->table[dwIndex];

			CONNECTION_INFO ConnectInfo;
			ConnectInfo.dwProtoclType = IPPROTO_UDP;
			ConnectInfo.dwLocalIp = TcpOwnerModule.dwLocalAddr;
			ConnectInfo.dwLocalPort = TcpOwnerModule.dwLocalPort;
			ConnectInfo.dwRemoteIp = 0;
			ConnectInfo.dwRemotePort = 0;
			ConnectInfo.dwConnectionState = 0;
			ConnectInfo.nCreateTime = TcpOwnerModule.liCreateTimestamp.QuadPart;
			ConnectInfo.nPID = TcpOwnerModule.dwOwningPid;
			ConnectInfo.strProcessName = strProcessName;
			ConnectInfo.strProcessPath = strLocalProcessPath;

			UdpListInfo.AddTail(ConnectInfo);
		}
		if (pTcpTable)
		{
			delete[] pTcpTable;
		}
	}
}
