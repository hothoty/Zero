// Auto created from IDLCompiler.exe
using System;
using System.Collections.Generic;
using System.Net;


namespace Rmi 
{

public class Proxy : ZNet.PKProxy
{
	public bool request_Udp(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.request_Udp; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );

		RemoteClass.Marshaler.Write( Msg, msg );

		return PacketSend( remote, pkOption, Msg );
	} 

	public bool reponse_Udp(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.reponse_Udp; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );

		RemoteClass.Marshaler.Write( Msg, msg );

		return PacketSend( remote, pkOption, Msg );
	} 

}

}

