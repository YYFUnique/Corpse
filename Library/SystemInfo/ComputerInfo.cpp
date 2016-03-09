#include "StdAfx.h"
#include "ComputerInfo.h"

#include "LsCommon/OsInfo.h"
#include "LsCommon/ErrorInfo.h"

CComputerInfo::CComputerInfo(void)
{
	m_Json.Clear();
	SetComputerInfo();
}

CComputerInfo::~CComputerInfo(void)
{

}

BOOL CComputerInfo::SetComputerInfo()
{
	m_Json.SetValue(_T("ComputerName"),GetComputerName());
	m_Json.SetValue(_T("ComputerProduct"),GetSystemName());
	m_Json.SetValue(_T("CPUInfo"),GetCpuInfo());

	m_Json.SetValue(_T("TotalMem"),(UINT)AfxGetTotalMem());
	m_Json.SetValue(_T("ScreenInfo"),(UINT)AfxGetScreenSize());
	m_Json.SetValue(_T("CameraInfo"),(UINT)GetCameraInfo());
	m_Json.SetValue(_T("ServerPack"),(UINT)GetServerPack());

	return TRUE;
}

CString CComputerInfo::GetComputerInfo()
{
	return m_Json.ToRawString();	
}