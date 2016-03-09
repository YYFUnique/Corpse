#include "DUIDialog.h"

const TCHAR* c_pCloseBtnName = _T("CloseBtn");
const TCHAR* c_pMaxBtnName   = _T("MaxBtn");
const TCHAR* c_pMinBtnName	 = _T("MinBtn");

stNotifyHandler CDUIDialog::notifyMap[] = 
{
	//��ť���
	stNotifyHandler(_T("click"),&CDUIDialog::OnButtonClick),

	//��ѡ�� ѡ��
	stNotifyHandler(_T("selectchanged"),&CDUIDialog::OnOptionSelectChange),

	//�������
	stNotifyHandler(_T("link"),&CDUIDialog::OnLinkClick),

	//Slider ֵ�ı�
	stNotifyHandler(_T("valuechanged"),&CDUIDialog::OnSliderValueChange),

	//edit ���� �ı�
	stNotifyHandler(_T("textchanged"),&CDUIDialog::OnEditTextChange),

	//�ı����а��� return��
	stNotifyHandler(_T("return"),&CDUIDialog::OnEditReturn),

	//��ʱ��
	stNotifyHandler(_T("timer"),&CDUIDialog::OnTimer),

	//�ӿؼ� ��ý���
	stNotifyHandler(_T("setfocus"),&CDUIDialog::OnFocusIn),

	//�ӿؼ� ʧȥ����
	stNotifyHandler(_T("killfocus"),&CDUIDialog::OnFocusOut)
};

CDUIDialog::CDUIDialog(const CDuiString& strBKImage)
{
	__Init(_T("skin.xml"),_T("skin\\"),_T(""),strBKImage);
}

CDUIDialog::CDUIDialog(const CDuiString& strGUI,const CDuiString& strPath,const CDuiString& strBKImage)
{
	__Init(strGUI,strPath,_T(""),strBKImage);
}

CDUIDialog::CDUIDialog(const CDuiString& strGUI,const CDuiString& strZipPath, TCHAR bZipPath,const CDuiString& strBKImage)
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

void CDUIDialog::__Init(const CDuiString& strGUI,const CDuiString& strPath,const CDuiString& strZip,const CDuiString& strBKImage)
{
	HWND hBkWnd = NULL;

	m_hr = S_OK;

	m_pBackWnd = NULL;

	//ʵ����� 
	CPaintManagerUI::SetInstance((HINSTANCE)::GetModuleHandle(NULL));

	CDuiString strResourcePath = CPaintManagerUI::GetInstancePath() + strPath;

	//��Դ·��
	CPaintManagerUI::SetResourcePath(strResourcePath);

	//zip��Դ������
	CPaintManagerUI::SetResourceZip(strZip);

	m_hr = ::CoInitialize(NULL);

	assert(SUCCEEDED(m_hr));

	if(!strBKImage.IsEmpty())
	{
		//gdi+ ʵ�����ʹ���
		CIrregularWindow::InitGDIplus();

		m_pBackWnd = new CIrregularWindow(strResourcePath + strBKImage);

		assert(m_pBackWnd != NULL && _T("new CIrregularWindow() ʧ��!"));

		if(m_pBackWnd)
		{
			hBkWnd = m_pBackWnd->GetHandle();
		}
	}

	//��������
	Create(hBkWnd, _T("__DUIDialog"), WS_OVERLAPPEDWINDOW, WS_EX_WINDOWEDGE);

	m_pm.Init(m_hWnd);

	if(!strGUI.IsEmpty())
	{
		CDialogBuilder builder;

		STRINGorID xml(const_cast<CDuiString&>(strGUI).GetData());

		CControlUI* pRoot = builder.Create(xml, (UINT)0, NULL, &m_pm);

		assert(pRoot && _T("GUI �ļ�����ʧ��!"));

		m_pm.AttachDialog(pRoot);
	}

	m_pm.AddNotifier(this);
}

LPCTSTR CDUIDialog::GetWindowClassName() const
{
	return _T("__DUIDialogClass");
}

