using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using ZNet;

public class CMain : MonoBehaviour {

	string input_text;
    List<string> received_texts;

    CoreClientNet m_Core;
    Rmi.Proxy proxy;
    Rmi.Stub stub;

    Vector2 currentScrollPos = new Vector2();

    void Awake()
	{
        input_text = "";
        received_texts = new List<string>();
        proxy = new Rmi.Proxy();
        stub = new Rmi.Stub();
    }

	void Start()
	{
		this.m_Core = new CoreClientNet();
        m_Core.Attach(proxy, stub);

        m_Core.server_join_handler = (ZNet.ConnectionInfo info) =>
        {
            this.received_texts.Add(string.Format("JoinOK [{0}:{1}] - move[{2}]", info.addr.m_ip, info.addr.m_port, info.moved));
            this.currentScrollPos.y = float.PositiveInfinity;
        };
        m_Core.server_leave_handler = (ZNet.ConnectionInfo info) =>
        {
            this.received_texts.Add(string.Format("Leave [{0}:{1}] - move[{2}]", info.addr.m_ip, info.addr.m_port, info.moved));
            this.currentScrollPos.y = float.PositiveInfinity;
        };

        m_Core.server_connect_result_handler = (bool isConnectSuccess) =>
        {
            if (isConnectSuccess)
            {
                this.received_texts.Add("Connected!");
                this.currentScrollPos.y = float.PositiveInfinity;
            }
            else
            {
                this.received_texts.Add("Connect Fail!");
                this.currentScrollPos.y = float.PositiveInfinity;
            }
        };

        m_Core.message_handler = (ZNet.ResultInfo result) =>
        {
            string str_msg = "Msg : ";
            str_msg += result.msg;
            this.received_texts.Add(str_msg);
            this.currentScrollPos.y = float.PositiveInfinity;
        };


        stub.reponse_Echo = (ZNet.RemoteID remote, ZNet.CPackOption pkOption, RemoteClass.CUserClass testClass, Dictionary<int, int> dic_test, string msg) =>
        {
            this.received_texts.Add(msg);
            this.currentScrollPos.y = float.PositiveInfinity;
            return true;
        };

        this.received_texts.Add("Unity Echo Client Sample.");
        this.currentScrollPos.y = float.PositiveInfinity;

        m_Core.Connect(
            "127.0.0.1",
            20000/*tcp port*/,
            0,/*protocol version*/
            0/*udp disable=0*/,
            true/*mobile*/,
            false/*RecoveryUse*/
        );
	}

    /// <summary>
    /// 정상적인 종료시에는 OnApplicationQuit매소드에서 disconnect를 호출해 줘야 유니티가 hang되지 않는다.
    /// </summary>
    void OnApplicationQuit()
    {
        m_Core.Destroy();
    }

	/// <summary>
	/// 네트워크 관련 이벤트 처리
	/// </summary>
	void Update()
	{
        m_Core.NetLoop();
	}
	
	void OnGUI()
	{
		// Received text.
		GUILayout.BeginVertical();
		currentScrollPos = GUILayout.BeginScrollView(currentScrollPos, 
		                                             GUILayout.MaxWidth(Screen.width), GUILayout.MinWidth(Screen.width),
		                                             GUILayout.MaxHeight(Screen.height - 100), GUILayout.MinHeight(Screen.height - 100));
		
		foreach (string text in this.received_texts)
		{
			GUILayout.BeginHorizontal();
			GUI.skin.label.wordWrap = true;
			GUILayout.Label(text);
			GUILayout.EndHorizontal();
		}
		
		GUILayout.EndScrollView();
		GUILayout.EndVertical();
		
		
		// Input.
		GUILayout.BeginHorizontal();
		this.input_text = GUILayout.TextField(this.input_text, GUILayout.MaxWidth(Screen.width - 100), GUILayout.MinWidth(Screen.width - 100),
		                                      GUILayout.MaxHeight(50), GUILayout.MinHeight(50));
		
		if (GUILayout.Button("Send", GUILayout.MaxWidth(100), GUILayout.MinWidth(100), GUILayout.MaxHeight(50), GUILayout.MinHeight(50)))
		{
			// 패킷 송신 테스트
			RemoteClass.CUserClass testClass = new RemoteClass.CUserClass();
			Dictionary<int,int> testDic = new Dictionary<int,int>();
			testDic.Add(1212345, 1230);

            proxy.request_Echo(RemoteID.Remote_Server, new CPackOption(PacketMode8.PM_None), testClass, testDic, this.input_text);
            this.input_text = "";
		}
		GUILayout.EndHorizontal();
	}
}
