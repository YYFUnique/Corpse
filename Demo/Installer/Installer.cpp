#include "StdAfx.h"
#include "Installer.h"
#include "resource.h"
#include <Shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

CInstaller::CInstaller()
{

}

CInstaller::~CInstaller()
{

}

DUI_BEGIN_MESSAGE_MAP(CInstaller,WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
DUI_END_MESSAGE_MAP()

LPCTSTR CInstaller::GetWindowClassName() const
{
	return _T("DuiLib::CInstaller");
}

CDuiString CInstaller::GetSkinFile()
{
	return _T("Installer.xml");
}

CDuiString CInstaller::GetZIPFileName() const
{
	return _T("Installer.zip");
}

CDuiString CInstaller::GetSkinFolder()
{
	return _T("");
}

LPCTSTR CInstaller::GetResourceID() const  
{  
	return MAKEINTRESOURCE(IDR_ZIP_SKIN);
};

void CInstaller::Notify(TNotifyUI& msg)
{
	CDuiString strCtlName = msg.pSender->GetName();

	if (msg.sType == _T("click"))
	{
		if (strCtlName == _T("BtnClose"))
			PostQuitMessage(0);
		else if (strCtlName == _T("BtnMin"))
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
		else
		{
			CTabLayoutUI* pWizardTab = (CTabLayoutUI*)m_PaintManager.FindControl(_T("WizardTab"));
			if (strCtlName == _T("BtnAccept") || strCtlName == _T("BtnNext"))
				pWizardTab->SelectItem(pWizardTab->GetCurSel()+1);
			else if (strCtlName == _T("BtnBack"))
				pWizardTab->SelectItem(pWizardTab->GetCurSel()-1);
			else if (strCtlName == _T("BtnCancel"))
				PostQuitMessage(0);
			else if (strCtlName == _T("BtnComplete"))
				PostQuitMessage(0);
		}
	}
}

CControlUI* CInstaller::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

void CInstaller::InitWindow()
{
	//SetIcon(IDI_MAINFRAME);

    //CMakeSfxFile MakeSfx;
 	//MakeSfx.ReleaseData();
 

 	/*CLzma Lzma;
 	
	DWORD dwSize = _countof(lpData);
 	Lzma.LzmaEncode(_T("C:\\test"),lpData,dwSize);
*/

	//BYTE lpData[512*1024];
	//DWORD dwSize = _countof(lpData);
	//HANDLE hFile = CreateFile(_T("C:\\test\\test2.zip"),GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
	//DWORD dwWrite = 0;
	//ReadFile(hFile,lpData,dwSize,&dwWrite,NULL);
	//CloseHandle(hFile);

	//CLzma Lzma;
	//Lzma.LzmaDecode(lpData,dwWrite,_T("C:\\test2\\"));

 	/*TCHAR szTargetFileName[MAX_PATH];
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
 	lpData = NULL;*/
}

void CInstaller::OnClick(DuiLib::TNotifyUI &msg)
{

}

LRESULT CInstaller::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return WindowImplBase::OnCreate(uMsg,wParam,lParam,bHandled);
}

LRESULT CInstaller::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

void CInstaller::OnMenuItemClick(CControlUI* pControl)
{
	ASSERT(pControl);
}
