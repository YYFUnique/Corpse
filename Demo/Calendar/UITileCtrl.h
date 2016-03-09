#pragma once

namespace DuiLib
{
	class CTileCtrlUI : public CTileLayoutUI , public IListOwnerUI
	{
	public:
		CTileCtrlUI(CPaintManagerUI& PaintManager,SIZE& ItemSize);
		virtual ~CTileCtrlUI();
	public:
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		virtual TListInfoUI* GetListInfo();
		virtual int GetCurSel() const;
		virtual void DoEvent(TEventUI& event);
		virtual bool Add(CControlUI* pControl);
		virtual bool AddNode(LPARAM lParam);
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