#pragma once

//当type为CP_ACP时，UNICODE转化为GBK；当type为CP_UTF8时，UNICODE转化为UTF8
BOOL Transform(LPCWSTR lpszwch, int& nSize, LPSTR lpszConvert, int nType = CP_ACP);

// 当type为CP_ACP时，GBK转化为UNICODE；当type为CP_UTF8时，UTF8转化为UNICODE
BOOL Transform(LPCSTR lpszch, int& nSize, LPWSTR lpszConvert, int nType = CP_ACP);