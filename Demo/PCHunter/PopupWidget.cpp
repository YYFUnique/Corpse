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
//   函数名:Init()
//   参数：pszXMLPath：xml文件的路径
//		  hWndParent:指定父窗体参数用来创建模式窗体
//		  pMainPaintManager:主窗体的m_PaintManager指针用来发通知到主窗体
//		  pBtnParent:触发创建本窗体的按钮，用来改变按钮显示的值
//		  ptPos:弹出的位置的左下角的坐标
//		  type：弹出的窗口的类型，这个类型会附带在传送消息到主窗体时，好让主窗体判断是什么类型的小挂件传来的消息
//		  pBtnParent：在改变CSliderUI的同时要改变换肤窗口的CComboUI的显示值，pBtnParent就是这个控件的指针
//===================================================================
void CPopupWidget::Init( LPCTSTR pszXMLPath, HWND hWndParent, CPaintManagerUI* pMainPaintManager, POINT ptPos, SLIDER_TYPE type, CControlUI * pBtnParent)
{  
	m_strXMLPath = pszXMLPath;
	m_MainWndpm = pMainPaintManager;
	m_WidgetType = type;
	m_pBtnParent = pBtnParent;
	Create(hWndParent, _T("PopupWdiget"), UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW);
	::ClientToScreen(hWndParent, &ptPos);
	//这里会计算应该显示的位置，好让自己出现在组合框上方
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
			//修改皮肤设置窗体的组合框的文本显示值，把实际的值通过SetTag函数来保存
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
			//设置SetTag函数好让主窗体知道是什么类型的小挂件窗体
			msg.pSender->SetTag((UINT_PTR)m_WidgetType);
			//通过主窗体的CPaintManager指针来发送消息到主窗体
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