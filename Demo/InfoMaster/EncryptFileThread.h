#pragma once

#include "DllCore/Thread/LsThread.h"

class CEncryptFile : public CLsThread
{
public:
	CEncryptFile();
	~CEncryptFile();

	BOOL StopThread(DWORD dwWaitTime , BOOL bTerminateThreadWhenWaitTimeout);
public:
	//Ĭ���̺߳���
	static UINT workThread(LPVOID lParam);
};