#include "StdAfx.h"
#include "InjectDll.h"
#include "Resource.h"
#include "Wow64Ext/Wow64InjectDll.h"
#include "DllCore/Utils/OsInfo.h"
#include "DllCore/Log/LogHelper.h"
#include "DllCore/Window/AdsorbWnd.h"
#include <CommDlg.h>
#include <tlhelp32.h>

//共享内存结构体，用于存储保护的PID
typedef struct SHWP_STRUCT_
{
	DWORD dwProcessId;
	TCHAR	   szEvent[20];
} SHWP_STRUCT, *LPSHWP_STRUCT;

CInjectDll::CInjectDll()
{
	m_pListView = NULL;
	m_pRemoteInject = NULL;
}

CInjectDll::~CInjectDll()
{
	if (m_pListView != NULL)
	{
		delete m_pListView;
	}

	if (m_pRemoteInject != NULL)
		delete m_pRemoteInject;

	POSITION pos = m_InjectInfoList.GetHeadPosition();
	while(pos)
	{
		const INJECT_INFO& InjectInfo = m_InjectInfoList.GetNext(pos);
		if (InjectInfo.hEvent != NULL)
		{
			SetEvent(InjectInfo.hEvent);
			CloseHandle(InjectInfo.hEvent);
		}
	}
}

void CInjectDll::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CInjectDll::GetWindowClassName() const
{
	return _T("InjectDll");
}

UILIB_RESOURCETYPE CInjectDll::GetResourceType() const
{
	return UILIB_FILE;
}

CDuiString CInjectDll::GetZIPFileName() const
{
	return _T("InjectDll.zip");
}

CDuiString CInjectDll::GetSkinFile()
{
	return _T("InjectDll.xml");
}

CDuiString CInjectDll::GetSkinFolder()
{
	return _T("InjectDll");
}

void CInjectDll::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_TEXTCHANGED)
		OnTextChanged(msg);
	else if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
		OnWindowInit(msg);
	else if (msg.sType == DUI_MSGTYPE_RETURN)
		OnReturn(msg);
	else if (msg.sType == DUI_MSGTYPE_KILLFOCUS)
		OnKillFocus(msg);
}

void CInjectDll::InitWindow()
{
	SetIcon(IDI_MAINFRAME);

	AdsorbWnd* pAdsorbWnd = AdsorbWnd::GetInstance();
	pAdsorbWnd->Initialize(m_hWnd, 50, DOCK_TYPE_ALL);
	//以32位编译的应用程序，只能注入到32位应用程序中
	//m_RemoteInject.Attach(_T("explorer.exe"), _T("C:\\test\\ITrayClock.dll"), 2000);
	//Wow64InjectX64(6016,_T("E:\\ProjectCode\\Remote V0.0.1\\Corpse\\Bin\\x64_Release\\ITrayClock.dll"));
	CEditUI2* pProcessID = (CEditUI2*)m_PaintManager.FindControl(_T("ProcessID"));
	pProcessID->SetFocus();
}

LRESULT CInjectDll::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (IsWindowVisible(m_hWnd) == FALSE)
		return TRUE;
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	CControlUI* pHover = m_PaintManager.FindControl(pt);
	if( pHover == NULL ) return 0;
	if (pHover->GetName() == _T("ProcessID"))
		m_pListView->ShowWindow(true,false);

	return TRUE;
}

LRESULT CInjectDll::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	/*if (uMsg == WM_TOGGLEDESKTOP)
	{
		if (lParam == DTRF_RAISE)
		{
			SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
		}
		else if (lParam == DTRF_LOWER)
		{
			DWORD dwStyle = GetWindowLongPtr(m_hWnd, GWL_EXSTYLE);
			if (dwStyle & WS_EX_TOPMOST)
			{
				SetWindowPos(m_hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

				HWND hWnd = (HWND)GetForegroundWindow();
				if (hWnd == m_hWnd)
				{
					do 
					{
						hWnd = GetWindow(hWnd, GW_HWNDNEXT);
						BOOL bRealWnd = ::IsWindowVisible(hWnd);
						if (::IsWindow(hWnd) && ::IsWindowVisible(hWnd) && ::IsWindowEnabled(hWnd) &&
							(GetWindowLong(hWnd, GWL_HWNDPARENT) == 0))
							break;
					} while (TRUE);
				}
				BringWindowToTop(hWnd);
			}
		}
	}*/

	return WindowImplBase::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}

