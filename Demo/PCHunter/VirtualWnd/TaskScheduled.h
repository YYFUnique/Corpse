#pragma once

//计划任务管理类
#include <Taskschd.h>
#include <atlstr.h>
typedef struct tagSCHEDULED_TASK_ACTION_ITEM
{
	TASK_ACTION_TYPE Type;
	CString strAction;
}SCHEDULED_TASK_ACTION_ITEM,*PSCHEDULED_TASK_ACTION_ITEM;

//Vista后系统提供两套接口,当Schedule服务启动时管理内容各自独立,当Schedule服务停止时两套内容又返回一致
typedef enum tagSCHEDULED_TASK_VERSION
{
	LS_SCHEDULED_TASK_VERSION1 = 1,
	LS_SCHEDULED_TASK_VERSION2 = 2,
}SCHEDULED_TASK_VERSION,*PSCHEDULED_TASK_VERSION;

//计划任务项信息
typedef struct tagSCHEDULED_TASK_INFO
{
	CString strTaskName;//任务名,如Adobe Flash Player Updater
	CString strTaskInfoPath;//任务信息路径,V1=XXX.job,V2=\xxx
	BOOL bEnable;//是否启用
	DWORD dwVersionFlag;//接口版本标志,对应LS_SCHEDULED_TASK_VERSION
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

	//枚举当前系统计划任务信息
	BOOL EnumScheduledTaskInfo(CScheduledTaskInfoList& TaskInfoList);
protected:
	//枚举当前系统计划任务信息(XP/2003接口方式)
	BOOL EnumScheduledTaskInfoV1(CScheduledTaskInfoList& TaskInfoList);

	////按照任务名称删除指定的计划任务(XP/2003接口方式)
	//BOOL DeleteScheduledTaskV1(const SCHEDULED_TASK_INFO& ScheduledTask);

	////禁用指定的计划任务(XP/2003接口方式)
	//BOOL DisableScheduledTaskV1(const SCHEDULED_TASK_INFO& ScheduledTask);

	//枚举当前系统计划任务信息(Vista接口方式)
	BOOL EnumScheduledTaskInfoV2(CScheduledTaskInfoList& TaskInfoList);

	BOOL GetTaskActions(IRegisteredTask* pRegisteredTask , SCHEDULED_TASK_INFO& TaskInfo);
};