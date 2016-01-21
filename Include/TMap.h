#pragma once
#include <map>

// 래핑 클래스
namespace Zero
{
	template< class KeyType, class ClassType >
	class TMap : public std::map<KeyType, ClassType>
	{
	public:
		inline bool Add( KeyType key, ClassType Data )
		{
			TMap<KeyType,ClassType>::iterator iter = find( key );
			if( iter == end() )
			{
				(*this)[key] = Data;
				return true;
			}
			return false;
		}

		inline ClassType Find( KeyType key )
		{
			TMap<KeyType,ClassType>::iterator iter = find( key );
			if( iter == end() )
				return NULL;
			return iter->second;
		}		

		inline bool Remove( KeyType key )
		{
			TMap<KeyType,ClassType>::iterator iter = find( key );
			if( iter == end() )
				return false;
			(*this).erase( iter );
			return true;
		}

		inline intptr_t GetCount()	{ return size(); }
	
		inline void Clear()
		{
			clear();
		}

		TMap& operator=( const TMap& src)
		{
			Clear();

			for( auto iter : src )
			{
				Add( iter.first, iter.second );
			}
			return *this;
		}

		TMap()	{ Clear(); }
		~TMap()	{}
	};
}

