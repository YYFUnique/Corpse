// DialogListLabelElement.cpp : implementation file
//
#include "stdafx.h"
#include "UIDesigner.h"
#include "DialogListLabelElement.h"
#include "DialogUIAttribEdit.h"
#include "LayoutManager.h"
#include "UIUtil.h"

//////////////////////////////////////////////////////////////////////////
// CDialogListLabelElement dialog

IMPLEMENT_DYNAMIC(CDialogListLabelElement, CDialog)

CDialogListLabelElement::CDialogListLabelElement(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogListLabelElement::IDD, pParent)
{
	m_pManager = NULL;
}

CDialogListLabelElement::~CDialogListLabelElement()
{
}

void CDialogListLabelElement::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogListLabelElement, CDialog)
// 	ON_BN_CLICKED(IDC_BUTTON_ATTRIB_ADD, &CDialogListLabelElement::OnBnClickedButtonAttribAdd)
// 	ON_BN_CLICKED(IDC_BUTTON_ATTRIB_DELETE, &CDialogListLabelElement::OnBnClickedButtonAttribDelete)
// 	ON_BN_CLICKED(IDC_BUTTON_ATTRIB_MODIFY, &CDialogListLabelElement::OnBnClickedButtonAttribModify)
// 	ON_LBN_SELCHANGE(IDC_LIST_DEFAULT_ATTRIB, &CDialogListLabelElement::OnLbnSelchangeListDefaultAttrib)
// 	ON_WM_DESTROY()
// 	ON_BN_CLICKED(IDC_BUTTON_SAVE_AS_STYLE, &CDialogListLabelElement::OnBnClickedButtonSaveAsStyle)
END_MESSAGE_MAP()


// CDialogListLabelElement message handlers

BOOL CDialogListLabelElement::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
// 	CRect rectUIProp;
// 	m_wndUIPropLocation.GetClientRect(&rectUIProp);
// 	m_wndUIPropLocation.MapWindowPoints(this, &rectUIProp);
// 	m_wndUIProperties.Create(WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER, rectUIProp, this, ID_UI_PROP_WND);
// 
// 	m_menuUI.LoadMenu(IDR_UI);
// 	m_btnAdd.m_hMenu = m_menuUI.GetSubMenu(0)->GetSafeHmenu();
// 	m_btnAdd.SizeToContent();
// 	m_btnAdd.m_bOSMenu = FALSE;
// 
// #ifdef _DEBUG
// 	this->GetDlgItem(IDC_BUTTON_SAVE_AS_STYLE)->ShowWindow(SW_SHOW);
// #endif
// 
// 	m_pManager = g_pMainFrame->GetActiveUIView()->GetPaintManager();
// 	ASSERT(m_pManager);
// 	const CStdStringPtrMap& DefaultAttribHash = m_pManager->GetDefaultAttribultes();
// 	for(int i=0; i<DefaultAttribHash.GetSize(); i++)
// 	{
// 		LPCTSTR pstrName = DefaultAttribHash.GetAt(i);
// 		m_lstDefaultAttrib.AddString(pstrName);
// 	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
