#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

#define HASH_TYPE_MD5     1
#define HASH_TYPE_CRC32   2
#define HASH_TYPE_SHA1    3
#define HASH_TYPE_SHA256  4

#define HASH_MAX 128

class LS_COMMON_API CHash
{ 
protected:
	int m_nHashValueLen;
public:

//==================================================================
//�� �� ��: HashType
//��    ��: ����Hash��������
//�������: [in] nHashType, Hash��������
//					HASH_TYPE_MD5, MD5
//					HASH_TYPE_CRC32, RC32
//					HASH_TYPE_SHA1, HA1
//�� �� ֵ: ���óɹ����ض�Ӧ�㷨��ָ��,���򷵻�0
//==================================================================
	static CHash* HashType(int nHashType);

//==================================================================
//�� �� ��: Init
//��    ��: Hash�㷨��ʼ����(���麯��)
//�������: ���������
//�� �� ֵ: �޷���ֵ
//==================================================================
	virtual  void Init()=0;

//==================================================================
//�� �� ��: Update
//��    ��: �����������(���麯��)
//�������: [in] pBuffer, �������ݻ����ַ
//			[in] dwSize, �������ݳ���
//�� �� ֵ: �޷���ֵ
//==================================================================
	virtual void Update(BYTE* pBuffer, DWORD dwSize)=0;

//==================================================================
//�� �� ��: Final
//��    ��: ��ü��ܺ��HASHֵ(���麯��)
//�������: [out] pHashvalue, Hashֵ
//�� �� ֵ: ����HASHֵ��λ��
//==================================================================
	virtual DWORD Final(BYTE* pHashvalue)=0;

//==================================================================
//�� �� ��: GetFileHash
//��    ��: ����ļ���HASHֵ
//�������: [in]  lpcszName, ȫ·���ļ���
//			[out] pHashvalue, HASHֵ
//�� �� ֵ: ��ȡ�ļ�HASHֵ�ɹ�����TRUE,���򷵻�FALSE
//==================================================================
	virtual BOOL GetFileHash(LPCTSTR lpcszName, BYTE* pHashvalue);

//==================================================================
//�� �� ��: GetHash
//��    ��: ���ض���Ϣ����
//�������: [in]  nHashType,HASH�㷨����
//					HASH_TYPE_MD5, MD5
//					HASH_TYPE_CRC32, RC32
//					HASH_TYPE_SHA1, HA1
//			[in]  pBuffer,�������ݵ�ַ
//			[in]  dwSize, �������ݴ�С
//			[out]  szHashText, �ַ�����ʽHASHֵ
//�� �� ֵ: �޷���ֵ
//==================================================================
	static void GetHash(int nHashType, BYTE* pBuffer, DWORD dwSize, LPTSTR szHashText);

//==================================================================
//�� �� ��: GetHash
//��    ��: ���ض���Ϣ����
//�������: [in]  nHashType,HASH�㷨����
//					HASH_TYPE_MD5, MD5
//					HASH_TYPE_CRC32, RC32
//					HASH_TYPE_SHA1, HA1
//			[in]  pBuffer,�������ݵ�ַ
//			[in]  dwSize, �������ݴ�С
//			[out] pHashvalue, HASHֵ
//�� �� ֵ: ����HASHֵ����
//==================================================================
	static DWORD GetHash(int nHashType, BYTE* pBuffer, DWORD dwSize, BYTE* pHashvalue); 
	
//==================================================================
//�� �� ��: GetBigEndianResult
//��    ��: ��HASHֵ�ֽ���������ֽ���ת��Ϊ�����ֽ���
//�������: [in,out] ���������ֽ���HASHֵ,���������ֽ���HASHֵ
//�� �� ֵ: ���������
//==================================================================
	virtual void GetBigEndianResult(BYTE* pHashValue);
	
//==================================================================
//�� �� ��: GetHashHexText
//��    ��: ��HASHֵת��Ϊ�ַ�����ʽ
//�������: [in]  pHashBuffer, ����HASHֵ�����ַ
//				[out] szHashText, �����ַ���HASHֵ
//			    nLen ����������
//�� �� ֵ: �޷���ֵ
//==================================================================
	virtual void GetHashHexText(const BYTE* pHashBuffer,LPTSTR szHashText);

//==================================================================
//�� �� ��: GetHashHexText
//��    ��: ��HASHֵת��ΪCStringg��ʽ
//�������: [in]  pHashBuffer, ����HASHֵ�����ַ
//			[out] szHashText, ����CString��ʽHASHֵ
//�� �� ֵ: �ɹ�����TRUE
//==================================================================
	virtual void GetHashHexText(const BYTE* pHashBuffer,CString& szHashText);

//==================================================================
//�� �� ��: Release
//��    ��: ���������������ͷ��ڴ�
//�������: ���������
//�� �� ֵ: �޷���ֵ
//==================================================================
	virtual void Release();

};



//void GetHashHexText(const BYTE* pHashBuffer,CString& szHashText);
//
//virtual DWORD GetBigEndianResult(void* pHashBuffer);