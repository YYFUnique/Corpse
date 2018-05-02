#pragma once

namespace DuiLib {

	typedef std::map<CDuiString, UINT> CCallbackMap;

	class TBCIAMainWnd : public WindowImplBase
	{
	public:
		TBCIAMainWnd();
		~TBCIAMainWnd();

	public:
		virtual LPCTSTR GetWindowClassName() const;
		virtual void OnFinalMessage( HWND hWnd );
		virtual CDuiString GetZIPFileName() const;
		virtual CDuiString GetSkinFile();
		virtual CDuiString GetSkinFolder();
		virtual UILIB_RESOURCETYPE GetResourceType() const;
		virtual void Notify(TNotifyUI& msg);
		virtual void InitWindow();

		void SetFolderFilePath(LPCTSTR lpszFolderFilePath);
		void SetZIPFileName(LPCTSTR lpszZIPFileName);
		void SetSkinFile(LPCTSTR lpszSkinFile);
		CPaintManagerUI* GetPaintManager();
		void SaveToControlCallbackMap(CDuiString strName, int nCallback);
	protected:

	protected:
		CDuiString m_strZIPFileName;
		CDuiString m_strFolderFilePath;
		CDuiString m_strSkinFile;
		CCallbackMap m_ControlCallbackMap;
	};

	////////////////////////////////////////////////////////////////////
	//// 使用内置资源
	////////////////////////////////////////////////////////////////////
	class TBCIAMessageBox : public WindowImplBase
	{
	public:
		TBCIAMessageBox();
		~TBCIAMessageBox();

	public:
		virtual LPCTSTR GetWindowClassName() const;
		virtual void OnFinalMessage( HWND hWnd );
		//virtual CDuiString GetZIPFileName() const;
		virtual CDuiString GetSkinFile();
		virtual CDuiString GetSkinFolder();
		virtual LPCTSTR GetResourceID() const;
		virtual UILIB_RESOURCETYPE GetResourceType() const;
		virtual void Notify(TNotifyUI& msg);
		virtual void InitWindow();

		CPaintManagerUI* GetPaintManager();
	protected:
		void OnClick(TNotifyUI& msg);
	private:
		CDuiString		m_strZIPFileName;
		CDuiString		m_strSkinFile;
		CDuiString		m_strSkinFolder;
		CCallbackMap m_ControlCallbackMap;
	};
}