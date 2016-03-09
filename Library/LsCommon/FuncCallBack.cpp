#include "stdafx.h"
#include "FuncCallBack.h"
#include "ErrorInfo.h"
#include "TextTools.h"
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

int CALLBACK ListCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	PLIST_SORT_ITEM_INFO pListSortInfo = (PLIST_SORT_ITEM_INFO) lParamSort;
	CListCtrl* pListCtrl = (CListCtrl*)CWnd::FromHandle(pListSortInfo->hListWnd);
	ASSERT(pListCtrl);

	int iSubItem = pListSortInfo->nSortColumnId;
	
	CString strItemFirst,strItemSecond;

	strItemFirst = pListCtrl->GetItemText(lParam1,iSubItem);
	strItemSecond = pListCtrl->GetItemText(lParam2,iSubItem);

	int nAscOrder;
	nAscOrder = pListSortInfo->bSortAscending == FALSE ? -1 : 1 ;
	int nRet = 0;
	LARGE_INTEGER LarFirst,LarSecond;
	switch (pListSortInfo->nDataType)
	{
	case STRING_TEXT:
			nRet = strItemFirst.CompareNoCase(strItemSecond);
		break;
	case NUMBER:
			nRet = CompareNum(strItemFirst,strItemSecond);
		break;
	case FILE_TIME:
		SYSTEMTIME SystemTime;

		FILETIME FileTimePrev,FileTimeNext;
		memset(&FileTimePrev,0,sizeof(FileTimePrev));
		memset(&FileTimeNext,0,sizeof(FileTimeNext));
		if (strItemFirst.IsEmpty() == FALSE)
		{
			GetFileTime(strItemFirst,SystemTime);
			SystemTimeToFileTime(&SystemTime,&FileTimePrev);
		}

		if (strItemSecond.IsEmpty() == FALSE)
		{
			GetFileTime(strItemSecond,SystemTime);
			SystemTimeToFileTime(&SystemTime,&FileTimeNext);
		}

		LarFirst.LowPart = FileTimePrev.dwLowDateTime;
		LarFirst.HighPart = FileTimePrev.dwHighDateTime;

		LarSecond.LowPart =FileTimeNext.dwLowDateTime;
		LarSecond.HighPart = FileTimeNext.dwHighDateTime;

			nRet = GetCompareByNumberic(LarFirst,LarSecond);
		break;
	case FILE_SIZE:
			nRet = CompareFileSize(strItemFirst,strItemSecond);
		break;
	case IP_STRING:
		break;
	case IP_STRING_PORT:
		break;
	default:
		nRet = -1;
	}
	return nRet  * nAscOrder;
}

int CompareNum(LPCTSTR lpszFirstNum,LPCTSTR lpszSecondNum)
{
	ASSERT_STR_NOT_NULL(lpszFirstNum);
	ASSERT_STR_NOT_NULL(lpszSecondNum);

	LARGE_INTEGER LarFirstNum,LarSecondNum;
	_stscanf_s(lpszFirstNum,_T("%IA64u"),&LarFirstNum.QuadPart,sizeof(LARGE_INTEGER));
	_stscanf_s(lpszSecondNum,_T("%IA64u"),&LarSecondNum.QuadPart,sizeof(LARGE_INTEGER));

	if (LarFirstNum.QuadPart-LarSecondNum.QuadPart > 0)
		return 1;
	else if (LarFirstNum.QuadPart-LarSecondNum.QuadPart == 0)
		return 0;
	else
		return -1;
}

int CompareFileSize(LPCTSTR lpszFirstNum,LPCTSTR lpszSecondNum)
{
	ASSERT_STR_NOT_NULL(lpszFirstNum);
	ASSERT_STR_NOT_NULL(lpszSecondNum);

	LARGE_INTEGER LarFirstNum,LarSecondNum;

	CString strFileSize(lpszFirstNum);
	strFileSize.Remove(_T(','));
	LarFirstNum.QuadPart =ConvertStringSizeToNum(strFileSize);
	strFileSize = lpszSecondNum;
	strFileSize.Remove(_T(','));
	LarSecondNum.QuadPart = ConvertStringSizeToNum(strFileSize);

	if (LarFirstNum.QuadPart-LarSecondNum.QuadPart > 0)
		return 1;
	else if (LarFirstNum.QuadPart-LarSecondNum.QuadPart == 0)
		return 0;
	else
		return -1;
}

