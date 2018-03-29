#pragma once

#include "LsThread.h"

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

//template class __declspec(dllexport) CLsList<CLsThread*,CLsThread*>;

//��CLsThread��Ĺ���
typedef CDuiList<CLsThread* , CLsThread*> CLsThreadList;

class DLL_API CLsThreadMgr
{
public:
	CLsThreadMgr(void);

	~CLsThreadMgr(void);

	//��������ĳ�ʼ������
	BOOL Initialize();

	//����һ���̵߳�����
	BOOL AddThreadToList(CLsThread* pLsThread);

	//������ɾ��һ���߳�(ֻ�Ǵ�������ժ����������ֹͣ�̼߳�������Դ)
	BOOL RemoveThreadFromList(CLsThread* pLsThread);

	//������ɾ��һ���߳�(��������ժ������ֹͣ�̼߳�������Դ)
	BOOL RemoveThreadFromList(THREAD_APP_ID LsThreadAppId , DWORD dwWaitTime = 2000);

	//ɾ�������߳�(ֹͣ�����������̶߳�����Դ)
	BOOL DeleteAllThread();

	//�����߳�ID�����߳�
	CLsThread* FindThreadById(THREAD_APP_ID LsThreadAppId , BOOL bRemoveFromList);

	//���ع�������Ƿ���ɳ�ʼ��
	BOOL IsInitComplete();

protected:
	//static BOOL CjThreadProc(LPVOID lParam);
protected:

	CLsThreadList m_LsThreadList;

	//�ٽ�����ʶ
	CRITICAL_SECTION m_CriticalSection;

	//�Ƿ���ɳ�ʼ��
	BOOL m_bInitComplete;
};

DLL_API CLsThreadMgr& GetLsThreadMgr();