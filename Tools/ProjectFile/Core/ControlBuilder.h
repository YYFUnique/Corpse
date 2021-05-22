#pragma once

class CControlBuilder : public IDialogBuilderCallback
{
public:
	CControlBuilder();
	~CControlBuilder();

public:
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
};