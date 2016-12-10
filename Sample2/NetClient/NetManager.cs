using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

/// <summary>
/// 클라이언트의 네트워크 관리자 라이브러리 프로젝트
/// 
/// 테스트용 클라이언트나 unity 3d 등에서도 이 라이브러리만 포함하면 함께 사용이 가능하도록
/// 
/// .NET 3.5 프레임웍으로 제작하였습니다
/// 
/// </summary>
namespace NetClient
{
    public class NetManager
    {
        ZNet.CoreClientNet core;

        public Rmi.Proxy proxy;
        public Rmi.Stub stub;

        bool first_connect = true;


        public delegate void InfoDelegate(string txt);
        public delegate void NetDelegate();
        public delegate void InternalDelegate(string txt);

        public InfoDelegate InfoHandler;
        public NetDelegate JoinHandler;
        public NetDelegate LeaveHandler;
        public NetDelegate JoinFailedHandler;
        public InternalDelegate InternalHandler;

        public bool IsConnected
        {
            get { return core.IsJoined(); }
        }


        public NetManager()
        {
            core = new ZNet.CoreClientNet();

            proxy = new Rmi.Proxy();
            stub = new Rmi.Stub();

            core.Attach(proxy, stub);


            // 서버로의 접속시도 결과 처리
            core.server_connect_result_handler = (bool isConnectSuccess) =>
            {
                if (isConnectSuccess)
                {
                    OutMsg("connect ok");
                }
                else
                {
                    OutMsg("connect fail");
                    if (JoinFailedHandler != null)
                        JoinFailedHandler();
                }
            };


            // 서버에 접속성공 이후 실제 입장성공 시점
            core.server_join_handler = (ZNet.ConnectionInfo info) =>
            {
                OutMsg("join server = {0}:{1}", info.addr.addr, info.addr.port);

                proxy.cs_login(ZNet.RemoteID.Remote_Server, ZNet.CPackOption.Basic, "홍길동");
                if (JoinHandler != null)
                    JoinHandler();
            };


            // 서버에서 퇴장됨
            core.server_leave_handler = (ZNet.ConnectionInfo info) =>
            {
                OutMsg("leave server {0}:{1}", info.addr.addr, info.addr.port);
                if (LeaveHandler != null)
                    LeaveHandler();
            };

            core.message_handler = (ZNet.ResultInfo result) =>
            {
                if (InternalHandler != null)
                    InternalHandler(result.msg);
            };


            // 로그인 성공 패킷 처리
            stub.sc_login_ok = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, NetCommon.CUser user_data) =>
            {
                OutMsg("server login complete.  my username = {0}, userID = {1}", user_data.username, user_data.userID);
                return true;
            };
        }

        void OutMsg(string txt)
        {
            if (InfoHandler != null)
                InfoHandler(txt);
        }
        public void OutMsg(string txt, params object[] args)
        {
            if (InfoHandler != null)
                InfoHandler(string.Format(txt, args));
        }


        // 접속하기
        public void Connect(string ipaddr, ushort portnum)
        {
            if (core.IsNetworkConnect())
            {
                OutMsg("connection is live.. ignore connect cmd.");
                return;
            }

            if (first_connect)
            {
                core.Connect(ipaddr, portnum, NetCommon.Vars.version);
                first_connect = false;
            }
            else
            {
                core.ReConnect(ipaddr, portnum);
            }
        }


        // 접속해제 (Connect함수로 다시 재접속 가능)
        public void Disconnect()
        {
            core.Leave();
        }

        // 접속종료 (네트워크 코어 파괴 - 즉 재접속 불가능)
        public void Destroy()
        {
            core.Destroy();
        }


        // 네트워크 이벤트 처리
        public void Update()
        {
            core.NetLoop();
        }
    }
}
