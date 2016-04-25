#pragma once

namespace DuiLib
{
	class CTileListCtrlUI : public CTileLayoutUI , public IListOwnerUI
	{
	public:
		CTileListCtrlUI(CPaintManagerUI& PaintManager);
		virtual ~CTileListCtrlUI();
	public:
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		virtual TListInfoUI* GetListInfo();
		virtual int GetCurSel() const;
		virtual void DoEvent(TEventUI& event);
		virtual bool Add(CControlUI* pControl);
		virtual bool AddNode(LPARAM lParam);
		virtual bool IsMultiSelect();
		virtual bool SelectItem(int iIndex, bool bTakeFocus = false);

		void EnsureVisible(int iIndex);
		void Scroll(int dx, int dy);

	protected:
		int m_iCurSel;
		TListInfoUI m_ListInfo;
		CDialogBuilder m_DialogBuilder;
		CPaintManagerUI& m_PaintManager;
	};
}