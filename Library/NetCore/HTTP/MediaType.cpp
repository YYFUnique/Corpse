#include "StdAfx.h"
#include "MediaType.h"

namespace NetCore
{
	MediaType::MediaType(LPCTSTR lpszMediaType)
	{
		Parse(lpszMediaType);
	}

	MediaType::MediaType(LPCTSTR lpszType, LPCTSTR lpszSubType)
		: m_strType(lpszType)
		, m_strSubType(lpszSubType)
	{

	}

	MediaType::MediaType(const MediaType& MimeType)
		: m_strType(MimeType.GetType())
		, m_strSubType(MimeType.GetSubType())
		, m_NVCParameters(MimeType.GetParameters())
	{

	}

	MediaType::~MediaType()
	{

	}

	MediaType& MediaType::operator =(const MediaType& MimeType)
	{
		if (&MimeType != this)
		{
			m_strType		= MimeType.GetType();
			m_strSubType	= MimeType.GetSubType();
			m_NVCParameters = MimeType.GetParameters();
		}

		return *this;
	}

	MediaType& MediaType::operator =(LPCTSTR lpszMediaType)
	{
		Parse(lpszMediaType);
		return *this;
	}

	void MediaType::Swap(MediaType &MimeType)
	{
		std::swap(m_strType, MimeType.m_strType);
		std::swap(m_strSubType, MimeType.m_strSubType);
		m_NVCParameters.Swap(MimeType.m_NVCParameters);
	}

	void MediaType::SetType(LPCTSTR lpszMediaType)
	{
		m_strType = lpszMediaType;
	}

	CString MediaType::GetType() const
	{
		return m_strType;
	}

	void MediaType::SetSubType(LPCTSTR lpszSubType)
	{
		m_strSubType = lpszSubType;
	}

	CString MediaType::GetSubType() const
	{
		return m_strSubType;
	}

	void MediaType::SetParameter(LPCTSTR lpszName, LPCTSTR lpszValue)
	{
		m_NVCParameters.Set(lpszName, lpszValue);
	}

	CString MediaType::GetParameter(LPCTSTR lpszName) const
	{
		return m_NVCParameters.Get(lpszName);
	}

	const NameValueCollection& MediaType::GetParameters() const
	{
		return m_NVCParameters;
	}

	BOOL MediaType::HasParameter(LPCTSTR lpszName) const
	{
		return m_NVCParameters.Has(lpszName);
	}

	void MediaType::RemoveParameter(LPCTSTR lpszName)
	{
		m_NVCParameters.Erase(lpszName);
	}

	CString MediaType::ToString() const
	{
		CString strResult;
		if (m_strType.IsEmpty())
			return strResult;

		strResult.Append(m_strType);
		if (m_strSubType.IsEmpty() == FALSE)
			strResult.AppendFormat(_T("/%s"), m_strSubType);

		ConstIterator it = m_NVCParameters.Begin();
		ConstIterator end = m_NVCParameters.End();

		for (; it != end; ++it)
			strResult.AppendFormat(_T("; %s=%s"), it->first, it->second);

		return strResult;
	}

	BOOL MediaType::Matches(const MediaType& MimeType) const
	{
		return Matches(MimeType.GetType(), MimeType.GetSubType());
	}

	BOOL MediaType::Matches(LPCTSTR lpszType, LPCTSTR lpszSubType) const
	{
		return _tcsicmp(m_strType, lpszType) == 0 && _tcsicmp(m_strSubType, lpszSubType) == 0;
	}

	BOOL MediaType::Matches(LPCTSTR lpszType) const
	{
		return _tcsicmp(m_strType, lpszType) == 0;
	}

	void MediaType::Parse(LPCTSTR lpszMimeType)
	{
		m_strType.Empty();
		m_strSubType.Empty();
		m_NVCParameters.Clear();


	}
}