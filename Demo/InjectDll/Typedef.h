#pragma once
#include "DllCore/Utils/DuiList.h"
typedef struct tagPROCESS_INFO
{
	UINT	 nProcessId;
	CString strProcessName;
}PROCESS_INFO, *PPROCESS_INFO;

typedef CDuiList<PROCESS_INFO, const PROCESS_INFO&> CProcessInfoList;