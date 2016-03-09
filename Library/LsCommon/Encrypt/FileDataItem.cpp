// FileDataItem.cpp : implementation file
//

#include "stdafx.h"
#include "FileDataItem.h"
#include "..\ErrorInfo.h"
#include "..\DebugInfo.h"
#include "..\RegClass.h"
#include "..\FileTools.h"

// #ifdef _DEBUG
// #define new DEBUG_NEW
// #undef THIS_FILE
// static char THIS_FILE[] = __FILE__;
// #endif


CFileDataItem::CFileDataItem()
{
}

CFileDataItem::~CFileDataItem()
{
	ClearAllDataItem();
}

void CFileDataItem::ClearAllDataItem()
{
	POSITION pos=m_FileDataItemList.GetHeadPosition();
	while(pos)
	{
		FILE_DATA_ITEM& FileDataItem=m_FileDataItemList.GetNext(pos);
		if (FileDataItem.pDataBuffer && FileDataItem.dwDataLen)
			delete[] FileDataItem.pDataBuffer;
	}
	m_FileDataItemList.RemoveAll();
}

void CFileDataItem::RemoveDataItem(DWORD dwItemType)
{
	POSITION pos=m_FileDataItemList.GetHeadPosition();
	while(pos)
	{
		POSITION curpos=pos;
		FILE_DATA_ITEM& FileDataItem=m_FileDataItemList.GetNext(pos);
		if (FileDataItem.dwItemType!=dwItemType)
			continue;

		if (FileDataItem.pDataBuffer && FileDataItem.dwDataLen)
			delete[] FileDataItem.pDataBuffer;
		m_FileDataItemList.RemoveAt(curpos);
		break;
	}
}

DWORD CFileDataItem::GetFileItemCount()
{
	return (DWORD)m_FileDataItemList.GetCount();
}

BOOL CFileDataItem::SetDataItem(DWORD dwItemType,DWORD dwValue)
{
	return CFileDataItem::SetDataItem(dwItemType,sizeof(dwValue),&dwValue);
}

BOOL CFileDataItem::SetDataItem(DWORD dwItemType,DWORD dwDataLen,const void* pDataBuffer)
{
	POSITION pos=m_FileDataItemList.GetHeadPosition();
	while(pos)
	{
		FILE_DATA_ITEM& FileDataItem=m_FileDataItemList.GetNext(pos);

		if (dwItemType>FileDataItem.dwItemType)
			continue;
		if (FileDataItem.dwItemType==dwItemType)
		{
			if (dwDataLen!=FileDataItem.dwDataLen)
			{
				BYTE* pNewBuffer=NULL;
				if (dwDataLen)
				{
					pNewBuffer=new BYTE[dwDataLen];
					if (pNewBuffer==NULL)
					{
						SetErrorInfo(CUSTOM_ERROR,0,_T("1设置文件信息时分配内存(长度=%d)失败!"),dwDataLen);
						OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
						return FALSE;
					}
				}

				if (FileDataItem.pDataBuffer && FileDataItem.dwDataLen)
					delete[] FileDataItem.pDataBuffer;
				FileDataItem.pDataBuffer=pNewBuffer;
			}
			FileDataItem.dwDataLen=dwDataLen;
			if (dwDataLen)
				memcpy(FileDataItem.pDataBuffer,pDataBuffer,dwDataLen);
		}
		else if (dwItemType<FileDataItem.dwItemType)
		{
			FILE_DATA_ITEM FileDataItem;
			FileDataItem.dwDataLen=dwDataLen;
			FileDataItem.dwItemType=dwItemType;
			FileDataItem.pDataBuffer=NULL;
			
			if (dwDataLen)
			{
				FileDataItem.pDataBuffer=new BYTE[dwDataLen];
				if (FileDataItem.pDataBuffer==NULL)
				{
					SetErrorInfo(CUSTOM_ERROR,0,_T("2设置文件信息时分配内存(长度=%d)失败!"),dwDataLen);
					OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
					return FALSE;
				}
				memcpy(FileDataItem.pDataBuffer,pDataBuffer,dwDataLen);
			}
			m_FileDataItemList.InsertBefore(pos,FileDataItem);
		}
		return TRUE;
	}
	if (pos==0)
	{
		FILE_DATA_ITEM FileDataItem;
		FileDataItem.dwDataLen=dwDataLen;
		FileDataItem.dwItemType=dwItemType;
		FileDataItem.pDataBuffer=NULL;
		
		if (dwDataLen)
		{
			FileDataItem.pDataBuffer=new BYTE[dwDataLen];
			if (FileDataItem.pDataBuffer==NULL)
			{
				SetErrorInfo(CUSTOM_ERROR,0,_T("3设置文件信息时分配内存(长度=%d)失败!"),dwDataLen);
				OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
				return FALSE;
			}
			memcpy(FileDataItem.pDataBuffer,pDataBuffer,dwDataLen);
		}
		m_FileDataItemList.AddTail(FileDataItem);
	}
	return TRUE;
}

