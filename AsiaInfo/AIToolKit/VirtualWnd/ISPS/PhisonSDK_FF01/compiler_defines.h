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

//
// _CPP_11_ENABLE_
//
// Clang >= 3.3
//   (http://clang.llvm.org/cxx_status.html)
// GCC >= 4.7
//   (https://gcc.gnu.org/projects/cxx0x.html)
// MSVC >= 11 (VS2012)
//   (https://msdn.microsoft.com/en-us/library/hh567368.aspx)
//
#if defined (__clang__)
#   if __clang_major__ >= 3 && __clang_minor__ >= 3
#       define _CPP_11_ENABLE_ 1
#   else
#       define _CPP_11_ENABLE_ 0
#   endif
#elif defined (__GNUC__) || defined (__GNUG__)
#   if __GNUC__ >= 4 && __GNUC_MINOR__ >= 6
#       define _CPP_11_ENABLE_ 1
#   else
#       define _CPP_11_ENABLE_ 0
#   endif
#elif defined(_MSC_VER)
#   if _MSC_VER >= 1700
#       define _CPP_11_ENABLE_ 1
#   else
#       define _CPP_11_ENABLE_ 0
#   endif
#endif
