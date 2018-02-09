#include "StdAfx.h"
#include "DynamicPackage.h"

CDynamicPackage::CDynamicPackage()
{
	
}

CDynamicPackage::~CDynamicPackage()
{

}

BOOL CDynamicPackage::Init()
{
	TCHAR szModulePath[MAX_PATH];
	GetModuleFileName(NULL, szModulePath, _countof(szModulePath));
	PathRemoveFileSpec(szModulePath);

	// ����ǩ����������·��
	TCHAR szToolPath[MAX_PATH];
	PathCombine(szToolPath, szModulePath, _T("SignTool"));

	m_strSignToolPath.Format(_T("%s\\SignTool.exe"), szToolPath);
	m_strCrossCertifyPath.Format(_T("%s\\MS_xs_UTN.crt"), szToolPath);

	CString strScriptPath;
	if (PathFileExists(strScriptPath))
		return FALSE;

	m_strScriptPath = strScriptPath;
	// Ĭ�ϵ�MakeNsis��·��
	//GetMakeNsisInstallPath(pDynamicPackage)

	return TRUE;
}