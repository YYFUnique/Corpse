
// MainFrm.h : CMainFrame ��Ľӿ�
//

#pragma once
class CPEToolView;
class CLeftView;
class CPELibrary;

class CMainFrame : public CFrameWnd
{
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
protected:
	CSplitterWnd m_wndSplitter;
	CPELibrary* m_pPELibrary;
public:

// ����
public:

// ��д
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	
// ʵ��
public:
	virtual ~CMainFrame();
	CPEToolView* GetRightPane();
	CLeftView* GetLeftPane();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnUpdateViewStyles(CCmdUI* pCmdUI);
	afx_msg void OnViewStyle(UINT nCommandID);
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropFiles(HDROP hDropInfo);
};


