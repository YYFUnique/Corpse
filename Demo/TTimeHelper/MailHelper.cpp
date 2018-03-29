#include "StdAfx.h"
#include "MailHelper.h"
#include "DllCore/Log/LogHelper.h"
CMailHelper::CMailHelper()
{
	m_pMailInfo = new SMailer::MailInfo;
	m_pWSHelper = new MUtils::WinSockHelper;
}

CMailHelper::~CMailHelper()
{
	if (m_pMailInfo != NULL)
	{
		delete m_pMailInfo;
		m_pMailInfo = NULL;
	}

	if (m_pWSHelper != NULL)
	{
		delete m_pWSHelper;
		m_pWSHelper = NULL;
	}
}

void CMailHelper::SetSubject(LPCTSTR lpszSubject)
{
	if (m_pMailInfo != NULL)
		m_pMailInfo->setSubject((LPCSTR)CStringA(lpszSubject));
}

void CMailHelper::SetPriority(MAIL_PRIORITY MailPriority)
{
	std::string strPriority;
	if (MailPriority == MAIL_PRIORITY_IMPORTANT)
		strPriority = SMailer::Priority::important;
	else if (MailPriority == MAIL_PRIORITY_NORMAIL)
		strPriority = SMailer::Priority::normal;
	else if (MailPriority == MAIL_PRIORITY_TRIVIAL)
		strPriority = SMailer::Priority::trivial;

	if (m_pMailInfo)
		m_pMailInfo->setPriority(strPriority);
}

void CMailHelper::SetMailContent(MAIL_CONTENT_TYPE MailContent, LPCTSTR lpszContent)
{
	if (m_pMailInfo == NULL)
		return;

	switch (MailContent)
	{
	case MAIL_CONTENT_TYPE_TEXT:
		{
			SMailer::TextPlainContent* pTextPlain = new SMailer::TextPlainContent((LPCSTR)CStringA(lpszContent));
			m_pMailInfo->addMimeContent(pTextPlain);
		}
		break;
	case MAIL_CONTENT_TYPE_HTML:
		{
			SMailer::TextHtmlContent*	pHtmlPlain = new SMailer::TextHtmlContent((LPCSTR)CStringA(lpszContent));
			m_pMailInfo->addMimeContent(pHtmlPlain);
		}
		break;
	case MAIL_CONTENT_TYPE_STREAM:
		{
			SMailer::AppOctStrmContent* pOctStrm = new SMailer::AppOctStrmContent((LPCSTR)CStringA(lpszContent));
			m_pMailInfo->addMimeContent(pOctStrm);
		}
		break;
	}
}

void CMailHelper::SetSenderInfo(LPCTSTR lpszSenderName, LPCTSTR lpszSenderAddress)
{
	if (m_pMailInfo)
	{
		m_pMailInfo->setSenderName((LPCSTR)CStringA(lpszSenderName));
		m_pMailInfo->setSenderAddress((LPCSTR)CStringA(lpszSenderAddress));
	}
}

void CMailHelper::SetReceiverInfo(LPCTSTR lpszReceiverName, LPCTSTR lpszReceiverAddress)
{
	if (m_pMailInfo)
		m_pMailInfo->setReceiver((LPCSTR)CStringA(lpszReceiverName), (LPCSTR)CStringA(lpszReceiverAddress));
}

BOOL CMailHelper::SendMail()
{
	BOOL bSuccess = FALSE;

	do 
	{
		if (m_pMailInfo == NULL)
			break;
		if (m_strServerName.IsEmpty())
			break;
		if (m_strUserName.IsEmpty())
			break;
		if (m_strUserPwd.IsEmpty())
			break;

		bSuccess = TRUE;
	} while (FALSE);

	//	如果经过了do...while(false) bSuccess还是FALSE说明，上面参数未初始化完成
	if (bSuccess == FALSE)
		return bSuccess;

	try
	{
		//	重置标识参数为FALSE
		bSuccess = FALSE;

		SMailer::MailSender MailSender((LPCSTR)m_strServerName, (LPCSTR)m_strUserName, (LPCSTR)m_strUserPwd);
		MailSender.setMail(&SMailer::MailWrapper(m_pMailInfo));
		MailSender.sendMail();

		bSuccess = TRUE;
	}
	catch (SMailer::MailException& e)
	{
		CString strMailException(e.what());
		QLOG_INFO(strMailException);
	}

	return bSuccess;
}

void CMailHelper::SetServerName(LPCTSTR lpszServerName)
{
	m_strServerName = CStringA(lpszServerName);
}

void CMailHelper::MailSenderLogin(LPCTSTR lpszUserName, LPCTSTR lpszUserPwd)
{
	m_strUserName = CStringA(lpszUserName);
	m_strUserPwd = CStringA(lpszUserPwd);
}
