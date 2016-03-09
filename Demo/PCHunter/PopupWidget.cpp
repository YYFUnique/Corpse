#include "stdafx.h"
#include "PopupWidget.h"

CPopupWidget::CPopupWidget() 
	:m_MainWndpm(NULL)
	,m_WidgetType(SLIDER_TYPE_UNDEFINED)
	,m_pBtnParent(NULL)
{
}

CPopupWidget::~CPopupWidget()
{
}


LPCTSTR CPopupWidget::GetWindowClassName() const 
{ 
	return _T("PopupWdiget");
}

void CPopupWidget::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

CDuiString CPopupWidget::GetSkinFile()
{
	return m_strXMLPath;
}

CDuiString CPopupWidget::GetSkinFolder()
{
	return _T("PCHunter");
}

//===================================================================
//   ������:Init()
//   ������pszXMLPath��xml�ļ���·��
//		  hWndParent:ָ�������������������ģʽ����
//		  pMainPaintManager:�������m_PaintManagerָ��������֪ͨ��������
//		  pBtnParent:��������������İ�ť�������ı䰴ť��ʾ��ֵ
//		  ptPos:������λ�õ����½ǵ�����
//		  type�������Ĵ��ڵ����ͣ�������ͻḽ���ڴ�����Ϣ��������ʱ�������������ж���ʲô���͵�С�Ҽ���������Ϣ
//		  pBtnParent���ڸı�CSliderUI��ͬʱҪ�ı任�����ڵ�CComboUI����ʾֵ��pBtnParent��������ؼ���ָ��
//===================================================================
void CPopupWidget::Init( LPCTSTR pszXMLPath, HWND hWndParent, CPaintManagerUI* pMainPaintManager, POINT ptPos, SLIDER_TYPE type, CControlUI * pBtnParent)
{  
	m_strXMLPath = pszXMLPath;
	m_MainWndpm = pMainPaintManager;
	m_WidgetType = type;
	m_pBtnParent = pBtnParent;
	Create(hWndParent, _T("PopupWdiget"), UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW);
	::ClientToScreen(hWndParent, &ptPos);
	//��������Ӧ����ʾ��λ�ã������Լ���������Ͽ��Ϸ�
	::SetWindowPos(*this, NULL, ptPos.x, ptPos.y - m_PaintManager.GetClientSize().cy , 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void CPopupWidget::InitWindow()
{
	m_pSlider = (CSliderUI*)m_PaintManager.FindControl(_T("Widget_slider"));

	if (m_pSlider)
	{
		if ( m_WidgetType == SLIDER_TYPE_CHANGE_LISTTRANS)
		{
			m_pSlider->SetMinValue(0);
			m_pSlider->SetMaxValue(255);
		//	m_pSlider->SetValue(m_pBtnParent->GetTag()* 100 / 255);
		}
		else if ( m_WidgetType == SLIDER_TYPE_CHANGE_WNDTRANS)
		{
			m_pSlider->SetMinValue(0);
			m_pSlider->SetMaxValue(150);
		//	m_pSlider->SetValue(m_pBtnParent->GetTag()* 100 / 150);
		}
		else if (m_WidgetType == SLIDER_TYPE_CHANGE_VOLUME)
		{
			m_pSlider->SetMinValue(0);
			m_pSlider->SetMaxValue(100);
		//	m_pSlider->SetValue(m_pBtnParent->GetTag());
		}
		m_pSlider->SetValue(m_pBtnParent->GetTag());
	}
}

void CPopupWidget::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click") ) 
	{
		if( msg.pSender->GetName() == _T("closebtn")) {
			Close(IDOK);
			return; 
		}
	}
	else if ( msg.sType == _T("valuechanged") ||  msg.sType == _T("movevaluechanged"))
	{
		if ( msg.pSender == m_pSlider)
		{
			//�޸�Ƥ�����ô������Ͽ���ı���ʾֵ����ʵ�ʵ�ֵͨ��SetTag����������
			CDuiString szValue;
			if ( m_WidgetType == SLIDER_TYPE_CHANGE_LISTTRANS)
			{
				szValue.SmallFormat(_T("%d%%"),m_pSlider->GetValue()* 100 / 255 );
				m_pBtnParent->SetTag(m_pSlider->GetValue());
				m_pBtnParent->SetText(szValue);
			}
			else if ( m_WidgetType == SLIDER_TYPE_CHANGE_WNDTRANS)
			{
				szValue.SmallFormat(_T("%d%%"),m_pSlider->GetValue() * 100 / 150);
				m_pBtnParent->SetTag(m_pSlider->GetValue());
				m_pBtnParent->SetText(szValue);
			}
			else if ( m_WidgetType == SLIDER_TYPE_CHANGE_VOLUME)
			{
				//m_pBtnParent->SetTag(m_pSlider->GetValue());
			}
			//����SetTag��������������֪����ʲô���͵�С�Ҽ�����
			msg.pSender->SetTag((UINT_PTR)m_WidgetType);
			//ͨ���������CPaintManagerָ����������Ϣ��������
			m_MainWndpm->SendNotify(msg, false);
		}
	}
}

LRESULT CPopupWidget::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DestroyWindow(m_hWnd);
	return 1;
}

LRESULT CPopupWidget::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_ESCAPE)
	{
		DestroyWindow(m_hWnd);
		return TRUE;
	}
	return FALSE;
}