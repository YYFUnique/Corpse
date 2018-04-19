#pragma once

class CDeviceNotify : public CNotifyPump
{
public:
	CDeviceNotify();
	~CDeviceNotify();

public:
	void SetPaintMagager(CPaintManagerUI* pPaintMgr);

	BOOL OnDeviceChanged(WPARAM wParam, LPARAM lParam);
protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnLoadItem(TNotifyUI& msg);
private:
	HANDLE				m_hDevNotify;
	CPaintManagerUI* m_pPaintManager;
};