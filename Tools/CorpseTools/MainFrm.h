
// MainFrm.h : CMainFrame ��Ľӿ�
//

#pragma once

class CMainFrame : public CFrameWnd
{
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
public:

// ����
public:

	BOOL ReadRegFromLanToolNode(LPCTSTR lpKeyName,DWORD &dwValue);
	BOOL WriteRegFromLanToolNode(LPCTSTR lpKeyName, DWORD dwValue);
// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
// ʵ��
public:
	virtual ~CMainFrame();
/*	BOOL SendData(CONNID ConnID,PMSG_HEAD pMsg);*/
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
	//CStatusBar        m_wndStatusBar;
	CSplitterWnd		m_WndSplitter;
// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
};


