#pragma once

class CPELibrary
{
public:
	CPELibrary();
	~CPELibrary();

public:
	BOOL ParsePEInfo(LPCTSTR lpszFilePath);
	CString& GetFileName();
	IMAGE_DOS_HEADER& GetImageDosHeader();
	IMAGE_NT_HEADERS& GetImageNtHeader();
protected:

	BOOL IsPeFile(LPVOID pImageBase);
protected:
	IMAGE_DOS_HEADER m_ImageDosHeader;
	PIMAGE_NT_HEADERS m_ImageNtHeader;

	CString m_strFileName;
};