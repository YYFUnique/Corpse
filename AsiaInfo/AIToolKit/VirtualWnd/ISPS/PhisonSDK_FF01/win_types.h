/*-----------------------------------------------------------------------------
 *
 *   Copyright (C) 2002-2015, Asiainfo Inc.  All Rights Reserved.
 *
 *   This program is an unpublished copyrighted work which is proprietary
 *   to Asiainfo, Inc. and contains confidential information that is not
 *   to be reproduced or disclosed to any other person or entity without
 *   prior written consent from Asiainfo, Inc. in each and every instance.
 *
 *   WARNING:  Unauthorized reproduction of this program as well as
 *   unauthorized preparation of derivative works based upon the
 *   program or distribution of copies by sale, rental, lease or
 *   lending are violations of federal copyright laws and state trade
 *   secret laws, punishable by civil and criminal penalties.
 *
 *-----------------------------------------------------------------------------*/

#pragma once

#include "base_types.h"

// ref:
//  https://msdn.microsoft.com/en-us/library/windows/desktop/aa383679(v=vs.85).aspx

#ifndef NO_STRICT
#	define FORWARD_DECLARE_HANDLE(name) struct name##__
#	define TM_TYPEDEF_HANDLE(name) typedef struct name##__ *name
#else
#	define FORWARD_DECLARE_HANDLE(name)
#	define TM_TYPEDEF_HANDLE(name) typedef HANDLE name
#endif

// ref:
//  http://en.wikibooks.org/wiki/Windows_Programming/Handles_and_Data_Types

struct _RTL_CRITICAL_SECTION;
struct _SYSTEMTIME;
struct _FILETIME;
struct _PROCESS_INFORMATION;
struct _STARTUPINFOW;
struct _SECURITY_ATTRIBUTES;
struct _ENCRYPTION_CERTIFICATE_HASH_LIST;
struct _WTS_SESSION_INFOW;
struct _SID;

FORWARD_DECLARE_HANDLE(SC_HANDLE);

namespace TM {
namespace Win {

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef uint64_t       QWORD;

typedef void*  HANDLE;
typedef ::_RTL_CRITICAL_SECTION		CRITICAL_SECTION;
typedef ::_SYSTEMTIME				SYSTEMTIME;
typedef ::_FILETIME					FILETIME;
typedef ::_PROCESS_INFORMATION		PROCESS_INFORMATION;
typedef ::_STARTUPINFOW				STARTUPINFOW;
typedef ::_SECURITY_ATTRIBUTES		SECURITY_ATTRIBUTES;
typedef ::_ENCRYPTION_CERTIFICATE_HASH_LIST ENCRYPTION_CERTIFICATE_HASH_LIST;
typedef ::_WTS_SESSION_INFOW		WTS_SESSION_INFOW;
typedef ::_SID						SID;
typedef ::_GUID                     GUID;

TM_TYPEDEF_HANDLE(SC_HANDLE);

}} // namespace TM::Win
