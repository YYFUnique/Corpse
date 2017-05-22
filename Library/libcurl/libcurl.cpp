#include "libcurl.h"
#include <crtdbg.h>

#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif

typedef struct LPARAMINFO
{
	CLibcurl*  pLibcurl;
	DWORD	 dwEvent;
}LPARAMINFO,*PLPARAMINFO;

CLibcurl::CLibcurl() 
	:m_pCurl(NULL)
	,m_pCallback(NULL)
{
	Init();
}

CLibcurl::~CLibcurl()
{
	if (m_pCurl != NULL)
	{
		::curl_easy_cleanup(m_pCurl);
		m_pCurl = NULL;
	}
    ::curl_global_cleanup(); 
}

BOOL CLibcurl::Init()
{
    m_curlCode = ::curl_global_init(CURL_GLOBAL_ALL);
    if (CURLE_OK != m_curlCode)
		return FALSE;

    m_pCurl = ::curl_easy_init(); 
    if (NULL == m_pCurl)
		return FALSE;

    return TRUE;
}

BOOL CLibcurl::SetCallback(ILibcurlCallback* pCallback)
{
	m_pCallback = pCallback;
	return TRUE;
}

BOOL CLibcurl::SetUserAgent(LPCTSTR lpszAgent)
{
	if (lpszAgent == NULL)
		return FALSE;
	
	int nLen = _tcslen(lpszAgent);
	if (nLen == 0)
		return FALSE;

	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_USERAGENT, lpszAgent);  

	return CURLE_OK == m_curlCode; 
}

BOOL CLibcurl::doHttpPost(DWORD dwEvent, LPCTSTR lpszURL, LPCTSTR lpszData, DWORD dwTimeout /*=10*/, DWORD dwHeader /*=0*/)
{
    ResetOpt();
    ::curl_easy_setopt(m_pCurl, CURLOPT_HEADER, dwHeader);
    ::curl_easy_setopt(m_pCurl, CURLOPT_URL, CStringA(lpszURL));
    ::curl_easy_setopt(m_pCurl, CURLOPT_POST, 1 );
    ::curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, CStringA(lpszData));
    ::curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, dwTimeout);
    ::curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, CLibcurl::ProcessFunc); 

	LPARAMINFO lParamInfo;
	lParamInfo.pLibcurl = this;
	lParamInfo.dwEvent = dwEvent;

	::curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, &lParamInfo); 
    m_curlCode = ::curl_easy_perform(m_pCurl);

	return CURLE_OK == m_curlCode; 
}

BOOL CLibcurl::doHttpGet(DWORD dwEvent, LPCTSTR lpszURL, LPCTSTR lpszData /*=NULL*/, DWORD dwTimeout /*=10*/, DWORD dwHeader /*= 0*/)
{
    ResetOpt();
    
	//由于在HTTP中使用GET方式获取网页，对地址长度有限制，故这里直接使用栈内存
	CString strHttpUrl(lpszURL);

	if (lpszData != NULL)
	{
		strHttpUrl.TrimRight(_T("?"));
		strHttpUrl.AppendFormat(_T("?%s"), lpszData);
	}

    ::curl_easy_setopt(m_pCurl, CURLOPT_URL, CStringA(strHttpUrl));
    ::curl_easy_setopt(m_pCurl, CURLOPT_HTTPGET, 1 );
    ::curl_easy_setopt(m_pCurl, CURLOPT_HEADER, dwHeader);
    ::curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, dwTimeout);
    ::curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, CLibcurl::ProcessFunc); 

	LPARAMINFO lParamInfo;
	lParamInfo.pLibcurl = this;
	lParamInfo.dwEvent = dwEvent;

    ::curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, &lParamInfo); 

    m_curlCode = ::curl_easy_perform(m_pCurl);

	return CURLE_OK == m_curlCode;
}

void CLibcurl::ResetOpt()
{
    ::curl_easy_reset(m_pCurl);
    //::curl_easy_setopt( m_curl, CURLOPT_VERBOSE, 1 );//set this to show debug message
}

size_t CLibcurl::ProcessFunc(LPVOID lpData, size_t size, size_t nmemb, LPVOID lParam)
{
	size_t nSizeRet = 0;
    LPARAMINFO* plParamInfo =(LPARAMINFO*)lParam;
	if (plParamInfo == NULL)
		return nmemb * size;

	if (plParamInfo->pLibcurl->m_pCallback != NULL)
		nSizeRet = plParamInfo->pLibcurl->m_pCallback->Progress(plParamInfo->dwEvent, lpData, size, nmemb);
	else
		nSizeRet = plParamInfo->pLibcurl->process(lpData, size, nmemb);

	return nSizeRet;
}

BOOL CLibcurl::GetErrorInfo(CString& strErrorInfo) const
{
	if (m_curlCode == CURLE_OK)
		return FALSE;

	strErrorInfo.Format(_T("%s"), CString(curl_easy_strerror(m_curlCode)));
	
	return TRUE;
}

int CLibcurl::process( void* data, size_t size, size_t nmemb ) 
{
    printf( "\n ===== CLibcurl::process( void* data, size_t size, size_t nmemb ) ===== \n" ); 
    return size * nmemb; 
}

//int CLibcurl::uploadFile( const char* url, const char* localfilename )
//{
//    m_upfilename = (char*)localfilename;
//    resetOpt();
//    ::curl_easy_setopt( m_curl, CURLOPT_URL, url );/**
//    ::curl_easy_setopt( m_curl, CURLOPT_UPLOAD, 1 );
//    ::curl_easy_setopt( m_curl, CURLOPT_READFUNCTION, CLibcurl::uploadFileReadFunc ); 
//    ::curl_easy_setopt( m_curl, CURLOPT_READDATA, this ); 
//    CURLcode res = ::curl_easy_perform( m_curl);
//    return ( 0 == res ) ? 0 : -1;
//}

//size_t CLibcurl::uploadFileReadFunc( void* ptr, size_t size, size_t nmemb, void *usrptr )
//{
//    CLibcurl* p =(CLibcurl*) usrptr;
//    static FILE* pf = NULL;
//    if( !pf ) 
//    {
//        pf = fopen( p->m_upfilename, "r" );
//        if( !pf )    
//        {
//            fprintf( stderr, "cannot open file: %s \n", p->m_upfilename ); 
//            return 0;
//        }
//    }
//
//    if( pf )
//    {
//         int nread = fread( ptr, size, nmemb, pf );
//         if( !nread )
//         {
//             fclose( pf );
//             pf = NULL;    
//          }
//          return nread;
//     }
//
//     return 0;
//}