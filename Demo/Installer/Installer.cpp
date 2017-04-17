#include "StdAfx.h"
#include "Installer.h"
#include "resource.h"
#include "3DView.h"
#include "AnimLayout.h"
#include <Shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"Msimg32.lib")

CInstaller::CInstaller()
:m_nZStep(40)
,m_nYStep(10)
,m_nFrameIndex(0)
{
	m_hBitmapBefore = NULL;
	m_hBitmapAfter = NULL;
}

CInstaller::~CInstaller()
{

}

void CInstaller::OnFinalMessage( HWND hWnd )
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CInstaller::GetWindowClassName() const
{
	return _T("CInstaller");
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
	return _T("Installer");
}

UILIB_RESOURCETYPE CInstaller::GetResourceType() const
{
	return UILIB_FILE;
}

void CInstaller::Notify(TNotifyUI& msg)
{
	CDuiString strCtlName = msg.pSender->GetName();

	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (strCtlName == _T("BtnClose"))
			PostQuitMessage(0);
		else if (strCtlName == _T("BtnSMin"))
		{
			//SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
			CTabLayoutUI* pTab = (CTabLayoutUI*)m_PaintManager.FindControl(_T("WizardTab"));
			if (pTab->GetCount() == pTab->GetCurSel()+1)
				return;

			C3DViewUI* p3dView = (C3DViewUI*)m_PaintManager.FindControl(_T("VLayoutTotal"));
			CDuiRect rcPos = p3dView->GetPos();
			LPBYTE lpBit = NULL;
			//p3dView->StartEffect(fa);

			m_hBitmapBefore = CRenderEngine::GenerateBitmap(&m_PaintManager,p3dView,p3dView->GetPos());
			p3dView->Get3dParam().nOffsetZ = m_nZStep ;
			p3dView->Get3dParam().nRotateY = 0;
			p3dView->SetBmpOrig(m_hBitmapBefore);
			p3dView->Update();    
			m_nFrameIndex = 1;
			p3dView->StartEffect(TRUE);
			m_PaintManager.SetTimer(p3dView,1000,20);
		}
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
			else
				OnClick(msg);
		}
	}
	else if (msg.sType == DUI_MSGTYPE_TIMER)
		OnTimer(msg);
	else if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
		OnInitDialog(msg);
}

void CInstaller::OnInitDialog(TNotifyUI& msg)
{
	CAnimLayoutUI* pAniLayout = (CAnimLayoutUI*)msg.pSender->GetInterface(_T("AnimLayout"));
	pAniLayout->StartEffect();
	//ShowWindow(true);
	//CControlUI* pControl = m_PaintManager.FindControl(_T("VLayoutTotal"));
	//CAnimLayoutUI* pAniLayout = (CAnimLayoutUI*)pControl->GetInterface(_T("AnimLayout"));
	//pAniLayout->StartEffect();
	//m_PaintManager.KillTimer(pAniLayout);
}

void CInstaller::OnTimer(TNotifyUI& msg)
{
	if (msg.pSender == m_PaintManager.FindControl(_T("1")))
	{
		int nMaxBeforeFrame, nMaxAfterFrame;
		nMaxBeforeFrame = 90 / m_nYStep;
		nMaxAfterFrame = 180 / m_nYStep;

		C3DViewUI* p3dView = (C3DViewUI*)m_PaintManager.FindControl(_T("VLayoutTotal"));
		if(NULL != p3dView)
		{      
			if(m_nFrameIndex <= nMaxBeforeFrame)
			{
				p3dView->Get3dParam().nOffsetZ = m_nZStep * m_nFrameIndex;
				p3dView->Get3dParam().nRotateY = m_nYStep * m_nFrameIndex;
				p3dView->SetBmpOrig(m_hBitmapBefore);

				p3dView->Update();   
			}
			else if(m_nFrameIndex > nMaxBeforeFrame && m_nFrameIndex < nMaxAfterFrame)
			{
				if (m_hBitmapAfter == NULL)
				{
					CTabLayoutUI* pTab = (CTabLayoutUI*)m_PaintManager.FindControl(_T("WizardTab"));
					int nSel = pTab->GetCurSel();
					if (pTab->GetCount() <= nSel +1)
						return;
					p3dView->StartEffect(FALSE);
					pTab->SelectItem(nSel+1);
					//CVerticalLayoutUI* pHead = (CVerticalLayoutUI*)pTab->GetItemAt(nSel+1);
					m_hBitmapAfter = CRenderEngine::GenerateBitmap(&m_PaintManager,p3dView,p3dView->GetPos());
					p3dView->StartEffect(TRUE);
				}

				p3dView->Get3dParam().nOffsetZ = m_nZStep * (nMaxAfterFrame - m_nFrameIndex);
				p3dView->Get3dParam().nRotateY =  m_nYStep * (m_nFrameIndex - nMaxAfterFrame);
				p3dView->SetBmpOrig(m_hBitmapAfter);
				p3dView->Update();        
			}
			else
			{
				p3dView->StartEffect(FALSE);
				m_PaintManager.KillTimer(p3dView);
				p3dView->Invalidate();
				DeleteObject(m_hBitmapAfter);
				m_hBitmapAfter = NULL;
				DeleteObject(m_hBitmapBefore);
				m_hBitmapBefore = NULL;
			}
		}
		++m_nFrameIndex;
	}
}

CControlUI* CInstaller::CreateControl(LPCTSTR pstrClass)
{
	CControlUI* pControl = NULL;
	if (_tcsicmp(pstrClass, _T("3DView")) == 0)
		pControl = new C3DViewUI;
	if (_tcsicmp(pstrClass, _T("AnimLayout")) == 0)
		pControl = new CAnimLayoutUI;
	return pControl;
}

void CInstaller::InitWindow()
{
	//CControlUI* pControl = m_PaintManager.FindControl(_T("VLayoutTotal"));
		//CAnimLayoutUI* pAniLayout = (CAnimLayoutUI*)pControl->GetInterface(_T("AnimLayout"));
		//pAniLayout->StartEffect();
	//	m_PaintManager.SetTimer(pControl,1001,10);
	SetIcon(IDI_MAINFRAME);

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
	if (msg.pSender == m_PaintManager.FindControl(_T("BtnMin")))
	{
		CControlUI* pControl = m_PaintManager.FindControl(_T("VLayoutBody"));
		//CAnimLayoutUI* pAniLayout = (CAnimLayoutUI*)pControl->GetInterface(_T("AnimLayout"));
		//pAniLayout->StartEffect();
		m_PaintManager.SetTimer(pControl,1001,100);
	}
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
