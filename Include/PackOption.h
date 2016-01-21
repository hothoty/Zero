#pragma once
#include "BaseEnums.h"
#include "BaseTypes.h"

namespace Zero
{
	// shared 패킷옵션
	class CPackOption : public Zero::CSerializable
	{
	public:
		Protocol8 m_protocol;
        PacketMode8 m_pack_mode;
		order_t m_Dummy;
		RemoteID m_remote;
		RemoteID m_from;
		bool m_ext;
		bool m_ak;


		inline bool Flexible()	const { return !(m_pack_mode == PM_None || m_pack_mode == PM_Encrypt_Mode); }
		inline bool IsUdp()		const { return m_protocol == Protocol_Udp; }

		void Initialize();

		CPackOption();
		~CPackOption();

		CPackOption(const CPackOption* pSrc);
		CPackOption(const PacketMode8 pkMode, const Protocol8 protocolMode=Protocol_Tcp);

		static const CPackOption Basic;
		static const CPackOption Encrypt;
		static const CPackOption Compress;
		static const CPackOption Udp;
		static const CPackOption EncUdp;

		static int GetSerializeSize() { return sizeof(pkpro_t) + sizeof(pkpack_t) + sizeof(order_t) + sizeof(RemoteID) + sizeof(RemoteID); }

		// 순서변경금지(변경시CUserToken.cs적용)
		AUTO_SERIALIZE5(m_protocol, m_pack_mode, m_Dummy, m_remote, m_from);
	};
}


