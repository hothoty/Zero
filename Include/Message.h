#pragma once

namespace Zero
{
	/**
	메세지 Stream 클래스

	- 기본값 : 내부 버퍼 사용 (버퍼 사이즈 자동 관리)

	- 외부버퍼 사용 가능 : 버퍼를 쓰기/읽기시 zero copy,  이 때 사이즈는 당연히 외부에서 알아서 관리해야함

	*/
	class CMessage
	{
	public:
		CMessage();
		~CMessage();

		CSerializer m_Stream;

	public:
		inline CMessage& operator=(const CMessage& src)
		{
			src.CopyTo(*this);
			return *this;
		}

		inline CMessage(const CMessage &src)
		{
			src.CopyTo(*this);
		}

		// 쓰기
		void WriteStart(Zero::PacketType PktType, Zero::CPackOption& pkDefault, const Int16 PkInternalValue=0, const bool isIDL=false);
		void Write(const CSerializable* pData);

		// PkOption덮어쓰기
		void WriteOption(const byte* pSrc, const Zero::CPackOption* pOption);

		// 읽기
		Zero::PacketType ReadStart(OUT Zero::CPackOption& pkDefault);
		void Read(const CSerializable* pData);

		// 데이터 정보
		inline byte* GetData()			{ return m_Stream.GetData(); }	// 데이터
		inline byte* GetData() const	{ return m_Stream.GetData(); }	// 데이터
		inline intptr_t GetCursor() const { return m_Stream.GetCursor(); }		// 커서 위치
		inline intptr_t GetBufSize() const	{ return m_Stream.GetBuffSize(); }	// 버퍼 크기

		// 데이터 복원 (내부버퍼 사용 - 메모리 복사) --> 메모리 복사시 꼭 필요한 만큼만 사용하게끔 처리함
		void RestoreInternal(const byte *ibuf, const intptr_t isize);

		// 데이터 복원 (외부버퍼 그대로 사용하기)
		void RestoreExternal(byte *ibuf, const intptr_t isize);

	private:
		inline void CopyTo(CMessage &dest) const
		{
			dest.m_Stream = m_Stream;
		}
	};


	class CRecvedMessage
	{
	public:
		CMessage msg;
		RemoteID remote;

		PacketType pkID;
		CPackOption pkop;

		CRecvedMessage() : remote(Remote_None), pkID(PacketType_MaxCount)
		{
		}
		void From(RemoteID remoteID, PacketType id, CPackOption op)
		{
			remote = remoteID;
			pkID = id;
			pkop = op;
		}
	};
}








