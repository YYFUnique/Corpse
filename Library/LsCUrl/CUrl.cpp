#include "stdafx.h"
#include "CUrl.h"

CCUrl::CCUrl() 
	: m_curl(NULL)
{
}

CCUrl::~CCUrl()
{
    ::curl_easy_cleanup( m_curl ); 
    ::curl_global_cleanup(); 
}

int CCUrl::init()
{
    CURLcode res = ::curl_global_init( CURL_GLOBAL_ALL );
    if( CURLE_OK != res ) 
    {
        fprintf( stderr, "curl_global_init failed: %d \n", res ); 
        return -1;
    }

    m_curl = ::curl_easy_init(); 
    if( NULL == m_curl )
    {
        fprintf( stderr, "curl_easy_init failed\n" ); 
        return -1;
    }
    return 0;
}

int CCUrl::doHttpPost(LPCTSTR lpszUrl,LPCTSTR lpszData, DWORD dwTimeout /* = 10 */, DWORD dwWithheader /* = 0 */ )
{
	CStringA strUrl(lpszUrl);
	CStringA strData(lpszData);
    resetOpt();
    ::curl_easy_setopt( m_curl, CURLOPT_HEADER, dwWithheader );
    ::curl_easy_setopt( m_curl, CURLOPT_URL, strUrl );
    ::curl_easy_setopt( m_curl, CURLOPT_POST, 1 );
    ::curl_easy_setopt( m_curl, CURLOPT_POSTFIELDS, strData );
    ::curl_easy_setopt( m_curl, CURLOPT_TIMEOUT, dwTimeout );
    ::curl_easy_setopt( m_curl, CURLOPT_WRITEFUNCTION, CCUrl::processFunc ); 
    ::curl_easy_setopt( m_curl, CURLOPT_WRITEDATA, this );
    CURLcode res = ::curl_easy_perform( m_curl);
    return ( 0 == res ) ? 0 : -1;
}

int CCUrl::doHttpGet(LPCTSTR lpszUrl,LPCTSTR lpszData, DWORD dwTimeout /* = 10 */, DWORD dwWithheader /* = 0 */ )
{
    resetOpt();
	CString strHtmlFileName(lpszUrl);
	if (strHtmlFileName.Left(7).CompareNoCase(_T("http://")) != 0)
		strHtmlFileName.Insert(0,_T("http://"));
   
	CString strData(lpszData);
	if (strData.Left(1).CompareNoCase(_T("?")) != 0)
		strData.Insert(0,_T("?"));

	strHtmlFileName += strData;

	CStringA strHtml(strHtmlFileName);

    ::curl_easy_setopt( m_curl, CURLOPT_URL, strHtml );
    ::curl_easy_setopt( m_curl, CURLOPT_HTTPGET, 1 );
    ::curl_easy_setopt( m_curl, CURLOPT_HEADER, dwWithheader );
    ::curl_easy_setopt( m_curl, CURLOPT_TIMEOUT, dwTimeout );
    ::curl_easy_setopt( m_curl, CURLOPT_WRITEFUNCTION, CCUrl::processFunc ); 
    ::curl_easy_setopt( m_curl, CURLOPT_WRITEDATA, this ); 

    CURLcode ErrorCode = ::curl_easy_perform( m_curl);

    return ErrorCode;
}

void CCUrl::resetOpt()
{
    ::curl_easy_reset( m_curl );
    //::curl_easy_setopt( m_curl, CURLOPT_VERBOSE, 1 );//set this to show debug message

}

size_t CCUrl::processFunc( void* ptr, size_t size, size_t nmemb, void *usrptr )
{
    CCUrl* p =(CCUrl*) usrptr;
	if (p->m_Process != NULL)
		return p->m_Process( ptr, size, nmemb );
	else
		return p->process( ptr, size, nmemb );
}



int CCUrl::process( void* data, size_t size, size_t nmemb ) 
{
    printf( "\n ===== CCUrl::process( void* data, size_t size, size_t nmemb ) ===== \n" ); 
    return size * nmemb;
}

BOOL CCUrl::SetProcessFunc(CURL_PROCESS Process)
{
	if (Process != NULL)
		m_Process = Process;
	return m_Process != NULL;
}

//     int CCUrl::uploadFile( const char* url, const char* localfilename )

//     {

//         m_upfilename = (char*)localfilename;

//         resetOpt();

//         ::curl_easy_setopt( m_curl, CURLOPT_URL, url );/**<Upload to this url place*/

//         ::curl_easy_setopt( m_curl, CURLOPT_UPLOAD, 1 );

//         ::curl_easy_setopt( m_curl, CURLOPT_READFUNCTION, CCUrl::uploadFileReadFunc ); 

//         ::curl_easy_setopt( m_curl, CURLOPT_READDATA, this ); 

//         CURLcode res = ::curl_easy_perform( m_curl);

//         return ( 0 == res ) ? 0 : -1;

//     }

//

//     size_t CCUrl::uploadFileReadFunc( void* ptr, size_t size, size_t nmemb, void *usrptr )

//     {

//         CCUrl* p =(CCUrl*) usrptr;

//         static FILE* pf = NULL;

//         if( !pf ) 

//         {

//             pf = fopen( p->m_upfilename, "r" );

//             if( !pf )    

//             {

//                 fprintf( stderr, "cannot open file: %s \n", p->m_upfilename ); 

//                 return 0;

//             }

//         }

// 

//         if( pf )

//         {

//             int nread = fread( ptr, size, nmemb, pf );

//             if( !nread )

//             {

//                 fclose( pf );

//                 pf = NULL;    

//             }

//             

//             return nread;

//         }

// 

//         return 0;

//     }



 //end namespace
