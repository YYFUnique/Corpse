#pragma once

namespace DuiLib
{
	class CNavigateNodeUI : public CHorizontalLayoutUI
	{
	public:
		CNavigateNodeUI();
		~CNavigateNodeUI();

	public:
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		bool Add(CControlUI* pControl);
		void PaintStatusImage(HDC hDC);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	public:
		void SetNormalImage(LPCTSTR lpszNormalImage);
		void SetHotImage(LPCTSTR lpszHotImage);
		virtual SIZE EstimateSize(SIZE szAvailable);
	protected:
		bool OnButtonEvent(LPVOID lParam);

	private:
		UINT m_nButtonState;
		CDuiString m_sNormalImage;
		CDuiString m_sHotImage;
	};
}