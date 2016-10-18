#include "stdafx.h"
#include "Toolbar.h"
#include "Resource.h"
#include "Lzma/Lzma.h"
#include "Lzma/MakeSfxFile.h"

CToolbar::CToolbar()
{
	
}

LPCTSTR CToolbar::GetWindowClassName() const
{
	return _T("Toolbar");
}

CDuiString CToolbar::GetSkinFile()
{
	return _T("main.xml");
}

CDuiString CToolbar::GetZIPFileName() const
{
	return m_strZipFileName;
}

CDuiString CToolbar::GetSkinFolder()
{
	return _T("Toolbar");
}

UILIB_RESOURCETYPE CToolbar::GetResourceType() const
{
	return UILIB_FILE;
}

void CToolbar::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if(msg.sType == DUI_MSGTYPE_MENU)
		OnItemMenu(msg);
	else if(msg.sType == DUI_MSGTYPE_ITEMMENU)
		OnItemMenu(msg);
}

void CToolbar::InitWindow()
{
	SetIcon(IDI_MAINFRAME);
	RECT rcWnd = {0};
	GetWindowRect(m_hWnd,&rcWnd);
	int nWidth = GetSystemMetrics(SM_CXSCREEN);
	int nLeft = (nWidth - (rcWnd.right - rcWnd.left))/2;
	SetWindowPos(m_hWnd,HWND_TOPMOST,nLeft,0,0,0,SWP_NOSIZE|SWP_NOSENDCHANGING);
 /*
 	CMakeSfxFile MakeSfx;
 	if (MakeSfx.OpenTargetFile(_T("E:\\ProjectCode\\Remote V0.0.1\\Corpse\\Bin\\Win32_Debug\\SfxFile.exe")) == FALSE)
		return;
	
	CLzma Lzma;
	BYTE lpData[2048];

	PSFX_PACKAGE_INFO pSfxPackageInfo = (PSFX_PACKAGE_INFO)lpData;
	pSfxPackageInfo->dwRunMode = ONE_INSTANCE_CHECK;
	pSfxPackageInfo->UacOption = UAC_OPTION_NONE;
	pSfxPackageInfo->UnpackageOption = UNPACKAGE_OPTION_TEMP;
	pSfxPackageInfo->UnpackageOperate = UNPACKAGE_OPERATE_OPENDIRECTORY;

	UNPACKAGE_OPERATE_INFO OperateInfo;
	ZeroMemory(&OperateInfo,sizeof(UNPACKAGE_OPERATE_INFO));
	OperateInfo.bOpenDirectory = TRUE;
	memcpy(lpData+sizeof(SFX_PACKAGE_INFO),&OperateInfo,sizeof(UNPACKAGE_OPERATE_INFO));

 	if (MakeSfx.AppendData(lpData,sizeof(SFX_PACKAGE_INFO)+sizeof(UNPACKAGE_OPERATE_INFO),SECTION_TYPE_DATA) == FALSE)
		return;

	BYTE bData[1024*900];
	DWORD dwSize = _countof(bData);
	LPBYTE lpEncodeData = bData;
 	
	do 
	{
		BOOL bRet = Lzma.LzmaEncode(_T("E:\\ProjectCode\\Remote V0.0.1\\Corpse\\Bin\\Win32_Debug\\Installer"),lpEncodeData,dwSize);
		if (bRet == FALSE)
		{
			lpEncodeData = new BYTE[dwSize];
			if (lpEncodeData == NULL)
				break;

			bRet = Lzma.LzmaEncode(_T("E:\\ProjectCode\\Remote V0.0.1\\Corpse\\Bin\\Win32_Debug\\Installer"),lpEncodeData,dwSize);
		}
		if (bRet == FALSE)
			break;

		if (MakeSfx.AppendData(lpEncodeData,dwSize) == FALSE)
			break;
		MakeSfx.MakeBinaryFile(_T("C:\\LSGuardClientSetup.exe"));
	} while (FALSE);
	
	if (lpEncodeData != NULL && lpEncodeData != bData)
		delete[] lpEncodeData;*/
}

CControlUI* CToolbar::CreateControl(LPCTSTR pstrClass)
{
	CControlUI* pControl = NULL;
	if (_tcsicmp(pstrClass,_T("Win32Control")) == 0)
	{
		HWND hButton = CreateWindow(_T("Button"), _T("有种就来点击我！"), WS_VISIBLE | WS_CHILD,     
			1, 40, 799, 559, m_hWnd, NULL, CPaintManagerUI::GetInstance(), NULL);   
	}
	return pControl;
}

void CToolbar::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("BtnClose"))
		PostQuitMessage(0);
	else if (msg.pSender->GetName() == _T("MinBtn"))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

void CToolbar::OnItemMenu(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("Rich"))
	{
		CMenuWnd* pMenu = new CMenuWnd();
		CDuiPoint pt = msg.ptMouse;
		ClientToScreen(m_hWnd, &pt);
		STRINGorID strXmlFile(_T("ContentMenu.xml"));
		pMenu->Init(NULL,strXmlFile,pt,&m_PaintManager);
	}
}