#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_CURL_API 
#elif defined  LS_EXPORTS
#define LS_CURL_API __declspec(dllexport)
#else
#define LS_CURL_API __declspec(dllimport)
#endif

#include "curl/curl.h"
//#include <curl/types.h>
#include "curl/easy.h"
//const long CLIENTTIMEOUT = 10;

/*
*返回处理函数
*/
typedef int (*CURL_PROCESS)( void* data, size_t size, size_t nmemb );

/*! @class CCUrl CCUrl.h "CCUrl.h"
* @brief This is the CCUrl class, a wrapper of libcurl.
*
*/
class LS_CURL_API CCUrl/*CCUrl*/
{
public:
    CCUrl();
    virtual ~CCUrl();

    /**
    * @brief <b>Summary:</b>
    *    Initiate this CURL session
    * @return    0, if success. <br>
    *            -1, if failed.
    *******************************************************************************/
    int    init();

protected:
    /**
    * @brief <b>Summary:</b>
    *        This function is virtual function, which will be called by automatic, 
    *    as soon as there is data received that needs to be processed.<br>
    *        The size of the data pointed to by 'data' is 'size' multiplied with 'nmemb', 
    *    it will not be zero terminated. <br>
    * @note    You should override this function to do you own busyness. 
    *        You save the data to a file, or process it anyway.
    * @param[in] data, point to the data
    * @param[in] size, the size        
    * @param[in] nmemb, the unit in bytes        
    * @return 
    *****************************************************************************/
    virtual int process( void* data, size_t size, size_t nmemb );

public:
    /**
    * @brief <b>Summary:</b>
    *    Post 'data' to 'url', and the server response data is restored in resultData.
    *    The process function is 'pfun' 
    * @param[in]    url
    * @param[in]    data, the data to be sent to server. 
    *                    eg. "id=admin&passwd=123456"
    * @param[in]    timeout, the limit time in seconds to do this process.
    *                    default value 10
    * @param[in]    withheader, 
    *                    1 indicate the server response data including header
    *                    0 indicate the server response data excluding header
    *                    default value 0
    * @return    0, if success. </br>
    *            -1, if failed.
    *******************************************************************************/
    int doHttpPost(LPCTSTR lpszUrl,LPCTSTR lpszData, DWORD dwTimeout = 10, DWORD dwWithheader = 0 );

    /**
    * @brief <b>Summary:</b>
    *    Get 'url' using 'data' to be set in the header, 
    *    and the server response data is restored in resultData.
    *    The process function is 'pfun' 
    * @param[in]    url
    * @param[in]    data, the data to be sent to server. 
    *                    eg. "id=admin&passwd=123456"
    * @param[in]    timeout, the limit time in seconds to do this process.
    *                    default value 10
    * @param[in]    withheader, 
    *                    1 indicate the server response data including header
    *                    0 indicate the server response data excluding header
    *                    default value 0
    * @return    0, if success. </br>
    *            -1, if failed.
    *******************************************************************************/
    int doHttpGet(LPCTSTR lpszUrl,LPCTSTR lpszData, DWORD dwTimeout = 10, DWORD dwWithheader = 0 );


public:
    //CURLcode setOpt( CURLoption option ); 


    /**
    * @brief <b>Summary:</b>
    *    Reset all options of a libcurl session handle
    * @return void, no return
    *******************************************************************************/
    void resetOpt(); 
    

	BOOL SetProcessFunc(CURL_PROCESS Process);
private:
    /**
    * @brief <b>Summary:</b>
    *        This function gets called by libcurl as soon as there is data received
    *    that needs to be saved.<br>
    *        The size of the data pointed to by ptr is size multiplied with nmemb, 
    *    it will not be zero terminated. <br>
    * @param[in] 
    * @param[in]    
    * @param[in]    
    * @param[in] usrptr, the user ptr, 
    *            set by option CURLOPT_READDATA or CURLOPT_WRITEDATA...
    * @return    the number of bytes actually taken care of. <br>
    *        If that amount differs from the amount passed to your function,
    *    it'll signal an error to the library and it will abort the transfer 
    *    and return CURLE_WRITE_ERROR. 
    *****************************************************************************/
    static size_t processFunc( void* ptr, size_t size, size_t nmemb, void *usrptr );

private:
    CURL * m_curl;
	CURL_PROCESS m_Process;
};    
