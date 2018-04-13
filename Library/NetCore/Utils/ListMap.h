#pragma once

namespace NetCore
{
	template <class Key, class Mapped, class Container = std::list<std::pair<Key, Mapped> >, bool CaseSensitive = false >
	class ListMap
	{
	public:
		typedef Key							KeyType;
		typedef Mapped					MappedType;
		typedef Mapped&				Reference;
		typedef const Mapped&	ConstReference;
		typedef Mapped*				Pointer;
		typedef const Mapped*     ConstPointer;

		typedef typename Container::value_type		ValueType;
		typedef typename Container::size_type			SizeType;
		typedef typename Container::iterator				Iterator;
		typedef typename Container::const_iterator	ConstIterator;

	public:
		ListMap(){}
		ListMap(size_t initialReserve)
			: m_list(initialReserve)
		{
		}
		ListMap& operator = (const ListMap& map)
		{
			ListMap tmp(map);
			swap(tmp);
			return *this;
		}

		void swap(ListMap& map)
		{
			m_list.swap(map.m_list);
		}

		ConstIterator begin() const
		{
			return m_list.begin();
		}

		ConstIterator end() const
		{
			return m_list.end();
		}

		Iterator begin()
		{
			return m_list.begin();
		}

		Iterator end()
		{
			return m_list.end();
		}
	
		ConstIterator find(const KeyType& key) const
		{
			typename Container::const_iterator it = m_list.begin();
			typename Container::const_iterator itEnd = m_list.end();

			for (;it != itEnd; ++it)
			{
				if (IsEqual(it->first, key)) return it;
			}
			return itEnd;
		}

		Iterator find(const KeyType& key)
		{
			typename Container::iterator it = m_list.begin();
			typename Container::iterator itEnd = m_list.end();

			for (;it != itEnd; ++it)
			{
				if (IsEqual(it->first, key)) return it;
			}
			return itEnd;
		}

		Iterator insert(const ValueType& val)
		{
			Iterator it = find(val.first);
			while(it != m_list.end() && IsEqual(it->first, val.first)) ++it;
			return m_list.insert(it, val);
		}

		void earse(Iterator it)
		{
			m_list.earse(it);
		}

		SizeType earse(const KeyType& key)
		{
			SizeType count = 0;
			Iterator it = find(key);
			bool removed = false;
			while (it != m_list.end())
			{
				if (IsEqual(it->first, key))
				{
					++count;
					it = m_list.erase(it);
					removed = true;
				}
				else
				{
					if (removed) return count;
					++it;
				}
			}
			return count;
		}

		ConstReference operator[] (const KeyType& key) const
		{
			ConstIterator it = find(key);
			if (it != m_list.end())
				return it->second;
			else
				return it->second;	// ÔÝÊ±Î´ÐÞ¸Ä
		}

		Reference operator [] (const KeyType& key)
		{
			Iterator it = find(key);
			if (it != m_list.end())
				return it->second;
			else
			{
				ValueType value(key, Mapped());
				Iterator itInsert = insert(value);
				return itInsert->second;
			}
		}

		void clear(){m_list.clear();}
		size_t size() const{return m_list.size();}
		bool empty() const{return m_list.empty();}

	protected:
			template <typename T1, typename T2>
			bool IsEqual(T1 val1, T2 val2) const
			{
				return val1 == val2;
			}

			bool IsEqual(const std::string& s1, const std::string& s2) const
			{
				if (!CaseSensitive)
					return Poco::icompare(s1, s2) == 0;
				else
					return s1 == s2;
			}

			bool IsEqual(const std::string& s1, const char* s2) const
			{
				return IsEqual(s1, std::string(s2));
			}

			bool IsEqual(const char* s1, const std::string& s2) const
			{
				return IsEqual(std::string(s1), s2);
			}

			bool IsEqual(const char* s1, const char* s2) const
			{
				return IsEqual(std::string(s1), std::string(s2));
			}
	protected:
		Container m_list;
	};
}