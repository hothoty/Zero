// Auto created from IDLCompiler.exe
using System;
using System.Collections.Generic;
using System.Net;


namespace Rmi 
{

public class Proxy : ZNet.PKProxy
{
	public bool request_message(ZNet.RemoteID remote, ZNet.CPackOption pkOption, RemoteClass.CUserClass testClass, Dictionary<Int32,Int32> dic_test, string msg )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.request_message; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );

		RemoteClass.Marshaler.Write( Msg, testClass );
		RemoteClass.Marshaler.Write( Msg, dic_test );
		RemoteClass.Marshaler.Write( Msg, msg );

		return PacketSend( remote, pkOption, Msg );
	} 

	public bool request_message(ZNet.RemoteID[] remotes, ZNet.CPackOption pkOption, RemoteClass.CUserClass testClass, Dictionary<Int32,Int32> dic_test, string msg )
	{
		foreach(var obj in remotes)
			request_message(obj, pkOption, testClass, dic_test, msg );
		return true;
	}

	public bool reponse_message(ZNet.RemoteID remote, ZNet.CPackOption pkOption, RemoteClass.CUserClass testClass, Dictionary<Int32,Int32> dic_test, string msg )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.reponse_message; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );

		RemoteClass.Marshaler.Write( Msg, testClass );
		RemoteClass.Marshaler.Write( Msg, dic_test );
		RemoteClass.Marshaler.Write( Msg, msg );

		return PacketSend( remote, pkOption, Msg );
	} 

	public bool reponse_message(ZNet.RemoteID[] remotes, ZNet.CPackOption pkOption, RemoteClass.CUserClass testClass, Dictionary<Int32,Int32> dic_test, string msg )
	{
		foreach(var obj in remotes)
			reponse_message(obj, pkOption, testClass, dic_test, msg );
		return true;
	}

}

}

