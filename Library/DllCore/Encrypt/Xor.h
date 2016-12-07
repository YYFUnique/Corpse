#pragma once


#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

//==================================================================
//�� �� ��: memxor
//��    ��: ����������,keyΪ-1
//�������: [in,out] dest, �����׵�ַ
//			[in]	 count, ���ݴ�С
//�� �� ֵ: �����޷���ֵ
//==================================================================
DLL_API void memxor(void* dest,size_t count);

//==================================================================
//�� �� ��: XOROperate
//��    ��: ��input�е����ݣ�ȫ�������(XOR)����
//�������: Ҫ��������Ŀ������
//				 ��Ŀ�����ݽ������Ĳ���
//�� �� ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//==================================================================
DLL_API BOOL XOROperate(TCHAR *Input , DWORD dwOperateNum);