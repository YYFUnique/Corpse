
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "CorpseTools.h"

#include "MainFrm.h"
#include "Common/Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()
//
//static UINT indicators[] =
//{
//	ID_SEPARATOR,           // ״̬��ָʾ��
//	ID_INDICATOR_CAPS,
//	ID_INDICATOR_NUM,
//	ID_INDICATOR_SCRL,
//};

// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//if (!m_wndStatusBar.Create(this))
	//{
	//	TRACE0("δ�ܴ���״̬��\n");
	//	return -1;      // δ�ܴ���
	//}
	//m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	//SetTitle(_T("��������ר�ù���"));
	::SetProp(m_hWnd, _T("ToolSemaphore"), (HANDLE)1);//�����ź�����ֻ��ʹ��һ��ʵ��ʹ�ã�
	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	int dwWidth,dwHeight;
	//��ȡ���һ��ʹ�õĴ��ڴ�С
	if ( !( ReadRegFromLanToolNode(_T("ToolWidth"),(DWORD& )dwWidth) &&
		ReadRegFromLanToolNode(_T("ToolHeight"),(DWORD& )dwHeight)) 
		|| dwHeight<70)//��ֹ��������С����ʽ�ر�
	{
		dwWidth=894;
		dwHeight=640;
	}

	::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, dwWidth, dwHeight, SWP_NOMOVE);
	CenterWindow();

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	return TRUE;
}

// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame ��Ϣ�������
// BOOL CMainFrame::SendData(CONNID ConnID,PMSG_HEAD pMsg)
// {
// 	m_Server.Send()
/*}*/

void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	RECT rcWnd;
	GetWindowRect(&rcWnd);
	DWORD dwWidth = rcWnd.right - rcWnd.left;
	DWORD dwHeight = rcWnd.bottom - rcWnd.top;
	WriteRegFromLanToolNode(_T("ToolWidth"), (DWORD&)dwWidth);
	WriteRegFromLanToolNode(_T("LToolHeight"), (DWORD&)dwHeight);
	::RemoveProp(m_hWnd, _T("ToolSemaphore")); //�Ƴ��ź�����ֻ��ʹ��һ��ʵ��ʹ�ã�
	CFrameWnd::OnClose();
}

//��ȡ	HKEY_LOCAL_MACHINE\\SOFTWARE\\Lonsin\\LanGuardTool�µ�	��ֵ
BOOL CMainFrame::ReadRegFromLanToolNode(LPCTSTR lpKeyName,DWORD &dwValue)
{
	DWORD dwValueT = 0; 
	DWORD dwErrorCode;
	BOOL bRet=FALSE;
	dwErrorCode = RegGetToolDwordValue(lpKeyName, dwValueT);
	if (dwErrorCode == ERROR_SUCCESS)
	{
		dwValue = dwValueT;
		bRet=TRUE;
	}

	return bRet;	
}

//д��	HKEY_LOCAL_MACHINE\\SOFTWARE\\Lonsin\\LanGuardTool�µ�	��ֵ
BOOL CMainFrame::WriteRegFromLanToolNode(LPCTSTR lpKeyName, DWORD dwValue)
{
	DWORD dwErrorCode = RegSetToolDwordValue(lpKeyName, dwValue);
	if (dwErrorCode == ERROR_SUCCESS)
		return TRUE;
	return FALSE;
}