#pragma once

namespace Zero
{
	enum IResultLevel
	{
		IMsg = 0,
		IWrn,
		IErr,
		ICri,
	};
	AUTO_SERIALIZE_ENUM8(IResultLevel);

	class CResultInfo : public CSerializable
	{
	public:
		CResultInfo();
		CResultInfo(IResultLevel lev, TCHAR* message, ...);
		CResultInfo(TCHAR* message, ...);		// level지정을 안하면 메세지 박스 처리함에 주의

		IResultLevel m_Level;
		String msg;

		AUTO_SERIALIZE2(m_Level, msg);
	};
}


