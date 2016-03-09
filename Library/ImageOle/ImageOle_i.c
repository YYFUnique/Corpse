

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Tue Nov 11 15:09:07 2014
 */
/* Compiler settings for .\ImageOle.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IComponentRegistrar,0xa817e7a2,0x43fa,0x11d0,0x9e,0x44,0x00,0xaa,0x00,0xb6,0x77,0x0a);


MIDL_DEFINE_GUID(IID, IID_IRichEditImageOle,0xBEC6387B,0x2315,0x453D,0x97,0x4D,0xBB,0x75,0xE6,0xC6,0xF7,0xAB);


MIDL_DEFINE_GUID(IID, LIBID_ImageOleLib,0xA239D61C,0xEFA7,0x4880,0xA4,0x46,0x69,0xC6,0x2A,0xDE,0x9C,0x6B);


MIDL_DEFINE_GUID(CLSID, CLSID_CompReg,0x699286B3,0xFFA7,0x492A,0x81,0x97,0x24,0xB6,0x75,0x2F,0x74,0x34);


MIDL_DEFINE_GUID(IID, DIID__IRichEditImageOleEvents,0x7DEE7780,0xA720,0x4905,0xBF,0xC8,0x31,0x2B,0xF8,0xCE,0x4B,0xD4);


MIDL_DEFINE_GUID(CLSID, CLSID_RichEditImageOle,0x4E37008B,0x3613,0x4A86,0x87,0xD8,0x22,0x91,0xCE,0xED,0x10,0x8E);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



