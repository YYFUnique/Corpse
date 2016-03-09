#pragma once
#include "afxwin.h"
#include "UIProperties.h"

//////////////////////////////////////////////////////////////////////////
// CDialogListLabelElement dialog

class CDialogListLabelElement : public CDialog
{
	DECLARE_DYNAMIC(CDialogListLabelElement)

public:
	CDialogListLabelElement(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogListLabelElement();

// Dialog Data
	enum { IDD = IDD_DIALOG_LIST_LABEL_ELEMENT };

private:
	CUIProperties m_wndUIProperties;
	CPaintManagerUI* m_pManager;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	virtual BOOL OnInitDialog();
	BOOL GetDefaultAttrib(CControlUI* pControl, CString& strValue);

private:

public:

};
