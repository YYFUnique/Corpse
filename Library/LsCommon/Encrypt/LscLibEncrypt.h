#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

/*****************************************************************************
 * �⼸������Ϊ���ذ�ȫ��⹤�ߵ������ļ����ܽ�������
 */

//==================================================================
//�� �� ��: LibEncryptFile
//��    ��: ����Ŀ���ļ��ļ��ܱ���
//�������: [in]  lpszSrcFile, Դ�ļ�·��
//			[out] lpszDestFile, Դ�ļ��ļ��ܱ����ļ�·��
//�� �� ֵ: �ļ����ܱ��ݳɹ�����TRUE,���򷵻�FALSE
//==================================================================
LS_COMMON_API BOOL LibEncryptFile(LPCTSTR lpszSrcFile, LPCTSTR lpszDestFile);

//==================================================================
//�� �� ��: LibDecrypt
//��    ��: Ŀ���ļ�����
//�������: [in] lpszEncFile,Ŀ���ļ�λ��
//�� �� ֵ: ���ؽ��ܺ��ļ����ݻ����׵�ַ
//==================================================================
LS_COMMON_API LPSTR LibDecrypt(LPCTSTR lpszEncFile);

//==================================================================
//�� �� ��: LibBufferFree
//��    ��: �ͷŽ����ļ����ݻ���
//�������: [in,out] �����ļ����ݻ����׵�ַ
//�� �� ֵ: �޷���ֵ
//==================================================================
LS_COMMON_API void LibBufferFree(void *pBuffer);
