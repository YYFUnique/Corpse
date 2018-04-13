#pragma once

namespace NetCore
{
	/// Limits for basic sanity checks when reading a header
	typedef enum tagLimits
	{
		MAX_NAME_LENGTH  = 256,
		MAX_VALUE_LENGTH = 8192,
		DFL_FIELD_LIMIT  = 100
	}Limits;

	class NET_API MessageHeader : public NameValueCollection
	{
	public:
		MessageHeader();
		MessageHeader(const MessageHeader& MsgHeader);

		virtual ~MessageHeader();
	public:
		MessageHeader& operator= (const MessageHeader& MsgHeader);
	public:
		virtual void Write(CString& strNameValue) const;
		virtual void Read() const;

		int GetFieldLimit() const;
		void SetFieldLimit(int nLimit);

		BOOL HasToken(LPCTSTR lpszFieldName, LPCTSTR lpszToken) const;

		/// For example, the string 
		///   text/plain; q=0.5, text/html, text/x-dvi; q=0.8
		/// is split into the elements
		///   text/plain; q=0.5
		///   text/html
		///   text/x-dvi; q=0.8
		//static void SplitElements(LPCTSTR lpsz)
	private:
		int m_nFieldLimit;
	};
}