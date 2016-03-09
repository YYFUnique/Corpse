#pragma once
#include "../Resource.h"

// CCleanProject 对话框

class CCleanProject : public CDialog
{
	DECLARE_DYNAMIC(CCleanProject)

public:
	CCleanProject(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCleanProject();

// 对话框数据
	enum { IDD = IDD_CLEAN_PROJECT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
