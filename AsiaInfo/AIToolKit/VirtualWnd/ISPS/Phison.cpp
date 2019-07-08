#include "StdAfx.h"
#include "Phison.h"
#include <Windows.h>
#include <atlstr.h>

CPhison::CPhison()
{
	m_bLoad = FALSE;
	m_pPaintManager = NULL;
	m_pPhisonSDK = NULL;
}

CPhison::~CPhison()
{
	m_bLoad = FALSE;
	m_pPaintManager = NULL;
	if (m_pPhisonSDK != NULL)
	{
		m_pPhisonSDK->CloseAPI();
		delete m_pPhisonSDK;
		m_pPhisonSDK = NULL;
	}
}

DUI_BEGIN_MESSAGE_MAP(CPhison, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnLoadItem)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

void CPhison::SetPaintManager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CPhison::OnLoadItem(TNotifyUI& msg)
{
	if (m_bLoad == TRUE)
		return;

	m_bLoad = TRUE;
	if (m_pPhisonSDK == NULL)
	{
		m_pPhisonSDK = new PhisonSDK_FF01;
		m_pPhisonSDK->InitAPI();
	}

	if (m_pPhisonSDK == NULL)
		return;

	DriveLettersList_t DriveLetters;
	Result res = m_pPhisonSDK->GetDriveLetterList(&DriveLetters);
	if (res.IsFailCode())
	{
		EnableWindows(false);
		return;
	}

	//m_pDirectoryHandler = new CDirectoryChangeHandlerEx;
	//m_pDirectoryWatcher = new CDirectoryChangeWatcher;
	//CString strWatchDirectory = _T("F:\\");
	//m_pDirectoryWatcher->WatchDirectory(strWatchDirectory, 0x000000FF, m_pDirectoryHandler, TRUE, NULL, NULL);

	//m_pFileWatcher = new CFileSystemWatcher;
	//if (m_pFileWatcher != NULL)
	//{
	//	DriveLetters_t Driver =  DriveLetters.at(0);

	//	CString strMonitorPath;
	//	strMonitorPath.Format(_T("%s:"), CString(Driver.dataLetter));

	//	//m_pFileWatch->Run(_T("F:"),TRUE,0x000000FF,DealFunc,NULL);
	//	m_pFileWatcher->Run(strMonitorPath, TRUE, FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION, DealFunc, this);
	//
	//	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, this, 0, NULL);
	//}

	OnBtnRefresh();
}

void CPhison::OnBtnRefresh()
{
	BOOL bSuccess = FALSE;
	do 
	{
		DriveLettersList_t DriveLetters;
		Result res = m_pPhisonSDK->GetDriveLetterList(&DriveLetters);
		if (res.IsFailCode())
		{
			if (res.Code() == 71)
			{
				m_pPhisonSDK->InitAPI();
				res = m_pPhisonSDK->GetDriveLetterList(&DriveLetters);
			}

			if (res.IsFailCode())
				break;
		}

		CComboUI* pDiskDrive = (CComboUI*)m_pPaintManager->FindControl(_T("DiskDrive"));
		pDiskDrive->RemoveAll();
		for (UINT nIndex=0; nIndex<DriveLetters.size(); ++nIndex)
		{
			DriveLetters_t DriverLabel = DriveLetters.at(nIndex);
			CListLabelElementUI* pDriverData = new CListLabelElementUI;
			pDiskDrive->Add(pDriverData);
			pDriverData->SetText(CString(DriverLabel.dataLetter));

			CListLabelElementUI* pDriverSys = new CListLabelElementUI;
			pDiskDrive->Add(pDriverSys);
			pDriverSys->SetText(CString(DriverLabel.sysLetter));
		}

		pDiskDrive->SelectItem(0);
		bSuccess = TRUE;
	} while (FALSE);

	if (bSuccess == FALSE)
	{
		CComboUI* pDiskDrive = (CComboUI*)m_pPaintManager->FindControl(_T("DiskDrive"));
		pDiskDrive->RemoveAll();
	}

	EnableWindows(bSuccess != FALSE);
}

void CPhison::OnBtnRead()
{
	CComboUI* pDiskDrive = (CComboUI*)m_pPaintManager->FindControl(_T("DiskDrive"));
	if (pDiskDrive == NULL)
		return;

	int nIndex = pDiskDrive->GetCurSel();
	CListLabelElementUI* pSelectItem = (CListLabelElementUI*)pDiskDrive->GetItemAt(nIndex);

	CStringA strDriveName(pSelectItem->GetText());
	if (m_pPhisonSDK != NULL)
		m_pPhisonSDK->SetWriteProtect(strDriveName.GetAt(0), true, false);
}

void CPhison::OnBtnReadTmp()
{
	CComboUI* pDiskDrive = (CComboUI*)m_pPaintManager->FindControl(_T("DiskDrive"));
	if (pDiskDrive == NULL)
		return;

	int nIndex = pDiskDrive->GetCurSel();
	CListLabelElementUI* pSelectItem = (CListLabelElementUI*)pDiskDrive->GetItemAt(nIndex);
	CStringA strDriveName(pSelectItem->GetText());
	if (m_pPhisonSDK != NULL)
		m_pPhisonSDK->SetWriteProtect(strDriveName.GetAt(0), true, true);
}

