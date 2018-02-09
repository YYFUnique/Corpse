#pragma once

#include "DllCore/Utils/StdPtr.h"

typedef struct tagPROJECT_DETAILS_INFO
{
	CStdArray	strMainFile;
	CStdArray	strIncludeFile;
	CStdArray	strResFile;
}PROJECT_DETAILS_INFO,*PPROJECT_DETAILS_INFO;

class CProjectTool
{
public:
	CProjectTool();
	~CProjectTool();

public:
	BOOL GetProjectInfo(LPCTSTR lpszProjectFilePath, PROJECT_DETAILS_INFO& ProjectDetailInfo);
};