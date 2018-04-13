#include "StdAfx.h"
#include "MessageHeader.h"

namespace NetCore
{
	MessageHeader::MessageHeader()
		:m_nFieldLimit(DFL_FIELD_LIMIT)
	{
	}

	MessageHeader::MessageHeader(const MessageHeader& MsgHeader)
		: NameValueCollection(MsgHeader)
		, m_nFieldLimit(DFL_FIELD_LIMIT)
	{
	}

	MessageHeader::~MessageHeader()
	{

	}

	MessageHeader& MessageHeader::operator =(const MessageHeader& MsgHeader)
	{
		NameValueCollection::operator =(MsgHeader);
		return *this;
	}

	void MessageHeader::Write(CString &strNameValue) const
	{
		ConstIterator it = Begin();
		while (it != End())
		{
			strNameValue.AppendFormat(_T("%s: %s\r\n"),it->first, it->second);
			++it;
		}
	}

	void MessageHeader::Read() const
	{

	}

	int MessageHeader::GetFieldLimit() const
	{
		return m_nFieldLimit;
	}

	void MessageHeader::SetFieldLimit(int nLimit)
	{
		m_nFieldLimit = nLimit;
	}

	BOOL MessageHeader::HasToken(LPCTSTR lpszFieldName, LPCTSTR lpszToken) const
	{
		return FALSE;
	}

}