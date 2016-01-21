#pragma once
#include <unordered_map>

namespace Zero
{
	/**

	C# Dictionary 비슷하게 만든 std::unordered_map 래핑 클래스

	*/
	template<class _Kty, class _Ty, class _Hasher = std::hash<_Kty>, class _Keyeq = std::equal_to<_Kty>, class _Alloc = std::allocator<std::pair<const _Kty, _Ty> >>
	class CDictionary : public std::unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>
	{
	public:
		CDictionary();
		CDictionary(const CDictionary& src);
		~CDictionary();

		#ifdef _WIN32
		__declspec(property(get = GetCount)) size_t Count;
		#endif

		bool Add(const _Kty& key, const _Ty& Data);
		void Clear();
		bool TryGetValue(const _Kty key, _Ty& Data);
		bool ContainsKey(const _Kty& key);
		bool Remove(const _Kty& key);
		size_t GetCount() const;
		bool IsEmpty() const;
	};
}

#include "Dictionary.inl"