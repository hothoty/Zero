#pragma once
#pragma comment(lib,"winmm.lib")

namespace Zero
{
	class CTIME
	{
	public:
		CTIME();
		~CTIME();

		bool GetPrecision(DWORD dwTimeMS);		// 정밀시간계산(누적오차허용안함)
		bool Get(DWORD dwTimeMS);				// 시간계산
		void Reset();
		DWORD GetTimeBack(DWORD dwTimeMS);		// 남아있는시간 구하기

	private:
		DWORD m_dwBacksettime;
		DWORD m_dwNowsettime;
	};

	class CFPS
	{
	public:
		CFPS();
		~CFPS();

		float Get();

	private:
		bool  m_bUpdate;
		DWORD m_dwStartTime, m_dwFrame;
		float m_fFPS;
	};


	/**
	{
		Zero::AutoTimer t;		// 타이머 시작
		Zero::String dummy;
		DBRECV->Exec_Login( pParam->idName, pParam->password, dummy );
		DWORD tt = t.Get();		// 시작부터 현재까지의 지난시간
	}
	*/
	class AutoTimer
	{
	private:
		DWORD m_Time;
	public:
		AutoTimer();
		void Reset();
		DWORD Get();
	};


	class TUtil
	{
	public:

		// timeGetTime값
		static DWORD GetTime();


		// time_t -> Int64값으로 변환
		static void MakeTime2Int64( const time_t& t_value, Int64& outputValue );


		// MakeTime2Int64함수로 만들어진 Int64 값을 원래의 time_t값으로 복원한다
		static void RestoreInt64_2Time( const Int64& makedValue, time_t& outputValue );


		// 시간 알아내기 @param nAfterTimeSec 미래의 시간(60이면 60초 뒤의 시간을 의미)
		static void GetTime_Value(time_t& outputTime, const int& nAfterTimeSec=0);


		// 시간 알아내기 (몇시간뒤/몇분뒤...)
		static void GetTime_AfterHour(time_t& outputTime, const int& nAfterTimeHour);
		static void GetTime_AfterMin(time_t& outputTime, const int& nAfterTimeMin);


		// 특정시간에서 특정시간이 되기까지 몇초가 남았는지 알아내기 : 0보다 작은값이 리턴되면 지난시간을 의미함
		static void GetRemainTime(const time_t& Time1, const time_t& Time2, int& outputTime);
	};
}


