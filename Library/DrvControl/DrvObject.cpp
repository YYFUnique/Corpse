#include "StdAfx.h"
#include "DrvObject.h"
#include "DllCore/Utils/ErrorInfo.h"

// 默认驱动存放位置
#define DRV_FILE_PATH		_T("\\SystemRoot\\system32\\drivers\\")

CDrvObject::CDrvObject()
{
	m_hDrv = INVALID_HANDLE_VALUE;
}

CDrvObject::~CDrvObject()
{
	CloseDrv();
}

BOOL CDrvObject::OpenDrv()
{
	if (m_hDrv != INVALID_HANDLE_VALUE)
		return TRUE;

	m_hDrv = CreateFile(m_strLinkName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hDrv == INVALID_HANDLE_VALUE)
		return FALSE;

	return TRUE;
}

BOOL CDrvObject::IsOpen()
{
	return m_hDrv != INVALID_HANDLE_VALUE;
}

void CDrvObject::CloseDrv()
{
	if (m_hDrv != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hDrv);
		m_hDrv = INVALID_HANDLE_VALUE;
	}
}

//通用DeviceIoControl函数
BOOL CDrvObject::DeviceIoControl(DWORD dwIoControlCode , LPVOID lpInBuffer , DWORD nInBufferSize, LPVOID lpOutBuffer/* = NULL*/, DWORD nOutBufferSize/* = 0*/ , LPDWORD lpBytesReturned/* = NULL*/)
{
	DWORD dwBytesReturned = 0;
	if (lpBytesReturned == NULL)
		lpBytesReturned = &dwBytesReturned;

	if (OpenDrv() == FALSE)
	{
		SetErrorInfo(CUSTOM_ERROR,0,_T("设置控制码到驱动失败，请确保驱动成功已经打开"));
		return FALSE;
	}

	if (::DeviceIoControl(m_hDrv , dwIoControlCode , lpInBuffer , nInBufferSize , lpOutBuffer , nOutBufferSize , lpBytesReturned , NULL) == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("与底层驱动通信失败"));
		return FALSE;
	}

	return TRUE;
}

BOOL CDrvObject::StartDrvService()
{
	return FALSE;
}

BOOL CDrvObject::StopDrvService()
{
	return FALSE;
}

BOOL CDrvObject::DelDrvService()
{
	return FALSE;
}

CString CDrvObject::GetDrvFilePath()
{
	CString strDrvFilePath;
	strDrvFilePath.Format(_T("%s%s"), DRV_FILE_PATH, GetDrvFileName());
	return strDrvFilePath;
}
