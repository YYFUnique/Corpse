#include "StdAfx.h"
#include "CharacterSet.h"

BOOL IsSpaceChar(int ch) {
	return (ch == ' ') || ((ch >= 0x09) && (ch <= 0x0d));
}

BOOL IsADigit(int ch) {
	return (ch >= '0') && (ch <= '9');
}

BOOL IsASCII(int ch) {
	return (ch >= 0) && (ch < 0x80);
}

BOOL IsLowerCase(int ch) {
	return (ch >= 'a') && (ch <= 'z');
}

BOOL IsUpperCase(int ch) {
	return (ch >= 'A') && (ch <= 'Z');
}

CHAR MakeLowerCase(CHAR ch)
{
	if (ch < 'A' || ch > 'Z')
		return ch;
	else
		return static_cast<CHAR>(ch - 'A' + 'a');
}

CHAR MakeUpperCase(CHAR ch) {
	if (ch < 'a' || ch > 'z')
		return ch;
	else
		return static_cast<char>(ch - 'a' + 'A');
}

int CompareCaseInsensitive(const char *a, const char *b)
{
	while (*a && *b) {
		if (*a != *b) {
			char upperA = MakeUpperCase(*a);
			char upperB = MakeUpperCase(*b);
			if (upperA != upperB)
				return upperA - upperB;
		}
		a++;
		b++;
	}
	// Either *a or *b is nul
	return *a - *b;
}

int CompareNCaseInsensitive(const char *a, const char *b, size_t len)
{
	while (*a && *b && len) {
		if (*a != *b) {
			char upperA = MakeUpperCase(*a);
			char upperB = MakeUpperCase(*b);
			if (upperA != upperB)
				return upperA - upperB;
		}
		a++;
		b++;
		len--;
	}
	if (len == 0)
		return 0;
	else
		// Either *a or *b is nul
		return *a - *b;
}
