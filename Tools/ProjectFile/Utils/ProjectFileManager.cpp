#include "StdAfx.h"
#include "ProjectFileManager.h"

CProjectFileManager::CProjectFileManager()
	: m_ProjectFileType(PROJECT_FILE_TYPE_RELATIVE)
	, m_dwNoSuchFileColor(0xFFFF0000)
	, m_dwErrorAttributeColor(0xFF00FF00)
{

}

CProjectFileManager::~CProjectFileManager()
{

}

CString CProjectFileManager::GetProjectName() const
{
	return m_strProjectName;
}

void CProjectFileManager::SetProjectName(LPCTSTR lpszProjectName)
{
	m_strProjectName = lpszProjectName;
}

CString CProjectFileManager::GetProjectFilePath() const
{
	return m_strProjectFilePath;
}

void CProjectFileManager::SetProjectFilePath(LPCTSTR lpszProjectFilePath)
{
	m_strProjectFilePath = lpszProjectFilePath;
}

DWORD CProjectFileManager::GetNoSuchFileColor() const
{
	return m_dwNoSuchFileColor;
}

void CProjectFileManager::SetNoSuchFileColor(DWORD dwNoSuchFileColor)
{
	m_dwNoSuchFileColor = dwNoSuchFileColor;
}

DWORD CProjectFileManager::GetErrorAttributeColor() const
{
	return m_dwErrorAttributeColor;
}

void CProjectFileManager::SetErrorAttributeColor(DWORD dwErrorAttributeColor)
{
	m_dwErrorAttributeColor = dwErrorAttributeColor;
}

PROJECT_FILE_PATH_TYPE CProjectFileManager::GetProjectFileType() const
{
	return m_ProjectFileType;
}

void CProjectFileManager::SetProjectFileType(PROJECT_FILE_PATH_TYPE ProjectFileType)
{
	if (ProjectFileType >= PROJECT_FILE_TYPE_LAST)
		return;

	m_ProjectFileType = ProjectFileType;
}