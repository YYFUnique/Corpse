#pragma once
#include "libcurl/libcurl.h"

class CHTTPResponse : public ILibcurlCallback
{
public:
	int Progress(LPVOID lpData, size_t size, size_t nmemb);
};