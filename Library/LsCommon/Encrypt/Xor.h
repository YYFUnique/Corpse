#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

//==================================================================
//�� �� ��: memxor
//��    ��: ����������,keyΪ-1
//�������: [in,out] dest, �����׵�ַ
//			[in]	 count, ���ݴ�С
//�� �� ֵ: �����޷���ֵ
//==================================================================
LS_COMMON_API void memxor(void* dest,size_t count);

//==================================================================
//�� �� ��: XOROperate
//��    ��: ��input�е����ݣ�ȫ�������(XOR)����
//�������: Ҫ��������Ŀ������
//				 ��Ŀ�����ݽ������Ĳ���
//�� �� ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
//==================================================================
LS_COMMON_API BOOL XOROperate(TCHAR *Input , DWORD dwOperateNum);