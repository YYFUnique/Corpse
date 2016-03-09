#ifndef __UIGROUPLAYOUT_H__
#define __UIGROUPLAYOUT_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CGroupLayoutUI : public CContainerUI
	{
	public:
		CGroupLayoutUI();
		~CGroupLayoutUI();

		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual LPCTSTR GetClass() const;
		virtual void SetPos(RECT rc);
		void PaintStatusImage(HDC hDC);
		void PaintText(HDC hDC);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		virtual void PaintBorder(HDC hDC);
	public:
		void SetGroupRoundImage(LPCTSTR pstrValue);
	private:
		DWORD  m_dwTextColor;
		HDC			m_hBackground;
		HBITMAP m_hBitMap;
		RECT        m_rcTextArea;
		CDuiString m_strGroupBoxRoundRectImage;
	};
}

#endif //__UIGROUPLAYOUT_H__