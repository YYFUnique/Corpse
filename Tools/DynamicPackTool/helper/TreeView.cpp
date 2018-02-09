#include "StdAfx.h"
#include "TreeView.h"
#include "../Solution.h"

#define WRYH_NONE_FONT		1	//	΢���ź�һ������
#define WRYH_BOLD_FONT		2	//	΢���źڼӴ�����

CProjectTreeView::CProjectTreeView(CSolution* pSolution, CPaintManagerUI* pManagerUI)
{
	m_pSolution = pSolution;
	m_pManagerUI = pManagerUI;
}

CProjectTreeView::~CProjectTreeView()
{

}

BOOL CProjectTreeView::LoadSolution()
{
	BOOL bSuccess = FALSE;

	// ���ؽ������
	CTreeViewUI* pTreeView = (CTreeViewUI*)m_pManagerUI->FindControl(_T("Project"));

	// ���ø��ڵ���Ŀ����
	CTreeNodeUI* pTreeNode = new CTreeNodeUI;
	pTreeView->Add(pTreeNode);

	CProjectItemInfoList SolutionInfo;
	CString strSolutionFile;

	do 
	{
		if (m_pSolution == NULL)
			break;

		 strSolutionFile = m_pSolution->GetSolutionFilePath();

		TCHAR szSolutionPath[MAX_PATH];
		_tcscpy(szSolutionPath, strSolutionFile);
		PathRemoveFileSpec(szSolutionPath);

		//������Ŀ����
		m_pSolution->GetSolutionProject(SolutionInfo);
		POSITION pos = SolutionInfo.GetHeadPosition();

		while(pos)
		{
			const PROJECT_ITEM_INFO& ProjectItemInfo = SolutionInfo.GetNext(pos);

			CTreeNodeUI* pProjectNode = new CTreeNodeUI;
			pTreeView->Add(pProjectNode);

			DWORD dwFont = WRYH_NONE_FONT;
			if (ProjectItemInfo.bActive)
			{
				//	���ü�����Ŀ�ڵ�ָ��
				dwFont = WRYH_BOLD_FONT;
				m_pActiveNode = pProjectNode;
			}

			pProjectNode->SetItemText(ProjectItemInfo.strProjectName);
			pProjectNode->SetName(ProjectItemInfo.strProjectGUID);
			pProjectNode->GetItemButton()->SetStyle(_T("TreeProject"));
			pProjectNode->GetItemButton()->SetFont(dwFont);
			/*pProjectNode->GetDottedLine()->SetStyle(_T("TreeProjectDotLine"));
			pProjectNode->GetDottedLine()->SetVisible(true);*/
			pProjectNode->SetFixedHeight(27);

			TCHAR szProjectFilePath[MAX_PATH];
			// ������Ŀ�ļ�
			PathCombine(szProjectFilePath, szSolutionPath, ProjectItemInfo.strProjectFilePath);
			if (PathFileExists(szProjectFilePath) == FALSE)
				continue;

			CProjectTool* pProjectTool = new CProjectTool;
			if (pProjectTool == NULL)
				continue;

			PROJECT_DETAILS_INFO ProjectDetailInfo;
			if (pProjectTool->GetProjectInfo(szProjectFilePath, ProjectDetailInfo) == FALSE)
				continue;

			InsertProjectDetailInfo(pProjectNode, ProjectDetailInfo);
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (PathFileExists(strSolutionFile))
	{
		CDuiString strTipInfo;
		LPTSTR lpszProjectName = PathFindFileName(strSolutionFile);
		PathRemoveExtension(lpszProjectName);
		strTipInfo.Format(_T("�������'%s'(%d����Ŀ)"), lpszProjectName, SolutionInfo.GetCount());
		pTreeNode->SetItemText(strTipInfo);
		pTreeNode->SetName(_T("TreeRoot"));
		pTreeNode->GetItemButton()->SetStyle(_T("TreeRoot"));
		pTreeNode->GetItemButton()->SetFont(WRYH_NONE_FONT);
		pTreeNode->SetFixedHeight(30);
	}

	return bSuccess;
}

void CProjectTreeView::InsertProjectDetailInfo(CTreeNodeUI* pProjectNode, PROJECT_DETAILS_INFO& ProjectDetailInfo)
{
	////////////////////////////////////////////////////////////////
	//// �����Ҫ�ļ�
	////////////////////////////////////////////////////////////////
	CTreeNodeUI* pProjectMainFile = new CTreeNodeUI;
	pProjectNode->Add(pProjectMainFile);

	DWORD dwCount = ProjectDetailInfo.strMainFile.GetCount();
	if (dwCount != 0)
	{
		for (UINT n=0; n<dwCount; ++n)
		{
			CTreeNodeUI* pMainFile = new CTreeNodeUI;
			pProjectMainFile->Add(pMainFile);

			pMainFile->SetItemText(ProjectDetailInfo.strMainFile.GetAt(n));
			pMainFile->SetItemFont(WRYH_NONE_FONT);
			pMainFile->GetFolderButton()->SetStyle(_T("TreeNsi"));
			pMainFile->SetVisibleFolderBtn(true);
			pMainFile->SetFixedHeight(27);
		}
	}

	pProjectMainFile->SetItemText(_T("���ļ�"));
	pProjectMainFile->SetItemFont(WRYH_NONE_FONT);
	pProjectMainFile->SetVisibleFolderBtn(true);
	pProjectMainFile->GetFolderButton()->SetStyle(_T("TreeFolder"));
	//pProjectMainFile->GetFolderButton()->SetFont(WRYH_NONE_FONT);
	pProjectMainFile->SetFixedHeight(27);

	////////////////////////////////////////////////////////////////
	//// ��������ļ�
	////////////////////////////////////////////////////////////////
	CTreeNodeUI* pProjectIncFile = new CTreeNodeUI;
	pProjectNode->Add(pProjectIncFile);

	dwCount = ProjectDetailInfo.strIncludeFile.GetCount();
	if (dwCount != 0)
	{
		for(UINT n=0; n<dwCount; ++n)
		{
			CTreeNodeUI* pIncFile = new CTreeNodeUI;
			pProjectIncFile->Add(pIncFile);

			pIncFile->SetItemText(ProjectDetailInfo.strIncludeFile.GetAt(n));
			pIncFile->SetItemFont(WRYH_NONE_FONT);
			pIncFile->GetFolderButton()->SetStyle(_T("TreeNsi"));
			pIncFile->SetVisibleFolderBtn(true);
			pIncFile->SetFixedHeight(27);
		}
	}

	pProjectIncFile->SetItemText(_T("�����ļ�"));
	pProjectIncFile->SetItemFont(WRYH_NONE_FONT);
	pProjectIncFile->SetVisibleFolderBtn(true);
	pProjectIncFile->GetFolderButton()->SetStyle(_T("TreeFolder"));
	pProjectIncFile->SetFixedHeight(27);


	////////////////////////////////////////////////////////////////
	//// �����Դ�ļ�
	////////////////////////////////////////////////////////////////
	CTreeNodeUI* pProjectResFile = new CTreeNodeUI;
	pProjectNode->Add(pProjectResFile);

	dwCount = ProjectDetailInfo.strResFile.GetCount();
	if (dwCount != 0)
	{
		for(UINT n=0; n<dwCount; ++n)
		{
			CTreeNodeUI* pResFile = new CTreeNodeUI;
			pProjectResFile->Add(pResFile);

			pResFile->SetItemText(ProjectDetailInfo.strResFile.GetAt(n));
			pResFile->SetItemFont(WRYH_NONE_FONT);
			pResFile->GetFolderButton()->SetStyle(_T("TreeRes"));
			pResFile->SetVisibleFolderBtn(true);
			pResFile->SetFixedHeight(27);
		}
	}
	pProjectResFile->SetItemText(_T("��Դ�ļ�"));
	pProjectResFile->SetItemFont(WRYH_NONE_FONT);
	pProjectResFile->SetVisibleFolderBtn(true);
	pProjectResFile->GetFolderButton()->SetStyle(_T("TreeFolder"));
	pProjectResFile->SetFixedHeight(27);
}

BOOL CProjectTreeView::SetSelectItemStart()
{
	BOOL bSuccess = FALSE;
	do 
	{
		CTreeViewUI* pTreeView = (CTreeViewUI*)m_pManagerUI->FindControl(_T("Project"));
		if (pTreeView == NULL)
			break;

		m_pActiveNode->SetItemFont(WRYH_NONE_FONT);

		int nIndex = pTreeView->GetCurSel();
		CTreeNodeUI* pNode = (CTreeNodeUI*)pTreeView->GetItemAt(nIndex);
		if (pNode == NULL)
			break;

		pNode->SetItemFont(WRYH_BOLD_FONT);

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL CProjectTreeView::GetActiveProjectGUID(CString& strActiveProjectGUID)
{
	CTreeViewUI* pTreeView = (CTreeViewUI*)m_pManagerUI->FindControl(_T("Project"));
	if (pTreeView == NULL)
		return FALSE;

	int nIndex = pTreeView->GetCurSel();
	CTreeNodeUI* pNode = (CTreeNodeUI*)pTreeView->GetItemAt(nIndex);
	if (pNode == NULL)
		return FALSE;

	strActiveProjectGUID = pNode->GetText();
	return TRUE;
}