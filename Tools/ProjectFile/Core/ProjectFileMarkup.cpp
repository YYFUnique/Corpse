#include "StdAfx.h"
#include "ProjectFileMarkup.h"

CProjectFileMarkup::CProjectFileMarkup()
{

}

BOOL CProjectFileMarkup::LoadProjectFile(LPCTSTR lpszProjectFile)
{
	m_xml.Initialize();
	if (HIWORD(lpszProjectFile) != NULL ) {
		if (*(lpszProjectFile) == _T('<')) {
			if (!m_xml.Load(lpszProjectFile)) return FALSE;
		}
		else {
			if (PathFileExists(lpszProjectFile))
				LoadFromFile(lpszProjectFile);
		}
	}

	return TRUE;
}

void CProjectFileMarkup::LoadFromFile(LPCTSTR lpszProjectFile)
{
	BOOL bSuccess = FALSE;
	BYTE* pByte = NULL;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	do 
	{
		hFile = ::CreateFile(lpszProjectFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if( hFile == INVALID_HANDLE_VALUE ) 
			break;
		DWORD dwSize = ::GetFileSize(hFile, NULL);
		if( dwSize == 0 ) 
			break;
		if ( dwSize > 4096*1024 ) 
			break;

		DWORD dwRead = 0;
		pByte = new BYTE[ dwSize ];
		::ReadFile( hFile, pByte, dwSize, &dwRead, NULL );

		if (dwRead != dwSize) {
			m_xml.Release();
			break;
		}
		bool ret = m_xml.LoadFromMem(pByte, dwSize, XMLFILE_ENCODING_UTF8);
	} while (FALSE);
	
	if (pByte != NULL)
		delete[] pByte;
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
}

CMarkupNode CProjectFileMarkup::GetRoot()
{
	return m_xml.GetRoot();
}