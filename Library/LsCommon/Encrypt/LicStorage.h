#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

//==================================================================
//�� �� ��: SaveLicInfo
//��    ��: ���ܲ�����ͻ�������ַ���������ļ�
//�������: [in] �ͻ�������ַ����׵�ַ
//			[in] �ͻ�������ַ�����С
//�� �� ֵ: ���ܲ����ֿͻ�������ַ����ɹ�����TURE,���򷵻�FALSE
//==================================================================
LS_COMMON_API BOOL SaveLicInfo(const void* pLicInfo,DWORD dwLicInfoLen);

//==================================================================
//�� �� ��: LoadLicInfo
//��    ��: ���ܿͻ�������ļ�����,�������ܺ��ַ����ָ�(��','�ָ�),���������ַ���������
//�������: [out] strLicStringArray, ���ܺ������ļ�����,����','Ϊ�ָ��,�ָ���Ӵ�
//�� �� ֵ: ���ܿͻ�������ļ��ɹ�����TRUE,���򷵻�FALSE
//==================================================================
LS_COMMON_API BOOL LoadLicInfo(CStringArray& strLicStringArray);

//==================================================================
//�� �� ��: CheckModuleLicInfo
//��    ��: ���ҿͻ�������ļ����Ƿ���Ŀ���Ӵ�
//�������: [in] lpszLicInfo,����ҵ��Ӵ�
//�� �� ֵ: ����ͻ�������ļ��д���Ŀ���Ӵ�����TRUE,���򷵻�FALSE
//==================================================================
LS_COMMON_API BOOL CheckModuleLicInfo(LPCTSTR lpszLicInfo);

//LS_COMMON_API BOOL ConvertLicInfoToStringArray(const void* pLicInfo,DWORD dwLicInfoLen,CStringArray& strLicStringArray);
