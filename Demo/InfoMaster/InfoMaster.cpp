#include "stdafx.h"
#include "resource.h"
#include "InfoMaster.h"
#include "DllCore/Encrypt/LsRc4.h"
#include "DllCore/Utils/ErrorInfo.h"
#include "DllCore/Json/JsonObject.h"
#include "DllCore/Thread/LsThreadMgr.h"

#include <CommDlg.h>
#include <atlstr.h>

#define OPEN_FILE_TITLE		_T("请选择辰锐终端安全客户端")
#define SAVE_FILE_TITLE		_T("请选择辰锐终端安全客户端打包输出位置")

//RC4密码
#define lpszFilePwd				_T("haidunM4000mem")
//存储选择APK文件路径
#define MAIN_REG_PATH		_T("SOFTWARE\\InfoMaster")

CInfoMaster::CInfoMaster()
{
	m_pEncryptFile = NULL;
	m_pEncryptProcess = NULL;
}

CInfoMaster::~CInfoMaster()
{
	if (m_pEncryptFile != NULL)
	{
		delete m_pEncryptFile;
		m_pEncryptFile = NULL;
	}

	if (m_pEncryptProcess != NULL)
	{
		delete m_pEncryptProcess;
		m_pEncryptProcess = NULL;
	}
}

void CInfoMaster::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CInfoMaster::GetWindowClassName() const
{
	return _T("TxPoliceMobileTool");
}

UILIB_RESOURCETYPE CInfoMaster::GetResourceType() const
{
#ifdef _DEBUG
	return UILIB_FILE;
#else
	return UILIB_ZIPRESOURCE;
#endif
}

LPCTSTR CInfoMaster::GetResourceID() const
{
	return MAKEINTRESOURCE(IDR_SKIN);
}

CDuiString CInfoMaster::GetSkinFile()
{
	return _T("InfoMaster.xml");
}

CDuiString CInfoMaster::GetSkinFolder()
{
#ifdef _DEBUG
	return _T("InfoMaster");
#else
	return _T("");
#endif
}

void CInfoMaster::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

void CInfoMaster::InitWindow()
{
	SetIcon(IDI_MAINFRAME);
	//从注册表中读取上次保存路径
	HKEY hKey = NULL;
	do 
	{
		DWORD dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, MAIN_REG_PATH, NULL, KEY_READ, &hKey);
		if (dwRet != ERROR_SUCCESS)
			break;

		BYTE lpData[MAX_PATH];
		DWORD dwLen = _countof(lpData);

		dwRet = RegQueryValueEx(hKey, _T("TsrsPath"), NULL, NULL, lpData, &dwLen);
		if (dwRet != ERROR_SUCCESS)
			break;

		CEditUI2* pApkFile = (CEditUI2*)m_PaintManager.FindControl(_T("ApkPath"));
		if (pApkFile == NULL)
			return;

		pApkFile->SetText((LPCTSTR)lpData);
	} while (FALSE);

	if (hKey != NULL)
	{
		RegCloseKey(hKey);
		hKey = NULL;
	}
}

LRESULT CInfoMaster::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	switch (uMsg)
	{
		case WM_ENCRYPT_PROGRESS_POSITION:
				if (m_pEncryptProcess != NULL)
				{
					m_pEncryptProcess->SetEncryptProcess(lParam);
					if (lParam == 100)
					{
						m_pEncryptProcess->Close(IDOK);
						m_pEncryptProcess = NULL;
					}
				}
				bHandled = TRUE;
			break;
		case WM_ENCRYPT_SUCCESS:
				MessageBox(m_hWnd, _T("打包配置文件成功"), _T("提示"), MB_OK|MB_ICONINFORMATION);
				bHandled = TRUE;
			break;
		default:
			return __super::HandleCustomMessage(uMsg,wParam,lParam,bHandled);
	}

	return TRUE;
}

void CInfoMaster::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == m_PaintManager.FindControl(_T("BtnClose")))
		PostQuitMessage(0);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnMin")))
		PostMessage(WM_SYSCOMMAND,SC_MINIMIZE,0);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnSave")))
		OnSave(msg);
	else if (msg.pSender == m_PaintManager.FindControl(_T("LoadApk")))
		OnLookUpApkFile(msg);
	else if (msg.pSender == m_PaintManager.FindControl(_T("Close")))
		PostQuitMessage(0);
}

