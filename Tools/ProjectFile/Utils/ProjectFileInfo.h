#pragma once

class CProjectFileInfo : public IMarkup
{
public:
	CProjectFileInfo();
public:
	LPVOID GetInterface(LPCTSTR pstrName);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

public:
	void SetFileName(LPCTSTR lpszFileName);
	CString GetFileName() const;

	CString GetDestFileName() const;
	void SetDestFileName(LPCTSTR lpszProjectFilePath);

	void SetFileSrcPath(LPCTSTR lpszFilePath);
	CString GetFileSrcPath() const;

	void SetFileDestPath(LPCTSTR lpszFilePath);
	CString GetFileDestPath() const;

protected:
	CString		m_strFileName;
	CString		m_strDestName;
	CString		m_strFileSrc;
	CString		m_strFileDest;
};