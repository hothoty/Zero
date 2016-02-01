// Auto created from IDLCompiler.exe
using System;
using System.Collections.Generic;
using System.Net;


namespace Rmi 
{
	
public class Stub : ZNet.PKStub
{
	public delegate bool GroupTestCommandDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string cmd);
	public GroupTestCommandDelegate GroupTestCommand = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string cmd)
	{
		return false;
	};
	public delegate bool ChatDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg);
	public ChatDelegate Chat = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg)
	{
		return false;
	};

	public override bool ProcessMsg(ZNet.CRecvedMsg rm) 
	{
		ZNet.RemoteID remote = rm.remote;
		if( remote == ZNet.RemoteID.Remote_None )
		{
			//err
		}

		ZNet.CPackOption pkOption = rm.pkop;
		ZNet.CMessage __msg = rm.msg;
		ZNet.PacketType PkID = rm.pkID;
		if( PkID < ZNet.PacketType.PacketType_User )
			return true;

		switch( PkID ) 
		{
		case Common.GroupTestCommand: 
			{
				string cmd; RemoteClass.Marshaler.Read(__msg, out cmd);

				bool bRet = GroupTestCommand( remote, pkOption, cmd );
				if( bRet==false )
					NeedImplement("GroupTestCommand");
			} 
			break; 

		case Common.Chat: 
			{
				string msg; RemoteClass.Marshaler.Read(__msg, out msg);

				bool bRet = Chat( remote, pkOption, msg );
				if( bRet==false )
					NeedImplement("Chat");
			} 
			break; 

			default: goto __fail;
		}

		return true;

		__fail:
		{
			//err
			return false;
		}
	}

}

}

