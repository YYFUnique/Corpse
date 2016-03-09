#include "stdafx.h"
#include "2dline.h"

namespace UI
{

Line::Line()
{
	m_eLineType = LINE_NORMAL; 
	m_pt1.x = m_pt1.y = 0;
	m_pt2.x = m_pt2.y = 0;
}

LINE_INTERSECT HorzLine::Calc2lineIntersect(Line* pLine, POINTF* pPt)
{
	LINE_INTERSECT eRet = LINE_INTERSECT_NO;
	bool  bIntersect = false;

	switch (pLine->m_eLineType)
	{
	case LINE_HORZ:
		{
			bIntersect = false;
		}
		break;

	case LINE_VERT:
		{
			pPt->x = ((VertLine*)pLine)->x;
			pPt->y = y;
			bIntersect = true;
		}
		break;

	case LINE_NORMAL:
		{
			pPt->y = y;
			((NormalLine*)pLine)->GetX(y, &pPt->x);
			bIntersect = true;
		}
		break;

	case LINE_POINT:
		{
			PointLine* p = ((PointLine*)pLine);
			if (p->pt.y == y)
			{
				pPt->y = p->pt.y;
				pPt->x = p->pt.x;
				bIntersect = true;
			}
		}
		break;
	}

	if (bIntersect)
	{
		if ((pPt->x > m_pt1.x && pPt->x > m_pt2.x) ||
			(pPt->x < m_pt1.x && pPt->x < m_pt2.x))
		{
			eRet = LINE_INTERSECT_OUT;
		}
		else
		{
			eRet = LINE_INTERSECT_IN;
		}
	}
	return eRet;
}


float HorzLine::CalcPosPercent(const POINTF& pt) 
{
	return (pt.x - m_pt1.x)/(m_pt2.x - m_pt1.x);
}

LINE_INTERSECT VertLine::Calc2lineIntersect(Line* pLine, POINTF* pPt)
{
	LINE_INTERSECT eRet = LINE_INTERSECT_NO;
	bool  bIntersect = false;

	switch (pLine->m_eLineType)
	{
	case LINE_VERT:
		{
			bIntersect = false;
		}
		break;

	case LINE_HORZ:
		{
			pPt->y = ((HorzLine*)pLine)->y;
			pPt->x = x;
			bIntersect = true;
		}
		break;

	case LINE_NORMAL:
		{
			pPt->x = x;
			((NormalLine*)pLine)->GetY(x, &pPt->y);
			bIntersect = true;
		}
		break;

	case LINE_POINT:
		{
			PointLine* p = ((PointLine*)pLine);
			if (p->pt.x == x)
			{
				pPt->y = p->pt.y;
				pPt->x = p->pt.x;
				bIntersect = true;
			}
		}
		break;
	}

	if (bIntersect)
	{
		if ((pPt->y > m_pt1.y && pPt->y > m_pt2.y) ||
			(pPt->y < m_pt1.y && pPt->y < m_pt2.y))
		{
			eRet = LINE_INTERSECT_OUT;
		}
		else
		{
			eRet = LINE_INTERSECT_IN;
		}
	}
	return eRet;
}


float VertLine::CalcPosPercent(const POINTF& pt) 
{
	return (pt.y - m_pt1.y)/(m_pt2.y - m_pt1.y);
}
LINE_INTERSECT PointLine::Calc2lineIntersect(Line* pLine, POINTF* pPt)
{
	LINE_INTERSECT eRet = LINE_INTERSECT_NO;
	bool  bIntersect = false;

	switch (pLine->m_eLineType)
	{
	case LINE_POINT:
		{
			PointLine* p = ((PointLine*)pLine);
			if (p->pt.x == pt.x && p->pt.y == pt.y)
			{
				pPt->x = pt.x;
				pPt->y = pt.y;
				bIntersect = true;
			}
		}
		break;

	case LINE_HORZ:
		{
			HorzLine* p = ((HorzLine*)pLine);
			if (pt.y == p->y)
			{
				pPt->y = pt.y;
				pPt->x = pt.x;
				bIntersect = true;
			}
		}
		break;

	case LINE_VERT:
		{
			VertLine* p = ((VertLine*)pLine);
			if (pt.x == p->x)
			{
				pPt->y = pt.y;
				pPt->x = pt.x;
				bIntersect = true;
			}
		}
		break;

	case LINE_NORMAL:
		{
			NormalLine* p = (NormalLine*)pLine;

            float y = 0.0f;
            p->GetY(pt.x, &y);

			if (pt.y - y == 0)
			{
				pPt->y = pt.y;
				pPt->x = pt.x;
				bIntersect = true;
			}
		}
		break;
	}

	if (bIntersect)
	{
		eRet = LINE_INTERSECT_IN;
	}
	return eRet;
}

LINE_INTERSECT NormalLine::Calc2lineIntersect(Line* pLine, POINTF* pPt)
{
	LINE_INTERSECT eRet = LINE_INTERSECT_NO;
	bool  bIntersect = false;

	switch (pLine->m_eLineType)
	{
	case LINE_POINT:
		{
			PointLine* p = ((PointLine*)pLine);

            float y = 0.0f;
            GetY(p->pt.x, &y);

			if (p->pt.y - y == 0)
			{
				pPt->y = p->pt.y;
				pPt->x = p->pt.x;
				bIntersect = true;
			}
		}
		break;

	case LINE_HORZ:
		{
			pPt->y = ((HorzLine*)pLine)->y;
			GetX(pPt->y, &pPt->x);
			bIntersect = true;
		}
		break;

	case LINE_VERT:
		{
			pPt->x = ((VertLine*)pLine)->x;
			GetY(pPt->x, &pPt->y);
			bIntersect = true;
		}
		break;

	case LINE_NORMAL:
		{
			NormalLine* p = (NormalLine*)pLine;
			if (p->k - k == 0)  // Æ½ÐÐ
			{
				bIntersect = false;  
			}
			else
			{
				pPt->x = (p->b- b)/(k - p->k);
				GetY(pPt->x, &pPt->y);
				bIntersect = true;
			}
		}
		break;
	}

	if (bIntersect)
	{
		if ((pPt->x > m_pt1.x && pPt->x > m_pt2.x) ||
			(pPt->x < m_pt1.x && pPt->x < m_pt2.x))
		{
			eRet = LINE_INTERSECT_OUT;
		}
		else
		{
			eRet = LINE_INTERSECT_IN;
		}
	}
	return eRet;
}

float NormalLine::CalcPosPercent(const POINTF& pt)
{
	return (pt.x - m_pt1.x)/(m_pt2.x - m_pt1.x);
}
Line* Line::CreateLine(const POINT& pt1, const POINT& pt2)
{
	POINTF dpt1 = {(float)pt1.x, (float)pt1.y};
	POINTF dpt2 = {(float)pt2.x, (float)pt2.y};

	return Line::CreateLine(dpt1, dpt2);
}
Line* Line::CreateLine(const POINTF& pt1, const POINTF& pt2)
{
	Line* pRet = NULL;
	if (pt1.x == pt2.x && pt1.y == pt2.y)
	{
		PointLine* p = new PointLine;
		p->pt.x = pt1.x;
		p->pt.y = pt1.y;

		pRet = p;
	}
	else if (pt1.y == pt2.y)
	{
		HorzLine* p = new HorzLine;
		p->y = pt1.y;

		pRet = p;
	}
	else if (pt1.x == pt2.x)
	{
		VertLine* p = new VertLine;
		p->x = pt1.x;

		pRet = p;
	}
	else 
	{
		NormalLine* p = new NormalLine;
		p->k = (pt2.y - pt1.y)/(pt2.x-pt1.x);
		p->b = pt1.y - p->k * pt1.x;

		pRet = p;
	}

	if (pRet)
	{
		pRet->m_pt1.x = pt1.x;
		pRet->m_pt2.x = pt2.x;
		pRet->m_pt1.y = pt1.y;
		pRet->m_pt2.y = pt2.y;
	}
	return pRet;
}

}