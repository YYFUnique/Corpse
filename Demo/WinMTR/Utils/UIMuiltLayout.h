#pragma once

#define DUI_CTR_IWONER				_T("IOwner")
#define DUI_CTR_MUILTLAYOUT	_T("MuiltLayout")

class IOwner
{
public:
	~IOwner(){}
public:
	virtual void Notify(LPCTSTR lpszNotify) = 0;
};

class CMuiltLayoutUI : public CContainerUI, public IOwner
{
public:
	CMuiltLayoutUI();
	~CMuiltLayoutUI();
public:
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void SetText(int iIndex, LPCTSTR lpszText);
	bool Add(CControlUI* pControl);
	bool AddAt(CControlUI* pControl, int iIndex);
	bool Remove(CControlUI* pControl);
	void RemoveAll();
	int GetCurSel() const;
	bool SelectItem(int iIndex);
	bool SelectItem(CControlUI* pControl);
public:
	virtual void Notify(LPCTSTR lpszNotify);
protected:
	CTabLayoutUI*			m_pBody;
	CHorizontalLayoutUI* m_pHeader;
};