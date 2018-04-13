#pragma once

namespace NetCore
{
	class NET_API MediaType
	{
	public:
		MediaType(LPCTSTR lpszMediaType);
		MediaType(LPCTSTR lpszType, LPCTSTR lpszSubType);
		MediaType(const MediaType& MimeType);

		~MediaType();
	public:
		MediaType& operator = (const MediaType& MimeType);
		MediaType& operator = (LPCTSTR lpszMediaType);

		void Swap(MediaType& MimeType);

		void SetType(LPCTSTR lpszMediaType);
		LPCTSTR GetType() const;

		void SetSubType(LPCTSTR lpszSubType);
		LPCTSTR GetSubType() const;

		void SetParameter(LPCTSTR lpszName, LPCTSTR lpszValue);
		LPCTSTR GetParameter() const;

		BOOL HasParameter(LPCTSTR lpszName) const;
		void RemoveParameter(LPCTSTR lpszName);

		const NameValueCollection& GetParameters() const;
		LPCTSTR ToString() const;

		BOOL Matches(LPCTSTR lpszType) const;
		BOOL Matches(const MediaType& MimeType) const;
		BOOL Matches(LPCTSTR lpszType, LPCTSTR lpszSubType) const;

	protected:
		void Parse(LPCTSTR lpszMimeType);
	private:
		MediaType();
	private:
		CString		m_strType;
		CString		m_strSubType;
		NameValueCollection m_NVCParameters;
	};
}