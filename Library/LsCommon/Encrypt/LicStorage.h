#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

//==================================================================
//函 数 名: SaveLicInfo
//功    能: 加密并保存客户端许可字符串至许可文件
//输入参数: [in] 客户端许可字符串首地址
//			[in] 客户端许可字符串大小
//返 回 值: 加密并保持客户端许可字符串成功返回TURE,否则返回FALSE
//==================================================================
LS_COMMON_API BOOL SaveLicInfo(const void* pLicInfo,DWORD dwLicInfoLen);

//==================================================================
//函 数 名: LoadLicInfo
//功    能: 解密客户端许可文件内容,并将解密后字符串分割(以','分割),并保存至字符串数组中
//输入参数: [out] strLicStringArray, 解密后的许可文件内容,并以','为分割符,分割的子串
//返 回 值: 解密客户端许可文件成功返回TRUE,否则返回FALSE
//==================================================================
LS_COMMON_API BOOL LoadLicInfo(CStringArray& strLicStringArray);

//==================================================================
//函 数 名: CheckModuleLicInfo
//功    能: 查找客户端许可文件中是否有目的子串
//输入参数: [in] lpszLicInfo,需查找的子串
//返 回 值: 如果客户端许可文件中存在目的子串返回TRUE,否则返回FALSE
//==================================================================
LS_COMMON_API BOOL CheckModuleLicInfo(LPCTSTR lpszLicInfo);

//LS_COMMON_API BOOL ConvertLicInfoToStringArray(const void* pLicInfo,DWORD dwLicInfoLen,CStringArray& strLicStringArray);
