#pragma once

typedef enum _tagTIME_TYPE
{
	TIME_TYPE_MORNING = 0,
	TIME_TYPE_NOON,
	TIME_TYPE_AFTERNOON,
	TIME_TYPE_NIGHT,
}TIME_TYPE;

class CFlashDemo : public WindowImplBase
{
public:
	CFlashDemo();
	~CFlashDemo();

public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual LPCTSTR GetResourceID() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	void OnClick(TNotifyUI& msg);
	void OnShowActiveX(TNotifyUI& msg);
	void SetDemoBkImage();
private:
	TIME_TYPE m_TimeType;
	CDuiString m_strFlashPath;
};