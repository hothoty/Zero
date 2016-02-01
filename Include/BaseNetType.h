#pragma once
#include <functional>

namespace Zero
{
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