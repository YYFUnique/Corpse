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

	HWND hMainDlg = pInfoMaster->GetHWND();
	PostMessage(hMainDlg, WM_ENCRYPT_PROGRESS_POSITION, 0, 10);
	SHDeleteDirectory(szTmpFolder);

	PostMessage(hMainDlg, WM_ENCRYPT_PROGRESS_POSITION, 0, 15);

	//将APK文件加压到指定目录
	CLzma Lzma;
	Lzma.LzmaDecode(pInfoMaster->GetZipFilePath(), szTmpFolder);

	PostMessage(hMainDlg, WM_ENCRYPT_PROGRESS_POSITION, 0, 40);

	//拷贝加密文件到APK解压目录
	CString strCryptFile, strNewCryptFile;
	strCryptFile.Format(_T("%s\\InfoMaster"), szTmpFile);
	strNewCryptFile.Format(_T("%s\\META-INF\\InfoMaster"), szTmpFolder);
	CopyFile(strCryptFile, strNewCryptFile, FALSE);

	PostMessage(hMainDlg, WM_ENCRYPT_PROGRESS_POSITION, 0, 60);

	//将APK解压目录打包
	Lzma.LzmaEncode(szTmpFolder, pInfoMaster->GetEncryptFilePath());

	PostMessage(hMainDlg, WM_ENCRYPT_PROGRESS_POSITION, 0, 90);

	//删除APK解压目录
	SHDeleteDirectory(szTmpFolder);
	
	PostMessage(hMainDlg, WM_ENCRYPT_PROGRESS_POSITION, 0, 100);
	
	PostMessage(hMainDlg, WM_ENCRYPT_SUCCESS, NULL, NULL);
	return TRUE;
}

