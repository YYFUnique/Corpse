#include "StdAfx.h"
#include "NameValueCollection.h"

namespace NetCore
{
	void Swap(NameValueCollection &nvc, NameValueCollection& nvc2)
	{
		nvc.Swap(nvc2);
	}

	NameValueCollection::NameValueCollection()
	{

	}

	NameValueCollection::NameValueCollection(const NameValueCollection& nvc)
		:m_Map(nvc.m_Map)
	{

	}

	NameValueCollection::~NameValueCollection()
	{

	}

	NameValueCollection& NameValueCollection::operator =(const NameValueCollection& nvc)
	{
		if (&nvc != this)
			m_Map = nvc.m_Map;

		return *this;
	}

	LPCTSTR NameValueCollection::operator [](LPCTSTR lpszName) const
	{
		ConstIterator it = m_Map.find(lpszName);
		if (it != m_Map.end())
			return it->second;
		return _T("");
	}

	void NameValueCollection::Swap(NameValueCollection& nvc)
	{
		std::swap(m_Map, nvc.m_Map);
	}

	void NameValueCollection::Set(LPCTSTR lpszName, LPCTSTR lpszValue)
	{
		Iterator it = m_Map.find(lpszName);
		if (it != m_Map.end())
			it->second = lpszValue;
		else
			m_Map.insert(HeaderMap::ValueType(lpszName, lpszValue));
	}

	void NameValueCollection::Add(LPCTSTR lpszName, LPCTSTR lpszValue)
	{
		m_Map.insert(HeaderMap::ValueType(lpszName, lpszValue));
	}


	LPCTSTR NameValueCollection::Get(LPCTSTR lpszName) const
	{
		ConstIterator it = m_Map.find(lpszName);
		if (it != m_Map.end())
			return it->second;
		return _T("");
	}

	LPCTSTR NameValueCollection::Get(LPCTSTR lpszName, LPCTSTR lpszDefaultValue) const
	{
		ConstIterator it = m_Map.find(lpszName);
		if (it != m_Map.end())
			return it->second;
		else
			return lpszDefaultValue;
	}

	BOOL NameValueCollection::Has(LPCTSTR lpszName) const
	{
		return m_Map.find(lpszName) != m_Map.end();
	}

	ConstIterator NameValueCollection::Find(LPCTSTR lpszName) const
	{
		return m_Map.find(lpszName);
	}

	ConstIterator NameValueCollection::Begin() const
	{
		return m_Map.begin();
	}

	ConstIterator NameValueCollection::End() const
	{
		return m_Map.end();
	}

	BOOL NameValueCollection::IsEmpty() const
	{
		return m_Map.empty();
	}

	size_t NameValueCollection::GetSize() const
	{
		return m_Map.size();
	}

	void NameValueCollection::Erase(LPCTSTR lpszName)
	{
		//m_Map.erase(lpszName);
		m_Map.earse(lpszName);
	}

	void NameValueCollection::Clear()
	{
		m_Map.clear();
	}
}