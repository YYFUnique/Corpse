#pragma once

class CWMITools 
{
public:
	CWMITools();
	~CWMITools();

public:
	BOOL GetWin32_OperatingSystem(LPCTSTR wszColumnName,CDuiString& strType);
};