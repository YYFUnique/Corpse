#pragma once

namespace DuiLib
{
	class CNavigateBarUI : public CHorizontalLayoutUI
	{
	public:
		CNavigateBarUI();
		~CNavigateBarUI();

	public:
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		/*UINT GetControlFlags() const;*/

		void SetFont(int nFont);
		void SetNavigatePath(LPCTSTR pstrValue);

		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	protected:
		
		bool EventNotify(LPVOID lParam);
		void PraseItemNode();
		void SwitchInternVisible(bool bVisible = true);
	protected:
		BOOL			m_bClicked;
		CEditUI2*		m_pNavigateBar;
		//CDuiString	m_strNavigatePath;
	};
}