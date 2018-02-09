#include "StdAfx.h"
#include "../Utils/ErrorInfo.h"
#include "DllCore/Utils/ServiceControl.h"

#include <initguid.h>
#include <mstask.h>
#pragma comment(lib, "taskschd.lib")

CTaskScheduled::CTaskScheduled()
{

}

CTaskScheduled::~CTaskScheduled()
{

}

void CTaskScheduled::OnPaint()
{
	CListUI* pTaskInfo = (CListUI*)m_pPaintManager->FindControl(_T("Job"));
	if (pTaskInfo == NULL)
		return;
	
	pTaskInfo->RemoveAll();

	CScheduledTaskInfoList TaskScheduledInfo;
	if (EnumScheduledTaskInfo(TaskScheduledInfo) == FALSE)
		return;

	POSITION pos = TaskScheduledInfo.GetHeadPosition();
	while(pos)
	{
		const SCHEDULED_TASK_INFO& TaskInfo = TaskScheduledInfo.GetNext(pos);
		CListTextElementUI* pListElementUI = new CListTextElementUI;
		pTaskInfo->Add(pListElementUI);

		pListElementUI->SetText(0,TaskInfo.strTaskName);
		pListElementUI->SetText(1,TaskInfo.strTaskInfoPath);
		//pListElementUI->SetText(2,TaskInfo.)
	}
}

BOOL CTaskScheduled::EnumScheduledTaskInfo(CScheduledTaskInfoList& TaskInfoList)
{
	BOOL bSuccess = FALSE;
	TaskInfoList.RemoveAll();

	//if (OsIsVistaOrLater() && IsServiceRunning(_T("Schedule")))
		bSuccess = EnumScheduledTaskInfoV2(TaskInfoList);

	EnumScheduledTaskInfoV1(TaskInfoList);

	return TaskInfoList.IsEmpty() == FALSE;
}

