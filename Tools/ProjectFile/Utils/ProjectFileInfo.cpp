#include "StdAfx.h"
#include "ProjectFileInfo.h"

CProjectFileInfo::CProjectFileInfo()
{

}

LPVOID CProjectFileInfo::GetInterface(LPCTSTR pstrName)
{
	if (_tcsicmp(pstrName, MARK_FILE) == 0)
		return static_cast<CProjectFileInfo*>(this);
	
	return NULL;
}

void CProjectFileInfo::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcsicmp(pstrName, _T("name")) == 0)
		SetFileName(pstrValue);
	else if (_tcsicmp(pstrName, _T("src")) == 0)
		SetFileSrcPath(pstrValue);
	else if (_tcsicmp(pstrName, _T("dest")) == 0)
		SetFileDestPath(pstrValue);
	else if (_tcsicmp(pstrName, _T("destname")) == 0)
		SetDestFileName(pstrValue);
}

void CProjectFileInfo::SetFileName(LPCTSTR lpszFileName)
{
	m_strFileName = lpszFileName;
}

CString CProjectFileInfo::GetFileName() const
{
	return m_strFileName;
}

void CProjectFileInfo::SetDestFileName(LPCTSTR pstrValue)
{
	m_strDestName = pstrValue;
}

CString CProjectFileInfo::GetDestFileName() const
{
	return m_strDestName;
}

void CProjectFileInfo::SetFileSrcPath(LPCTSTR lpszFilePath)
{
	m_strFileSrc = lpszFilePath;
}

CString CProjectFileInfo::GetFileSrcPath() const
{
	return m_strFileSrc;
}

void CProjectFileInfo::SetFileDestPath(LPCTSTR lpszFilePath)
{
	m_strFileDest = lpszFilePath;
}

CString CProjectFileInfo::GetFileDestPath() const
{
	return m_strFileDest;
}
