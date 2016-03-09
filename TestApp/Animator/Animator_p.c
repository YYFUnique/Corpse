

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Mon Nov 10 17:33:14 2014
 */
/* Compiler settings for .\Animator.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "Animator_i.h"

#define TYPE_FORMAT_STRING_SIZE   3                                 
#define PROC_FORMAT_STRING_SIZE   1                                 
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _Animator_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } Animator_MIDL_TYPE_FORMAT_STRING;

typedef struct _Animator_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } Animator_MIDL_PROC_FORMAT_STRING;

typedef struct _Animator_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } Animator_MIDL_EXPR_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const Animator_MIDL_TYPE_FORMAT_STRING Animator__MIDL_TypeFormatString;
extern const Animator_MIDL_PROC_FORMAT_STRING Animator__MIDL_ProcFormatString;
extern const Animator_MIDL_EXPR_FORMAT_STRING Animator__MIDL_ExprFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ICOleImage_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ICOleImage_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IImageOle_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IImageOle_ProxyInfo;



#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need a Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const Animator_MIDL_PROC_FORMAT_STRING Animator__MIDL_ProcFormatString =
    {
        0,
        {

			0x0
        }
    };

static const Animator_MIDL_TYPE_FORMAT_STRING Animator__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */

			0x0
        }
    };


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ICOleImage, ver. 0.0,
   GUID={0x7F891C05,0x3991,0x4F27,{0xB5,0x0E,0x40,0xCB,0xEF,0xB8,0x84,0x9A}} */

#pragma code_seg(".orpc")
static const unsigned short ICOleImage_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO ICOleImage_ProxyInfo =
    {
    &Object_StubDesc,
    Animator__MIDL_ProcFormatString.Format,
    &ICOleImage_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ICOleImage_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Animator__MIDL_ProcFormatString.Format,
    &ICOleImage_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _ICOleImageProxyVtbl = 
{
    0,
    &IID_ICOleImage,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */
};


static const PRPC_STUB_FUNCTION ICOleImage_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION
};

CInterfaceStubVtbl _ICOleImageStubVtbl =
{
    &IID_ICOleImage,
    &ICOleImage_ServerInfo,
    7,
    &ICOleImage_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IImageOle, ver. 0.0,
   GUID={0x7CDE1B04,0x1910,0x40C3,{0x9E,0x1B,0xE7,0xAF,0x1B,0xA6,0xE2,0x0F}} */

#pragma code_seg(".orpc")
static const unsigned short IImageOle_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IImageOle_ProxyInfo =
    {
    &Object_StubDesc,
    Animator__MIDL_ProcFormatString.Format,
    &IImageOle_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IImageOle_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    Animator__MIDL_ProcFormatString.Format,
    &IImageOle_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IImageOleProxyVtbl = 
{
    0,
    &IID_IImageOle,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */
};


static const PRPC_STUB_FUNCTION IImageOle_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION
};

CInterfaceStubVtbl _IImageOleStubVtbl =
{
    &IID_IImageOle,
    &IImageOle_ServerInfo,
    7,
    &IImageOle_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    Animator__MIDL_TypeFormatString.Format,
    0, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x70001f4, /* MIDL Version 7.0.500 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

const CInterfaceProxyVtbl * _Animator_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IImageOleProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ICOleImageProxyVtbl,
    0
};

const CInterfaceStubVtbl * _Animator_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IImageOleStubVtbl,
    ( CInterfaceStubVtbl *) &_ICOleImageStubVtbl,
    0
};

PCInterfaceName const _Animator_InterfaceNamesList[] = 
{
    "IImageOle",
    "ICOleImage",
    0
};

const IID *  _Animator_BaseIIDList[] = 
{
    &IID_IDispatch,
    &IID_IDispatch,
    0
};


#define _Animator_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _Animator, pIID, n)

int __stdcall _Animator_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _Animator, 2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _Animator, 2, *pIndex )
    
}

const ExtendedProxyFileInfo Animator_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _Animator_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _Animator_StubVtblList,
    (const PCInterfaceName * ) & _Animator_InterfaceNamesList,
    (const IID ** ) & _Animator_BaseIIDList,
    & _Animator_IID_Lookup, 
    2,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

