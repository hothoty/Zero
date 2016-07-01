#pragma once
#include "basetypes.h"

namespace Zero
{
	class CStartOption
	{
	public:
		/**********************************************************
		*******          주로 설정하게 되는 내용          *********
		***********************************************************/

		String m_IpAddressListen;	// Listen IP주소 : "127.0.0.1"형태
		UInt16 m_PortListen;		// Listen (기본값 20000)

		// 인증과정에서  입력한 protocol_version값과 일치하지 않은경우 경고를 출력해준다
		UInt32 m_ProtocolVersion;


		/**
		udp port목록(유효값 : 1~60000)
		- 기본 : 0 -> tcp port + 1로 1개 자동으로 등록
		- 보통 1~4개까지 등록, 가급적 20개넘지 않게(port 1개당 수천명 커버 가능, 여러port등록시 자동 분산처리)
		  특히udp 많이 사용할경우 포트1개당 1000명 기준으로 무난히 처리(1유저당 초당20개 recv계산)
		*/
		Zero::ArrInt m_UdpPorts;


		// 로직 스레드 - 기본 1개, 0 = 사용안함(수동으로 NetLoop호출 필요)
		int m_LogicThreadCount;


		// IO스레드 - 기본값 0 = 자동설정
		int m_IoThreadCount;


		/**
		OnUpdateEvent 발생 시간
		- 기본값 0 = Update 이벤트 발생시키지 않음
		- 유효값 : 0~10000(10초), 10000이상인 경우 10000로 간주함
		*/
		int m_UpdateTimeMs;


		/**
		최대 접속 처리 가능 숫자
		- 기본값 1000
		- 유효값 1이상
		- 한계값 : ZDefine::Max_ConnectionCount참고
		- 설정값보다 동접이 많을경우 실시간으로 한계를 늘려준다(m_bExpandMaxConnect참고)
		- 실시간 동접 확장기능 작동시 순간적으로 아주 약간 서버가 느려질수 있음(따라서 가급적 예측동접을 미리 맞게 설정할것)
		- m_bAutoLimitConnection기능 사용시 해당 기능을 처리하기 위해 m_MaxConnectionCount값에 설정한 값보다 100개정도 더 여유있게 사용함
		*/
		int m_MaxConnectionCount;

		/**
		실시간 동접 확장기능 사용 여부 : 최대 접속 처리 한계값 자동 증가 (한계값 ZDefine::Max_ConnectionCount 참고)
		- 기본값 false
		- true로 설정하면 동접 숫자가 m_MaxConnectionCount값에 도달하면 실시간으로 계속 늘어난다
		*/
		bool m_bExpandMaxConnect;

		/**
		접속숫자 자동 제한 기능
		- 기본값 true
		- 최대 접속 처리 숫자를 넘어 접속시도시 자동적으로 상황을 알려주고 접속을 종료시켜주는 기능
		- 실시간 동접 확장기능이 꺼져있는 상태에서만 작동함
		- 서버와 서버간의 접속인 경우에는 예외적으로 접속할수 있음(서버간 연결이 일시적으로 끊김으로 인한 재접속 같은 상황 대비)
		*/
		bool m_bAutoLimitConnection;



		// 자동 연결복구 제한시간 : 기본값 60000(1분) - 접속시 Mobile옵션 사용한경우만 해당
		int m_RecoveryTimeMs;

		
		// 서버간 주기적으로 정보교환Tick : 기본값 60000(60초)
		int m_RefreshServerTickMs;



		/**********************************************************
		*******          특수한 경우 최적화 내용          *********
		***********************************************************/


		/**
		Rmi Disable 옵션
		- 기본값 false
		*/
		bool m_DisableRmi;

		/**
		패킷 최대 길이(Byte)
		- 기본값 4096(4k) - 클라이언트 패킷조작대비
		- 최대값 2000000000
		*/
		Zero::pktsize_t m_MaxLengthPacket;
		
		/**
		서버간 통신전용 패킷 최대 길이(Byte)
		- 권장값
		  1. Lan환경(기본값) : 262144 Byte (256k)
		  2. Lan환경X : 16384(Byte)
		- 최대값 2000000000
		*/
		Zero::pktsize_t m_MaxLengthServerPacket;


