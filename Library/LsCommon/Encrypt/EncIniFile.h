#pragma once
// EncIniFile.h : header fi
//
#include "FileDataItem.h"


#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////////
// CEncIniFile window

class LS_COMMON_API CEncIniFile : public CFileDataItem
{
// Construction
public:
	CEncIniFile();

// Attributes
public:

// Operations
public:

//==================================================================
//�� �� ��: GetFileName
//��    ��: ��ȡ�����һ�β������ļ���
//�������: ���������
//�� �� ֵ: ���������һ�ɲ������ļ���
//==================================================================
	CString& GetFileName();

//==================================================================
//�� �� ��: SaveDataToFile
//��    ��: �������ݵ�ָ�������ļ���
//�������: [in] dwFileVersion,���������ļ����õİ汾��(ֻ֧��Ϊ1�İ汾��)
//			[in] lpszFileName, �����ļ���ȫ·���ļ���
//			[in] lpszMagicText, ħ����
//�� �� ֵ: �����ļ����ݱ���ɹ�����TRUE,���򷵻�FALSE
//==================================================================
	BOOL SaveDataToFile(DWORD dwFileVersion,LPCTSTR lpszFileName,LPCTSTR lpszMagicText);

//==================================================================
//�� �� ��: LoadDataFromFile
//��    ��: ���������ļ�����
//�������: [in] �����ļ���ȫ·���ļ���
//			[in] �����ļ���ħ����
//�� �� ֵ: ���������ļ����ݳɹ�����TRUE,���򷵻�FALSE
//==================================================================
	BOOL LoadDataFromFile(LPCTSTR lpszFileName,LPCTSTR lpszMagicText);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLsEncIniFile)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEncIniFile();

	// Generated message map functions
protected:
	DWORD m_dwFileVersion;
	CString m_strFileName;
	static const BYTE m_byEncryptKeyIv[48];
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
