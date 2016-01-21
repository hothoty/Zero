#pragma once

namespace RemoteClass
{
	class CUserClass : public Zero::CSerializable
	{
	public:
		Zero::CDictionary<Int32, Int32> map_test;
		Zero::StringW str_test;
		Int64 int_test;

		CUserClass()
		{
			map_test.Add(123, 456);
			map_test.Add(456, 789);
			str_test = L"abc°¡³ª´Ù";
			int_test = 12345000;
		}

		AUTO_SERIALIZE3(map_test, str_test, int_test);
	};
}