#ifndef _DUIDIALOG_H_
#define _DUIDIALOG_H_

/**
  * 
*/

#include "DuiLib\UIlib.h"
#include "IrregularWindow.h"

using namespace DuiLib;

class CDUIDialog;

typedef void (CDUIDialog::*NotifyHandler)(TNotifyUI& msg);

struct stNotifyHandler
{
	CDuiString strNotifyName;

	NotifyHandler handler;

	stNotifyHandler(const TCHAR* pNotifyName,NotifyHandler pHandler)
		:strNotifyName(pNotifyName)
		,handler(pHandler)
	{
	}
};

#ifndef count_of
	#define count_of(x) ((x == NULL) ? 0 : (sizeof(x) / sizeof(x[0])))
#endif

class CDUIDialog : public CWindowWnd, public INotifyUI
{
public:

	/*
		gui�ļ����� skin.xml 
		��Դ·���� EXE�ļ�·�� + "skin";
		BkImage �����ʵ�����ʹ��� �� ����һ��pngͼƬ�Ϳ�����
	*/
	CDUIDialog(const CDuiString& strBKImage = _T(""));

	/*
		strGUI:�ļ���
		strPath: ��Դ·�� ����� EXE�ļ�·��,��:"skin"
	*/
	CDUIDialog(const CDuiString& strGUI,const CDuiString& strPath,const CDuiString& strBKImage = _T(""));

	/*
		bZipPath: ���ֵ��ʵ û�õ� ��Ϊ�������湹�캯���и�����
		strPath:zip��Դ����·��,��:"skin\\skin.zip"
		CDUIDialog(_T("skin.xml"),_T("skin\\skin.zip")
	*/
	CDUIDialog(const CDuiString& strGUI,const CDuiString& strZipPath, TCHAR bZipPath /*=true*/,const CDuiString& strBKImage = _T(""));

	bool Show(bool bCenter = true);

	~CDUIDialog();

protected:
    /**********�¼�����***********/

	//���ô�����ʽ
	virtual void OnSetWindowStyle(LONG& dwStyle);

	//����ʾ֮ǰ
	virtual void PreShow();

	//��ʾ֮��
	virtual void AfterShow();

	//���ϣ��WM_CLOSE ʹ��Ĭ�ϴ���(���ٴ���) ���� 0,���򷵻ط�0
	virtual HRESULT OnClose();

	//��ť����¼�
	virtual void OnButtonClick(TNotifyUI& msg);

	//COptionUI ѡ�з����仯ʱ
	virtual void OnOptionSelectChange(TNotifyUI& msg);

	//��� CTextUI���������
	virtual void OnLinkClick(TNotifyUI& msg);

	//Slider ֵ�����仯
	virtual void OnSliderValueChange(TNotifyUI& msg);

	//�ı��� ���ݷ����ı�ʱ
	virtual void OnEditTextChange(TNotifyUI& msg);

	//�ı����а��� return��
	virtual void OnEditReturn(TNotifyUI& msg);

	//�ӿؼ�  ��ʱ��
	virtual void OnTimer(TNotifyUI& msg);

	//�ӿؼ� ��ý���
	virtual void OnFocusIn(TNotifyUI& msg);

	//�ӿؼ� ʧȥ����
	virtual void OnFocusOut(TNotifyUI& msg);

	virtual void OnMaxmize(TNotifyUI& msg);

	virtual void OnMinimize(TNotifyUI& msg);

protected:

	virtual LPCTSTR GetWindowClassName() const;

	virtual void Notify(TNotifyUI& msg);

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

	void __Init(const CDuiString& strGUI,const CDuiString& strPath,const CDuiString& strZip,const CDuiString& BkImage);

	//��ӦWM_CREATE��Ϣ (�������麯�� û��,ӦΪ�ڹ��캯���������е��õ�)
	HRESULT OnCreate();

private:

	static stNotifyHandler notifyMap[];

protected:

	 CPaintManagerUI m_pm;

	 CIrregularWindow* m_pBackWnd;

	 HRESULT m_hr;
};


#endif