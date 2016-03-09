#include "stdafx.h"
#include "EncryptString.h"
#include "Rijndael.h"

const unsigned char byKeyIv[48] = {0xa3, 0x86, 0x78, 0x9b, 0x58, 0xf3, 0x8a, 0x90, 0x94, 0x91, 0xc4, 0x76, 0x3, 0x65, 0x26, 0x5,
0x80, 0xe7, 0x39, 0x8a, 0x3e, 0x33, 0xff, 0x88, 0x8a, 0xbd, 0xe7, 0x92, 0x9f, 0xbf, 0x53, 0x3d,
0x97, 0xa9, 0x1b, 0x29, 0xf4, 0x1a, 0xd7, 0xa0, 0x8d, 0x57, 0x77, 0x28, 0xe0, 0x77, 0xb, 0xe2};


// �����ַ���,�����������,����lpszText��lpbyEncryptTextָ��ͬһ��ַ
// �������ĳ���
DWORD EncryptString(LPCTSTR lpszText,BYTE* lpbyEncryptText)
{
	CRijndael Aes;
	DWORD dwLen=(DWORD)_tcslen(lpszText)+sizeof(TCHAR);
	
	Aes.SetKey(byKeyIv,byKeyIv+32,32,16);
	Aes.Encrypt(lpszText,lpbyEncryptText,dwLen,CRijndael::CBC);
	
	return dwLen;
}

// ����������Ϣ�Ͷ�Ӧ���ȣ�����ַ�������
void DecryptString(BYTE* lpbyEncryptText,DWORD dwEncryptBufferLen,LPTSTR lpszText)
{
	CRijndael Aes;
	Aes.SetKey(byKeyIv,byKeyIv+32,32,16);
	Aes.Decrypt(lpbyEncryptText,lpszText,dwEncryptBufferLen,CRijndael::CBC);
}
