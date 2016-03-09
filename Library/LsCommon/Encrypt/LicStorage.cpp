#include "stdafx.h"
#include "LicStorage.h"
#include "..\TextTools.h"
#include "..\FileTools.h"
#include "..\ErrorInfo.h"
#include "Rijndael.h"

const DWORD MAX_LIC_INFO_LENGTH=64*1024;//最大长度64K
const unsigned char byLicKeyIv[48] = {0x3F, 0x15, 0x16, 0x1C, 0x98, 0x0F, 0xA6, 0x66, 0xEA, 0xA3, 0x7B, 0x8D, 0x33, 0x54, 0xA6, 0x45, 
		0x7E, 0xD7, 0x1D, 0x08, 0x5B, 0x77, 0x3D, 0x19, 0xF3, 0x46, 0x94, 0xDB, 0x23, 0xA3, 0x10, 0x34, 
		0x60, 0xA8, 0xEC, 0x9F, 0x69, 0x7E, 0xFD, 0x13, 0xF2, 0xBD, 0x33, 0x06, 0xD6, 0x3B, 0x72, 0x0E};

//BOOL SaveLicInfo(const void* pLicInfo,DWORD dwLicInfoLen)
//{
//	BOOL bSuccess=FALSE;
//	TCHAR szLicFilePath[MAX_PATH];
//
//	if (dwLicInfoLen<4)
//	{
//		SetErrorInfo(CUSTOM_ERROR,0,_T("系统信息长度不正确(无效值=%d)"),dwLicInfoLen);
//		return FALSE;
//	}
//	else if (dwLicInfoLen>MAX_LIC_INFO_LENGTH)
//	{
//		SetErrorInfo(CUSTOM_ERROR,0,_T("系统信息超出最大长度(无效值=%d)"),dwLicInfoLen);
//		return FALSE;
//	}
//
//	CRijndael Aes;BYTE byEncBuffer[MAX_LIC_INFO_LENGTH];
//	Aes.SetKey(byLicKeyIv,byLicKeyIv+32,32,16);
//	Aes.Encrypt(pLicInfo,byEncBuffer,dwLicInfoLen,CRijndael::CBC);
//
//	if (GetCommonSystemInfoPath(szLicFilePath,_T("lginfo.dat"))==TRUE)
//		if (WriteBufferToFile(byEncBuffer,dwLicInfoLen,szLicFilePath)==TRUE)
//			bSuccess=TRUE;
//		else
//			SetErrorTitle(_T("保存系统信息失败"));
//		
//	return bSuccess;
//}
//
BOOL LoadLicInfo(CStringArray& strLicStringArray)
{
	return TRUE;
}
//	BOOL bSuccess=FALSE;
//	TCHAR szLicFilePath[MAX_PATH];
//
//	if (GetCommonSystemInfoPath(szLicFilePath,_T("lginfo.dat"))==FALSE)
//		return FALSE;
//	
//	BYTE byLicBuffer[MAX_LIC_INFO_LENGTH];DWORD dwLicInfoLen=sizeof(byLicBuffer);
//	if (ReadFileContent(szLicFilePath,byLicBuffer,dwLicInfoLen)==FALSE)
//	{
//		SetErrorTitle(_T("读取系统信息失败"));
//		return FALSE;
//	}
//
//	CRijndael Aes;BYTE byDecBuffer[MAX_LIC_INFO_LENGTH];
//	Aes.SetKey(byLicKeyIv,byLicKeyIv+32,32,16);
//	Aes.Decrypt(byLicBuffer,byDecBuffer,dwLicInfoLen,CRijndael::CBC);
//	
//	CString strLicInfo((char*)byDecBuffer,dwLicInfoLen);
//	AnalyseFormattedTextInfoItem(strLicInfo,strLicStringArray,',');
//	return TRUE;
//}

// BOOL ConvertLicInfoToStringArray(const void* pLicInfo,DWORD dwLicInfoLen,CStringArray& strLicStringArray)
// {
// 	CString strLicInfo((char*)pLicInfo,dwLicInfoLen);
// 	AnalyseFormattedTextInfoItem(strLicInfo,strLicStringArray,',');
// 	return TRUE;
// }

BOOL CheckModuleLicInfo(LPCTSTR lpszLicInfo)
{
	CStringArray strLicStringArray;
	LoadLicInfo(strLicStringArray);
	for (int i=0;i<strLicStringArray.GetSize();i++)
	{
		CString strtmpLicence = strLicStringArray.ElementAt(i);
		if (!_tcsicmp(strtmpLicence,lpszLicInfo))
		{
			return TRUE;
		}
	}
	return FALSE; 
}