#pragma once

//�ƻ����������
#include <Taskschd.h>
#include <atlstr.h>
typedef struct tagSCHEDULED_TASK_ACTION_ITEM
{
	TASK_ACTION_TYPE Type;
	CString strAction;
}SCHEDULED_TASK_ACTION_ITEM,*PSCHEDULED_TASK_ACTION_ITEM;

//Vista��ϵͳ�ṩ���׽ӿ�,��Schedule��������ʱ�������ݸ��Զ���,��Schedule����ֹͣʱ���������ַ���һ��
typedef enum tagSCHEDULED_TASK_VERSION
{
	LS_SCHEDULED_TASK_VERSION1 = 1,
	LS_SCHEDULED_TASK_VERSION2 = 2,
}SCHEDULED_TASK_VERSION,*PSCHEDULED_TASK_VERSION;

//�ƻ���������Ϣ
typedef struct tagSCHEDULED_TASK_INFO
{
	CString strTaskName;//������,��Adobe Flash Player Updater
	CString strTaskInfoPath;//������Ϣ·��,V1=XXX.job,V2=\xxx
	BOOL bEnable;//�Ƿ�����
	DWORD dwVersionFlag;//�ӿڰ汾��־,��ӦLS_SCHEDULED_TASK_VERSION
	DWORD dwActionCount;
	SCHEDULED_TASK_ACTION_ITEM ActionItems[10];
}SCHEDULED_TASK_INFO,  *PSCHEDULED_TASK_INFO;

typedef CDuiList<SCHEDULED_TASK_INFO,const SCHEDULED_TASK_INFO&> CScheduledTaskInfoList;

class CTaskScheduled : public CBase
{
public:
	CTaskScheduled();
	~CTaskScheduled();
public:
	virtual void OnPaint();

	//ö�ٵ�ǰϵͳ�ƻ�������Ϣ
	BOOL EnumScheduledTaskInfo(CScheduledTaskInfoList& TaskInfoList);
protected:
	//ö�ٵ�ǰϵͳ�ƻ�������Ϣ(XP/2003�ӿڷ�ʽ)
	BOOL EnumScheduledTaskInfoV1(CScheduledTaskInfoList& TaskInfoList);

	////������������ɾ��ָ���ļƻ�����(XP/2003�ӿڷ�ʽ)
	//BOOL DeleteScheduledTaskV1(const SCHEDULED_TASK_INFO& ScheduledTask);

	////����ָ���ļƻ�����(XP/2003�ӿڷ�ʽ)
	//BOOL DisableScheduledTaskV1(const SCHEDULED_TASK_INFO& ScheduledTask);

	//ö�ٵ�ǰϵͳ�ƻ�������Ϣ(Vista�ӿڷ�ʽ)
	BOOL EnumScheduledTaskInfoV2(CScheduledTaskInfoList& TaskInfoList);

	BOOL GetTaskActions(IRegisteredTask* pRegisteredTask , SCHEDULED_TASK_INFO& TaskInfo);
};