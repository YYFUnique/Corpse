#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

#define LS_ENCRYPT_ALG_AES	1

/////////////////////////////////////////////////////////////////////////////
// CEncryptObject window

class LS_COMMON_API CEncryptObject
{
// Construction
public:
	CEncryptObject();
	enum { ECB=0, CBC=1, CFB=2 };
// Attributes
public:

//==================================================================
//�� �� ��: GetKeyLength
//��    ��: ��ȡ��Կ�ĳ���(���麯��)
//�������: ���������
//�� �� ֵ: ������Կ����
//==================================================================
	virtual int GetKeyLength()=0;//Get Key Length

//==================================================================
//�� �� ��: GetBlockSize
//��    ��: ��ȡ���ܿ��С(���麯��)
//�������: ���������
//�� �� ֵ: ���ؼ��ܿ��С
//==================================================================
	virtual int GetBlockSize()=0;//Block Size

// Operations
public:

//==================================================================
//�� �� ��: Create
//��    ��: ���������㷨
//�������: [in] �����㷨ID
//					LS_ENCRYPT_ALG_AES Rijndael�㷨
//�� �� ֵ: �����ɹ�����ָ�������㷨����,ʧ�ܷ���NULL
//==================================================================
	static CEncryptObject* Create(DWORD dwAlgId);

//==================================================================
//�� �� ��: SetKey
//��    ��: ��չ�û���ԿΪ�Ự��Կ(���麯��)			
//�������: [in] lpKey, ��Կ
//			[in] lpChain, ���ݿ���
//			[in] nKeyLength, ��Կ����
//			[in] nBlockSize,���ݿ��С
//�� �� ֵ: �޷���ֵ
//==================================================================
	virtual void SetKey(const void* lpKey, const void* lpChain, int nKeyLength, int nBlockSize)=0;

//==================================================================
//�� �� ��: ResetChain
//��    ��: �������ݿ���(���麯��)
//�������: ���������
//�� �� ֵ: �޷���ֵ
//==================================================================
	virtual void ResetChain()=0;

//==================================================================
//�� �� ��: Encrypt
//��    ��: ����(���麯��)
//�������: [in]  in, ��������ݻ����ַ
//			[out] result, ���ܺ����Ļ����ַ
//			[in]  dwBufferLen, �������ݻ���ܺ����Ļ����С
//			[in]  iMode, ���ܷ�ʽ
//					ECB ���Ӵ�����(Electronic Code Book)
//					CBC �������(Chain Block Coding)
//					CFB	���뷴��(Cipher Feedback)
//�� �� ֵ: �������óɹ�����TRUE,���򷵻�FALSE
//==================================================================
	virtual BOOL Encrypt(const void* in, void* result, DWORD dwBufferLen, int iMode)=0;

//==================================================================
//�� �� ��: Decrypt
//��    ��: ����(���麯��)
//�������: [in]  in, ��������ݻ����ַ
//			[out] result, ���ܺ����ݻ����ַ
//			[in]  dwBufferLen, �������ݻ���ܺ����ݻ����С
//			[in]  iMode, ���ܷ�ʽ
//					ECB ���Ӵ�����(Electronic Code Book)
//					CBC �������(Chain Block Coding)
//					CFB	���뷴��(Cipher Feedback)
//�� �� ֵ: �������óɹ�����TRUE,���򷵻�FALSE
//==================================================================
	virtual BOOL Decrypt(const void* in, void* result, DWORD dwBufferLen, int iMode)=0;

//==================================================================
//�� �� ��: Release
//��    ��: �ͷ��ڴ�
//�������: ���������
//�� �� ֵ: �޷���ֵ
//==================================================================
	virtual void Release();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEncryptObject)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEncryptObject();

	// Generated message map functions
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
