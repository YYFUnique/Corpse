// CleanProject.cpp : 实现文件
//

#include "stdafx.h"
#include "../CommonDlg.h"
#include "CleanProject.h"


// CCleanProject 对话框

IMPLEMENT_DYNAMIC(CCleanProject, CDialog)

CCleanProject::CCleanProject(CWnd* pParent /*=NULL*/)
	: CDialog(CCleanProject::IDD, pParent)
{

}

CCleanProject::~CCleanProject()
{
}

void CCleanProject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCleanProject, CDialog)
END_MESSAGE_MAP()


// CCleanProject 消息处理程序
