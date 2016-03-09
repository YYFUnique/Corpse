#pragma once
#include "afxcmn.h"
#include <vector>

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

class LS_COMMON_API CListHeaderInfo
{
public:
	CListHeaderInfo(const CString& strDescription , DWORD dwPosition = LVCFMT_LEFT, DWORD dwListWidth = 0);
	~CListHeaderInfo();

	CString strColumnDes;
	DWORD dwColumnWidth;
	DWORD dwPosition;
};


class LS_COMMON_API CListCtrlInfo : public CListCtrl
{
public:
	CListCtrlInfo(void);
	~CListCtrlInfo(void);

	/************************************************************************/
	/*函数名称：AddColumn                                                                */
	/*函数功能：向列表框添加列表头字段												*/
	/*输入参数：lpszItem	 用于显示的列表头字段									*/
	/*返  回 值：无																				*/
	/************************************************************************/
	void AddColumn(LPCTSTR lpszItem);
protected:
	//std::vector<CListHeaderInfo> m_ListItemInfo;
};
