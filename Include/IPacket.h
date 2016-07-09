#pragma once

namespace Zero
{
	class IProxy;
	class IStub;

	class IPacket
	{
	public:
		IPacket();
		virtual ~IPacket();

		virtual void Attach(IProxy* proxy, IStub* stub) = 0;
		virtual void AttachProxy(IProxy* proxy) = 0;
		virtual void AttachStub(IStub* stub) = 0;
		virtual void DetachProxy(IProxy* proxy) = 0;
		virtual void DetachStub(IStub* stub) = 0;

		virtual void NeedImplement(StringA pkname) = 0;
		virtual RemoteID GetSelfRemoteID() = 0;

	protected:
		virtual void NeedDefine(Int16 pkid) = 0;
		bool Recv(const CPackOption &pkOption, const Zero::PacketType pkType, CRecvedMessage& RecvMsg, const CSafeArray<IStub*>& arrStub);
		void SendCS(const Zero::RemoteID remote, const UInt32 cev, const UInt32 prv, const Zero::String na, const UInt16 np, const Int32 authkey, const UInt16 nps, ArrByte* pDummy, bool rcm, bool rcc, Guid ck, const Zero::RemoteID selfID, bool ms, Guid mk);
		virtual intptr_t Send(const RemoteID remote, CPackOption &pkOption, CMessage& msg, const bool bForceInternal = false);
		friend IProxy;
	};


	class IProxy
	{
	public:
		IPacket* m_owner;
		bool PacketSend(const RemoteID remote, CPackOption& pkOption, CMessage &msg);
		IProxy();
		virtual ~IProxy();
	};
	class IStub
	{
	public:
		IPacket *m_owner;
		virtual bool ProcessMsg(CRecvedMessage& rm) = 0;
		IStub();
		virtual ~IStub();
	};
}

