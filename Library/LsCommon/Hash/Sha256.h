#pragma once
#include "Hash.h"

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

#ifndef uint8
#define uint8  unsigned char
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

typedef struct
{
    uint32 total[2];
    uint32 state[8];
    uint8 buffer[64];
}SHA256_CTX;

class LS_COMMON_API CSHA256: public CHash 
{
private:
	SHA256_CTX context;
public:
//==================================================================
//�� �� ��: Init
//��    ��: Hash�㷨��ʼ����(����ʵ�ʹ���,��Ϊʵ�ֵײ�ӿ�)
//�������: ���������
//�� �� ֵ: �޷���ֵ
//==================================================================
	void Init();

//==================================================================
//�� �� ��: Update
//��    ��: �����������(����ʵ�ʹ���,��Ϊʵ�ֵײ�ӿ�)
//�������: [in] pBuffer, �������ݻ����ַ
//			[in] dwSize, �������ݳ���
//�� �� ֵ: �޷���ֵ
//==================================================================
	void Update(BYTE* pBuffer, DWORD dwSize);

//==================================================================
//�� �� ��: Final
//��    ��: ��ü��ܺ��HASHֵ(����ʵ�ʹ���,��Ϊʵ�ֵײ�ӿ�)
//�������: [out] pHashvalue, Hashֵ
//�� �� ֵ: ����HASHֵ��λ��
//==================================================================
	DWORD Final(BYTE* pHashvalue);

private:
//==================================================================
//�� �� ��: sha256_starts
//��    ��: ��ʼ��SHA256�����㷨
//�������: [out] ctx, SHA256�㷨������
//�� �� ֵ: �޷���ֵ
//==================================================================
	void sha256_starts( SHA256_CTX *ctx );

//==================================================================
//�� �� ��: sha256_update
//��    ��: ��������ܽ��
//�������: [in] ctx, �㷨������
//			[in] input, ����������ָ��
//			[in] length, ���������ݳ���
//�� �� ֵ: �޷���ֵ
//==================================================================
    void sha256_update( SHA256_CTX *ctx, const uint8 *input, uint32 length );

//==================================================================
//�� �� ��: sha256_finish
//��    ��: ��ȡ���ܺ�����
//�������: [in]  ctx, �����㷨������
//			[out] digest,���ܺ�����
//�� �� ֵ: �޷���ֵ
//==================================================================
    void sha256_finish( SHA256_CTX *ctx, uint8 digest[32] );

};

