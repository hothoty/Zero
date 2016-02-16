#pragma once

namespace Zero
{
	// shared
	class CMasterInfo : public Zero::CSerializable
	{
	public:
		Zero::RemoteID		m_remote;
		Zero::String		m_Description;
		int					m_ServerType;
		Zero::NetAddress	m_Addr;
		size_t				m_Clients;

		CMasterInfo()
		{
			m_remote = RemoteID_Define::Remote_None;
			m_ServerType = -1;
			m_Clients = 0;
		}
		CMasterInfo(RemoteID remote, String str, int svr_type, NetAddress addr, int clients)
		{
			m_remote = remote;
			m_Description = str;
			m_ServerType = svr_type;
			m_Addr = addr;
			m_Clients = clients;
		}

		AUTO_SERIALIZE5(m_remote, m_Description, m_ServerType, m_Addr, m_Clients);
	};
	typedef std::shared_ptr<CMasterInfo> CMasterInfoPtr;
	typedef CSafeArray<CMasterInfo> MasterInfoArr;
}

