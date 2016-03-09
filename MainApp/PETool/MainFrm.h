
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
class CPEToolView;
class CLeftView;
class CPELibrary;

class CMainFrame : public CFrameWnd
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 属性
protected:
	CSplitterWnd m_wndSplitter;
	CPELibrary* m_pPELibrary;
public:

// 操作
public:

// 重写
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	
// 实现
public:
	virtual ~CMainFrame();
	CPEToolView* GetRightPane();
	CLeftView* GetLeftPane();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// 生成的消息映射函数
protected:
	afx_msg void OnUpdateViewStyles(CCmdUI* pCmdUI);
	afx_msg void OnViewStyle(UINT nCommandID);
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropFiles(HDROP hDropInfo);
};