BOOL CFileDataItem::GetDataItem(DWORD dwItemType,DWORD& dwDataLen,const void*& pDataBuffer)
{
	POSITION pos=m_FileDataItemList.GetHeadPosition();
	while(pos)
	{
		FILE_DATA_ITEM& FileDataItem=m_FileDataItemList.GetNext(pos);
		
		if (dwItemType!=FileDataItem.dwItemType)
			continue;

		pDataBuffer=FileDataItem.pDataBuffer;
		dwDataLen=FileDataItem.dwDataLen;
		return TRUE;
	}
	SetErrorInfo(CUSTOM_ERROR,0,_T("不存在指定的数据项!"));
	OutputDebugStringWhenDebugMode(_T("获取数据项时%s"), GetThreadErrorInfoString());
	return FALSE;
}

BOOL CFileDataItem::GetAllDataBuffer(void* pDataBuffer,DWORD& dwDataLen)
{
	DWORD dwAllDataLen=0;
	POSITION pos=m_FileDataItemList.GetHeadPosition();
	while(pos)
	{
		FILE_DATA_ITEM& FileDataItem=m_FileDataItemList.GetNext(pos);
		dwAllDataLen+=sizeof(DWORD)+sizeof(DWORD)+FileDataItem.dwDataLen;
	}

	if (pDataBuffer==NULL)
	{
		dwDataLen=dwAllDataLen;
		return TRUE;
	}
	else if (dwAllDataLen>dwDataLen)
	{
		dwDataLen=dwAllDataLen;
		SetErrorInfo(SYSTEM_ERROR,ERROR_BUFFER_OVERFLOW,_T("获取所有数据项内容失败:"));
		OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
		return FALSE;
	}

	DWORD dwOffset=0;
	pos=m_FileDataItemList.GetHeadPosition();
	while(pos)
	{
		FILE_DATA_ITEM& FileDataItem=m_FileDataItemList.GetNext(pos);
		*(DWORD*)((BYTE*)pDataBuffer+dwOffset)=FileDataItem.dwItemType;
		*(DWORD*)((BYTE*)pDataBuffer+dwOffset+sizeof(DWORD))=FileDataItem.dwDataLen;
		if (FileDataItem.dwDataLen)
			memcpy((BYTE*)pDataBuffer+dwOffset+sizeof(DWORD)+sizeof(DWORD),FileDataItem.pDataBuffer,FileDataItem.dwDataLen);
		dwOffset+=sizeof(DWORD)+sizeof(DWORD)+FileDataItem.dwDataLen;
	}
	return TRUE;
}

BOOL CFileDataItem::EnumDataItem(DWORD& dwItemType,DWORD& dwDataLen,const void*& pDataBuffer)
{
	POSITION pos=m_FileDataItemList.GetHeadPosition();
	while(pos)
	{
		FILE_DATA_ITEM& FileDataItem=m_FileDataItemList.GetNext(pos);
		
		if (dwItemType!=-1 && dwItemType>=FileDataItem.dwItemType)
			continue;
		
		dwItemType=FileDataItem.dwItemType;
		pDataBuffer=FileDataItem.pDataBuffer;
		dwDataLen=FileDataItem.dwDataLen;
		return TRUE;
	}
//	SetErrorInfo(CUSTOM_ERROR,0,_T("数据项已枚举完毕!"));
	return FALSE;
}

//BOOL CFileDataItem::WriteDataToFile(LPCTSTR lpcszFileName)
//{
//	BOOL bRet = FALSE;
//
//	DWORD dwBufferLen;
//	if (!GetAllDataBuffer(NULL, dwBufferLen))
//		return bRet;
//		
//	BYTE* pBuf = new BYTE[dwBufferLen];
//	if (pBuf==NULL)
//		return bRet;
//	else
//	{
//		memset(pBuf,0,dwBufferLen);	
//		
//		if (GetAllDataBuffer(pBuf, dwBufferLen))
//			bRet = WriteBufferToFile(pBuf, dwBufferLen, lpcszFileName);
//		
//		delete[] pBuf;
//	}
//
//	return bRet;
//}
