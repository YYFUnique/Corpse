#pragma once

#include "DllCore/Utils/StdPtr.h"

namespace DuiLib
{
	typedef enum tagANIMATE_TYPE_HOVER
	{
		ANIMATE_TYPE_NONE	=	0,
		ANIMATE_TYPE_ENTER	= 1,
		ANIMATE_TYPE_LEAVE	= -1,
	}ANIMATE_TYPE;

	class /*UILIB_API*/ CAnimButtonUI : public CButtonUI
	{
	public:
		CAnimButtonUI();

	public:
		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		virtual void DoEvent(TEventUI& event);
		void PaintStatusImage(HDC hDC);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	public:
		void StopAnimate();
		void SetElapse(UINT uElapse);
		void SetImageList(LPCTSTR pstrValue);
	protected:
		ANIMATE_TYPE m_nAnimateType;
		int m_nHoveState;
		UINT m_uElapse;

		CStdArray			  m_SkinFile;
		CStdStringPtrMap m_ImageSkin;
	};
}