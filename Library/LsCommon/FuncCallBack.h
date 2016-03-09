#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

typedef enum
{
	STRING_TEXT = 1,			//字符文本
	NUMBER,							//数字比较
	FILE_TIME,						//FileTime结构体
	FILE_SIZE,							//文件大小
	IP_STRING,						//IP地址
	IP_STRING_PORT,			//IP地址+对应的端口
}LIST_CTRL_COLUMN_FORMAT,*PLIST_CTRL_COLUMN_FORMAT;

typedef struct _LIST_SORT_ITEM_INFO//用于列表框排序
{
	HWND hListWnd;								//当前控件窗口句柄
	int nSortColumnId;								//要排序的列
	BOOL bSortAscending;						//是否升序
	LIST_CTRL_COLUMN_FORMAT nDataType;//排序方式
}LIST_SORT_ITEM_INFO,*PLIST_SORT_ITEM_INFO;

LS_COMMON_API int CALLBACK ListCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

//========================================
//函 数 名:	CompareNum
//功    能:	比较字符串对应的数字大小
//参数说明:lpszFirstNum	第一个字符串
//				 lpszSecondNum 第二个字符串
//返 回 值: 返回大小，如果第一个大返回1，第二个大返回-1，否则返回0
//========================================
LS_COMMON_API int CompareNum(LPCTSTR lpszFirstNum,LPCTSTR lpszSecondNum);

LS_COMMON_API int CompareFileSize(LPCTSTR lpszFirstNum,LPCTSTR lpszSecondNum);

LS_COMMON_API BOOL SerarchTextInList(FINDREPLACE* pFindText,LPCTSTR lpszSerarchFor,CListCtrl& ListCtrl);

LS_COMMON_API BOOL CompareItem(FINDREPLACE* pFindText,LPCTSTR lpszItemText,LPCTSTR lpszSerarchFor);

LS_COMMON_API int GetCompareByNumberic(const LARGE_INTEGER& LarFirst,const LARGE_INTEGER& LarSecond);

LS_COMMON_API BOOL GetFileTime(LPCTSTR strTime,SYSTEMTIME& systime);