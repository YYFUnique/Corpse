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
	STRING_TEXT = 1,			//�ַ��ı�
	NUMBER,							//���ֱȽ�
	FILE_TIME,						//FileTime�ṹ��
	FILE_SIZE,							//�ļ���С
	IP_STRING,						//IP��ַ
	IP_STRING_PORT,			//IP��ַ+��Ӧ�Ķ˿�
}LIST_CTRL_COLUMN_FORMAT,*PLIST_CTRL_COLUMN_FORMAT;

typedef struct _LIST_SORT_ITEM_INFO//�����б������
{
	HWND hListWnd;								//��ǰ�ؼ����ھ��
	int nSortColumnId;								//Ҫ�������
	BOOL bSortAscending;						//�Ƿ�����
	LIST_CTRL_COLUMN_FORMAT nDataType;//����ʽ
}LIST_SORT_ITEM_INFO,*PLIST_SORT_ITEM_INFO;

LS_COMMON_API int CALLBACK ListCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

//========================================
//�� �� ��:	CompareNum
//��    ��:	�Ƚ��ַ�����Ӧ�����ִ�С
//����˵��:lpszFirstNum	��һ���ַ���
//				 lpszSecondNum �ڶ����ַ���
//�� �� ֵ: ���ش�С�������һ���󷵻�1���ڶ����󷵻�-1�����򷵻�0
//========================================
LS_COMMON_API int CompareNum(LPCTSTR lpszFirstNum,LPCTSTR lpszSecondNum);

LS_COMMON_API int CompareFileSize(LPCTSTR lpszFirstNum,LPCTSTR lpszSecondNum);

LS_COMMON_API BOOL SerarchTextInList(FINDREPLACE* pFindText,LPCTSTR lpszSerarchFor,CListCtrl& ListCtrl);

LS_COMMON_API BOOL CompareItem(FINDREPLACE* pFindText,LPCTSTR lpszItemText,LPCTSTR lpszSerarchFor);

LS_COMMON_API int GetCompareByNumberic(const LARGE_INTEGER& LarFirst,const LARGE_INTEGER& LarSecond);

LS_COMMON_API BOOL GetFileTime(LPCTSTR strTime,SYSTEMTIME& systime);