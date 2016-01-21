namespace Zero
{
	template<class _Kty, class _Ty, class _Hasher, class _Keyeq, class _Alloc>
	CDictionary<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>::CDictionary()
	{
	}

	template<class _Kty, class _Ty, class _Hasher, class _Keyeq, class _Alloc>
	CDictionary<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>::CDictionary(const CDictionary& src)
	{
		for( auto iter : src )
		{
			Add( iter.first, iter.second );
		}
	}

	template<class _Kty, class _Ty, class _Hasher, class _Keyeq, class _Alloc>
	CDictionary<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>::~CDictionary()
	{
        try
		{
            clear();
		}
        catch(...)
		{
            assert(false);
        }
	}

	template<class _Kty, class _Ty, class _Hasher, class _Keyeq, class _Alloc>
	inline size_t CDictionary<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>::GetCount() const
	{
		return size();
	}

	template<class _Kty, class _Ty, class _Hasher, class _Keyeq, class _Alloc>
	inline bool CDictionary<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>::IsEmpty() const
	{
		return (size() == 0);
	}

	template<class _Kty, class _Ty, class _Hasher, class _Keyeq, class _Alloc>
	inline bool CDictionary<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>::TryGetValue(const _Kty key, _Ty& Data)
	{
		CDictionary<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>::iterator iter = find( key );
		if( iter == end() )
			return false;
		Data = iter->second;
		return true;
	}

	template<class _Kty, class _Ty, class _Hasher, class _Keyeq, class _Alloc>
	inline bool CDictionary<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>::Remove(const _Kty& key)
	{
		CDictionary<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>::iterator iter = find( key );
		if( iter == end() )
			return false;
		(*this).erase( iter );
		return true;
	}

	template<class _Kty, class _Ty, class _Hasher, class _Keyeq, class _Alloc>
	inline void CDictionary<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>::Clear()
	{
		clear();
	}

	template<class _Kty, class _Ty, class _Hasher, class _Keyeq, class _Alloc>
	inline bool CDictionary<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>::ContainsKey(const _Kty& key)
	{
		CDictionary<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>::iterator iter = find( key );
		if( iter == end() )
			return false;
		return true;
	}

	template<class _Kty, class _Ty, class _Hasher, class _Keyeq, class _Alloc>
	inline bool CDictionary<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>::Add(const _Kty& key, const _Ty& Data)
	{
		CDictionary<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>::iterator iter = find( key );
		if( iter == end() )
		{
			(*this)[key] = Data;
			return true;
		}
		return false;
	}
}