void CInfoMaster::OnSave(TNotifyUI& msg)
{
	CEditUI2* pApkPath = (CEditUI2*)m_PaintManager.FindControl(_T("ApkPath"));
	if (pApkPath == NULL)
		return;

	if (pApkPath->GetText().IsEmpty())
	{
		MessageBox(m_hWnd, _T("请选择需要配置的客户端"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}

	CEditUI2* pDoName = (CEditUI2*)m_PaintManager.FindControl(_T("DoName"));
	if (pDoName == NULL)
		return;

	if (pDoName->GetText().IsEmpty())
	{
		MessageBox(m_hWnd, _T("请输入需要配置的域名"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}

	BOOL bSuccess = FALSE;

	do 
	{
		CString strCryptString;
		if (GetCryptContext(strCryptString) == FALSE)
		{
			SetErrorTitle(_T("创建加密内容失败"));
			break;
		}

		//产生加密配置文件
		if (CreateCryptFile(strCryptString) == FALSE)
		{
			SetErrorTitle(_T("向文件中写入加密内容失败"));
			break;
		}

		//输出APK文件
		if (OutputPackageFile() == FALSE)
			break;

		bSuccess = TRUE;
	} while (FALSE);

	if (bSuccess == FALSE)
		MessageBox(m_hWnd, GetThreadErrorInfoString(), _T("提示"), MB_OK|MB_ICONWARNING);
}

BOOL CInfoMaster::CreateCryptFile(LPCTSTR lpszCryptString)
{
	BOOL bSuccess = FALSE;

	HANDLE hCryptFile = INVALID_HANDLE_VALUE;
	LPBYTE lpData = NULL;
	do 
	{
		DWORD dwLen = _tcslen(lpszCryptString) * sizeof(TCHAR);

		lpData = new BYTE[dwLen];
		if (lpData == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR, 0 , _T("加密文件失败:分配内存失败"));
			break;
		}

		TCHAR szTmpPath[MAX_PATH];
		DWORD dwSize = _countof(szTmpPath);
		DWORD dwRet = GetTempPath(dwSize, szTmpPath);
		if (dwRet ==0 || dwRet > dwSize)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("获取临时目录失败"));
			break;
		}

		CString strCryptFileName;
		strCryptFileName.Format(_T("%s\\InfoMaster"), szTmpPath);

		//在temp目录创建指定文件来存放需要保存的内容
		hCryptFile = CreateFile(strCryptFileName , GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
		if (hCryptFile == INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0 ,_T("创建加密文件失败"));
			break;
		}

		//进行RC4加密
		CLsRc4 Rc4;
		Rc4.Init((BYTE*)lpszFilePwd , _countof(lpszFilePwd));
		Rc4.Crypt((LPBYTE)(LPCTSTR)lpszCryptString, lpData, dwLen);
		
		if (WriteFile(hCryptFile, lpData, dwLen, &dwLen ,NULL) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0 ,_T("写入加密文件失败"));
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (lpData != NULL)
	{
		delete[] lpData;
		lpData = NULL;
	}

	if (hCryptFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hCryptFile);
		hCryptFile = INVALID_HANDLE_VALUE;
	}

	return bSuccess;
}

BOOL CInfoMaster::GetCryptContext(CString& strCryptContext)
{
	BOOL bSuccess = FALSE;
	do 
	{
		CEditUI2* pDoName = (CEditUI2*)m_PaintManager.FindControl(_T("DoName"));
		if (pDoName == NULL)
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("获取设置域名控件失败"));
			break;
		}

		CJsonObject JsonObject;
		CString strDoName = pDoName->GetText();

		JsonObject.SetValue(_T("version"), 1);						//文件版本
		JsonObject.SetValue(_T("doname"), strDoName);		//域名

		strCryptContext = JsonObject.ToRawString();

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

void CInfoMaster::OnLookUpApkFile(TNotifyUI& msg)
{
	TCHAR szApkFilePath[MAX_PATH] = {0};
	_tcscpy_s(szApkFilePath, _countof(szApkFilePath), _T("tsrs.apk"));
	OPENFILENAME OFNFileName;
	ZeroMemory(&OFNFileName ,sizeof(OFNFileName)); 
	OFNFileName.lStructSize		= sizeof(OFNFileName); 
	OFNFileName.hwndOwner		= m_hWnd; 
	OFNFileName.lpstrFile			= szApkFilePath ; 
	OFNFileName.nMaxFile			= _countof(szApkFilePath);
	OFNFileName.lpstrFilter			= _T("APK文件\0*.apk\0"); 
	OFNFileName.nFilterIndex		= 1; 
	OFNFileName.lpstrFileTitle	= NULL; 
	OFNFileName.nMaxFileTitle	= 0;
	OFNFileName.lpstrTitle			= OPEN_FILE_TITLE;
	OFNFileName.lpstrInitialDir	= NULL; 
	OFNFileName.Flags				= OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST ; 
	if (GetOpenFileName(&OFNFileName))
	{
		CEditUI2* pApkFile = (CEditUI2*)m_PaintManager.FindControl(_T("ApkPath"));
		if (pApkFile == NULL)
			return;

		pApkFile->SetText(szApkFilePath);

		//保存选择文件路径
		HKEY hKey = NULL;
		do 
		{
			DWORD dwRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE, MAIN_REG_PATH, NULL, NULL,REG_OPTION_NON_VOLATILE, KEY_WRITE,NULL, &hKey, NULL);
			if (dwRet != ERROR_SUCCESS)
				break;

			DWORD dwLen = _tcslen(szApkFilePath) * sizeof(TCHAR);
			dwRet = RegSetValueEx(hKey, _T("TsrsPath"), 0, REG_SZ ,(LPBYTE)szApkFilePath, dwLen);
			if (dwRet != ERROR_SUCCESS)
				break;

		}while(FALSE);
	}
}

