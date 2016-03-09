#pragma once
#include "afxcmn.h"
#include "EditEx.h"

#define WM_LIST_DROPFILE  (WM_USER + 0x0100)  //�Զ�����Ϣ �϶��ļ�
#define WM_ITEM_EDIT         (WM_USER + 0x0101)	//�༭
#define WM_ITEM_INSRET		  (WM_USER + 0x0102)	//����
#define WM_ITEM_DELETE	  (WM_USER + 0x0103) //ɾ��
#define WM_MODIFIED_NOT_SAVE		(WM_USER + 0x0104)	//�༭δ����
#define IDC_EDIT_BOX			1001									//����һ���༭��ID

class CDragFile :
	public CListCtrl
{
public:
	CDragFile(void);
	~CDragFile(void);

	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	/*afx_msg void OnLButtonDown(UINT nFlags, CPoint point);*/
	afx_msg void OnDropFiles(HDROP hDropInfo);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMenuEdit();
	afx_msg void OnMenuInsret();
	afx_msg void OnMenuDelete();
public:
	BOOL EditLabel(_In_ int nItem , _In_ int nSubItem);
	afx_msg LRESULT OnEditEnd(WPARAM wParam , LPARAM lParam);
protected:
	CEditEx m_EditList;
	UINT iItem,iSubItem;
	CPoint Pt;
};
