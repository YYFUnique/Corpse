#pragma once

namespace DuiLib {

	typedef std::map<CDuiString, UINT> CCallbackMap;

	class CSkinEngine : public WindowImplBase
	{
	public:
		CSkinEngine();
		~CSkinEngine();

	public:
		virtual LPCTSTR GetWindowClassName() const;
		virtual void OnFinalMessage( HWND hWnd );
		virtual CDuiString GetZIPFileName() const;
		virtual CDuiString GetSkinFile();
		virtual CDuiString GetSkinFolder();
		virtual UILIB_RESOURCETYPE GetResourceType() const;
		virtual void Notify(TNotifyUI& msg);
		virtual void InitWindow();
		virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam , BOOL& bHandled);
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual CControlUI* CreateControl(LPCTSTR pstrClass);

		void SetFolderFilePath(LPCTSTR lpszFolderFilePath);
		void SetZIPFileName(LPCTSTR lpszZIPFileName);
		void SetSkinFile(LPCTSTR lpszSkinFile);
		CPaintManagerUI& GetPaintManager();
		void OnInstallOrUninstallOK();
	protected:
		void OnBtnAccept(TNotifyUI& msg);
		void OnBtnBack(TNotifyUI& msg);
		virtual void OnClick(TNotifyUI& msg);
		void OnInstallOrUninstall(TNotifyUI& msg);
		void OnComplete(TNotifyUI& msg);
	protected:
		CDuiString m_strZIPFileName;
		CDuiString m_strFolderFilePath;
		CDuiString m_strSkinFile;
	};

	class CLsCIAMessageBox : public WindowImplBase
	{
	public:
		CLsCIAMessageBox();
		~CLsCIAMessageBox();

	public:
		virtual LPCTSTR GetWindowClassName() const;
		virtual void OnFinalMessage( HWND hWnd );
		virtual CDuiString GetZIPFileName() const;
		virtual CDuiString GetSkinFile();
		virtual CDuiString GetSkinFolder();
		virtual void Notify(TNotifyUI& msg);
		virtual void InitWindow();

		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		CPaintManagerUI& GetPaintManager();

		void SetZIPFileName(LPCTSTR lpszZIPFileName);
		void SetSkinFile(LPCTSTR lpszSkinFile);
		void SetSkinFileFolder(LPCTSTR lpszSkinFileFolder);
	protected:
		virtual void OnClick(TNotifyUI& msg);
	private:
		CDuiString		m_strZIPFileName;
		CDuiString		m_strSkinFile;
		CDuiString		m_strSkinFolder;
	};
}