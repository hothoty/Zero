#pragma once
#include "Result.h"
#include "BaseEnums.h"
#include "Exception.h"

namespace Zero
{
	class IEventBase
	{
	public:
		virtual ~IEventBase() {};

		virtual void OnInformation(const Zero::CResultInfo& info) = 0;
		virtual void OnException(Zero::CException& e) = 0;

		virtual void OnUdp() {}
		//virtual void OnTestMessage(Zero::RemoteID remote, const byte* buffer, const pktsize_t length, void* con) {}

		virtual void OnGroupJoin(const Zero::RemoteID groupID, const Zero::RemoteID memberID, const int memberCount) {}
		virtual void OnGroupOut(const Zero::RemoteID groupID, const Zero::RemoteID memberID, const int memberCount) {}
	};
}


