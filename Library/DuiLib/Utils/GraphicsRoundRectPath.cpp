// GraphicsRoundRectPath.cpp: implementation of the CGraphicsRoundRectPath class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#ifdef RENDER_GDIPLUS
	#include "GraphicsRoundRectPath.h"

	#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[]=__FILE__;
	#define new DEBUG_NEW
	#endif

	CGraphicsRoundRectPath::CGraphicsRoundRectPath(INT x, INT y, INT width, INT height, INT cornerX, INT cornerY,BOOL topLeft,BOOL topRight,BOOL bottomRight,BOOL bottomLeft)
		:Gdiplus::GraphicsPath()
	{
		AddRoundRect(x,y,width,height,cornerX,cornerY,topLeft,topRight,bottomRight,bottomLeft);
	}
	void CGraphicsRoundRectPath::AddRoundRect(INT x, INT y, INT width, INT height, INT cornerX, INT cornerY,BOOL topLeft,BOOL topRight,BOOL bottomRight,BOOL bottomLeft)
	{
		INT elWid = 2*cornerX;
		INT elHei = 2*cornerY;

		if(topLeft)
		{
			AddArc(x,y,elWid,elHei,180,90); // ◊Û…œΩ«‘≤ª°
			AddLine(x+cornerX,y,x+width-(topRight?cornerX:0),y); // …œ±ﬂ
		}
		else
		{
			AddLine(x,y,x+width-(topRight?cornerX:0),y);
		}

		if(topRight)
		{
			AddArc(x+width-elWid,y, elWid,elHei,270,90); // ”“…œΩ«‘≤ª°
			AddLine(x+width,y+cornerY, x+width,y+height-(bottomRight?cornerY:0));// ”“±ﬂ
		}
		else
		{
			AddLine(x+width,y, x+width,y+height-(bottomRight?cornerY:0));// ”“±ﬂ
		}

		if(bottomRight)
		{
			AddArc(x+width-elWid,y+height-elHei, elWid,elHei,0,90); // ”“œ¬Ω«‘≤ª°
			AddLine(x+width-cornerX,y+height, x+(bottomLeft?cornerX:0),y+height); // œ¬±ﬂ
		}
		else
		{
			AddLine(x+width,y+height, x+(bottomLeft?cornerX:0),y+height); // œ¬±ﬂ
		}

		if(bottomLeft)
		{
			AddArc(x,y+height-elHei, elWid,elHei,90,90); 
			AddLine(x,y+(topLeft?cornerY:0), x, y+height-cornerY);
		}
		else
		{
			AddLine(x,y+(topLeft?cornerY:0), x, y+height);
		}

	}
#endif