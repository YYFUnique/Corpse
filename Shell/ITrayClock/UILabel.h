#pragma once

class CLabelUI 
{
public:
	CLabelUI();
	~CLabelUI();

public:
	void DoEvent(TEventUI& event);
	void DoPaint(HDC hDC, const RECT& rcPaint);
};