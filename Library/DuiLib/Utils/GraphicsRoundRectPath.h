// GraphicsRoundRectPath.h: interface for the CGraphicsRoundRectPath class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHICSROUNDRECTPATH_H__324A54FD_62A2_44F4_A84C_83870BBD8695__INCLUDED_)
#define AFX_GRAPHICSROUNDRECTPATH_H__324A54FD_62A2_44F4_A84C_83870BBD8695__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
	#ifdef RENDER_GDIPLUS
		#include "./gdiplus/include/GdiPlus.h"

		class CGraphicsRoundRectPath: public Gdiplus::GraphicsPath
		{
		public:
			CGraphicsRoundRectPath(){};
			CGraphicsRoundRectPath(INT x, INT y, INT width, INT height, INT cornerX, INT cornerY,BOOL topLeft=TRUE,BOOL topRight=TRUE,BOOL bottomRight=TRUE,BOOL bottomLeft=TRUE);

		public:
			void AddRoundRect(INT x, INT y, INT width, INT height, INT cornerX, INT cornerY,BOOL topLeft=TRUE,BOOL topRight=TRUE,BOOL bottomRight=TRUE,BOOL bottomLeft=TRUE);
		};
	#endif

#endif // !defined(AFX_GRAPHICSROUNDRECTPATH_H__324A54FD_62A2_44F4_A84C_83870BBD8695__INCLUDED_)
