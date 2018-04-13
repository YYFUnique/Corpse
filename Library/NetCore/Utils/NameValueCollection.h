#pragma once

namespace NetCore
{
	typedef ListMap<CString, LPCTSTR> HeaderMap;
	typedef HeaderMap::Iterator Iterator;
	typedef HeaderMap::ConstIterator ConstIterator;

	class NET_API NameValueCollection
	{
	public:
		NameValueCollection();
		NameValueCollection(const NameValueCollection& nvc);

		virtual ~NameValueCollection();
	public:
		LPCTSTR operator[] (LPCTSTR lpszName) const;
		NameValueCollection& operator = (const NameValueCollection& nvc);
		void Swap(NameValueCollection& nvc);
		void Set(LPCTSTR lpszName, LPCTSTR lpszValue);
		void Add(LPCTSTR lpszName, LPCTSTR lpszValue);
		LPCTSTR Get(LPCTSTR lpszName) const;
		LPCTSTR Get(LPCTSTR lpszName, LPCTSTR lpszDefaultValue) const;
		BOOL Has(LPCTSTR lpszName) const;
		ConstIterator Find(LPCTSTR lpszName) const;
		ConstIterator Begin() const;
		ConstIterator End() const;
		BOOL IsEmpty() const;
		size_t GetSize() const;
		void Erase(LPCTSTR lpszName);
		void Clear();
	private:
		HeaderMap m_Map;
	};
}