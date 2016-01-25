// Auto created from IDLCompiler.exe
using System;
using System.Collections.Generic;
using System.Net;


namespace Rmi 
{
	
public class Stub : ZNet.PKStub
{
	public delegate bool request_UdpDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg);
	public request_UdpDelegate request_Udp = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg)
	{
		return false;
	};
	public delegate bool reponse_UdpDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg);
	public reponse_UdpDelegate reponse_Udp = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg)
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
		case Common.request_Udp: 
			{
				string msg; RemoteClass.Marshaler.Read(__msg, out msg);

				bool bRet = request_Udp( remote, pkOption, msg );
				if( bRet==false )
					NeedImplement("request_Udp");
			} 
			break; 

		case Common.reponse_Udp: 
			{
				string msg; RemoteClass.Marshaler.Read(__msg, out msg);

				bool bRet = reponse_Udp( remote, pkOption, msg );
				if( bRet==false )
					NeedImplement("reponse_Udp");
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

