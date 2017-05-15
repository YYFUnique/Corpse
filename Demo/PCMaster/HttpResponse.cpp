#include "StdAfx.h"
#include "HttpResponse.h"

int CHTTPResponse::Progress(LPVOID lpData, size_t size, size_t nmemb)
{
	printf( "\n ===== CLibcurl::process( void* data, size_t size, size_t nmemb ) ===== \n" );
	return size * nmemb; 
}