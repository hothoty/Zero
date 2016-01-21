#pragma once
#include "Exception.h"

namespace Zero
{
	template<class ClassType, bool raw_type = false, typename size_type = intptr_t>
	class CSafeArray
	{
	public:
		CSafeArray();
		CSafeArray(const CSafeArray &src);
		CSafeArray(size_type newSize, bool bReserve=false);	// newSize 1이상필수, bReserve true면 Capacity만증가
		~CSafeArray();

		void Add(const ClassType& src);
		bool AddIfUnique(const ClassType& src);
		void Clear();
		void ClearWithCapacity();
		void Resize(size_type newSize, bool bNoCopy=false, bool bForceCapacityMode=false/*특정Capacity강제지정옵션*/);
		bool Contains(const ClassType& src);
		void Insert(size_type p_index, const ClassType& src);
		bool Remove(const ClassType& value);
		void RemoveAt(size_type p_index);
		void UseLowMemory();
		void UseHighMemory();
		bool ExtendCapacity(size_type newCapacity);		// 내용유지하면서 Capacity확장(입력된newCapacity만큼만), 현재Size보다 큰값 지정할때만 성공


		#ifdef _WIN32
		__declspec(property(get = GetCapacity))	size_type Capacity;
		__declspec(property(get = GetCount))	size_type Count;
		#endif

		inline size_type		GetCount()		const				{ return m_Size; }
		inline size_type		GetCapacity()	const				{ return m_Capacity; }
		inline ClassType*		GetData()							{ return m_Array; }
		inline const ClassType* GetData()		const				{ return m_Array; }
		inline ClassType&		operator[](size_type p_index) const	{ return ElementAt( p_index ); }
		inline CSafeArray&		operator=(const CSafeArray &src)	{ src.CopyTo(*this); return *this; }
		operator ClassType* ()										{ return m_Array; }


	private:
		ClassType* m_Array;
		size_type m_Size;
		size_type m_Capacity;
		int m_UseMemory;

		void		Initialize();
		size_type	AutoCapacity(size_type newSize);
		bool		BoundCheck(const size_type& p_index) const;
		void		CopyTo(CSafeArray &dest) const;
		ClassType&	ElementAt(size_type p_index) const;
	};
}

#include "SafeArray.inl"

