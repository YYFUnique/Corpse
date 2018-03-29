#pragma once

#include "Mail/SMailer.h"
#include "Mail/WinSockHelper.h"

typedef enum tagMAIL_PRIORITY
{
	MAIL_PRIORITY_IMPORTANT		= 0,	//	��Ҫ
	MAIL_PRIORITY_NORMAIL			= 1,	//	һ��
	MAIL_PRIORITY_TRIVIAL				=	2,	//	����Ҫ
}MAIL_PRIORITY;

typedef enum tagMAIL_CONTENT_TYPE
{
	MAIL_CONTENT_TYPE_TEXT		= 0,	//	�ı�����
	MAIL_CONTENT_TYPE_HTML		=	1,	//	HTML����
	MAIL_CONTENT_TYPE_STREAM = 2,	// �ļ����ݣ�������
}MAIL_CONTENT_TYPE;

class CMailHelper
{
public:
	CMailHelper();
	~CMailHelper();

public:
	BOOL SendMail();

	void SetServerName(LPCTSTR lpszServerName);
	void MailSenderLogin(LPCTSTR lpszUserName, LPCTSTR lpszUserPwd);

	void SetSubject(LPCTSTR lpszSubject);
	void SetPriority(MAIL_PRIORITY MailPriority);
	void SetMailContent(MAIL_CONTENT_TYPE MailContent, LPCTSTR lpszContent);
	void SetSenderInfo(LPCTSTR lpszSenderName, LPCTSTR lpszSenderAddress);
	void SetReceiverInfo(LPCTSTR lpszReceiverName, LPCTSTR lpszReceiverAddress);
protected:
	CStringA							m_strServerName;
	CStringA							m_strUserName;
	CStringA							m_strUserPwd;
	SMailer::MailInfo*			m_pMailInfo;
	MUtils::WinSockHelper* m_pWSHelper;
};