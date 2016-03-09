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
	/* �������ƣ�LzmaEncode																					   */
	/* �������ܣ���lpszTargetFilePathָ��Ŀ¼���ļ���ӵ�ZIPѹ���ڴ���				   */
	/* ��������������һ    lpszTargetFilePath   ָ��Ҫ��ӵ�ѹ���ļ���·����Ŀ¼     */
	/*					 ������    lpData ��ӵ�ѹ���ļ��е��ڴ�����								       */
	/*					 ������    dwLen    �ڴ����ݿ����ݳ���											       */
	/* �� �� ֵ�� �ɹ�����TRUE�����򷵻�FALSE														   */
	/*******************************************************************************/
	BOOL LzmaEncode(LPCTSTR lpszTargetFilePath,LPBYTE lpData,DWORD& dwLen);

	/*******************************************************************************/
	/* �������ƣ�LzmaEncode																					   */
	/* �������ܣ���lpszTargetFilePathָ��Ŀ¼���ļ����ѹ���ļ��������浽lpszBinFilePath�� */
	/* ��������������һ    lpszTargetFilePath   ָ��Ҫ��ӵ�ѹ���ļ���·����Ŀ¼     */
	/*					 ������    lpszBinFilePath ѹ���ļ�����·��								           */
	/* �� �� ֵ�� �ɹ�����TRUE�����򷵻�FALSE														   */
	/*******************************************************************************/
 	BOOL LzmaEncode(LPCTSTR lpszTargetFilePath,LPCTSTR lpszBinFilePath);

	/*******************************************************************************/
	/* �������ƣ�LzmaDecode																					   */
	/* �������ܣ���lpszTargetFilePathָ����ѹ���ļ����ͷŵ�	lpszPathFileָ��Ŀ¼��*/
	/* ��������������һ    lpszTargetFilePath   ѹ���ļ�·��								       */
	/*					 ������    lpszPathFile �ͷ��ļ����·��       								       */
	/* �� �� ֵ�� �ɹ�����TRUE�����򷵻�FALSE														   */
	/*******************************************************************************/
 	BOOL LzmaDecode(LPCTSTR lpszTargetFilePath,LPCTSTR lpszPathFile);

	/*******************************************************************************/
	/* �������ƣ�LzmaDecode																					   */
	/* �������ܣ���lpDataָ�������ݿ��ͷŵ�lpszPathFileָ����Ŀ¼��     				   */
	/* ��������������һ    lpData   ѹ���ļ��ڴ�����												    */
	/*					 ������    nLen     ѹ���ļ��ڴ����ݳ���										       */
	/*					 ������    lpszPathFile ѹ���ļ��ͷ�Ŀ¼										       */
	/* �� �� ֵ�� �ɹ�����TRUE�����򷵻�FALSE														   */
	/*******************************************************************************/
	BOOL LzmaDecode(LPBYTE lpData,UINT nLen,LPCTSTR lpszPathFile);
protected:
	/************************************************************************************/
	/* �������ƣ�AddFileToZip																						      */
	/* �������ܣ���lpszTargetFilePathָ��Ŀ¼�е��ļ����ļ���ӵ�hZip���ָ����ѹ���ļ���*/
	/* ��������������һ    hZip   Zip�ļ����																       */
	/*					 ������    lpszPathDirectory   ָ��Ҫ��ӵ�ѹ���ļ���Ŀ¼���ļ�		       */
	/* �� �� ֵ���ɹ�����TRUE�����򷵻�FALSE																	*/
	/*************************************************************************************/
	BOOL AddFileToZip(HZIP hZip,LPCTSTR lpszTargetFilePath);

	/************************************************************************************/
	/* �������ƣ�GetFileFromZip																						   */
	/* �������ܣ���hZipָ��λ�û�ȡѹ���ļ��������ļ��ͷŵ�lpszPathDirectoryָ��Ŀ¼*/
	/* ��������������һ    hZip   Zip�ļ����																       */
	/*					 ������    lpszPathDirectory �ļ��ͷ�Ŀ¼											       */
	/* �� �� ֵ���ɹ�����TRUE�����򷵻�FALSE																	*/
	/*************************************************************************************/
	BOOL GetFileFromZip(HZIP hZip,LPCTSTR lpszPathDirectory);

	/************************************************************************/
	/* ���������������ڹ���ָ��Ŀ¼���ļ��Ĵ�С                                           */
	/************************************************************************/
	DWORD GetPathFileSizeLen(LPCTSTR lpszTargetFilePath);

	DWORD GetFileSizeFromDirectory(LPCTSTR lpszDirectoryPathFile);

	DWORD GetFileSize(LPCTSTR lpszPathFile);
};