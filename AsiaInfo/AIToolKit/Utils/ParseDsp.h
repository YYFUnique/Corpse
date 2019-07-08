#pragma once

class CParseDsp
{
public:
	CParseDsp();
	~CParseDsp();

public:
	BOOL Init(LPCTSTR lpszDepluaContent);
	LPCTSTR GetFileName() const;
	LPCTSTR GetLuaFileType() const;
public:
	static BOOL PackageFile(LPCTSTR lpszpackageDir, LPCTSTR lpszPackageName);
	static BOOL UnPackageFile(LPCTSTR lpszUnpackageDir, LPCTSTR lpszPackage);
	static BOOL IsPackageHasDep(LPCTSTR lpszPackage, CString& strPackageDeplua);
protected:
	static BOOL PackageFileInDirectory(HZIP hPackage, LPCTSTR lpszPackageDir, LPCTSTR lpszBaseDir);
protected:
	CString m_strDepluaFileName;
	CString m_strDepluaFileType;
};