LRESULT CInjectDll::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bHandle = FALSE;
	if (uMsg == WM_KEYDOWN) {
		if (m_pListView->OnKeyDown(uMsg, wParam, lParam, bHandle) == FALSE)
			return FALSE;
	} else if (uMsg == WM_WINDOWPOSCHANGING) {
		if (m_pListView == NULL)
			return TRUE;
		return m_pListView->OnPosChanged(uMsg, wParam, lParam, bHandle);
	} else if (uMsg == WM_MOUSEWHEEL) {
		return m_pListView->OnMouseWheel(uMsg, wParam, lParam, bHandle);
	} else if (uMsg == WM_NCLBUTTONDOWN || uMsg == WM_NCRBUTTONDOWN) {
		// 点击应用程序标题栏应该关闭提示框
		if (wParam == HTCAPTION)
			m_pListView->ShowWindow(false);
	}
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

void CInjectDll::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == m_PaintManager.FindControl(_T("BtnClose")))
	{
		Close(IDOK);
		PostQuitMessage(0);
	}
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnMin")))
	{
		PostMessage(WM_SYSCOMMAND, SC_MINIMIZE, NULL);
	}
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnLookup")))
		OnFileLookup(msg);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnInject")))
	{
		CEditUI2* pProcessID = (CEditUI2*)m_PaintManager.FindControl(_T("ProcessID"));
		CEditUI2* pDllFile = (CEditUI2*)m_PaintManager.FindControl(_T("DllFile"));
		//MessageBox(m_hWnd, pProcessID->GetText(), _T("提示"), MB_OK|MB_ICONINFORMATION);
		if (pProcessID->GetText().IsEmpty())
		{
			MessageBox(m_hWnd, _T("需要注入进程ID不能为空，请输入进程ID！"), _T("提示"), MB_OK|MB_ICONINFORMATION);
			pProcessID->SetFocus();
			return;
		}

		if (pDllFile->GetText().IsEmpty())
		{
			MessageBox(m_hWnd, _T("请选择需要注入的动态库文件！"), _T("提示"), MB_OK|MB_ICONINFORMATION);
			return;
		}

		UINT nProcessId = pProcessID->GetTag();

		//内存映射句柄
		//HANDLE		hMapping;
		////共享内存结构体
		//LPSHWP_STRUCT	lpData;

		////创建内存共享
		//hMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x100, _T("PCMONITOR.")); 
		//if(hMapping != NULL) 
		//{ 
		//	lpData = (LPSHWP_STRUCT)MapViewOfFile(hMapping,FILE_MAP_ALL_ACCESS,0,0,0); 
		//	lpData->dwProcessId = 4880; 

		//	SECURITY_DESCRIPTOR SecutityDese; 
		//	::InitializeSecurityDescriptor(&SecutityDese, SECURITY_DESCRIPTOR_REVISION); 
		//	::SetSecurityDescriptorDacl(&SecutityDese,TRUE,NULL,FALSE); 

		//	SECURITY_ATTRIBUTES SecurityAttr; 
		//	SecurityAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
		//	SecurityAttr.bInheritHandle = FALSE; 
		//	SecurityAttr.lpSecurityDescriptor = &SecutityDese;

		//	/*HANDLE hEvent = CreateEvent(&SecurityAttr, FALSE, FALSE, _T("Global\\InjectDll_Event"));
		//	ResetEvent(lpData->hEvent);*/
		//}

		BOOL bInjectRet = FALSE;
		// 用于32位进程注入64应用，进程位数相同，请使用原始方式
		if (Is64BitPorcess(nProcessId))
		{
			bInjectRet = Wow64InjectX64(nProcessId, pDllFile->GetText());
			if (bInjectRet)
				MessageBox(m_hWnd, _T("远程进程加载动态库成功！"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		}
		else
		{
			bInjectRet = InjectDll(nProcessId, pDllFile->GetText());
			if (bInjectRet)
				MessageBox(m_hWnd, _T("远程进程加载动态库成功！"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		}
	}
}

void CInjectDll::OnKillFocus(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("ProcessID"))
	{
		if (m_pListView != NULL)
			m_pListView->ShowWindow(false);
	}
}

void CInjectDll::OnWindowInit(TNotifyUI& msg)
{
	CEditUI2* pPID = (CEditUI2*)m_PaintManager.FindControl(_T("ProcessID"));
	m_pListView = new CListViewWnd(m_hWnd, pPID);	
}

void CInjectDll::OnTextChanged(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("ProcessID"))
	{
		CDuiString strProcess = msg.pSender->GetText();

		CProcessInfoList ProcessInfoList;
		EnumProcessesBySnapshot(ProcessInfoList);
		m_pListView->RemoveAll();

		POSITION pos = ProcessInfoList.GetHeadPosition();
		while(pos)
		{
			const PROCESS_INFO& ProcessInfo = ProcessInfoList.GetNext(pos);
			if (strProcess.IsEmpty() || _tcsnicmp(ProcessInfo.strProcessName, strProcess, strProcess.GetLength()) != 0)
				continue;
			m_pListView->Add(ProcessInfo);
		}
		m_pListView->ResizeWnd();		
	}
}

void CInjectDll::OnReturn(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("ProcessID"))
	{
		m_pListView->ShowWindow(false);
	}
}

