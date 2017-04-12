#pragma once

#include "DuiLib/Utils/Utils.h"
#include "DuiLib/UIlib.h"
#include "DataMgr.h"
#include "controlex/ControlInclude.h"
//////////////////////////////////////////////////////////////////////////
///

class CMainPage : public CNotifyPump
{
public:
	CMainPage();

public:
	void SetPaintMagager(CPaintManagerUI* pPaintMgr);

	DUI_DECLARE_MESSAGE_MAP()
	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged( TNotifyUI &msg );
	virtual void OnItemClick( TNotifyUI &msg );

private:
	CPaintManagerUI* m_pPaintManager;
};

//////////////////////////////////////////////////////////////////////////
///

class CMainWnd : public WindowImplBase
{
public:
	CMainWnd(void);
	~CMainWnd(void);

public:
	virtual LPCTSTR GetWindowClassName( void ) const;
	
	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile();
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual LPCTSTR GetResourceID() const;
	virtual CDuiString GetZIPFileName() const;

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void OnFinalMessage( HWND );
	virtual void InitWindow();
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

		LRESULT lResult = 0;

		switch(uMsg)
		{
		case WM_GETWPPAGE_OK:
			{
				lResult = OnGetWPPageOK(uMsg, wParam, lParam, bHandled);
				break;
			}
		case WM_GETWPINFO_OK:
			{
				lResult = OnGetWPInfoOK(uMsg, wParam, lParam, bHandled);
				break;
			}
		default: break;
		}
		return lResult;
	}
public:
	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		PostQuitMessage(0);
		return 0;
	}

	virtual LRESULT OnGetWPPageOK(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		WPPage* pPage = (WPPage*)wParam;
		if(pPage != NULL)
		{
			CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_PaintManager.FindControl(_T("wp_tablayout"));
			if(pTabLayout)
			{
				CString sTabPanel;
				sTabPanel.Format(_T("wp_tabpanel@%s"), pPage->sType);
				CVerticalLayoutUI* pTabPanel = (CVerticalLayoutUI*)m_PaintManager.FindControl(sTabPanel);
				if(pTabPanel == NULL)
				{
					pTabPanel = new CVerticalLayoutUI();
					pTabPanel->SetName(sTabPanel);
					pTabPanel->SetFixedHeight(pTabLayout->GetHeight());
					pTabLayout->Add(pTabPanel);
					pTabPanel->EnableScrollBar(true, false);
					pTabPanel->GetVerticalScrollBar()->SetLineSize(pTabLayout->GetHeight());
					pTabLayout->SelectItem(pTabPanel);
					for(int i = 1; i <= pPage->nTotalPage; i++)
					{
						CString sPanel;
						sPanel.Format(_T("wp_panel_%s_%d"), pPage->sType, i);
						CWPPanel* pPanel = new CWPPanel();
						pPanel->SetFixedHeight(pTabPanel->GetHeight());
						pPanel->SetName(sPanel);
						pTabPanel->Add(pPanel);
						//DoEvents();
					}
				}
			}

			delete pPage;
			pPage = NULL;
		}
		return 0;
	}

	virtual LRESULT OnGetWPInfoOK(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		WPInfoKey* pKey = (WPInfoKey*)wParam;
		if(pKey != NULL)
		{
			CString sPanel;
			sPanel.Format(_T("wp_panel_%s_%d"), pKey->sType, pKey->nPage);
			CWPPanel* pPanel = (CWPPanel*)m_PaintManager.FindControl(sPanel);
			if(pPanel != NULL)
			{
				std::vector<WPInfo> vInfos;
				if(m_Data.GetBizhi(pKey->sType, pKey->nPage, vInfos))
				{
					for(UINT i = 0; i < vInfos.size(); i++)
					{
						WPInfo Info = vInfos[i];
						CString sID = Info.id;
						CString sThumbMid = Info.thumb_mid;
						CString sThumbNail = Info.thumb_nail;
						CString sButton;
						sButton.Format(_T("wp_button_%s_%d"), pKey->sType,_ttoi(Info.id));
						CWPButton* pButton = new CWPButton();
						pButton->SetName(sButton);
						pButton->SetWPImage(sThumbMid);
						pButton->SetUserData(sThumbNail);
						pPanel->Add(pButton);
						//DoEvents();
					}
				}
			}
			delete pKey;
			pKey = NULL;
		}
		return 0;
	}

public:
	virtual void Notify( TNotifyUI &msg );
	DUI_DECLARE_MESSAGE_MAP()
	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged( TNotifyUI &msg );
	virtual void OnItemClick( TNotifyUI &msg );

private:
	CMainPage m_MainPage;
	CDataMgr m_Data;
};
