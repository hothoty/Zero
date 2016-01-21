#pragma once 
//#include <stdint.h>

namespace Zero
{
	/**
	Interlock 클래스

	- 32비트
	  CInterLocked::Increment32();
	  CInterLocked::Decrement32();
	  CInterLocked::Exchange32()
	  CInterLocked::CompareExchange32()

	- 64비트
	  CInterLocked::Increment64();
	  CInterLocked::Decrement64();
	  CInterLocked::Exchange64()
	  CInterLocked::CompareExchange64()

	- (intptr_t)
	  CInterLocked::IncrementPtr(val);
	  CInterLocked::DecrementPtr(val);

	*/
	class CInterLocked
	{
	public:
		#define DECLARE_INTERLOCK(type, bit, cast, if64)\
		inline static type Increment##bit(volatile type *target)\
		{\
			return InterlockedIncrement##if64((cast)target);\
		}\
		inline static type Decrement##bit(volatile type *target)\
		{\
			return InterlockedDecrement##if64((cast)target);\
		}\
		inline static type Exchange##bit(volatile type *target, type Value)\
		{\
			return InterlockedExchange##if64((cast)target, Value);\
		}\
		inline static type CompareExchange##bit(volatile type *target, type exchange, type comperand)\
		{\
			return InterlockedCompareExchange##if64((cast)target, exchange, comperand);\
		}

		DECLARE_INTERLOCK(Int32,32,volatile long*,);
		#ifdef _WIN64
		DECLARE_INTERLOCK(Int64,64,volatile __int64*,64);
		#endif

		inline static intptr_t IncrementPtr(volatile intptr_t *target)
		{
			#ifdef _WIN64
			return CInterLocked::Increment64((Int64*)target);
			#else
			return CInterLocked::Increment32((Int32*)target);
			#endif
		}
		inline static intptr_t DecrementPtr(volatile intptr_t *target)
		{
			#ifdef _WIN64
			return CInterLocked::Decrement64((Int64*)target);
			#else
			return CInterLocked::Decrement32((Int32*)target);
			#endif
		}
	};
}

