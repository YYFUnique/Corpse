#pragma once

namespace DuiLib
{
	class CRichEditExUI : public CRichEditUI, public IListOwnerUI
	{
	public:
		CRichEditExUI();
		~CRichEditExUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		bool Add(CControlUI* pControl);
		bool AddAt(CControlUI* pControl, int iIndex);
	public:
		virtual TListInfoUI* GetListInfo();
		virtual int GetCurSel() const;
		virtual bool SelectItem(int iIndex, bool bTakeFocus = false);
		virtual void DoEvent(TEventUI& event);
		virtual bool IsMultiSelect();

	private:
		int m_iCurSel;
		TListInfoUI m_ListInfo;
	};
}
