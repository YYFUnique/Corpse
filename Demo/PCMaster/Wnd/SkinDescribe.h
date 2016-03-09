#pragma once

// inline double CalculateDelay(double state) {
// 	return pow(state, 2);
// }

class CSkinDescribeUI : public CTileLayoutUI
{
public:
	enum { SCROLL_TIMERID = 10 };
	CSkinDescribeUI();
	~CSkinDescribeUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void DoEvent(TEventUI& event);
protected:
	UINT m_uButtonState;
	POINT m_ptLastMouse;
	LONG m_dwDelayDeltaY;
	DWORD m_dwDelayNum;
	DWORD m_dwDelayLeft;
};