BOOL CTaskScheduled::EnumScheduledTaskInfoV1(CScheduledTaskInfoList& TaskInfoList)
{
	BOOL bSuccess = FALSE;
	ITaskScheduler *pITS = NULL;
	IEnumWorkItems *pIEnum = NULL;

	do 
	{
		HRESULT hRet = CoCreateInstance(CLSID_CTaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskScheduler, (void **)&pITS);
		if (FAILED(hRet))
		{
			SetErrorInfo(COM_ERROR , hRet , _T("枚举计划任务时创建管理对象失败"));
			break;
		}

		hRet = pITS->Enum(&pIEnum);
		if (FAILED(hRet))
		{
			SetErrorInfo(COM_ERROR , hRet , _T("枚举计划任务时创建枚举对象失败"));
			break;
		}

		LPWSTR *lpwszNames;
		DWORD dwFetchedTasks = 0;
		while (SUCCEEDED(pIEnum->Next(100 , &lpwszNames , &dwFetchedTasks)))
		{
			if (dwFetchedTasks == 0)
				break;

			for(DWORD i=0;i<dwFetchedTasks;i++)
			{
				SCHEDULED_TASK_INFO TaskInfo;
				TaskInfo.strTaskInfoPath = lpwszNames[i];
				TaskInfo.dwVersionFlag = LS_SCHEDULED_TASK_VERSION1;

				PathRemoveExtensionW(lpwszNames[i]);
				TaskInfo.strTaskName = lpwszNames[i];

				/*int nIndex = FindTaskByName(TaskInfoList , TaskInfo.strTaskName);
				if (nIndex != -1)
				{
					SCHEDULED_TASK_INFO& TaskInfo = TaskInfoList.GetAt(nIndex);
					TaskInfo.dwVersionFlag |= LS_SCHEDULED_TASK_VERSION1;
					continue;
				}*/

				ITask *pITask;
				hRet = pITS->Activate(lpwszNames[i] , IID_ITask,(IUnknown**) &pITask);
				if (FAILED(hRet))
					break;

				TaskInfo.bEnable = FALSE;
				DWORD dwTaskInfo;
				if (pITask->GetFlags(&dwTaskInfo) == S_OK)
					TaskInfo.bEnable = (dwTaskInfo & TASK_FLAG_DISABLED) == 0;

				TaskInfo.dwActionCount = 1;
				TaskInfo.ActionItems[0].Type = TASK_ACTION_EXEC;
				LPWSTR pwszTaskInfo;
				if (pITask->GetApplicationName(&pwszTaskInfo) == S_OK)
				{
					TaskInfo.ActionItems[0].strAction = pwszTaskInfo;
					CoTaskMemFree(pwszTaskInfo);
				}

				if (pITask->GetParameters(&pwszTaskInfo) == S_OK)
				{
					if (wcslen(pwszTaskInfo))
					{
						TaskInfo.ActionItems[0].strAction += _T(" ");
						TaskInfo.ActionItems[0].strAction += pwszTaskInfo;
					}
					CoTaskMemFree(pwszTaskInfo);
				}
				TaskInfo.ActionItems[0].strAction.TrimRight(' ');

				TaskInfoList.AddTail(TaskInfo);

				pITask->Release();
			}
			CoTaskMemFree(lpwszNames);
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (pIEnum != NULL)
	{
		pIEnum->Release();
		pIEnum = NULL;
	}
	
	if (pITS != NULL)
	{
		pITS->Release();
		pITS = NULL;
	}

	return bSuccess;
}

BOOL CTaskScheduled::EnumScheduledTaskInfoV2(CScheduledTaskInfoList& TaskInfoList)
{
	ITaskService *pService = NULL;
	ITaskFolder *pRootFolder = NULL;
	IRegisteredTaskCollection* pTaskCollection = NULL;
	BOOL bSuccess = FALSE;

	do 
	{
		HRESULT hRet = CoCreateInstance(CLSID_TaskScheduler,NULL,CLSCTX_INPROC_SERVER,IID_ITaskService,(void**)&pService);
		if (FAILED(hRet))
		{
			SetErrorInfo(COM_ERROR , hRet , _T("枚举计划任务时创建管理对象失败"));
			break;
		}

		hRet = pService->Connect(_variant_t() , _variant_t() , _variant_t() , _variant_t());
		if (FAILED(hRet))
		{
			SetErrorInfo(COM_ERROR , hRet , _T("枚举计划任务时连接管理对象失败"));
			break;
		}

		hRet = pService->GetFolder(_bstr_t( L"\\") , &pRootFolder);
		if (FAILED(hRet))
		{
			SetErrorInfo(COM_ERROR , hRet , _T("枚举计划任务时连接根目录失败"));
			break;
		}

		hRet = pRootFolder->GetTasks(NULL, &pTaskCollection);
		if (FAILED(hRet))
		{
			SetErrorInfo(COM_ERROR , hRet , _T("枚举计划任务根目录失败"));
			break;
		}

		LONG nTaskCount = 0;
		pTaskCollection->get_Count(&nTaskCount);

		for(LONG i=0;i < nTaskCount; i++)
		{
			IRegisteredTask* pRegisteredTask = NULL;
			hRet = pTaskCollection->get_Item(_variant_t(i+1), &pRegisteredTask);
			if (FAILED(hRet))
				continue;

			SCHEDULED_TASK_INFO TaskInfo;
			TaskInfo.dwActionCount = 0;
			TaskInfo.dwVersionFlag = LS_SCHEDULED_TASK_VERSION2;
			BSTR bstrTaskInfo = NULL;

			hRet = pRegisteredTask->get_Name(&bstrTaskInfo);
			if (SUCCEEDED(hRet))
			{
				TaskInfo.strTaskName = bstrTaskInfo;
				SysFreeString(bstrTaskInfo);
			}

			hRet = pRegisteredTask->get_Path(&bstrTaskInfo);
			if (SUCCEEDED(hRet))
			{
				TaskInfo.strTaskInfoPath = bstrTaskInfo;
				SysFreeString(bstrTaskInfo);
			}

			VARIANT_BOOL bTaskEnable;
			hRet = pRegisteredTask->get_Enabled(&bTaskEnable);
			TaskInfo.bEnable = bTaskEnable;

			GetTaskActions(pRegisteredTask , TaskInfo);

			TaskInfoList.AddTail(TaskInfo);
			pRegisteredTask->Release();
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (pTaskCollection != NULL)
	{
		pTaskCollection->Release();
		pTaskCollection = NULL;
	}

	if (pRootFolder != NULL)
	{
		pRootFolder->Release();
		pRootFolder = NULL;
	}

	if (pService != NULL)
	{
		pService->Release();
		pService = NULL;
	}

	return bSuccess;
}

BOOL CTaskScheduled::GetTaskActions(IRegisteredTask* pRegisteredTask , SCHEDULED_TASK_INFO& TaskInfo)
{
	BOOL bSuccess = FALSE;

	ITaskDefinition* pTaskDefinition = NULL;
	IActionCollection* pActions = NULL;

	do 
	{
		TaskInfo.dwActionCount = 0;

		HRESULT hRet = pRegisteredTask->get_Definition(&pTaskDefinition);
		if (FAILED(hRet))
		{
			SetErrorInfo(COM_ERROR , hRet , _T("获取计划任务定义信息失败"));
			break;
		}

		hRet = pTaskDefinition->get_Actions(&pActions);
		if (FAILED(hRet))
		{
			SetErrorInfo(COM_ERROR , hRet , _T("获取计划任务操作信息集合失败"));
			break;
		}

		LONG nActionCount = 0;
		pActions->get_Count(&nActionCount);

		IAction* pAction = NULL;
		for(LONG i=0;i<nActionCount;i++)
		{
			if (pAction)
			{
				pAction->Release();
				pAction = NULL;
			}
			hRet = pActions->get_Item(_variant_t(i+1), &pAction);
			if (FAILED(hRet))
			{
				SetErrorInfo(COM_ERROR , hRet , _T("获取计划任务操作信息项失败"));
				continue;
			}

			SCHEDULED_TASK_ACTION_ITEM& ActionItem = TaskInfo.ActionItems[TaskInfo.dwActionCount];

			pAction->get_Type(&ActionItem.Type);
			if (FAILED(hRet))
			{
				SetErrorInfo(COM_ERROR , hRet , _T("获取计划任务操作类型失败"));
				continue;
			}

			BSTR bstrActionInfo;
			if (ActionItem.Type == TASK_ACTION_EXEC)
			{	
				IExecAction *pExecAction = NULL;
				hRet = pAction->QueryInterface(IID_IExecAction, (void**) &pExecAction);
				if (FAILED(hRet))
				{
					SetErrorInfo(COM_ERROR , hRet , _T("获取计划任务操作对象失败"));
					continue;
				}

				hRet = pExecAction->get_Path(&bstrActionInfo);
				if (SUCCEEDED(hRet))
				{
					ActionItem.strAction = bstrActionInfo;
					SysFreeString(bstrActionInfo);
				}

				hRet = pExecAction->get_Arguments(&bstrActionInfo);
				if (SUCCEEDED(hRet))
				{
					ActionItem.strAction += _T(" ");
					ActionItem.strAction += bstrActionInfo;
					SysFreeString(bstrActionInfo);
				}
				pExecAction->Release();
			}
			else if (ActionItem.Type == TASK_ACTION_SEND_EMAIL)
			{
				IEmailAction *pEmailAction = NULL;
				hRet = pAction->QueryInterface(IID_IEmailAction, (void**) &pEmailAction);
				if (FAILED(hRet))
				{
					SetErrorInfo(COM_ERROR , hRet , _T("获取计划任务操作对象失败"));
					continue;
				}

				hRet = pEmailAction->get_To(&bstrActionInfo);
				if (SUCCEEDED(hRet))
				{
					ActionItem.strAction = _T("收件地址:");
					ActionItem.strAction += bstrActionInfo;
					SysFreeString(bstrActionInfo);
				}

				hRet = pEmailAction->get_Subject(&bstrActionInfo);
				if (SUCCEEDED(hRet))
				{
					ActionItem.strAction += _T(",邮件主题:");
					ActionItem.strAction += bstrActionInfo;
					SysFreeString(bstrActionInfo);
				}
				pEmailAction->Release();
			}
			else if (ActionItem.Type == TASK_ACTION_SHOW_MESSAGE)
			{
				IShowMessageAction *pShowMessageAction = NULL;
				hRet = pAction->QueryInterface(IID_IShowMessageAction, (void**) &pShowMessageAction);
				if (FAILED(hRet))
				{
					SetErrorInfo(COM_ERROR , hRet , _T("获取计划任务操作对象失败"));
					continue;
				}

				hRet = pShowMessageAction->get_Title(&bstrActionInfo);
				if (SUCCEEDED(hRet))
				{
					ActionItem.strAction = _T("标题:");
					ActionItem.strAction += bstrActionInfo;
					SysFreeString(bstrActionInfo);
				}

				hRet = pShowMessageAction->get_MessageBody(&bstrActionInfo);
				if (SUCCEEDED(hRet))
				{
					ActionItem.strAction += _T(",文字:");
					ActionItem.strAction += bstrActionInfo;
					SysFreeString(bstrActionInfo);
				}

				pShowMessageAction->Release();
			}
			else if (ActionItem.Type == TASK_ACTION_COM_HANDLER)
			{
				ActionItem.strAction = _T("...");
			}

			ActionItem.strAction.TrimRight(' ');
			if (ActionItem.strAction.IsEmpty() == FALSE)
			{
				TaskInfo.dwActionCount++;
				if (TaskInfo.dwActionCount == _countof(TaskInfo.ActionItems))
					break;
			}
		}
		if (pAction)
		{
			pAction->Release();
			pAction = NULL;
		}
		bSuccess = TRUE;

	} while (0);

	if (pTaskDefinition != NULL)
	{
		pTaskDefinition->Release();
		pTaskDefinition = NULL;
	}

	if (pActions != NULL)
	{
		pActions->Release();
		pActions = NULL;
	}

	return bSuccess;
}