#pragma once
#include "afxcmn.h"

#include "DragFile.h"
#include "SelfStruct.h"
// CAddIniRuleDlg �Ի���

class CAddIniRuleDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddIniRuleDlg)

public:
	CAddIniRuleDlg(CString& strTargetFilePath , CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAddIniRuleDlg();

	//void AddFileToItem(LPCTSTR strFilePath, LPCTSTR strCurrent);
// �Ի�������
	enum { IDD = IDD_DIALOG_ADD_RULE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()

	CString& m_strTargetFilePath;
public:

	MODIFY_SAVE m_ModifySave;
	void ChangeModifyStatus(MODIFY_SAVE MDStatus=NOT_MODIFY);
	//afx_msg void OnBnClickedBtnAddTargetFilepath();
	afx_msg void OnBnClickedSaveToIni();
	afx_msg void OnBnClickedBtnLookup();
	afx_msg LRESULT OnModifyNotSave(WPARAM wParam , LPARAM lParam);
//	afx_msg void OnLvnBeginlabeleditListAddFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSaveToIniUpdate();
};
