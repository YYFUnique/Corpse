#pragma once

class DRV_API CDrvObject
{
public:
	CDrvObject();
	~CDrvObject();
public:
	virtual BOOL StartDrvService();
	virtual BOOL StopDrvService();
	virtual BOOL DelDrvService();
	virtual LPCTSTR GetDrvFileName() = 0;
	virtual CString GetDrvFilePath();
public:
	BOOL OpenDrv();
	// 可能出现异常现象，比如驱动服务停止后，重新启动，那么将出现这里返回TRUE，但是不能和驱动正常通讯
	BOOL IsOpen();
	void CloseDrv();
	BOOL DeviceIoControl(DWORD dwIoControlCode , LPVOID lpInBuffer , DWORD nInBufferSize , LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned);
protected:
	HANDLE	m_hDrv;
	CString		m_strLinkName;		// 应用层与底层驱动的连接符号
};