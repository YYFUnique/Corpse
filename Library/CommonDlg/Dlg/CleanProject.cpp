// CleanProject.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "../CommonDlg.h"
#include "CleanProject.h"


// CCleanProject �Ի���

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


// CCleanProject ��Ϣ�������
