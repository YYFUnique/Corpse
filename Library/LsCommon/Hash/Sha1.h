#pragma once
#include "Hash.h"

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

typedef struct 
{
    unsigned long state[5];
    unsigned long count[2];
    unsigned char buffer[64];
} SHA1_CTX;

class LS_COMMON_API CSHA1: public CHash
{
public:
	CSHA1();
private:
	SHA1_CTX context;
public:
	void Init();
	void Update(BYTE* pBuffer, DWORD dwSize);
	DWORD Final(BYTE* pHashvalue);
private:

//==================================================================
//�� �� ��: SHA1Init
//��    ��: ��ʼ��SHA1�㷨
//�������: ���������
//�� �� ֵ: �޷���ֵ
//==================================================================
	void SHA1Init();

//==================================================================
//�� �� ��: SHA1Update
//��    ��: SHA1�㷨����
//�������: [in] �������ݻ����ַ
//			[in] �������ݳ���
//�� �� ֵ: �޷���ֵ
//==================================================================
	void SHA1Update(const void* input,DWORD inputLen);

//==================================================================
//�� �� ��: SHA1Final
//��    ��: ��ȡ��SHA1���ܺ�HASHֵ
//�������: [out] HASHֵ
//�� �� ֵ: ����HASHֵ����(20)
//==================================================================
	DWORD SHA1Final(BYTE digest[20]);

//==================================================================
//�� �� ��: SHA1Transform
//��    ��: SHA1������λ�����㷨
//�������: [in,out] state, �������������,�����λ���ܺ�����
//			[in]	 buffer, ���������ݿ�
//�� �� ֵ: �޷���ֵ
//==================================================================
	void SHA1Transform(unsigned long state[5], unsigned char buffer[64]);	

//==================================================================
//�� �� ��: GetHashAlgName
//��    ��: ��ȡHASH�㷨����("SHA1")
//�������: ���������
//�� �� ֵ: HASH�㷨����
//==================================================================
	const TCHAR* GetHashAlgName();

//==================================================================
//�� �� ��: GetHashLen
//��    ��: ��ȡHASHֵ����(20)
//�������: ���������
//�� �� ֵ: HASHֵ����
//==================================================================
	DWORD GetHashLen();

};

