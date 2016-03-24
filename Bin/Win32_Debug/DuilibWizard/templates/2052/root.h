#ifndef _[!output UPPER_CASE_SAFE_PROJECT_IDENTIFIER_NAME]_H_
#define _[!output UPPER_CASE_SAFE_PROJECT_IDENTIFIER_NAME]_H_
#pragma once

class C[!output SAFE_PROJECT_IDENTIFIER_NAME] : public WindowImplBase
{
public:
	C[!output SAFE_PROJECT_IDENTIFIER_NAME]();
	~C[!output SAFE_PROJECT_IDENTIFIER_NAME]();

public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFolder();
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual void Notify(TNotifyUI& msg);
	void InitWindow();
	
	CControlUI* CreateControl(LPCTSTR pstrClass);
protected:
private:
};

#endif //_[!output UPPER_CASE_SAFE_PROJECT_IDENTIFIER_NAME]_H_