void CDUIDialog::Notify(TNotifyUI& msg)
{
	for(size_t i = 0; i < count_of(notifyMap); ++i)
	{
		if(msg.sType == notifyMap[i].strNotifyName && notifyMap[i].handler != NULL)
		{
			(this->*notifyMap[i].handler)(msg);
		}
	}
}

bool CDUIDialog::Show(bool bCenter)
{
	if(SUCCEEDED(m_hr))
	{
		PreShow();

		if(bCenter)
		{
			if(m_pBackWnd)
			{
				m_pBackWnd->CenterWindow();
			}
			else
			{
				CenterWindow();
			}
		}
		ShowWindow(true);
		    
		AfterShow();

		CPaintManagerUI::MessageLoop();

		return true;
	}
	
	return false;
}

void CDUIDialog::PreShow()
{
}

void CDUIDialog::AfterShow()
{
}

LRESULT CDUIDialog::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		 {
			 return lRes;
		 }

		 return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}
}

void CDUIDialog::OnSetWindowStyle(LONG& dwStyle)
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

HRESULT CDUIDialog::OnCreate()
{
    LONG styleValue = ::GetWindowLong(m_hWnd, GWL_STYLE);

	OnSetWindowStyle(styleValue);

    ::SetWindowLong(m_hWnd, GWL_STYLE, styleValue );
	
	if(m_pBackWnd)
	{
		m_pBackWnd->AttachWindow(m_hWnd);
	}

	return 0;
}

HRESULT CDUIDialog::OnClose()
{
	//���������һ���ر�ǰ��ȷ�Ͽ�

	return S_OK;
}

//��ť����¼�
void CDUIDialog::OnButtonClick(TNotifyUI& msg)
{
	if(msg.pSender)
	{
		if(msg.pSender->GetName() == c_pCloseBtnName)
		{
			if(OnClose() == S_OK)
			{
				::DestroyWindow(m_hWnd);
				if(m_pBackWnd)
				{
					::DestroyWindow(m_pBackWnd->GetHandle());
				}
			}
		}
		else if(msg.pSender->GetName() == c_pMaxBtnName)
		{
			OnMaxmize(msg);
		}
		else if(msg.pSender->GetName() == c_pMinBtnName)
		{
			OnMinimize(msg);
		}
	}
}

//COptionUI ѡ�з����仯ʱ
void CDUIDialog::OnOptionSelectChange(TNotifyUI& msg)
{
}

//��� CTextUI���������
void CDUIDialog::OnLinkClick(TNotifyUI& msg)
{

}

void CDUIDialog::OnSliderValueChange(TNotifyUI& msg)
{
}

void CDUIDialog::OnEditTextChange(TNotifyUI& msg)
{
}

void CDUIDialog::OnEditReturn(TNotifyUI& msg)
{
	//MessageBox(m_hWnd,_T("���»س���!"),_T("����"),MB_OK);
}

void CDUIDialog::OnTimer(TNotifyUI& msg)
{
}

//�ӿؼ� ��ý���
void CDUIDialog::OnFocusIn(TNotifyUI& msg)
{
}

//�ӿؼ� ʧȥ����
void CDUIDialog::OnFocusOut(TNotifyUI& msg)
{
}

void CDUIDialog::OnMaxmize(TNotifyUI& msg)
{
	if(m_pBackWnd)
	{
		::SendMessage(m_pBackWnd->GetHandle(),WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}
	else
	{
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}
}

void CDUIDialog::OnMinimize(TNotifyUI& msg)
{
	if(m_pBackWnd)
	{
		::SendMessage(m_pBackWnd->GetHandle(),WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
	else
	{
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
}

CDUIDialog::~CDUIDialog()
{
	if(SUCCEEDED(m_hr))
	{
		::CoUninitialize();
	}

	if(m_pBackWnd)
	{
		CIrregularWindow::UnInitGDIplus();

		delete m_pBackWnd;

		m_pBackWnd = NULL;
	}
}