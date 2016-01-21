#pragma once
#include "Serialize.h"

namespace Zero
{
	struct NetAddress : public CSerializable
	{
		String m_ip;
		StringA m_ipa;
		UInt16 m_port;

		NetAddress();
		NetAddress(String addr, UInt16 port);
		NetAddress(const NetAddress& src);
		void MakeAdressA(const char* addr, UInt16 port);
		void MakeAdress(String addr, UInt16 port);
		String ToString() const;

		inline bool operator==(const NetAddress &src) const
		{
			return m_ip == src.m_ip && m_port == src.m_port;
		}
		inline bool operator!=(const NetAddress &src) const
		{
			return !(m_ip == src.m_ip && m_port == src.m_port);
		}

		AUTO_SERIALIZE2(m_ip, m_port);
	};
}