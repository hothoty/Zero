#pragma once

namespace Zero
{
	// shared
	enum Protocol8
	{
		Protocol_Tcp = 0,
		Protocol_Udp				// 순서보장
	};

	// shared
	enum PacketMode8
	{
		PM_None = 0,				// 가공하지 않음
		PM_Encrypt_Mode = 1,		// 기본 암호화

		PM_Encrypt_AES = 20,		// AES암호(느림)
		PM_Compress = 21,			// 압축처리

#ifdef DEF_RSA_MODE
		PM_Encrypt_RSA = 100,		// [매우느림] RSA방식 암호 : 이걸 사용시 로직스레드 숫자를 cpu개수만큼 충분히 설정 -> 내부적 성능향상에 큰 도움이된다
#endif
	};
	
	// shared : Int32(RemoteID)
	enum RemoteID_Define
	{
		Remote_None = -1,			// 유효하지 않음
		Remote_Server = -2,			// 서버전용ID
		Remote_P2Ps = -3,			// 서버전용ID P2P
		Remote_Reserved = -4,		// 서버전용 예약
		Remote_Master = -100,		// 마스터클라이언트 예약
		Remote_Group = -10000,		// Group  이보다 작은값 모두 해당 : if (remoteID < Remote_Group) -->  이렇게 GroupID판단가능
        Remote_Client = 0,			// Client 이보다 큰값 모두 해당   : if (remoteID > Remote_Client) --> 이렇게 ClientID판단가능
	};

	#define IS_REMOTE_GROUP(remote)		(remote < Remote_Group)
	#define IS_REMOTE_CLIENT(remote)	(remote > Remote_Client)
	#define IS_REMOTE_MASTER(remote)	(remote <= Remote_Master && remote > Remote_Group)

	// shared : 2byte
	enum PacketType
	{
		PacketType_Internal = -1,
		PacketType_Event = -2,

		PacketType_User = 0,
		PacketType_MaxCount = 32767
	};
}

