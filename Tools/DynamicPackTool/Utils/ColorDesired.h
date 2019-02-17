#pragma once

/**
 * Holds a desired RGB colour.
 */

class ColorDesired
{
public:
	ColorDesired(COLORREF lCol = 0);
	ColorDesired(UINT nRed, UINT nGreen, UINT nBlue);
public:
	BOOL operator== (const ColorDesired& other) const;
public:
	void Set(COLORREF lCol);
	void Set(UINT nRed, UINT nGreen, UINT nBlue);
	void Set(const char* val);
	COLORREF GetColor() const;
	UINT GetRed() const;
	UINT GetGreen() const;
	UINT GetBlue() const;

	static UINT GetValueOfHex(const char chColorVal) ;
private:
	COLORREF m_lColor;
};