#include "StdAfx.h"
#include "Project.h"

CProjectTool::CProjectTool()
{

}

CProjectTool::~CProjectTool()
{

}

BOOL CProjectTool::GetProjectInfo(LPCTSTR lpszProjectFilePath, PROJECT_DETAILS_INFO& ProjectDetailInfo)
{
	ProjectDetailInfo.strMainFile.Add(_T("setup.nsi"));
	ProjectDetailInfo.strIncludeFile.Add(_T("ttimehelper.nsh"));
	ProjectDetailInfo.strResFile.Add(_T("TTimeHelper.ico"));
	ProjectDetailInfo.strResFile.Add(_T("uninst.ico"));
		
	return TRUE;
}