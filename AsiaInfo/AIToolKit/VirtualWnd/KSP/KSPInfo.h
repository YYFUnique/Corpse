#pragma once

class CKSPInfo : public CNotifyPump
{
public:
	CKSPInfo();
	~CKSPInfo();

public:
	void SetPaintManager(CPaintManagerUI* pPaintMgr);
	BOOL HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
protected:
	void OnLookupReleaseZipFile();
	void CloneSourceKernel(LPCTSTR lpszSourceDspName);
	void EnumPluginKernelVersion(CStdArray& strKernelPackageList);
	BOOL GetNoDepPackage(LPCTSTR lpszSourceDspFile, CString& strNoDepPackage);
	BOOL GetDspFileByPreName(LPCTSTR lpszDspPreName, CString& strDspFileName);
	BOOL GetDepluaPlugin(LPCTSTR lpszNoDepPackage, CString& strDepluaPlugin);
	BOOL RenameDirectoryToNewName(LPCTSTR lpszBaseDirectoryName, LPCTSTR lpszDirectoryNewName);
protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnClick(TNotifyUI& msg);
	void OnLoadItem(TNotifyUI& msg);
	void OnRefresh(TNotifyUI& msg);
	void OnReturn(TNotifyUI& msg);
	void OnKillFocus(TNotifyUI& msg);
	void OnTextChanged(TNotifyUI& msg);
protected:
	void ClonePackage();
	void ExportPackage();
	void LookupPackageFile();
	void UnpackageFile();
private:
	static UINT ThreadProc(LPVOID lParam);
protected:
	BOOL						m_bExit;
	NLock						m_pLock;
	CListViewWnd*		m_pListView;
	HANDLE					m_hFileEvent;
	HANDLE					m_hThread;
	CString						m_strReleasePath;
	CStdArray				m_strKernelPackageList;

	BOOL					   m_bLoad;
	CControlBuilder	   m_RootBuilder;
	CDialogBuilder	   m_DialogBuilder;
	CPaintManagerUI* m_pPaintManager;
};