void CPhison::OnBtnWrite()
{
	CComboUI* pDiskDrive = (CComboUI*)m_pPaintManager->FindControl(_T("DiskDrive"));
	if (pDiskDrive == NULL)
		return;

	int nIndex = pDiskDrive->GetCurSel();
	CListLabelElementUI* pSelectItem = (CListLabelElementUI*)pDiskDrive->GetItemAt(nIndex);
	CStringA strDriveName(pSelectItem->GetText());
	if (m_pPhisonSDK != NULL)
		m_pPhisonSDK->SetWriteProtect(strDriveName.GetAt(0), false, false);
}

void CPhison::OnBtnWriteTmp()
{
	CComboUI* pDiskDrive = (CComboUI*)m_pPaintManager->FindControl(_T("DiskDrive"));
	if (pDiskDrive == NULL)
		return;

	int nIndex = pDiskDrive->GetCurSel();
	CListLabelElementUI* pSelectItem = (CListLabelElementUI*)pDiskDrive->GetItemAt(nIndex);
	CStringA strDriveName(pSelectItem->GetText());
	Result sRet = m_pPhisonSDK->SetWriteProtect(strDriveName.GetAt(0), false, true);
	return;
}

void CPhison::OnBtnFormat()
{
	CComboUI* pDiskDrive = (CComboUI*)m_pPaintManager->FindControl(_T("DiskDrive"));
	if (pDiskDrive == NULL)
		return;

	int nIndex = pDiskDrive->GetCurSel();
	CListLabelElementUI* pSelectItem = (CListLabelElementUI*)pDiskDrive->GetItemAt(nIndex);
	CStringA strDriveName(pSelectItem->GetText());
	CStringA strTipInfo;
	strTipInfo.Format("TESTING-%c", strDriveName.GetAt(0));
	Result res = m_pPhisonSDK->FormatPartition(strDriveName.GetAt(0), strTipInfo);
	if (res.IsFailCode())
		QLOG_INFO(_T("格式化分区[%c]失败:%d.\n"), strDriveName.GetAt(0), res.Code());
	return;
}

void CPhison::OnBtnAdjust()
{
	CComboUI* pDiskDrive = (CComboUI*)m_pPaintManager->FindControl(_T("DiskDrive"));
	if (pDiskDrive == NULL)
		return;

	int nIndex = pDiskDrive->GetCurSel();
	CListLabelElementUI* pSelectItem = (CListLabelElementUI*)pDiskDrive->GetItemAt(nIndex);
	CStringA strDriveName(pSelectItem->GetText());

	DeviceInfo_t DevInfo;
	Result ret = m_pPhisonSDK->GetDeviceInfo(strDriveName.GetAt(0), &DevInfo);
	if (ret.IsFailCode())
		QLOG_INFO(_T("获取分区[%c]设备信息失败:%d.\n"), strDriveName.GetAt(0), ret.Code());

	size_t nPublicSize = (DevInfo.sysSectors > DevInfo.dataSectors) ? DevInfo.dataSectors : DevInfo.sysSectors;
	ret = m_pPhisonSDK->ResizePublicPartitions(strDriveName.GetAt(0), nPublicSize, "ISPS2 SYS", "ISPS2 DATA");
	if (ret.IsFailCode())
		QLOG_INFO(_T("重新调整公共分区失败%d.\n"), ret.Code());
}

void CPhison::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("BtnRefresh"))
		OnBtnRefresh();
	else if (msg.pSender->GetName() == _T("BtnRead"))
		OnBtnRead();
	else if (msg.pSender->GetName() == _T("BtnReadTmp"))
		OnBtnReadTmp();
	else if (msg.pSender->GetName() == _T("BtnWrite"))
		OnBtnWrite();
	else if (msg.pSender->GetName() == _T("BtnWriteTmp"))
		OnBtnWriteTmp();
	else if (msg.pSender->GetName() == _T("BtnFormat"))
		OnBtnFormat();
	else if (msg.pSender->GetName() == _T("BtnAdjust"))
		OnBtnAdjust();
}

void CPhison::EnableWindows(bool bEnable /* = TRUE */)
{
	CButtonUI* pBtnRefresh = (CButtonUI*)m_pPaintManager->FindControl(_T("BtnRefresh"));
	if (pBtnRefresh && pBtnRefresh->IsEnabled() == FALSE)
		pBtnRefresh->SetEnabled(TRUE);

	CButtonUI* pRead = (CButtonUI*)m_pPaintManager->FindControl(_T("BtnRead"));
	if (pRead && pBtnRefresh->IsEnabled() != bEnable)
		pRead->SetEnabled(bEnable);

	CButtonUI* pReadTmp = (CButtonUI*)m_pPaintManager->FindControl(_T("BtnReadTmp"));
	if (pReadTmp && pReadTmp->IsEnabled() != bEnable)
		pReadTmp->SetEnabled(bEnable);

	CButtonUI* pWrite = (CButtonUI*)m_pPaintManager->FindControl(_T("BtnWrite"));
	if (pWrite && pWrite->IsEnabled() != bEnable)
		pWrite->SetEnabled(bEnable);

	CButtonUI* pWriteTmp = (CButtonUI*)m_pPaintManager->FindControl(_T("BtnWriteTmp"));
	if (pWriteTmp && pWriteTmp->IsEnabled() != bEnable)
		pWriteTmp->SetEnabled(bEnable);

	CButtonUI* pFormat = (CButtonUI*)m_pPaintManager->FindControl(_T("BtnFormat"));
	if (pFormat && pFormat->IsEnabled() != bEnable)
		pFormat->SetEnabled(bEnable);
}