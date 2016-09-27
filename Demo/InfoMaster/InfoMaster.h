#pragma once

#include "EncryptFileThread.h"
#include "EncryptProcess.h"
#include <atlstr.h>

class CInfoMaster : public WindowImplBase
{
public:
	CInfoMaster();
	~CInfoMaster();
//override
public:
	virtual void OnFinalMessage(HWND hWnd);
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual LPCTSTR GetResourceID() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
		
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CDuiString GetZipFilePath();
	CDuiString GetEncryptFilePath();
protected:
	virtual void OnClick(TNotifyUI& msg);
	void OnSave(TNotifyUI& msg);

	void OnLookUpApkFile(TNotifyUI& msg);
	BOOL CreateCryptFile(LPCTSTR lpszCryptString);
	BOOL OutputPackageFile();
	BOOL GetCryptContext(CString& strCryptContext);
protected:
	CDuiString m_strZipFileName;
	CDuiString m_strEncryptFilePath;

	CEncryptFile* m_pEncryptFile;
	CEncryptProcess* m_pEncryptProcess;
};