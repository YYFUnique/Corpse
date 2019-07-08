/*-----------------------------------------------------------------------------
 *
 *   Copyright (C) 2002-2013, Asiainfo Inc.  All Rights Reserved.
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

#include "platform_defines.h"
#include "base_types.h"

#include <string>


namespace TMPS { namespace Firmware {


class Result;


namespace IO {


#if _OS_WIN_
	typedef wchar_t  CharT;
	static const CharT PathSep = CharT('\\');
#else
	typedef char     CharT;
	static const CharT PathSep = CharT('/');
#endif
typedef std::basic_string<CharT>  StringT;


static const size_t MaxPathLen = 255;
static const size_t PathBuffLen = (MaxPathLen + 1) * 2;


struct FSYS
{
	FSYS();
	~FSYS();

	uint32_t entries();     // Number of FAT entries (= number of clusters + 2)
	uint32_t cluterSize();  // Sectors per cluster (1,2,4...128)

private:
	FSYS(const FSYS&);
	FSYS& operator= (const FSYS&);

	void *opaque_;

	friend class Internal;
};


enum FormatRule
{
	FDISK = 0,
	SFD = 1
};


typedef void* FD;  // file descriptor


enum OpenMode
{
	READ,      // open for read
	OVERWRITE  // write from the begin of file
};


struct FileInfo
{
	uint32_t  size;     // File size
	uint16_t  modDate;  // Last modified date
	uint16_t  modTime;  // Last modified time
	bool      readOnly; // is read only ?
};


struct FileNode
{
	StringT   name;
	bool      isDir;
	FileInfo  info;
};


struct DIR
{
	DIR();
	~DIR();

private:
	DIR(const DIR&);
	DIR& operator= (const DIR&);

	void *opaque_;

	friend class Internal;
};


}}} // namespace TMPS::Firmware::IO
