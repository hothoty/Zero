#pragma once
#include "Guid.h"

namespace Zero
{
	/**
	버퍼설명

	- 내부 버퍼 사용 (내부버퍼 자동 관리)

	- 외부버퍼 사용시 주의점 (네트워크 패킷 수신한 것을 복사하지 않고 그대로 사용가능함)

	  1. ReSize 불가

	  2. PushData처리시 버퍼크기가 넘어가는 데이터를 채우려고할때 허용안함 (내부버퍼 사용할경우엔 버퍼크기 자동으로 늘어남)


	- Serialize 자동화 사용법 - AUTO_SERIALIZE1 ~ AUTO_SERIALIZE30 까지 사용가능

	@code
		class CData : public CSerializable
		{
		public:
			int m_Number1;
			int m_Number2;

			AUTO_SERIALIZE2(m_Number1, m_Numbe2);	// 2개의 데이터 처리시
		};
	@end

	- enum 자동 직렬화 시키기 예제
	@code
		AUTO_SERIALIZE_ENUM(enumValue);				// 일반용도(32비트)

		AUTO_SERIALIZE_ENUM16(enumValue);			// 16비트 enum 범위 -INT16_MAX 32767~32767까지 가능

		AUTO_SERIALIZE_ENUM8(enumValue);			// 8비트 enum 범위 -INT8_MAX
	@end

	*/

	class CSerializer;
	class CSerializable
	{
	public:
		virtual void Serialize(bool bStore, CSerializer& clsSerial) = 0;
	};

	// 스트링 직렬화시 최대 글자 개수 제약 있음 : MaxLengthString(1024)
	class CSerializer
	{
	public:
		CSerializer();
		~CSerializer();
		CSerializer(const CSerializer &src);

		void CursorReset(intptr_t pos=0);
		void SetExternalBuffer(byte* pBuf, const intptr_t nSize);				// 외부버퍼 사용하기 시작
		void ExtendCapacity(intptr_t isize);									// 버퍼Capacity조정 : 늘리기만 가능

		inline bool		IsExternalBuffer() const { return m_bExternalBuffer; }	// 현재 외부버퍼 사용중인지 알아내기
		inline byte*	GetData()	const { return m_pBuffer; }					// 데이터 정보
		inline intptr_t	GetCursor() const { return m_nCursor; }					// 현재 커서 위치
		intptr_t		GetBuffSize() const;									// 버퍼 사이즈

		inline void SetForceCursor(int n) { m_nCursor = n;  }	// 사용금지(managed용도)

		CSafeArray<byte, true,Int32>* GetArr() { return &m_Data; }

		// 버퍼크기 조정 - Write시작/패킷읽어들이기 용도, 따라서 NoCopy옵션 사용함 - 외부버퍼 사용시 사이즈 조정 불가능!
		void ReSizeBuffer(intptr_t isize, bool ignoreCapacityMode=false/*패킷읽어들이는 용도와 같이 Capacity늘릴때적용*/);

		// 기본 데이터 타입 등록
	#define MAKE_SERIALIZE_DATA(data)\
		inline CSerializer& operator<<(const data& b)	{ PushData(	(char*)&b,	sizeof(data) );	return *this; }\
		inline CSerializer& operator>>(data &b)			{ PopData(	(char*)&b,	sizeof(data) );	return *this; }
		MAKE_SERIALIZE_DATA(bool);
		MAKE_SERIALIZE_DATA(sbyte);
		MAKE_SERIALIZE_DATA(byte);
		MAKE_SERIALIZE_DATA(Int16);
		MAKE_SERIALIZE_DATA(UInt16);
		MAKE_SERIALIZE_DATA(Int32);
		MAKE_SERIALIZE_DATA(UInt32);
		MAKE_SERIALIZE_DATA(Int64);
		MAKE_SERIALIZE_DATA(UInt64);
		MAKE_SERIALIZE_DATA(float);
		MAKE_SERIALIZE_DATA(double);
		MAKE_SERIALIZE_DATA(Guid);

		CSerializer& operator<<(CSerializable& object);
		CSerializer& operator<<(CSerializable* object);
		CSerializer& operator>>(CSerializable& object);
		CSerializer& operator>>(CSerializable* object);
		CSerializer& operator<<(const StringW& data);
		CSerializer& operator>>(StringW& data);
		CSerializer& operator<<(const StringA& data);
		CSerializer& operator>>(StringA& data);
		CSerializer& operator<<(const std::wstring& data);
		CSerializer& operator>>(std::wstring& data);
		CSerializer& operator<<(const std::string& data);
		CSerializer& operator>>(std::string& data);

		template<typename T> CSerializer& operator<<(const T& data);
		template<typename T> CSerializer& operator<<(const T* data);
		template<typename T> CSerializer& operator>>(T& data);
		template<typename T> CSerializer& operator>>(T* data);

		template<typename elem, bool raw_type, typename size_type> CSerializer& operator<<(const CSafeArray<elem, raw_type, size_type>& Ar);
		template<typename elem, bool raw_type, typename size_type> CSerializer& operator>>(CSafeArray<elem, raw_type, size_type>& Ar);

		template<typename elem> CSerializer& operator<<(const std::vector<elem>& Ar);
		template<typename elem> CSerializer& operator>>(std::vector<elem>& Ar);

		template<class K, class T> CSerializer& operator<<(const TMap<K,T>& Ar);
		template<class K, class T> CSerializer& operator>>(TMap<K,T>& Ar);
		template<typename K, typename V> CSerializer& operator<<(const CDictionary<K,V>& Ar);
		template<typename K, typename V> CSerializer& operator>>(CDictionary<K,V>& Ar);

		CSerializer& operator=(const CSerializer& src)
		{
			src.CopyTo(*this);
			return *this;
		}

	public:
		void PushData(const char* pData, intptr_t nSize);
		void PopData(char* pData, intptr_t nSize);
	
	private:
		enum { MaxLengthString = 1024, MaxSizeMap = 1048576 };

		byte* m_pBuffer;			// 현재 사용중인 버퍼
		bool m_bExternalBuffer;		// 외부 버퍼를 사용중인지
		intptr_t m_nExternalSize;	// 외부 버퍼 사이즈

		CSafeArray<byte, true,Int32> m_Data;
		intptr_t m_nCursor;

		void Initialize();
		void CopyTo(CSerializer &dest) const;
	};
}

#include "Serialize.inl"

