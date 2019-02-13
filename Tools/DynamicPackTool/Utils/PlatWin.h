#pragma once

class Platfrom
{
public:
	Platfrom(const Platfrom&);
	Platfrom &operator=(const Platfrom&) ;
public:
	Platfrom();
	~Platfrom();
public:
	static int Clamp(int nVla, int nMinVal, int nMaxVal);
};