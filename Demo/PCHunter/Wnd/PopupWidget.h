#pragma once

typedef enum tagSLIDER_TYPE				 //选择小挂件的类型
{
	SLIDER_TYPE_CHANGE_LISTTRANS = 0,				//改变主窗体的播放列表都名都
	SLIDER_TYPE_CHANGE_WNDTRANS = 1,	//改变主窗体皮肤的透明度
	SLIDER_TYPE_CHANGE_VOLUME = 2,					//改变主窗体的音量，这个是给以后用的	
	SLIDER_TYPE_UNDEFINED = 3,								//未定义
}SLIDER_TYPE;

class CPopupWidget:  public WindowImplBase
{
public:
    CPopupWidget();
    virtual ~CPopupWidget();  

public:
	LPCTSTR GetWindowClassName() const;

	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();

	void Init(LPCTSTR pszXMLPath, HWND hWndParent, CPaintManagerUI* pMainPaintManager, POINT ptPos, SLIDER_TYPE type, CControlUI * pBtnParent );
	void Notify(TNotifyUI& msg);
   
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
protected:
	CDuiString		m_strXMLPath;
	CPaintManagerUI *m_MainWndpm;
	CControlUI		*m_pBtnParent;
	CSliderUI		*m_pSlider;
	SLIDER_TYPE		m_WidgetType;
};
