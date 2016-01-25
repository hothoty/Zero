// Auto created from IDLCompiler.exe
using System;
using System.Collections.Generic;
using System.Net;


namespace Rmi 
{
	
public class Stub : ZNet.PKStub
{
	public delegate bool request_messageDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg);
	public request_messageDelegate request_message = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg)
	{
		return false;
	};
	public delegate bool reponse_messageDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg);
	public reponse_messageDelegate reponse_message = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg)
	{
		return false;
	};
	public delegate bool request_move_to_serverDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, int server_type);
	public request_move_to_serverDelegate request_move_to_server = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, int server_type)
	{
		return false;
	};
	public delegate bool reponse_move_to_serverDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, bool result, ZNet.NetAddress addr);
	public reponse_move_to_serverDelegate reponse_move_to_server = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, bool result, ZNet.NetAddress addr)
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
		case Common.request_message: 
			{
				string msg; RemoteClass.Marshaler.Read(__msg, out msg);

				bool bRet = request_message( remote, pkOption, msg );
				if( bRet==false )
					NeedImplement("request_message");
			} 
			break; 

		case Common.reponse_message: 
			{
				string msg; RemoteClass.Marshaler.Read(__msg, out msg);

				bool bRet = reponse_message( remote, pkOption, msg );
				if( bRet==false )
					NeedImplement("reponse_message");
			} 
			break; 

		case Common.request_move_to_server: 
			{
				int server_type; RemoteClass.Marshaler.Read(__msg, out server_type);

				bool bRet = request_move_to_server( remote, pkOption, server_type );
				if( bRet==false )
					NeedImplement("request_move_to_server");
			} 
			break; 

		case Common.reponse_move_to_server: 
			{
				bool result; RemoteClass.Marshaler.Read(__msg, out result);
				ZNet.NetAddress addr; RemoteClass.Marshaler.Read(__msg, out addr);

				bool bRet = reponse_move_to_server( remote, pkOption, result, addr );
				if( bRet==false )
					NeedImplement("reponse_move_to_server");
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

