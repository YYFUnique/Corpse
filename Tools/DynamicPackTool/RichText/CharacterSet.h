#pragma once

BOOL IsSpaceChar(int ch) ;
BOOL IsADigit(int ch);
BOOL IsASCII(int ch);
BOOL IsLowerCase(int ch);
BOOL IsUpperCase(int ch);
CHAR MakeLowerCase(CHAR ch);
CHAR MakeUpperCase(CHAR ch);

int CompareCaseInsensitive(const char *a, const char *b);
int CompareNCaseInsensitive(const char *a, const char *b, size_t len);