#include "StdAfx.h"
#include "RefSocketObject.h"

namespace NetCore
{
	//
	// AtomicCounter
	//

	AtomicCounter::AtomicCounter()
		:m_counter(0)
	{

	}

	AtomicCounter::AtomicCounter(ValueType InitialValue)
		:m_counter(InitialValue)
	{

	}

	AtomicCounter::AtomicCounter(const AtomicCounter& Counter)
		:m_counter(Counter.value())
	{

	}

	AtomicCounter::~AtomicCounter()
	{

	}

	AtomicCounter::operator ValueType() const
	{
		return m_counter;
	}

	ValueType AtomicCounter::value() const
	{
		return m_counter;
	}

	ValueType AtomicCounter::operator ++()
	{
		return InterlockedIncrement(&m_counter);
	}

	ValueType AtomicCounter::operator ++(int)
	{
		ValueType Restult = InterlockedIncrement(&m_counter);
		return Restult - 1;
	}

	ValueType AtomicCounter::operator --()
	{
		return InterlockedDecrement(&m_counter);
	}

	ValueType AtomicCounter::operator --(int)
	{
		ValueType Result = InterlockedDecrement(&m_counter);
		return Result+1;
	}

	BOOL AtomicCounter::operator !() const
	{
		return m_counter == 0;
	}


	//
	// RefCountObject
	//

	RefCountObject::RefCountObject()
		:m_counter(1)
	{
	
	}

	RefCountObject::~RefCountObject()
	{

	}

	int RefCountObject::ReferenceCount() const
	{
		return m_counter.value();
	}

	void RefCountObject::Duplicate() const
	{
		++m_counter;
	}

	void RefCountObject::Release() const
	{
		if (--m_counter == 0)
		{
			delete this;
		}
	}	
}