#pragma once

template <typename T>
class SplitVector
{
public:
	SplitVector(){
		Init();
	}
	virtual ~SplitVector() { 
		delete[] m_pBody;
		m_pBody = NULL;
	}
public:
	int GetGrowSize() const{
		return m_nGrowSize;
	}
	void SetGrowSize(int nGrowSize){
		m_nGrowSize = nGrowSize;
	}

	/// Reallocate the storage for the buffer to be newSize and
	/// copy exisiting contents to the new buffer.
	/// Must not be used to decrease the size of the buffer.
	void ReAllocate(int nNewSize){
		if (nNewSize > m_nSize) {
			// Move the gap to the end
			GapTo(m_nBodyLen);
			T* pNewBody = new T[nNewSize];
			if ((m_nSize != 0) && (m_pBody != NULL)) {
				memmove(pNewBody, m_pBody, sizeof(T) * m_nBodyLen);
				delete[] m_pBody;
			}

			m_pBody = pNewBody;
			m_nGapLen += nNewSize - m_nSize;
			m_nSize = nNewSize;
		}
	}

	/// Retrieve the character at a particular position.
	/// Retrieving positions outside the range of the buffer returns 0.
	/// The assertions here are disabled since calling code can be
	/// simpler if out of range access works and returns 0.
	T ValueAt(int nPos) const{
		if (nPos < m_nPartLen) {
			if (nPos < 0)
				return 0;
			return m_pBody[nPos];
		} else {
			if (nPos > m_nBodyLen)
				return 0;
			else
				return m_pBody[m_nGapLen + nPos];
		}
	}
	void SetValueAt(int nPos, T v) {
		if (nPos < m_nPartLen) {
			if (nPos >= 0)
				m_pBody[nPos ] = v;
		} else {
			if (nPos < m_nBodyLen)
				m_pBody[m_nGapLen + nPos] = v;
		}
	}

	T& operator[](int nPos) const{
		if (nPos < m_nPartLen) 
			return m_pBody[nPos];
		else
			return m_pBody[m_nGapLen + nPos];
	}

	// Retrieve the length of the buffer
	int Length() const{
		return m_nBodyLen;
	}

	/// Insert a single value into the buffer.
	/// Inserting at positions outside the current range fails.
	void Insert(int nPos, T v) {
		if ((nPos < 0) || (nPos > m_nBodyLen)) 
			return;

		RoomFor(1);
		GapTo(nPos);
		m_pBody[m_nPartLen] = v;
		++m_nBodyLen;
		++m_nPartLen;
		--m_nGapLen;
	}

	/// Insert a number of elements into the buffer setting their value.
	/// Inserting at positions outside the current range fails.
	void InsertValue(int nPos, int nLen, T v) {
		if (nLen > 0) {
			if ((nPos < 0) || (nPos > m_nBodyLen))
				return;

			RoomFor(nLen);
			GapTo(nPos);
			for (int n=0; n<nLen; ++n)
				m_pBody[m_nPartLen + n] = v;
			m_nBodyLen += nLen;
			m_nPartLen += nLen;
			m_nGapLen -= nLen;
		}
	}

	/// Ensure at least length elements allocated,
	/// appending zero valued elements if needed.
	void EnsureLength(int nLen) {
		int nLength = Length();
		if (nLength < nLen)
			InsertValue(nLength, nLen - nLength, 0);
	}

	/// Insert text into the buffer from an array.
	void InsertFromArray(int nPosToInsert, const T s[], int nPosFrom, int nLen) {
		if (nLen > 0) {
			if ((nPosToInsert < 0) || (nPosToInsert > m_nBodyLen))
				return;

			RoomFor(nLen);
			GapTo(nPosToInsert);
			memmove(m_pBody + m_nPartLen, s + nPosFrom, sizeof(T) * nLen);
			m_nBodyLen += nLen;
			m_nPartLen += nLen;
			m_nGapLen -= nLen;
		}
	}


	/// Delete one element from the buffer.
	void Delete(int nPos) {
		if ((nPos < 0) || (nPos >= m_nBodyLen))
			return;
		DeleteRange(nPos, 1);
	}

	/// Delete a range from the buffer.
	/// Deleting positions outside the current range fails.
	void DeleteRange(int nPos, int nLen) {
		if ((nPos < 0) || (nPos + nLen) > m_nBodyLen)
			return;
		if ((nPos == 0) && (nLen == m_nBodyLen)) {
			// Full deallocation returns storage and is faster
			delete[] m_pBody;
			Init();
		} else if (nLen > 0) {
			GapTo(nPos);
			m_nBodyLen -= nLen;
			m_nGapLen += nLen;
		}
	}

	/// Delete all the buffer contents.
	void DeleteAll() {
		DeleteRange(0, m_nBodyLen);
	}

	// Retrieve a range of elements into an array
	void GetRange(T* pBuffer, int nPos, int nLen) const {
		// Split into up to 2 ranges, before and after the split then use memcpy on each.
		int nRangeLen = 0;
		if (nPos < m_nPartLen) {
			int nPartAfterPos = m_nPartLen - nPos;
			nRangeLen = nLen;
			if (nRangeLen > nPartAfterPos)
				nRangeLen = nPartAfterPos;
		}
		memcpy(pBuffer, m_pBody + nPos, nRangeLen * sizeof(T));
		pBuffer += nRangeLen;
		nPos = nPos + nRangeLen + m_nGapLen;
		int nRangeLen2 = nLen - nRangeLen;
		memcpy(pBuffer, m_pBody + nPos ,nRangeLen2 * sizeof(T));
	}

	T* BufferPointer() {
		RoomFor(1);
		GapTo(m_nBodyLen);
		m_pBody[m_nBodyLen] = 0;
		return m_pBody;
	}

	T* RangePointer(int nPos, int nLen) {
		if (nPos < m_nPartLen) {
			if ((nPos + nLen) > m_nPartLen) {
				// Range overlaps gap, so move gap to start of range.
				GapTo(nPos);
				return m_pBody + nPos + m_nGapLen;
			} else {
				return m_pBody + nPos;
			}
		} else {
			return m_pBody + nPos + m_nGapLen;
		}
	}

	int GapPosition() const {
		return m_nPartLen;
	}
protected:
	/// Move the gap to a particular position so that insertion and
	/// deletion at that point will not require much copying and
	/// hence be fast.
	void GapTo(int nPos){
		if (nPos != m_nPartLen) {
			if (nPos < m_nPartLen)
				memmove(m_pBody + nPos + m_nGapLen, m_pBody + nPos, sizeof(T) * (m_nPartLen - nPos));
			else
				memmove(m_pBody + m_nPartLen, m_pBody + m_nPartLen + m_nGapLen, sizeof(T) * (nPos - m_nPartLen));

			m_nPartLen = nPos;
		}
	}

	/// Check that there is room in the buffer for an insertion,
	/// reallocating if more space needed.
	void RoomFor(int nLen) {
		if (m_nGapLen <= nLen) {
			while (m_nGrowSize < m_nSize / 6)
				m_nGrowSize *= 2;
			ReAllocate(m_nSize + nLen + m_nGrowSize);
		}
	}

	void Init(){
		m_pBody = NULL;
		m_nBodyLen = 0;
		m_nGrowSize = 8;
		m_nSize = 0;
		m_nPartLen = 0;
		m_nGapLen = 0;
	}
protected:
	T*		m_pBody;
	int	m_nBodyLen;
	int	m_nSize;
	int	m_nPartLen;
	int	m_nGapLen;		/// invariant: gapLength == size - lengthBody
	int	m_nGrowSize;
};