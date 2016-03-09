#include "stdafx.h"
#include "DoubleWndBase.h"

#define	DUILIB_DEFAULT_SKIN_FOLDER		_T("skin\\")
#define	DUILIB_DEFAULT_SKIN_FILE				_T("skin.zip")
#define	DUILIB_DEFAULT_SKIN_XML_FILE		_T("main_dlg.xml")
#define	DUILIB_DOUBLE_WINDOW_CLASSNAME	 _T("LsGuiFoundation")

LPCTSTR szCloseBtnName = _T("CloseBtn");		//�رհ�ť
LPCTSTR szMaxBtnName   = _T("MaxBtn");		//��󻯰�ť
LPCTSTR szMinBtnName	= _T("MinBtn");		//��С����ť
LPCTSTR szSkinBtnName	= _T("SkinBtn");		//������ť


stNotifyHandler CDoubleWndBase::notifyMap[] = 
{
	//��ť���
	stNotifyHandler(_T("click"),&CDoubleWndBase::OnButtonClick),

	//��ѡ�� ѡ��
	stNotifyHandler(_T("selectchanged"),&CDoubleWndBase::OnOptionSelectChange),

	//�������
	stNotifyHandler(_T("link"),&CDoubleWndBase::OnLinkClick),

	//Slider ֵ�ı�
	stNotifyHandler(_T("valuechanged"),&CDoubleWndBase::OnSliderValueChange),

	//edit ���� �ı�
	stNotifyHandler(_T("textchanged"),&CDoubleWndBase::OnEditTextChange),

	//�ı����а��� return��
	stNotifyHandler(_T("return"),&CDoubleWndBase::OnEditReturn),

	//��ʱ��
	stNotifyHandler(_T("timer"),&CDoubleWndBase::OnTimer),

	//�ӿؼ� ��ý���
	stNotifyHandler(_T("setfocus"),&CDoubleWndBase::OnFocus),

	//�ӿؼ� ʧȥ����
	stNotifyHandler(_T("killfocus"),&CDoubleWndBase::OnKillFocus)
};

CDoubleWndBase::CDoubleWndBase(const CDuiString& strGUI,const CDuiString& strZipPath, TCHAR bZipPath ,const CDuiString& strBKImage /* = _T */)
{
	int pos = strZipPath.ReverseFind(_T('\\'));
	CDuiString strPath;
	CDuiString strZip = strZipPath;
	if(pos >= 0)
	{
		strPath = strZipPath.Left(pos + 1);
		strZip  = strZipPath.Right(strZipPath.GetLength() - (pos + 1));
	}
	__Init(strGUI,strPath,strZip,strBKImage);
}

CDoubleWndBase::CDoubleWndBase(const CDuiString& strGUI,const CDuiString& strPath,const CDuiString& strBKImage /* = _T */)
{
	__Init(strGUI,strPath,_T(""),strBKImage);
}

CDoubleWndBase::CDoubleWndBase(const CDuiString& strBKImage /* = _T */)
{
	__Init(DUILIB_DEFAULT_SKIN_FILE,DUILIB_DEFAULT_SKIN_FOLDER,_T(""),strBKImage);
}


CDoubleWndBase::~CDoubleWndBase(void)
{
	if(SUCCEEDED(m_hRet))
		::CoUninitialize();

	if(m_pBackWnd)
	{
		CIrregularWindow::UnInitGDIplus();
		delete m_pBackWnd;
		m_pBackWnd = NULL;
	}
}


void CDoubleWndBase::__Init(const CDuiString& strGUI,const CDuiString& strPath,const CDuiString& strZip,const CDuiString& strBKImage)
{
	HWND hBkWnd = NULL;
	m_pBackWnd = NULL;

	//ʵ����� 
	CPaintManagerUI::SetInstance((HINSTANCE)::GetModuleHandle(NULL));

	CDuiString strResourcePath = CPaintManagerUI::GetInstancePath() + strPath;
	//��Դ·��
	CPaintManagerUI::SetResourcePath(strResourcePath);
	//zip��Դ������
	CPaintManagerUI::SetResourceZip(strZip);
	m_hRet = ::CoInitialize(NULL);
	ASSERT(SUCCEEDED(m_hRet));

	if(strBKImage.IsEmpty() == FALSE)
	{
		CIrregularWindow::InitGDIplus();
		m_pBackWnd = new CIrregularWindow(strResourcePath + strBKImage);
		ASSERT(m_pBackWnd != NULL && _T("new CIrregularWindow() ʧ��!"));

		if(m_pBackWnd)
			hBkWnd = m_pBackWnd->GetHandle();
	}

	//��������
	Create(hBkWnd, _T("__DUIDialog"), WS_OVERLAPPEDWINDOW, WS_EX_WINDOWEDGE);
	m_pm.Init(m_hWnd);

	if(strGUI.IsEmpty() == FALSE)
	{
		CDialogBuilder builder;
		STRINGorID xml(const_cast<CDuiString&>(strGUI).GetData());
		CControlUI* pRoot = builder.Create(xml, (UINT)0, NULL, &m_pm);
		ASSERT(pRoot && _T("����˫�㴰�ڽ���XML�ļ�ʧ�ܣ�"));

		m_pm.AttachDialog(pRoot);
	}

	m_pm.AddNotifier(this);
}

