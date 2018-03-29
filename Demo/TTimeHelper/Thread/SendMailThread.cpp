#include "StdAfx.h"
#include "DllCore/Thread/LsThread.h"
#include "SendMailThread.h"
#include "DllCore/Utils/Registry.h"

#define THREAD_APP_ID_SEND_MAIL	10	//	发送邮件线程ID

CSendMailThread::CSendMailThread()
{
	m_pMailHelper = NULL;
	m_ThreadAppId = (THREAD_APP_ID)THREAD_APP_ID_SEND_MAIL;
	m_pfnThreadProc = (LPTHREAD_START_ROUTINE)SendMailThreadFun;

	m_pThreadParam = this;
}

CSendMailThread::~CSendMailThread()
{

}

void CSendMailThread::SetMailHelper(CMailHelper* pMailHelper)
{
	m_pMailHelper = pMailHelper;
}

void CSendMailThread::SendMail()
{
	CString strRegPath = GetRegistryPath(_T("TTimeHelper"));
	DWORD dwSend = FALSE;
	LsRegQueryValue(HKEY_LOCAL_MACHINE, strRegPath, _T("bSendMail"), dwSend);
	if (dwSend != FALSE)
		return;

	dwSend = TRUE;
	if (m_pMailHelper->SendMail() != FALSE)
		LsRegSetValue(HKEY_LOCAL_MACHINE, strRegPath, _T("bSendMail"), dwSend);
}

UINT CSendMailThread::SendMailThreadFun(LPVOID lParam)
{
	CSendMailThread* pSendMail = (CSendMailThread*)lParam;
	if (pSendMail == NULL)
		return FALSE;

	while(TRUE)
	{
		if (pSendMail->m_bExitThread == TRUE)
			break;

		DWORD dwRet = WaitForSingleObject(pSendMail->m_NotificationEvent, INFINITE);
		if (dwRet == WAIT_OBJECT_0)
			pSendMail->SendMail();
	}

	return TRUE;
}