void CInjectDll::OnFileLookup(TNotifyUI& msg)
{
	TCHAR szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof(szFilePath));

	OPENFILENAME SaveFileName = {0};
	SaveFileName.lStructSize		= sizeof(SaveFileName); 
	SaveFileName.hwndOwner		= m_hWnd; 
	SaveFileName.lpstrFile			= szFilePath; 
	SaveFileName.nMaxFile			= _countof(szFilePath);
	SaveFileName.lpstrFilter			= _T("动态链接库\0*.dll\0\0"); 
	SaveFileName.nFilterIndex		= 1; 
	SaveFileName.lpstrFileTitle	= NULL; 
	SaveFileName.nMaxFileTitle	= 0; 
	SaveFileName.lpstrTitle			= _T("请选择需要注入的动态库");
	SaveFileName.lpstrInitialDir	= NULL ; 
	SaveFileName.Flags				= OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST ; 

	if (GetOpenFileName(&SaveFileName) == FALSE)
		return ;

	CEditUI2* pDllFilePath = (CEditUI2*)m_PaintManager.FindControl(_T("DllFile"));
	if (pDllFilePath != NULL)
		pDllFilePath->SetText(szFilePath);
}

BOOL CInjectDll::EnumProcessesBySnapshot(CProcessInfoList& ProcessInfoList)
{
	BOOL bSuccess = FALSE;
	HANDLE hSnap;
	DWORD dwWinlogonProcessId = 0;
	do
	{
		hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnap == INVALID_HANDLE_VALUE)
			break;

		PROCESSENTRY32 ProcessEntry;
		ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hSnap, &ProcessEntry) == FALSE)
			break;

		do
		{
			PROCESS_INFO ProcessInfo;
			ProcessInfo.nProcessId = ProcessEntry.th32ProcessID;
			ProcessInfo.strProcessName = ProcessEntry.szExeFile;
			ProcessInfoList.AddTail(ProcessInfo);
		}while(Process32Next(hSnap, &ProcessEntry) == TRUE);

		bSuccess = TRUE;
	}while(FALSE);

	if (hSnap != INVALID_HANDLE_VALUE)
		CloseHandle(hSnap);

	return bSuccess;
}

BOOL CInjectDll::InjectDll(DWORD dwRemoteProcessId, LPCTSTR lpszFilePath)
{
	if (m_pRemoteInject == NULL)
		m_pRemoteInject = new CRemoteThreadInject;
	return m_pRemoteInject->InjectDll(dwRemoteProcessId, lpszFilePath);
}