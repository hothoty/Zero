// Auto created from IDLCompiler.exe
using System;
using System.Collections.Generic;
using System.Net;


namespace Rmi 
{
	
public class Stub : ZNet.PKStub
{
	public delegate bool request_lobby_listDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption);
	public request_lobby_listDelegate request_lobby_list = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption)
	{
		return false;
	};
	public delegate bool notify_lobby_listDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, Dictionary<ZNet.RemoteID,ZNet.MasterInfo> lobby_list);
	public notify_lobby_listDelegate notify_lobby_list = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, Dictionary<ZNet.RemoteID,ZNet.MasterInfo> lobby_list)
	{
		return false;
	};
	public delegate bool ChatDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg);
	public ChatDelegate Chat = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string msg)
	{
		return false;
	};
	public delegate bool request_move_to_serverDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, int server_type, string lobbyname_if_login, int roomnum_if_lobby);
	public request_move_to_serverDelegate request_move_to_server = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, int server_type, string lobbyname_if_login, int roomnum_if_lobby)
	{
		return false;
	};
	public delegate bool reponse_move_to_serverDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, bool result, ZNet.NetAddress addr);
	public reponse_move_to_serverDelegate reponse_move_to_server = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, bool result, ZNet.NetAddress addr)
	{
		return false;
	};
	public delegate bool request_make_roomDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string name);
	public request_make_roomDelegate request_make_room = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, string name)
	{
		return false;
	};
	public delegate bool request_join_roomDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, Guid roomID);
	public request_join_roomDelegate request_join_room = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, Guid roomID)
	{
		return false;
	};
	public delegate bool room_lobby_makeroomDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, Guid roomID, string name, int number, ZNet.RemoteID remote_svr, Guid userID);
	public room_lobby_makeroomDelegate room_lobby_makeroom = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, Guid roomID, string name, int number, ZNet.RemoteID remote_svr, Guid userID)
	{
		return false;
	};
	public delegate bool room_lobby_joinroomDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, Guid roomID, Guid userID);
	public room_lobby_joinroomDelegate room_lobby_joinroom = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, Guid roomID, Guid userID)
	{
		return false;
	};
	public delegate bool room_lobby_outroomDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, Guid roomID, Guid userID);
	public room_lobby_outroomDelegate room_lobby_outroom = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, Guid roomID, Guid userID)
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
		case Common.request_lobby_list: 
			{

				bool bRet = request_lobby_list( remote, pkOption );
				if( bRet==false )
					NeedImplement("request_lobby_list");
			} 
			break; 

		case Common.notify_lobby_list: 
			{
				Dictionary<ZNet.RemoteID,ZNet.MasterInfo> lobby_list; RemoteClass.Marshaler.Read(__msg, out lobby_list);

				bool bRet = notify_lobby_list( remote, pkOption, lobby_list );
				if( bRet==false )
					NeedImplement("notify_lobby_list");
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

		case Common.request_move_to_server: 
			{
				int server_type; RemoteClass.Marshaler.Read(__msg, out server_type);
				string lobbyname_if_login; RemoteClass.Marshaler.Read(__msg, out lobbyname_if_login);
				int roomnum_if_lobby; RemoteClass.Marshaler.Read(__msg, out roomnum_if_lobby);

				bool bRet = request_move_to_server( remote, pkOption, server_type, lobbyname_if_login, roomnum_if_lobby );
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

		case Common.request_make_room: 
			{
				string name; RemoteClass.Marshaler.Read(__msg, out name);

				bool bRet = request_make_room( remote, pkOption, name );
				if( bRet==false )
					NeedImplement("request_make_room");
			} 
			break; 

		case Common.request_join_room: 
			{
				Guid roomID; RemoteClass.Marshaler.Read(__msg, out roomID);

				bool bRet = request_join_room( remote, pkOption, roomID );
				if( bRet==false )
					NeedImplement("request_join_room");
			} 
			break; 

		case Common.room_lobby_makeroom: 
			{
				Guid roomID; RemoteClass.Marshaler.Read(__msg, out roomID);
				string name; RemoteClass.Marshaler.Read(__msg, out name);
				int number; RemoteClass.Marshaler.Read(__msg, out number);
				ZNet.RemoteID remote_svr; RemoteClass.Marshaler.Read(__msg, out remote_svr);
				Guid userID; RemoteClass.Marshaler.Read(__msg, out userID);

				bool bRet = room_lobby_makeroom( remote, pkOption, roomID, name, number, remote_svr, userID );
				if( bRet==false )
					NeedImplement("room_lobby_makeroom");
			} 
			break; 

		case Common.room_lobby_joinroom: 
			{
				Guid roomID; RemoteClass.Marshaler.Read(__msg, out roomID);
				Guid userID; RemoteClass.Marshaler.Read(__msg, out userID);

				bool bRet = room_lobby_joinroom( remote, pkOption, roomID, userID );
				if( bRet==false )
					NeedImplement("room_lobby_joinroom");
			} 
			break; 

		case Common.room_lobby_outroom: 
			{
				Guid roomID; RemoteClass.Marshaler.Read(__msg, out roomID);
				Guid userID; RemoteClass.Marshaler.Read(__msg, out userID);

				bool bRet = room_lobby_outroom( remote, pkOption, roomID, userID );
				if( bRet==false )
					NeedImplement("room_lobby_outroom");
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