//��ť����¼�
void CDoubleWndBase::OnButtonClick(TNotifyUI& msg)
{
	if(msg.pSender == NULL)
		return;

	if(msg.pSender->GetName() == szCloseBtnName)
	{
		if(OnClose() == S_OK)
		{
			::DestroyWindow(m_hWnd);
			if(m_pBackWnd)
				::DestroyWindow(m_pBackWnd->GetHandle());
		}
	}
	else if(msg.pSender->GetName() == szMaxBtnName)
		OnMaxmize(msg);
	else if(msg.pSender->GetName() == szMinBtnName)
		OnMinimize(msg);
	else if(msg.pSender->GetName() == szSkinBtnName)
		OnSkinBtn(msg);
}

//COptionUI ѡ�з����仯ʱ
void CDoubleWndBase::OnOptionSelectChange(TNotifyUI& msg)
{
}

//��� CTextUI���������
void CDoubleWndBase::OnLinkClick(TNotifyUI& msg)
{

}

void CDoubleWndBase::OnSliderValueChange(TNotifyUI& msg)
{
}

void CDoubleWndBase::OnEditTextChange(TNotifyUI& msg)
{
}

void CDoubleWndBase::OnEditReturn(TNotifyUI& msg)
{
	//MessageBox(m_hWnd,_T("���»س���!"),_T("����"),MB_OK);
}

void CDoubleWndBase::OnTimer(TNotifyUI& msg)
{
}

//�ӿؼ� ��ý���
void CDoubleWndBase::OnFocus(TNotifyUI& msg)
{
}

//�ӿؼ� ʧȥ����
void CDoubleWndBase::OnKillFocus(TNotifyUI& msg)
{
}

void CDoubleWndBase::OnMaxmize(TNotifyUI& msg)
{
	if(m_pBackWnd)
		::SendMessage(m_pBackWnd->GetHandle(),WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	else
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
}

void CDoubleWndBase::OnMinimize(TNotifyUI& msg)
{
	if(m_pBackWnd)
		::SendMessage(m_pBackWnd->GetHandle(),WM_SYSCOMMAND, SC_MINIMIZE, 0);
	else
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

void CDoubleWndBase::OnSkinBtn(TNotifyUI& msg)
{

}

LPCTSTR CDoubleWndBase::GetWindowClassName() const
{
	return DUILIB_DOUBLE_WINDOW_CLASSNAME;
}

void CDoubleWndBase::Notify(TNotifyUI& msg)
{
	for(size_t i = 0; i < _countof(notifyMap); ++i)
	{
		if(msg.sType == notifyMap[i].strNotifyName && notifyMap[i].handler != NULL)
		{
			(this->*notifyMap[i].handler)(msg);
		}
	}
}

BOOL CDoubleWndBase::ShowWindow(BOOL bCenter)
{
	if(FAILED(m_hRet))
		return FALSE;

	OnPreWindowsShow();

	if(bCenter)
	{
		if(m_pBackWnd)
			m_pBackWnd->CenterWindow();
		else
			CenterWindow();
	}

	ShowWindow(true);
	OnAfterWindowsShow();

	CPaintManagerUI::MessageLoop();

	return TRUE;
}

void CDoubleWndBase::OnPreWindowsShow()
{
}

void CDoubleWndBase::OnAfterWindowsShow()
{
}

LRESULT CDoubleWndBase::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;

	switch(uMsg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0L);
		return 0;
	case WM_CREATE:
		return OnCreate();
	case WM_MOVE:
		{
			WORD x = LOWORD(lParam);
			WORD y = HIWORD(lParam);

			int iiiii  = 0;
		}
		break;
	case WM_SIZE:
		{
			short cx = LOWORD(lParam); 
			short cy = HIWORD(lParam);

			int iiii = 0;
		}
		break;
	}

	if(lRes) 
	{
		return lRes;
	}
	else
	{
		if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) 
			return lRes;

		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}
}

void CDoubleWndBase::OnSetWindowStyle(LONG& dwStyle)
{
	if(m_pBackWnd != NULL)
	{
		dwStyle &= ~WS_CAPTION;
		dwStyle &= ~WS_MAXIMIZEBOX; 
		dwStyle &= ~WS_MINIMIZEBOX; 
		dwStyle &= ~WS_THICKFRAME; 
		dwStyle &= ~WS_BORDER; 
		dwStyle &= ~WS_CAPTION;
	}

	dwStyle |= WS_CLIPSIBLINGS;
	dwStyle |= WS_CLIPCHILDREN;
}

HRESULT CDoubleWndBase::OnCreate()
{
	LONG styleValue = ::GetWindowLong(m_hWnd, GWL_STYLE);

	OnSetWindowStyle(styleValue);

	::SetWindowLong(m_hWnd, GWL_STYLE, styleValue );

	if(m_pBackWnd)
		m_pBackWnd->AttachWindow(m_hWnd);

	return 0;
}

HRESULT CDoubleWndBase::OnClose()
{
	//���������һ���ر�ǰ��ȷ�Ͽ�

	return S_OK;
}