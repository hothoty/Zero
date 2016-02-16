// Auto created from IDLCompiler.exe
using System;
using System.Collections.Generic;
using System.Net;


namespace Rmi 
{

public class Proxy : ZNet.PKProxy
{
	public bool request_lobby_list(ZNet.RemoteID remote, ZNet.CPackOption pkOption )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.request_lobby_list; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );


		return PacketSend( remote, pkOption, Msg );
	} 

	public bool notify_lobby_list(ZNet.RemoteID remote, ZNet.CPackOption pkOption, Dictionary<ZNet.RemoteID,ZNet.MasterInfo> lobby_list )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.notify_lobby_list; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );

		RemoteClass.Marshaler.Write( Msg, lobby_list );

		return PacketSend( remote, pkOption, Msg );
	} 

	public bool Chat(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.Chat; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );

		RemoteClass.Marshaler.Write( Msg, msg );

		return PacketSend( remote, pkOption, Msg );
	} 

	public bool request_move_to_server(ZNet.RemoteID remote, ZNet.CPackOption pkOption, int server_type, string lobbyname_if_login, int roomnum_if_lobby )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.request_move_to_server; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );

		RemoteClass.Marshaler.Write( Msg, server_type );
		RemoteClass.Marshaler.Write( Msg, lobbyname_if_login );
		RemoteClass.Marshaler.Write( Msg, roomnum_if_lobby );

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

	public bool request_make_room(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string name )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.request_make_room; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );

		RemoteClass.Marshaler.Write( Msg, name );

		return PacketSend( remote, pkOption, Msg );
	} 

	public bool request_join_room(ZNet.RemoteID remote, ZNet.CPackOption pkOption, Guid roomID )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.request_join_room; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );

		RemoteClass.Marshaler.Write( Msg, roomID );

		return PacketSend( remote, pkOption, Msg );
	} 

	public bool room_lobby_makeroom(ZNet.RemoteID remote, ZNet.CPackOption pkOption, Guid roomID, string name, int number, ZNet.RemoteID remote_svr, Guid userID )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.room_lobby_makeroom; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );

		RemoteClass.Marshaler.Write( Msg, roomID );
		RemoteClass.Marshaler.Write( Msg, name );
		RemoteClass.Marshaler.Write( Msg, number );
		RemoteClass.Marshaler.Write( Msg, remote_svr );
		RemoteClass.Marshaler.Write( Msg, userID );

		return PacketSend( remote, pkOption, Msg );
	} 

	public bool room_lobby_joinroom(ZNet.RemoteID remote, ZNet.CPackOption pkOption, Guid roomID, Guid userID )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.room_lobby_joinroom; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );

		RemoteClass.Marshaler.Write( Msg, roomID );
		RemoteClass.Marshaler.Write( Msg, userID );

		return PacketSend( remote, pkOption, Msg );
	} 

	public bool room_lobby_outroom(ZNet.RemoteID remote, ZNet.CPackOption pkOption, Guid roomID, Guid userID )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.room_lobby_outroom; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );

		RemoteClass.Marshaler.Write( Msg, roomID );
		RemoteClass.Marshaler.Write( Msg, userID );

		return PacketSend( remote, pkOption, Msg );
	} 

}

}

