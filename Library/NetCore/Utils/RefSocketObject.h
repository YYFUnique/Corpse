#pragma once

namespace NetCore
{
	typedef int ValueType;
	class NET_API AtomicCounter
	{
	public:
		AtomicCounter();
		explicit AtomicCounter(ValueType InitialValue);
		AtomicCounter(const AtomicCounter& Counter);
		~AtomicCounter();
	public:
		AtomicCounter& operator = (const AtomicCounter& Counter);
		AtomicCounter& operator = (ValueType value);
		operator ValueType() const;
		ValueType value() const;
		ValueType operator ++ ();
		ValueType operator ++ (int);
		ValueType operator -- ();
		ValueType operator -- (int);
		BOOL operator !() const;
	private:
		typedef volatile LONG ImplType;
		ImplType m_counter;
	};

	class NET_API RefCountObject
	{
	public:
		RefCountObject();
		void Duplicate() const;
		void Release()	const;
		int ReferenceCount() const;
	protected:
		virtual ~RefCountObject();
	private:
		RefCountObject(const RefCountObject&);
		RefCountObject& operator=(const RefCountObject&);
	private:
		mutable AtomicCounter m_counter;
	};
}