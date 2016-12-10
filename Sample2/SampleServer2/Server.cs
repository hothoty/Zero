using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

/// <summary>
/// c#의 Async/Await패턴을 실시간 소켓서버에 적용하여
/// 
/// 멀티 스레드 동기화 문제나 콜백지옥이 없는
/// 
/// 매우 간단한 서버를 제작하는 방법을 보여줍니다
/// 
/// 
/// </summary>
namespace SampleServer2
{
    class Server
    {
        // 네트워크 코어 클래스 선언
        ZNet.CoreServerNet net;


        // 패킷 송수신 rmi
        public Rmi.Proxy proxy;
        public Rmi.Stub stub;


        // 접속상태의 원격 클라이언트들
        public Dictionary<ZNet.RemoteID, RemoteClient> RemoteClients = new Dictionary<ZNet.RemoteID, RemoteClient>();


        Form1 form;



        public Server(Form1 f)
        {
            form = f;

            net = new ZNet.CoreServerNet();

            proxy = new Rmi.Proxy();
            stub = new Rmi.Stub();
            net.Attach(proxy, stub);

            InitStub();
        }

        public void Start()
        {
            // 서비스 시작 파라미터 선언
            ZNet.StartOption param = new ZNet.StartOption();


            // Listen 주소 설정
            param.m_IpAddressListen = "127.0.0.1";
            param.m_PortListen = 29999;


            // c#의 async/await기능을 이용할경우 반드시 0으로 설정
            param.m_LogicThreadCount = 0;



            ZNet.ResultInfo outResult = new ZNet.ResultInfo();
            if (net.Start(param, outResult))
            {
                form.Put("service start ok");
            }
            else
            {
                form.Put("start failed: {0}", outResult.msg);
                return;
            }
        }

        public void Update()
        {
            // async/await처리시 필요한 네트워크 패킷/이벤트 실행작업
            net.NetLoop();
        }

        RemoteClient GetRemoteClient(ZNet.RemoteID remote)
        {
            RemoteClient rc;
            if (RemoteClients.TryGetValue(remote, out rc) == false) return null;
            return rc;
        }

        void InitStub()
        {
            // 내부 메세지 출력
            net.message_handler = (ZNet.ResultInfo resultInfo) =>
            {
                form.Put("Msg : " + resultInfo.m_Level + resultInfo.msg);
            };


            // 예외 출력
            net.exception_handler = (e) =>
            {
                form.Put("exception : " + e.ToString());
            };


            // 클라 -> 서버로 입장
            net.client_join_handler = (ZNet.RemoteID remote, ZNet.NetAddress addr, ZNet.ArrByte move_server, ZNet.ArrByte move_param) =>
            {
                RemoteClient rc = new RemoteClient();
                rc.remoteID = remote;
                RemoteClients.Add(remote, rc);
                rc.addr = addr;
                form.Put("join rc : addr = {0}", addr.m_ip);
            };


            // 클라 -> 서버 퇴장
            net.client_leave_handler = (ZNet.RemoteID remote, bool bMoveServer) =>
            {
                RemoteClient rc = GetRemoteClient(remote);
                if (rc == null) return;

                form.Put("leave rc : addr = {0}", rc.addr.m_ip);
                RemoteClients.Remove(remote);
            };


            // 로그인 패킷 처리 샘플 : 오래걸리는 작업을 async/await을 사용하여 비동기로 순서성 있게 처리하는 방법을 보여줍니다
            stub.cs_login = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, string username) =>
            {
                RemoteClient rc = GetRemoteClient(remote);
                if (rc == null) return true;


                Async.Run(() =>
                {
                    // [Async부분] 응답시간이 오래걸리는 DB나 기타장치에서 유저데이터를 읽어오는 부분을 이곳에서 비동기로 처리합니다
                    rc.userID = 123;
                    rc.username = "홍길동";

                }, () =>
                {
                    // [Await부분] 비동기 처리가 완료된 후 실행되는 부분: 원래의 패킷을 받았을때 처리하던 메인 스레드에서 실행이 보장됨
                    form.Put("login result username = {0}  (Await)", rc.username);


                    // 유저에게 보내줄 데이터를 구성
                    NetCommon.CUser user = new NetCommon.CUser();
                    user.userID = rc.userID;
                    user.username = rc.username;
                    user.addr = rc.addr;
                    user.test_enum = NetCommon.SampleEnum.Abc;


                    // 로그인 성공 패킷을 보내주는 샘플코드
                    proxy.sc_login_ok(remote, ZNet.CPackOption.Basic, user);

                });
                return true;
            };

            // 클라이언트가 보낸 패킷 처리 샘플
            stub.Chat9 = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, string txt, int a, byte b, short rr) =>
            {
                proxy.Chat9(remote, pkOption, txt, a, b, rr);
                return true;
            };
        }
    }
}
