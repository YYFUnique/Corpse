#pragma once
#include "../Resource.h"

// CCleanProject �Ի���

class CCleanProject : public CDialog
{
	DECLARE_DYNAMIC(CCleanProject)

public:
	CCleanProject(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCleanProject();

// �Ի�������
	enum { IDD = IDD_CLEAN_PROJECT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
