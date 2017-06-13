#include "StdAfx.h"
#include "SfxMaker.h"
#include "resource.h"
#include <Shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

#include "Lzma/Lzma.h"
#include "Lzma/MakeSfxFile.h"

CSfxMaker::CSfxMaker()
{

}

CSfxMaker::~CSfxMaker()
{

}

DUI_BEGIN_MESSAGE_MAP(CSfxMaker,WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
DUI_END_MESSAGE_MAP()

LPCTSTR CSfxMaker::GetWindowClassName() const
{
	return _T("DuiLib::CSfxMaker");
}

CDuiString CSfxMaker::GetSkinFile()
{
	return _T("SfxMaker.xml");
}

CDuiString CSfxMaker::GetZIPFileName() const
{
	return _T("Installer.zip");
}

CDuiString CSfxMaker::GetSkinFolder()
{
	return _T("SfxMaker");
}

UILIB_RESOURCETYPE CSfxMaker::GetResourceType() const
{
	return UILIB_FILE;
}

CControlUI* CSfxMaker::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

void CSfxMaker::InitWindow()
{
	SetIcon(IDI_MAINFRAME);
	SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	//OnBtnCheck();
    //CMakeSfxFile MakeSfx;
 	//MakeSfx.ReleaseData();
 
 //	CLzma Lzma;
 //	
	//DWORD dwSize = _countof(lpData);
 //	Lzma.LzmaEncode(_T("C:\\test"),lpData,dwSize);


	/*BYTE lpData[512*1024];
	DWORD dwSize = _countof(lpData);
	HANDLE hFile = CreateFile(_T("C:\\test\\test2.zip"),GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
	DWORD dwWrite = 0;
	ReadFile(hFile,lpData,dwSize,&dwWrite,NULL);
	CloseHandle(hFile);

	CLzma Lzma;
	Lzma.LzmaDecode(lpData,dwWrite,_T("C:\\test2\\"));*/

 	TCHAR szTargetFileName[MAX_PATH];
 	GetModuleFileName(NULL,szTargetFileName,_countof(szTargetFileName));
 	PathRemoveFileSpec(szTargetFileName);
 	PathAppend(szTargetFileName,_T("Install.exe"));
 
 	CMakeSfxFile MakeSfx;
 	MakeSfx.OpenTargetFile(szTargetFileName);
  	PathRemoveFileSpec(szTargetFileName);
  	PathAppend(szTargetFileName,_T("Win32_Debug.zip"));
  
  	HANDLE hFile = CreateFile(szTargetFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
  
  	DWORD dwSize = GetFileSize(hFile,NULL);
 	LPBYTE lpData = new BYTE[dwSize];
  	ReadFile(hFile,lpData,dwSize,&dwSize,NULL);
 	
  	CloseHandle(hFile);
  	if (MakeSfx.AppendData(lpData,dwSize) != FALSE)
 		MakeSfx.MakeBinaryFile(_T("C:\\DumpOk.exe"));
 
 	delete[] lpData;
 	lpData = NULL;
}

LRESULT CSfxMaker::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return WindowImplBase::OnCreate(uMsg,wParam,lParam,bHandled);
}

LRESULT CSfxMaker::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(uMsg)
	{
	case WM_MENUCLICK:
		OnMenuItemClick((CControlUI*)lParam);
		break;
	default:
		bHandled = FALSE;
	}
	return TRUE;
}

void CSfxMaker::OnMenuItemClick(CControlUI* pControl)
{
	ASSERT(pControl);
}

void CSfxMaker::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_TABSELECT)
		OnTabSelect(msg);
}

void CSfxMaker::OnClick(TNotifyUI& msg)
{
	const CDuiString& strCtlName = msg.pSender->GetName();
	if (strCtlName == _T("BtnClose"))
		PostQuitMessage(0);
	else if (strCtlName == _T("BtnMin"))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	else if (strCtlName == _T("BtnNext"))
		OnNext();
	else if (strCtlName == _T("BtnPrev"))
		OnPrev();
}

void CSfxMaker::OnTabSelect(TNotifyUI& msg)
{
	//const CDuiString& strCtlName = msg.pSender->GetName();
	OnBtnCheck();
}

BOOL CSfxMaker::OnBtnCheck()
{
	CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_PaintManager.FindControl(_T("WizardTab"));
	if (pTabLayout == NULL)
		return FALSE;
	int nSel = pTabLayout->GetCurSel();
	CButtonUI* pBtnNext = (CButtonUI*)m_PaintManager.FindControl(_T("BtnNext"));
	CButtonUI* pBtnPrev = (CButtonUI*)m_PaintManager.FindControl(_T("BtnPrev"));
	pBtnPrev->SetEnabled();
	pBtnNext->SetEnabled();

	if ( nSel == pTabLayout->GetCount() - 1)
		pBtnNext->SetEnabled(false);
	else if(nSel == 0)
		pBtnPrev->SetEnabled(false);

	return TRUE;
}

void CSfxMaker::OnNext()
{
	CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_PaintManager.FindControl(_T("WizardTab"));
	if (pTabLayout == NULL)
		return;

	pTabLayout->SelectItem(pTabLayout->GetCurSel() + 1);
}

void CSfxMaker::OnPrev()
{
	CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_PaintManager.FindControl(_T("WizardTab"));
	if (pTabLayout == NULL)
		return;

	pTabLayout->SelectItem(pTabLayout->GetCurSel() - 1);
}
