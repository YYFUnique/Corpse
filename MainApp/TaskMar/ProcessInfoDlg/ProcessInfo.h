#pragma once
#include "../TaskMarDoc.h"
#include "../TaskMarView.h"
#include "../Resource.h"

#include "LsCommon/CommonFunc.h"
#include "LsCommon/FileTools.h"
#include "LsCommon/TextTools.h"
#include "LsCommon/Class/Process.h"
#include <Shlwapi.h>

#define		PROCESS_DETAIL_INFO			0	//��ϸ��Ϣ
#define		PROCESS_MODULE					1  //ģ��
#define       PROCESS_WINDOWS				2  //����
#define		PROCESS_MEMROY				3	//�ڴ�
#define		PROCESS_HANDLE					4  //���
#define		PROCESS_THREAD					5  //�߳�
#define		PROCESS_SERVICE					6  //����
#define		PROCESS_STARTTYPE				7	//������ʽ

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

