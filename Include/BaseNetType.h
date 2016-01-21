#pragma once
#include <functional>

namespace Zero
{
	/**
	shared 기본 마샬링 타입 (marshaler.cs, IDLParser.cpp::cIDLParser::cIDLParser)
	  --> cpp -> cs 항목으로


	1. 이름변경은 없으며 자동 마샬링되는 것들

	- 내부 타입 : PackInternal, Protocol8, PacketMode8, CPackOption, CParamAccept, PacketType, eUdpMode


	- 기본 데이터 타입
	  byte, bool, short, UInt16, Int32, UInt32, Int64, UInt64, float, double

	- 기타 타입
	  RemoteID, Guid


	2. 자동 마샬링과 함께 이름까지 자동 변경되는 것들 (항목 추가시 idl파일에서 "Rename"명령 사용)

	- 스트링
	  std::wstring	-> string
	  Zero::StringW -> string
	  Zero::String	-> string (유니코드 프로젝트 아닌경우 주의 - c#의 string은 유니코드임)

	*/

	typedef Int32	RemoteID;
	typedef Int32	pktsize_t;		// 패킷사이즈 : wsabuf의 len값 = unsigned long
	typedef UInt32	order_t;		// 패킷순서
	typedef sbyte	pkpro_t;		// 프로토콜 방식
	typedef sbyte	pkpack_t;		// pack size
	typedef Int16	pktype_t;		// 패킷 id size

	typedef CSafeArray<NetAddress>			NetAddrArr;

	typedef CSafeArray<Int32,true>			ArrInt;
	typedef CSafeArray<byte, true,Int32>	ArrByte;
	typedef CSafeArray<RemoteID,true,int>	ArrRemoteID;


	// lamda function list
	typedef std::function<void(Zero::CAdo*)> lamda_prepare_sp;
	typedef std::function<void(bool bComplete, Zero::CAdo*, intptr_t query_counter)> lamda_db_result;

	typedef std::function<void(void* &UserData)> lamda_request;
	typedef std::function<void(void* UserData)> lamda_result;

	AUTO_SERIALIZE_ENUM8(Protocol8);
	AUTO_SERIALIZE_ENUM8(PacketMode8);
	AUTO_SERIALIZE_ENUM16(PacketType);
}