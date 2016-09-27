#include "StdAfx.h"
#include "EncryptFileThread.h"
#include "InfoMaster.h"
#include "Lzma/Lzma.h"
#include "YCLibCore/Utils/FileTools.h"
#include "YCLibCore/Utils/ErrorInfo.h"

CEncryptFile::CEncryptFile()
{
	m_LsThreadAppId = LS_THREAD_APP_ID_ENCRYPT;
	m_pfnThreadProc = (LPTHREAD_START_ROUTINE)workThread;
	m_pThreadParam = this;
}

CEncryptFile::~CEncryptFile()
{

}

BOOL CEncryptFile::StopThread(DWORD dwWaitTime , BOOL bTerminateThreadWhenWaitTimeout)
{	
	return CLsThread::StopThread(dwWaitTime , bTerminateThreadWhenWaitTimeout);
}

UINT CEncryptFile::workThread(LPVOID lParam)
{
	CInfoMaster* pInfoMaster = (CInfoMaster*)lParam;

	TCHAR szTmpFile[MAX_PATH];
	DWORD dwSize = _countof(szTmpFile);
	DWORD dwRet = GetTempPath(dwSize, szTmpFile);
	if (dwRet = 0 || dwRet >dwSize)
	{
		SetErrorInfo(CUSTOM_ERROR, 0 , _T("��ȡϵͳ��ʱĿ¼ʧ��"));
		return FALSE;
	}

	//����tempĿ¼�µ��ļ�
	TCHAR szTmpFolder[MAX_PATH];
	PathCombine(szTmpFolder, szTmpFile, _T("TxMaster"));

	HWND hMainDlg = pInfoMaster->GetHWND();
	PostMessage(hMainDlg, WM_ENCRYPT_PROGRESS_POSITION, 0, 10);
	SHDeleteDirectory(szTmpFolder);

	PostMessage(hMainDlg, WM_ENCRYPT_PROGRESS_POSITION, 0, 15);

	//��APK�ļ���ѹ��ָ��Ŀ¼
	CLzma Lzma;
	Lzma.LzmaDecode(pInfoMaster->GetZipFilePath(), szTmpFolder);

	PostMessage(hMainDlg, WM_ENCRYPT_PROGRESS_POSITION, 0, 40);

	//���������ļ���APK��ѹĿ¼
	CString strCryptFile, strNewCryptFile;
	strCryptFile.Format(_T("%s\\InfoMaster"), szTmpFile);
	strNewCryptFile.Format(_T("%s\\META-INF\\InfoMaster"), szTmpFolder);
	CopyFile(strCryptFile, strNewCryptFile, FALSE);

	PostMessage(hMainDlg, WM_ENCRYPT_PROGRESS_POSITION, 0, 60);

	//��APK��ѹĿ¼���
	Lzma.LzmaEncode(szTmpFolder, pInfoMaster->GetEncryptFilePath());

	PostMessage(hMainDlg, WM_ENCRYPT_PROGRESS_POSITION, 0, 90);

	//ɾ��APK��ѹĿ¼
	SHDeleteDirectory(szTmpFolder);
	
	PostMessage(hMainDlg, WM_ENCRYPT_PROGRESS_POSITION, 0, 100);
	
	PostMessage(hMainDlg, WM_ENCRYPT_SUCCESS, NULL, NULL);
	return TRUE;
}

