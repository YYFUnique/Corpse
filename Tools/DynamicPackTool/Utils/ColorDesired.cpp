#include "StdAfx.h"
#include "ColorDesired.h"

ColorDesired::ColorDesired(COLORREF lCol /*= 0*/)
	: m_lColor(lCol)
{

}

ColorDesired::ColorDesired(UINT nRed, UINT nGreen, UINT nBlue)
{
	Set(nRed, nGreen, nBlue);
}

BOOL ColorDesired::operator ==(const ColorDesired &other) const
{
	return m_lColor == other.m_lColor;
}

void ColorDesired::Set(COLORREF lCol)
{
	m_lColor = lCol;
}

void ColorDesired::Set(UINT nRed, UINT nGreen, UINT nBlue)
{
	m_lColor = RGB(nRed, nGreen, nBlue);
}

void ColorDesired::Set(const char* val)
{
	if (*val == '#') 
		val++;

	UINT nRed = GetValueOfHex(val[0]) * 16 + GetValueOfHex(val[1]);
	UINT nGreen = GetValueOfHex(val[2]) * 16 + GetValueOfHex(val[3]);
	UINT nBlue = GetValueOfHex(val[4]) * 16 + GetValueOfHex(val[5]);

	Set(nRed, nGreen, nBlue);
}

COLORREF ColorDesired::GetColor() const
{
	return m_lColor;
}

UINT ColorDesired::GetRed() const
{
	return GetRValue(m_lColor);
}

UINT ColorDesired::GetGreen() const
{
	return GetGValue(m_lColor);
}

UINT ColorDesired::GetBlue() const
{
	return GetBValue(m_lColor);
}

UINT ColorDesired::GetValueOfHex(const char chColorVal) 
{
	if (chColorVal >= '0' && chColorVal <= '9')
		return chColorVal - '0';
	else if (chColorVal >= 'A' && chColorVal <= 'F')
		return chColorVal - 'A' + 10;
	else if (chColorVal >= 'a' && chColorVal <= 'f')
		return chColorVal - 'a' + 10;
	else
		return 0;
}