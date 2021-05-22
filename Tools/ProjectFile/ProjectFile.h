#pragma once

#define PATH_NO_SUCH_FILE	0x01		// 文件不存在
#define ERROR_ATTRIBUTE		0x02		// 属性不同

class CProjectFile : public WindowImplBase
{
public:
	CProjectFile();
	~CProjectFile();

public:
	void OnFinalMessage(HWND hWnd);
	virtual LPCTSTR GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFolder();
	UILIB_RESOURCETYPE GetResourceType() const;
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	void InitWindow();

	void Notify(TNotifyUI& msg);
protected:
	UINT OnCopyFiles();
	// 释放添加到Tag标签中的对象
	void ReleaseFileItemTagObject(CListUI* pList);
	void LoadProjectFileList(LPCTSTR lpszProjectFilePath);
	void AddToHistory(LPCTSTR lpszProjectName, LPCTSTR szProjectFilePath);
	void UpdateItemTextColor(CFileListItemUI* pFileListItem, DWORD dwTextColor) const;
	void CreateProjectFileItem(CListUI* pList, const TListInfoUI* pListInfo, IMarkup* pMarkup);
	void GetFilePath(CProjectFileInfo* pProjectFileInfo, CString& strSrcFile, CString& strDestFile) const;
	static UINT CopyFilesThreadProc(LPVOID lParam);
	int CompareFileAttribute(LPCTSTR lpszSrcFile , LPCTSTR lpszDestFile);
protected:
	void OnClick(TNotifyUI& msg);
	void OnCopyFiles(TNotifyUI& msg);
	LRESULT OnMenuClick(WPARAM wParam, LPARAM lParam);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	CHistory*						m_pHistory;
	CControlBuilder*			m_pCreateControl;
	CDialogBuilder			m_DialogBuilder;
	CProjectFileManager	m_ProjectFileManager;
	CProjectFileBuilder		m_ProjectFileBuilder;
};