// Auto created from IDLCompiler.exe
using System;
using System.Collections.Generic;
using System.Net;


namespace Rmi 
{

public class Proxy : ZNet.PKProxy
{
	public bool request_message(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.request_message; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );

		RemoteClass.Marshaler.Write( Msg, msg );

		return PacketSend( remote, pkOption, Msg );
	} 

	public bool reponse_message(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.reponse_message; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );

		RemoteClass.Marshaler.Write( Msg, msg );

		return PacketSend( remote, pkOption, Msg );
	} 

	public bool request_move_to_server(ZNet.RemoteID remote, ZNet.CPackOption pkOption, int server_type )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.request_move_to_server; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );

		RemoteClass.Marshaler.Write( Msg, server_type );

		return PacketSend( remote, pkOption, Msg );
	} 

	public bool reponse_move_to_server(ZNet.RemoteID remote, ZNet.CPackOption pkOption, bool result, ZNet.NetAddress addr )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.reponse_move_to_server; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );

		RemoteClass.Marshaler.Write( Msg, result );
		RemoteClass.Marshaler.Write( Msg, addr );

		return PacketSend( remote, pkOption, Msg );
	} 

}

}

