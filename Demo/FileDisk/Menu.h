#pragma once
#include "DuiLib/UIlib.h"

class CDuiMenu : public WindowImplBase
{
protected:
	virtual ~CDuiMenu(){};        // 私有化析构函数，这样此对象只能通过new来生成，而不能直接定义变量。就保证了delete this不会出错
	CDuiString  m_strXMLPath;

public:
	explicit CDuiMenu(LPCTSTR pszXMLPath): m_strXMLPath(pszXMLPath){}
	virtual LPCTSTR    GetWindowClassName()const{ return _T("CDuiMenu "); }
	virtual CDuiString GetSkinFolder()          { return _T("");            }
	virtual CDuiString GetSkinFile()            { return m_strXMLPath;      }
	virtual void       OnFinalMessage(HWND hWnd){ delete this;              }

	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		Close();
		bHandled = TRUE;
		return 0;
	}

	void Init(HWND hWndParent, POINT ptPos)
	{
		Create(hWndParent, _T("MenuWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
		::ClientToScreen(hWndParent, &ptPos);
		::SetWindowPos(*this, NULL, ptPos.x, ptPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	}

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL    bHandled = TRUE;

		switch( uMsg )
		{
		case WM_KILLFOCUS:    
			lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); 
			break; 
		default:
			bHandled = FALSE;
		}

		if(bHandled || m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) 
		{
			return lRes;
		}

		return __super::HandleMessage(uMsg, wParam, lParam);
	}
};