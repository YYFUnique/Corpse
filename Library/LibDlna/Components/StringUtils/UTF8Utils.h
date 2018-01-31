/*
 * INTEL CONFIDENTIAL
 * Copyright (c) 2002 - 2006 Intel Corporation.  All rights reserved.
 * 
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors.  Title to the
 * Material remains with Intel Corporation or its suppliers and
 * licensors.  The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and
 * licensors. The Material is protected by worldwide copyright and
 * trade secret laws and treaty provisions.  No part of the Material
 * may be used, copied, reproduced, modified, published, uploaded,
 * posted, transmitted, distributed, or disclosed in any way without
 * Intel's prior express written permission.
 
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly, by
 * implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 * 
 * $Workfile: UTF8Utils.h
 *
 *
 *
 */
#ifndef UTF8UTILS_H

#include <ctype.h>
#include <stdlib.h>

/*! \file UTF8Utils.h 
	\brief UTF-8 Encoding/Decoding.
*/

/*! \defgroup UTF8Utils DLNA - UTF8 Encoding/Decoding
	\brief 
	This module provides the encoding/decoding functionality of C strings from/to UTF8 Strings, stored in CDS metadata. 

	\{
*/

/*!	\brief Encodes a multibyte C-style string to UTF-8 compliant format, returned string must be freed.

	\param[in] mbcsStr The input string to be encoded.

	\returns A multi-byte UTF-8 string.
*/
char* EncodeToUTF8(const char* mbcsStr);

/*!	\brief Decodes a UTF-8 string to C-Style multibyte format, returned string must be freed.

	\param[in] utf8Str The input string to be encoded.

	\returns A C-Style multibyte format.
*/
char* DecodeFromUTF8(const char* utf8Str);

/*! \} */

#endif