BOOL CInfoMaster::OutputPackageFile()
{
	TCHAR szApkFilePath[MAX_PATH] = {0};
	_tcscpy_s(szApkFilePath, _countof(szApkFilePath), _T("tsrs.apk"));
	OPENFILENAME SaveFileName = {0};
	SaveFileName.lStructSize		= sizeof(SaveFileName); 
	SaveFileName.hwndOwner		= m_hWnd; 
	SaveFileName.lpstrFile			= szApkFilePath; 
	SaveFileName.nMaxFile			= _countof(szApkFilePath);
	SaveFileName.lpstrFilter			= _T("APK文件\0*.apk\0"); 
	SaveFileName.nFilterIndex		= 1; 
	SaveFileName.lpstrFileTitle	= NULL; 
	SaveFileName.nMaxFileTitle	= 0; 
	SaveFileName.lpstrTitle			= SAVE_FILE_TITLE;
	SaveFileName.lpstrInitialDir	= NULL ; 
	SaveFileName.Flags				= OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST ; 

	if (GetSaveFileName(&SaveFileName) == FALSE)
	{
		SetErrorInfo(CUSTOM_ERROR, 0, _T("请选择文件输出保存位置"));
		return FALSE;
	}

	CEditUI2* pApkFile = (CEditUI2*)m_PaintManager.FindControl(_T("ApkPath"));
	if (pApkFile == NULL)
	{
		SetErrorInfo(CUSTOM_ERROR, 0 ,_T("查找原始文件失败，请确认原始文件是否存在"));
		return FALSE;
	}

	m_strZipFileName = pApkFile->GetText();
	m_strEncryptFilePath = SaveFileName.lpstrFile;

	if (m_pEncryptFile == NULL)
		m_pEncryptFile = new CEncryptFile;
	else
		m_pEncryptFile->StopThread(0,0);

	m_pEncryptFile->SetThreadParam(this);
	m_pEncryptFile->StartThread();

	GetLsThreadMgr().AddThreadToList(m_pEncryptFile);

	//显示打包进度
	if (m_pEncryptProcess == NULL)
		m_pEncryptProcess = new CEncryptProcess(m_hWnd);

	m_pEncryptProcess->ShowModal();
	
	return TRUE;
}

CDuiString CInfoMaster::GetZipFilePath()
{
	return m_strZipFileName;
}

CDuiString CInfoMaster::GetEncryptFilePath()
{
	return m_strEncryptFilePath;
}
