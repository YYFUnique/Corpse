#pragma once

#define  DUI_CTR_FILELIST											(_T("FileList"))
#define  DUI_CTR_FILELISTITEM									(_T("FileListItem"))

namespace DuiLib
{
	class CFileListItemUI : public CListContainerElementUI
	{
	public:
		CFileListItemUI();
		~CFileListItemUI();

	public:
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		
		void SetPos( RECT rc );
		void DoEvent(TEventUI& event);

	protected:
	};

	class CFileListUI : public CListUI
	{
	public:
		CFileListUI();
		~CFileListUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		void DoEvent(TEventUI& event);
	protected:
		POINT m_ptDown;
		POINT m_ptMove;
		CControlUI* m_pSelectRect;
	};
}