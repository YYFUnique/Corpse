#pragma once

#define DUI_CTR_TABOPTION	_T("TabOption")

class CTabOptionUI : public CButtonUI
{
public:
	CTabOptionUI();
	~CTabOptionUI();
public:
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	bool Activate();
	void SetEnabled(bool bEnable = true);

	LPCTSTR GetGroup() const;
	void SetGroup(LPCTSTR pStrGroupName = NULL);
	bool IsSelected() const;
	virtual void Selected(bool bSelected);

	SIZE EstimateSize(SIZE szAvailable);

	void PaintStatusImage(HDC hDC);
	void PaintText(HDC hDC);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);
public:
	void SetOwner(IOwner* pOwner);
public:
	LPCTSTR GetForeImage();
	void SetForeImage(LPCTSTR pStrImage);
	void SetSelectedTextColor(DWORD dwTextColor);
	DWORD GetSelectedTextColor();
	void SetSelectedBkColor(DWORD dwBkColor);
	DWORD GetSelectBkColor();
	void SetHeadNormalImage(LPCTSTR lpszHeadNormal);
	void SetHeadHotImage(LPCTSTR lpszHeadHotImage);
	void SetHeadSelectedImage(LPCTSTR lpszHeadSelected);
	void SetNormalImage(LPCTSTR lpszNormalImage);
	void SetNormalHotImage(LPCTSTR lpszNormalHot);
	void SetPreSelectedImage(LPCTSTR lpszPreSelectedImage);
	void SetPreSelectedHotImage(LPCTSTR lpszPreSelectedHotImage);
	void SetNextSelectedImage(LPCTSTR lpszNextSelectedImage);
	void SetNextSelectedHotImage(LPCTSTR lpszNextSelectedImage);
	void SetSelectedImage(LPCTSTR lpszSelectedImage);
	void SetSelectedHotImage(LPCTSTR lpszSelectedHotImage);
	void SetSelectedPressImage(LPCTSTR lpszSelectedPressImage);
protected:
	IOwner*	m_pOwner;

	bool					m_bSelected;
	CDuiString		m_sGroupName;

	DWORD			m_dwSelectedBkColor;
	DWORD			m_dwSelectedTextColor;

	CDuiString m_strHeadNormalImage;
	CDuiString m_strHeadHotImage;
	CDuiString m_strHeadSelectedImage;
	CDuiString m_strNormalImage;
	CDuiString m_strNormalHotImage;
	CDuiString m_strPreSelectedImage;
	CDuiString m_strPreSelectedHotImage;
	CDuiString m_strNextSelectedImage;
	CDuiString m_strNextSelectedHotImage;
	CDuiString m_strSelectedImage;
	CDuiString m_strSelectedHotImage;
	CDuiString m_strSelectedPressImage;
};