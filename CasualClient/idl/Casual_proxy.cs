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

	public bool request_go_lobby(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string lobbyname )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.request_go_lobby; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );

		RemoteClass.Marshaler.Write( Msg, lobbyname );

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

	public bool request_out_room(ZNet.RemoteID remote, ZNet.CPackOption pkOption )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.request_out_room; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );


		return PacketSend( remote, pkOption, Msg );
	} 

	public bool room_lobby_makeroom(ZNet.RemoteID remote, ZNet.CPackOption pkOption, Guid roomID, string name, int number, ZNet.RemoteID remote_svr, ZNet.RemoteID remote_lobby, Guid userID )
	{
		ZNet.CMessage Msg = new ZNet.CMessage();
		ZNet.PacketType msgID = (ZNet.PacketType)Common.room_lobby_makeroom; 
		
		Msg.WriteStart( msgID, pkOption, 0, true );

		RemoteClass.Marshaler.Write( Msg, roomID );
		RemoteClass.Marshaler.Write( Msg, name );
		RemoteClass.Marshaler.Write( Msg, number );
		RemoteClass.Marshaler.Write( Msg, remote_svr );
		RemoteClass.Marshaler.Write( Msg, remote_lobby );
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