BOOL SerarchTextInList(FINDREPLACE* pFindReplace,LPCTSTR lpszSerarchFor,CListCtrl& ListCtrl)
{
	ASSERT(pFindReplace);
	static UINT nRol=ListCtrl.GetSelectionMark(),nCol=0;
	if (pFindReplace->Flags & FR_DIALOGTERM)
	{
		//关闭时，将行列初始化为0
		nRol = nCol =0;
		return FALSE;
	}

	if (nRol == -1)
		nRol = 0;

	if(pFindReplace->Flags & FR_FINDNEXT)	
	{
		UINT nState = ListCtrl.GetItemState(nRol,LVIS_SELECTED);
		ListCtrl.SetItemState(nRol,nState & ~LVIS_SELECTED,LVIS_SELECTED);
		UINT nTotalRol = 0;
		if (pFindReplace->Flags & FR_DOWN)
			nTotalRol = ListCtrl.GetItemCount();

		UINT nTotalCol =0;
		CHeaderCtrl* pHeader = ListCtrl.GetHeaderCtrl();
		if (pHeader != NULL)
			nTotalCol = ListCtrl.GetHeaderCtrl()->GetItemCount();
		
		CString strItemText;

		while(nRol != nTotalRol)
		{
			while(nCol<nTotalCol)
			{
				strItemText = ListCtrl.GetItemText(nRol,nCol);
				++nCol;
				if (CompareItem(pFindReplace,strItemText,lpszSerarchFor) != FALSE)
				{
					ListCtrl.SetItemState(nRol,LVIS_SELECTED,LVIS_SELECTED);
					ListCtrl.EnsureVisible(nRol,FALSE);
					return TRUE;
				}
			}
			nCol = 0;

			if (pFindReplace->Flags & FR_DOWN)
				++nRol;
			else 
				--nRol;
		}
		//在列表中没有找到对应的项
		nRol = 0;
		nCol = 0;
	}

	return FALSE;
}

BOOL CompareItem(FINDREPLACE* pFindText,LPCTSTR lpszItemText,LPCTSTR lpszSerarchFor)
{
	BOOL bSuccess = FALSE;
	if (pFindText->Flags & FR_MATCHCASE && pFindText->Flags & FR_WHOLEWORD)
		bSuccess = _tcscmp(lpszItemText,lpszSerarchFor) == 0;
	else if (pFindText->Flags & FR_WHOLEWORD)
		bSuccess = _tcsicmp(lpszItemText,lpszSerarchFor) == 0;
	else if (pFindText->Flags & FR_MATCHCASE)
		bSuccess = StrStr(lpszItemText,lpszSerarchFor) != NULL;
	else
		bSuccess = StrStrI(lpszItemText,lpszSerarchFor) != NULL;

	return bSuccess;
}

//比较两个数的大小
int GetCompareByNumberic(const LARGE_INTEGER& LarFirst,const LARGE_INTEGER& LarSecond)
{
	if (LarFirst.QuadPart-LarSecond.QuadPart > 0)
		return 1;
	else if (LarFirst.QuadPart-LarSecond.QuadPart == 0)
		return 0;
	else
		return -1;
}

BOOL GetFileTime(LPCTSTR strTime,SYSTEMTIME& systime)
{
	ZeroMemory(&systime,sizeof(systime));
	_stscanf_s(strTime,_T("%4d/%2d/%2d %2d:%2d:%2d"),&systime.wYear,&systime.wMonth,&systime.wDay,&systime.wHour,&systime.wMinute,&systime.wSecond);
	return TRUE;
}