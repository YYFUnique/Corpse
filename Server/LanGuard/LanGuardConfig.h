#pragma once

class CLanGuardConfig
{
public:
	CLanGuardConfig();

public:
	BOOL Load(LPCTSTR lpszConfigPath);
protected:
	~CLanGuardConfig();
};