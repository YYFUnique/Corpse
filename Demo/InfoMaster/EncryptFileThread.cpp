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
		SetErrorInfo(CUSTOM_ERROR, 0 , _T("获取系统临时目录失败"));
		return FALSE;
	}

	//构造temp目录下的文件
	TCHAR szTmpFolder[MAX_PATH];
	PathCombine(szTmpFolder, szTmpFile, _T("TxMaster"));

	SHDeleteDirectory(szTmpFolder);

	CWaitCursor WaitCursor;
	CLzma Lzma;
	Lzma.LzmaDecode(pInfoMaster->GetZipFilePath(), szTmpFolder);

	CString strCryptFile, strNewCryptFile;
	strCryptFile.Format(_T("%s\\InfoMaster"), szTmpFile);
	strNewCryptFile.Format(_T("%s\\META-INF\\InfoMaster"), szTmpFolder);
	CopyFile(strCryptFile, strNewCryptFile, FALSE);

	Lzma.LzmaEncode(szTmpFolder, pInfoMaster->GetEncryptFilePath());

	SHDeleteDirectory(szTmpFolder);

	HWND hMainDlg = pInfoMaster->GetHWND();
	PostMessage(hMainDlg, WM_ENCRYPT_SUCCESS, NULL, NULL);

	return TRUE;
}

