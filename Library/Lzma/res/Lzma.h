#pragma once

#include "XZip.h"

#ifdef LS_STATIC_LIB_CALL
#define LZMA_API
#elif defined LZMA_EXPORTS
#define LZMA_API __declspec(dllexport)
#else
#define LZMA_API __declspec(dllimport)
#endif

class LZMA_API CLzma
{
public:
	CLzma();
	~CLzma();

public:
	/*******************************************************************************/
	/* 函数名称：LzmaEncode																					   */
	/* 函数功能：将lpszTargetFilePath指定目录或文件添加到ZIP压缩内存中				   */
	/* 函数参数：参数一    lpszTargetFilePath   指定要添加到压缩文件的路径或目录     */
	/*					 参数二    lpData 添加到压缩文件中的内存数据								       */
	/*					 参数三    dwLen    内存数据库数据长度											       */
	/* 返 回 值： 成功返回TRUE，否则返回FALSE														   */
	/*******************************************************************************/
	BOOL LzmaEncode(LPCTSTR lpszTargetFilePath,LPBYTE lpData,DWORD& dwLen);

	/*******************************************************************************/
	/* 函数名称：LzmaEncode																					   */
	/* 函数功能：将lpszTargetFilePath指定目录或文件添加压缩文件，并保存到lpszBinFilePath中 */
	/* 函数参数：参数一    lpszTargetFilePath   指定要添加到压缩文件的路径或目录     */
	/*					 参数二    lpszBinFilePath 压缩文件生成路径								           */
	/* 返 回 值： 成功返回TRUE，否则返回FALSE														   */
	/*******************************************************************************/
 	BOOL LzmaEncode(LPCTSTR lpszTargetFilePath,LPCTSTR lpszBinFilePath);

	/*******************************************************************************/
	/* 函数名称：LzmaDecode																					   */
	/* 函数功能：将lpszTargetFilePath指定的压缩文件，释放到	lpszPathFile指定目录中*/
	/* 函数参数：参数一    lpszTargetFilePath   压缩文件路径								       */
	/*					 参数二    lpszPathFile 释放文件存放路径       								       */
	/* 返 回 值： 成功返回TRUE，否则返回FALSE														   */
	/*******************************************************************************/
 	BOOL LzmaDecode(LPCTSTR lpszTargetFilePath,LPCTSTR lpszPathFile);

	/*******************************************************************************/
	/* 函数名称：LzmaDecode																					   */
	/* 函数功能：将lpData指定的数据块释放到lpszPathFile指定的目录中     				   */
	/* 函数参数：参数一    lpData   压缩文件内存数据												    */
	/*					 参数二    nLen     压缩文件内存数据长度										       */
	/*					 参数三    lpszPathFile 压缩文件释放目录										       */
	/* 返 回 值： 成功返回TRUE，否则返回FALSE														   */
	/*******************************************************************************/
	BOOL LzmaDecode(LPBYTE lpData,UINT nLen,LPCTSTR lpszPathFile);
protected:
	/************************************************************************************/
	/* 函数名称：AddFileToZip																						      */
	/* 函数功能：将lpszTargetFilePath指定目录中的文件或文件添加到hZip句柄指定的压缩文件中*/
	/* 函数参数：参数一    hZip   Zip文件句柄																       */
	/*					 参数二    lpszPathDirectory   指定要添加到压缩文件的目录或文件		       */
	/* 返 回 值：成功返回TRUE，否则返回FALSE																	*/
	/*************************************************************************************/
	BOOL AddFileToZip(HZIP hZip,LPCTSTR lpszTargetFilePath);

	/************************************************************************************/
	/* 函数名称：GetFileFromZip																						   */
	/* 函数功能：将hZip指定位置获取压缩文件，并将文件释放到lpszPathDirectory指定目录*/
	/* 函数参数：参数一    hZip   Zip文件句柄																       */
	/*					 参数二    lpszPathDirectory 文件释放目录											       */
	/* 返 回 值：成功返回TRUE，否则返回FALSE																	*/
	/*************************************************************************************/
	BOOL GetFileFromZip(HZIP hZip,LPCTSTR lpszPathDirectory);

	/************************************************************************/
	/* 以下三个函数用于估算指定目录或文件的大小                                           */
	/************************************************************************/
	DWORD GetPathFileSizeLen(LPCTSTR lpszTargetFilePath);

	DWORD GetFileSizeFromDirectory(LPCTSTR lpszDirectoryPathFile);

	DWORD GetFileSize(LPCTSTR lpszPathFile);
};