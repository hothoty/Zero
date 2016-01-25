// Auto created from IDLCompiler.exe
using System;
using System.Collections.Generic;
using System.Net;


namespace Rmi 
{
	
public class Stub : ZNet.PKStub
{
	public delegate bool request_EchoDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, RemoteClass.CUserClass testClass, Dictionary<Int32,Int32> dic_test, string msg);
	public request_EchoDelegate request_Echo = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, RemoteClass.CUserClass testClass, Dictionary<Int32,Int32> dic_test, string msg)
	{
		return false;
	};
	public delegate bool reponse_EchoDelegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, RemoteClass.CUserClass testClass, Dictionary<Int32,Int32> dic_test, string msg);
	public reponse_EchoDelegate reponse_Echo = delegate(ZNet.RemoteID remote, ZNet.CPackOption pkOption, RemoteClass.CUserClass testClass, Dictionary<Int32,Int32> dic_test, string msg)
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
		case Common.request_Echo: 
			{
				RemoteClass.CUserClass testClass; RemoteClass.Marshaler.Read(__msg, out testClass);
				Dictionary<Int32,Int32> dic_test; RemoteClass.Marshaler.Read(__msg, out dic_test);
				string msg; RemoteClass.Marshaler.Read(__msg, out msg);

				bool bRet = request_Echo( remote, pkOption, testClass, dic_test, msg );
				if( bRet==false )
					NeedImplement("request_Echo");
			} 
			break; 

		case Common.reponse_Echo: 
			{
				RemoteClass.CUserClass testClass; RemoteClass.Marshaler.Read(__msg, out testClass);
				Dictionary<Int32,Int32> dic_test; RemoteClass.Marshaler.Read(__msg, out dic_test);
				string msg; RemoteClass.Marshaler.Read(__msg, out msg);

				bool bRet = reponse_Echo( remote, pkOption, testClass, dic_test, msg );
				if( bRet==false )
					NeedImplement("reponse_Echo");
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