		/**
		패킷 부하량 모니터링 옵션 - 기본값 true (미세한 성능저하 -> 최대성능 => false세팅)
		*/
		bool m_MonitorTransfer;


		/**
		가속처리할 기본 IO 처리 개수
		- 기본값 : 400000
		- 권장값 : 400000 ~ 800000
		*/
		int m_FastWorkIoCount;

		/**
		IO가속 가능 일반 패킷 숫자(256바이트 이하)
		- 기본값 : 200000, 가급적 그대로 사용하기를 권장함
		- 권장값 : 200000 ~ 400000
		*/
		int m_FastIoBufferCount;

		/**
		IO가속 개수(4k)
		- 기본값 : 2000, 특별한 경우 아니면 수정하지 말것
		- 권장값 : 2000 ~ 5000
		*/
		int m_FastIoBigBufferCount;


		// Nagle옵션 - 기본true
		bool m_NagleOption;


		/**
		UDP virtual mode Tick (client)
		- 기본 : 500(ms)
		- 권장 : 200 ~ 1000
		- 가상IP 판단시점 송신TICK
		*/
		int m_ClientTickUdpMS;

		/**
		UDP virtual alive Tick (client)
		- 기본 : 15000(ms)
		- 권장 : 10000 ~ 15000
		- 가상IP 판단이후 주소유지 및 갱신을 위한 송신TICK
		*/
		int m_ClientTickUdpAliveMS;

		/**
		client UDP 판단시간(real/virtual)
		- 기본 : 15000(15초)
		- 권장 : 5000 ~ 15000 (지나치게 길게 할 필요는 없음)
		- 접속 시점에 client에서 잘 수신후 응답하는지 판단시간
		*/
		int m_TimeUdpPrepareMS;

		/**
		client UDP 제한시간
		- 기본 : 20000(20초)
		- c->s 보냈을때 응답시간 (못받았을때 클라의 fromip주소를 서버에서 판단하기 위해 보내지는 제한시간)
		- udp특성상 유실될 수 있지만 일반적인 상황에서 c->s는 즉시 서버가 받아져 제한시간까지 갈 경우는 거의 없음
		- 단 일시적 서버 부하나 네트워크 오류 상태를 대비해 복구가 가능한 충분한 시간을 세팅
		*/
		int m_TimeUdpVPrepareMS;




		/**********************************************************
		*******             각종 디폴트 값 세팅           *********
		***********************************************************/

		/**
		Master전용 디폴트값
		*/
		void SetDefaultMasterParameter()
		{
			m_MaxConnectionCount = 100;
			m_FastWorkIoCount = 40000;
			m_FastIoBufferCount = 20000;
			m_FastIoBigBufferCount = 2000;
		}

		/**
		최고 동접 + 최대 성능 설정값(처음 시동시간은 느림/모니터링 비활성)
		*/
		void SetParamMaxSpeed()
		{
			m_LogicThreadCount = 32;
			m_MaxConnectionCount = 60000;
			m_bExpandMaxConnect = false;
			m_FastWorkIoCount = 800000;
			m_FastIoBufferCount = 400000;
			m_MonitorTransfer = false;
		}

		// 일반상황 설정 기본값
		CStartOption()
		{
			m_PortListen = 20000;
			m_ProtocolVersion = 0;
			m_LogicThreadCount = 1;
			m_IoThreadCount = 0;
			m_NagleOption = true;
			m_UpdateTimeMs = 0;
			m_MaxConnectionCount = 500;
			m_bExpandMaxConnect = false;
			m_bAutoLimitConnection = true;
			m_RecoveryTimeMs = 60000;
			m_FastWorkIoCount = 400000;
			m_FastIoBufferCount = 200000;
			m_FastIoBigBufferCount = 2000;
			m_MaxLengthPacket = 4096;
			m_MaxLengthServerPacket = 262144;	// 256KB
			m_MonitorTransfer = true;
			m_ClientTickUdpMS = 500;
			m_ClientTickUdpAliveMS = 15000;
			m_TimeUdpPrepareMS = 15000;
			m_TimeUdpVPrepareMS = 20000;
			m_DisableRmi = false;
			m_RefreshServerTickMs = 60000;
		}
	};
}

