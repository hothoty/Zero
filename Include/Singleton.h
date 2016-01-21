#pragma once

/**
전역 싱글턴 클래스 만들기

- 전역 싱글턴 클래스 자동 생성

- 클래스 내부 static 인스턴스 사용

- 외부에서 클래스 생성/복사 차단, 클래스 내부 인스턴스만 접근 가능

- FUNCTION_SINGLETON를 이용한 인터페이스 제공(싱글턴 클래스 인스턴스 사용)

@code
	class CSample
	{
		DECLARE_SINGLETON(CSample);
	public:
		void CallFunction();
	};

	FUNCTION_SINGLETON( CSample, Sample );

@사용법
	Sample()->CallFunction();

*/
#define DECLARE_SINGLETON( className )\
public:\
	static className* Instance()\
	{\
		static className instance;\
		return &instance;\
	}\
private:\
	className();\
	className& operator=(const className&);\
	className(const className&);


#define FUNCTION_SINGLETON( className , funcName ) \
	static className* ##funcName()\
	{\
		return className::Instance();\
	};


