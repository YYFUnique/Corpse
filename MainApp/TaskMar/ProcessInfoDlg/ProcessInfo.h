#pragma once
#include "../TaskMarDoc.h"
#include "../TaskMarView.h"
#include "../Resource.h"

#include "LsCommon/CommonFunc.h"
#include "LsCommon/FileTools.h"
#include "LsCommon/TextTools.h"
#include "LsCommon/Class/Process.h"
#include <Shlwapi.h>

#define		PROCESS_DETAIL_INFO			0	//详细信息
#define		PROCESS_MODULE					1  //模块
#define       PROCESS_WINDOWS				2  //窗口
#define		PROCESS_MEMROY				3	//内存
#define		PROCESS_HANDLE					4  //句柄
#define		PROCESS_THREAD					5  //线程
#define		PROCESS_SERVICE					6  //服务
#define		PROCESS_STARTTYPE				7	//启动方式

class CProcessInfo
{
public:
	CProcessInfo();
	virtual ~CProcessInfo();
	UINT GetTabCtrlID();
	virtual BOOL GetListColumn(CStringArray& strListColumn) = 0;
	static CProcessInfo* Create(DWORD dwListType);
	virtual BOOL ShowProcessInfo(CListCtrl& ListCtrl) = 0;
	CView* GetProcessListView();
	CImageList* GetImageListPtr();
	BOOL IsShowImageList();
	UINT GetSubMenu();
	virtual BOOL OnRButtonDown(UINT nID) = 0;
protected:
	UINT m_TabCtrlID;
	BOOL m_bIsAddList;
	CImageList m_ModuleImg;
	BOOL bShowImageList;
	UINT m_nSubMenuID;
};

