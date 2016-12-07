#include "Hash.h"

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#define MAX_BUFFER_SIZE	4096

class DLL_API CCRC32: public CHash
{
public:
	CCRC32();
private:
	DWORD dwCrc32;
public:

//==================================================================
//�� �� ��: Init
//��    ��: ��ʼ��CRC32�㷨
//�������: ���������
//�� �� ֵ: �޷���ֵ
//==================================================================
	void Init();

//==================================================================
//�� �� ��: Update
//��    ��: �����������
//�������: [in] pBuffer, �������ݻ����ַ
//			[in] dwSize, �������ݳ���
//�� �� ֵ: �޷���ֵ
//==================================================================
	void Update(BYTE* pBuffer, DWORD dwSize);
//==================================================================
//�� �� ��: Final
//��    ��: ��ȡ��CRC32�㷨�����HASHֵ
//�������: [out], pHashvalue, HASHֵ
//�� �� ֵ: ����HASHֵ��λ��
//==================================================================
	DWORD Final(BYTE* pHashvalue);

//==================================================================
//�� �� ��: GetBigEndianResult
//��    ��: �������ֽ���ת��Ϊ�����ֽ���
//�������: [in,out] pHashValue, ���뱾���ֽ���HASHֵ,��������ֽ���HASHֵ
//�� �� ֵ: �޷���ֵ
//==================================================================
	void GetBigEndianResult(BYTE* pHashValue); 

//==================================================================
//�� �� ��: GetHashHexText
//��    ��: ��HASHֵת��Ϊ16�����ַ�����ʽ
//�������: [in]  pHashBuffer, (δʹ��)
//			[out] szHashText, ���16�����ַ�����ʽHASHֵ
//�� �� ֵ: �޷���ֵ
//==================================================================
	virtual void GetHashHexText(const BYTE* pHashBuffer,LPTSTR szHashText);   

//==================================================================
//�� �� ��: GetHashHexText
//��    ��: ��HASHֵת��Ϊ16����CString��ʽ
//�������: [in]  pHashBuffer, �����ת��HASHֵ
//			[out] szHashText, 16����CString��ʽHASHֵ
//�� �� ֵ: �޷���ֵ
//==================================================================
	virtual void GetHashHexText(const BYTE* pHashBuffer,CString& szHashText);
	
//==================================================================
//�� �� ��: HashBuffer
//��    ��: ��ָ���������HASH����
//�������: [in] pHashBuffer, ����HASH����Ļ����ַ
//			[in] dwBufferSize, ����HASH����Ļ����С
//�� �� ֵ: ��������õ���HASHֵ
//==================================================================
	static DWORD HashBuffer(const void* pHashBuffer,DWORD dwBufferSize);

private:

//==================================================================
//�� �� ��: CalcCrc32
//��    ��: CRC32�����㷨
//�������: [in] byte, �������ݻ����ַ
//			[in] length, �������ݻ����С
//			[out] dwCrc32, ���ܺ����õ�����
//�� �� ֵ: �޷���ֵ
//==================================================================
	void CalcCrc32(const BYTE* byte, DWORD length, DWORD &dwCrc32);

protected:
	static const DWORD Crc32Table[256];
};