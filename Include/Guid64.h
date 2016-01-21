#pragma once
#include <time.h>

namespace Zero
{
	/**
	64비트 GUID 생성기 - 8바이트기 때문에 DB bigint데이터 1:1매칭가능
	*/
	class Guid64
	{
	public:
		Guid64();
		~Guid64();

		class CGuidInfo
		{
		public:
			int	m_Type1;
			int	m_Type2;
			int	m_Sequence;		// 초까지 같은시간대에 생성된경우 구분값(0부터 1,2,3...)
			tm	m_Time;			// 생성된 시간
		};

		Int64		Create(const byte& type1, const byte& type2);		// 1초내에 65536개 이상 시도시 실패함(0리턴)
		Int64		CreateForce(const byte& type1, const byte& type2);	// 실패할경우 재시도하여 강제 성공처리
		void		GetInfo(const Int64& guid, OUT CGuidInfo& outputData) const;

		struct tm	GetTime(const Int64& guid) const;			// Create된 시간
		UInt16		GetSequence(const Int64& guid) const;
		byte		GetType1(const Int64& guid) const;
		byte		GetType2(const Int64& guid) const;

	private:
		int m_nTime;
		int	m_nSequence;
	};
}





