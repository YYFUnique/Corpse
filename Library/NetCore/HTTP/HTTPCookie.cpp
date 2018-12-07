#include "StdAfx.h"
#include "HTTPCookie.h"

namespace NetCore
{
	HTTPCookie::HTTPCookie()
	{

	}

	CString HTTPCookie::ToString() const
	{
		return _T("HTTPCookie");
	}
}