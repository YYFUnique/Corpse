#pragma once

namespace UI
{

// 计算两条直线的交点
enum LINE_TYPE
{
	LINE_POINT,
	LINE_HORZ,
	LINE_VERT,
	LINE_NORMAL
};

enum LINE_INTERSECT
{
	LINE_INTERSECT_NO,  // 两条线不相交
	LINE_INTERSECT_OUT, // 两条线相交，但交点不在线段内
	LINE_INTERSECT_IN,  // 两条线相交，交点在线段内
};


struct POINTF
{
	float x;
	float y;
};

class Line
{
public:
	Line();
	virtual ~Line() {};

	virtual LINE_INTERSECT    Calc2lineIntersect(Line* pLine, POINTF* pPt) = 0;
	virtual float  CalcPosPercent(const POINTF& pt) = 0;
	virtual bool  GetY(float x, float* y) = 0;
	virtual bool  GetX(float y, float* x) = 0;

	static Line* CreateLine(const POINTF& pt1, const POINTF& pt2);
	static Line* CreateLine(const POINT& pt1, const POINT& pt2);

public:
	LINE_TYPE  m_eLineType;
	POINTF  m_pt1;
	POINTF  m_pt2;
};
class PointLine : public Line
{
public:
	PointLine() 
	{ 
		m_eLineType = LINE_POINT;
        pt.x = pt.y = 0.0f;
	}
	virtual LINE_INTERSECT    Calc2lineIntersect(Line* pLine, POINTF* pPt);
	virtual float  CalcPosPercent(const POINTF& pt) { return 0; }
	virtual bool  GetY(float x, float* y)
	{
		if (x != m_pt1.x)
			return false;

		if (y)
			*y = m_pt1.y;
		return true;
	}
	virtual bool  GetX(float y, float* x)
	{
		if (y != m_pt1.y)
			return false;

		if (x)
			*x = m_pt1.x;
		return true;
	}

public:
    POINTF  pt;
};

class HorzLine : public Line
{
public:
	HorzLine() { m_eLineType = LINE_HORZ; y = 0;}
	virtual LINE_INTERSECT    Calc2lineIntersect(Line* pLine, POINTF* pPt);
	virtual float  CalcPosPercent(const POINTF& pt);

	virtual bool  GetY(float x, float* y)
	{
		if (y)
			*y = m_pt1.y;
		return true;
	}
	virtual bool  GetX(float y, float* x)
	{
		return false;
	}
public:
	float y;
};

class VertLine : public Line
{
public:
	VertLine() { m_eLineType = LINE_VERT; x = 0; }
	virtual LINE_INTERSECT    Calc2lineIntersect(Line* pLine, POINTF* pPt);
	virtual float  CalcPosPercent(const POINTF& pt);

	virtual bool  GetY(float x, float* y)
	{
		return false;
	}
	virtual bool  GetX(float y, float* x)
	{
		if (x)
			*x = m_pt1.x;
		return true;
	}
public:
	float x;
};

class NormalLine : public Line
{
public:
	NormalLine()
	{
		m_eLineType = LINE_NORMAL;
		k = 0; b = 0;
	}
	virtual LINE_INTERSECT  Calc2lineIntersect(Line* pLine, POINTF* pPt);
	virtual float  CalcPosPercent(const POINTF& pt);

	virtual bool  GetY(float x, float* y)
	{
		if (y)
			*y = k*x + b;
		return true;
	}
	virtual bool  GetX(float y, float* x)
	{
		if (x)
			*x = (y - b) / k; // k为0时就不属于normalline了;;
		return true;
	}

public:
	float k;
	float b;
};

}