#pragma once

#include "DllCore/Thread/LsThread.h"
#include "../MailHelper.h"

class CSendMailThread : public CLsThread
{
public:
	CSendMailThread();
	~CSendMailThread();

public:
	void SetMailHelper(CMailHelper* pMailHelper);
	static UINT SendMailThreadFun(LPVOID lParam);
protected:
	void SendMail();
private:
	CMailHelper* m_pMailHelper;
};