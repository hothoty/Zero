#pragma once
#include <atlcoll.h>	// GUID_NULL에러시 추가
#include <rpc.h>		// rpc 컴파일 에러시 추가
#include <rpcdce.h>
#pragma comment(lib,"rpcrt4.lib")

namespace Zero
{
	class Guid: public GUID
	{
	public:
		Guid();
		~Guid();

		static Guid Create();
		static Guid	From(GUID src);
		static bool Restore(const StringW& strGuid, OUT GUID &uuid);
		bool IsNull();
		StringW ToStringW() const;
		StringA ToStringA() const;
	};
}

namespace std
{
	template <> class hash<Zero::Guid>
	{
	public:
		size_t operator()(Zero::Guid guid) const
		{
			const DWORD* pdwData = reinterpret_cast< const DWORD* >( &guid );
			return( pdwData[0]^pdwData[1]^pdwData[2]^pdwData[3] );
		}
	};

	template <> class equal_to<Zero::Guid>
	{
	public:
		bool operator()(const Zero::Guid& lhs, const Zero::Guid& rhs) const
		{
			return( (lhs == rhs) != 0 );
		}
	};
};