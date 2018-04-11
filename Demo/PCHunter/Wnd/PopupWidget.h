#pragma once

typedef enum tagSLIDER_TYPE				 //ѡ��С�Ҽ�������
{
	SLIDER_TYPE_CHANGE_LISTTRANS = 0,				//�ı�������Ĳ����б�����
	SLIDER_TYPE_CHANGE_WNDTRANS = 1,	//�ı�������Ƥ����͸����
	SLIDER_TYPE_CHANGE_VOLUME = 2,					//�ı������������������Ǹ��Ժ��õ�	
	SLIDER_TYPE_UNDEFINED = 3,								//δ����
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
