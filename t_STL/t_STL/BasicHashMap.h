#include "Tint.h"
#include "Pair.h"
#include "Type.h"

#include "HeapArray.h"
#include "LinkedList.h"

namespace t
{
	namespace details
	{
		template< class T, class U >
		class __BasicHashPairBase
		{
		public:
			using PairType = t::pair< T, U >;
			using ValueType = t::pair< type::add_const< T >, U >;
		public:
			constexpr __BasicHashPairBase( T key, U val ):
				m_pair( move( key ), move( val ) ) {}

			constexpr __BasicHashPairBase( PairType pair ):
				m_pair( move( pair.first ), move( pair.second ) ) {}

			constexpr auto const& pair() const { return m_pair; }
			constexpr auto const& value() const { return m_pair.second; }
			constexpr auto const& key() const { return m_pair.first; }

			constexpr auto& pair() { return m_pair; }
			constexpr auto& value() { return m_pair.second; }

			constexpr bool operator==( __BasicHashPairBase const& rhs )
			{
				return m_pair.first == rhs.m_pair.first;
			}

			constexpr bool operator==( T const& key )
			{
				return m_pair.first == key;
			}
		private:
			ValueType m_pair;
		};

		template< class T, class U, bool CacheHash = type::is_expensive_hash< T > >
		class __BasicHashPair;

		template< class T, class U >
		class __BasicHashPair< T, U, false > : public __BasicHashPairBase< T, U >
		{
		public:
			using BaseType = __BasicHashPairBase< T, U >;
			using PairType = BaseType::PairType;
			using ValueType = BaseType::ValueType;
		public:
			constexpr __BasicHashPair( T key, U val ):
				BaseType( std::move( key ), std::move( val ) ) {}

			constexpr __BasicHashPair( PairType pair ):
				BaseType( std::move( pair.first ), std::move( pair.second ) ) {}

			constexpr __BasicHashPair( PairType pair, uint64 ):
				BaseType( move( pair ) ) {}

			constexpr uint64 hash() const { return t::hasher< decltype( this->pair().first ) >::hash(this->pair().first); }
		};

		template< class T, class U >
		class __BasicHashPair< T, U, true > : public __BasicHashPairBase< T, U >
		{
		public:
			using BaseType = __BasicHashPairBase< T, U >;
			using PairType = BaseType::PairType;
			using ValueType = BaseType::ValueType;
		public:
			constexpr __BasicHashPair( T key, U val ):
				BaseType( std::move( key ), std::move( val ) ),
				m_hash( t::hasher< T >::hash( this->key() ) ) {}

			constexpr __BasicHashPair( PairType pair ):
				BaseType( std::move( pair ) ),
				m_hash( t::hasher< T >::hash( this->key() ) ) {}

			constexpr __BasicHashPair( PairType pair, uint64 hash ):
				BaseType( move( pair ) ),
				m_hash( hash ) {}

			constexpr uint64 hash() const { return m_hash; }
		private:
			uint64 m_hash;
		};
	}

	template< class KeyTy, class ValTy >
	class BasicHashMap
	{
	public:
		using KeyType = KeyTy;
		using MappedType = ValTy;
		using ValueType = pair< type::add_const< KeyType >, MappedType >;
		
	private:
		using ValueWrapperType = details::__BasicHashPair< KeyTy, ValTy >;
		using BucketType = LinkedList< ValueWrapperType >;
		using PairType = pair< KeyType, MappedType >;

	public:
		constexpr BasicHashMap():
			m_buckets( 8 ) {}

		constexpr BasicHashMap( uint64 expectedNumel ):
			m_buckets( expectedNumel * 4 / 3 ) {}

		constexpr ~BasicHashMap() = default;

		constexpr bool insert( PairType&& pair )
		{
			ValueWrapperType wrapper( move( pair ) );

			return insertWrapper( move( wrapper ) );
		}

		constexpr bool insert( PairType const& pair )
		{
			auto cpy = pair;

			ValueWrapperType wrapper( move( cpy ) );

			return insertWrapper( move( wrapper ) );
		}

		constexpr MappedType& at( KeyType const& key )
		{
			auto index = t::hasher< KeyType >::hash( key ) % m_buckets.size();

			LinkedList< ValueWrapperType >& bucket = m_buckets[ index ];

			auto it = bucket.find( key );

			if ( it == bucket.end() )
				throw std::runtime_error("Could not find key!");

			return it->value();
		}

		constexpr MappedType& operator[]( KeyType const& key )
		{
			auto ptr = find( key );

			if ( ptr != nullptr )
			{
				return *ptr;
			}
			
			ValueWrapperType wrapper( key, {} );
			insertWrapper( move( wrapper ), true );
			return *find( key );
		}

		constexpr MappedType* find( KeyType const& key )
		{
			auto index = t::hasher< KeyType >::hash( key ) % m_buckets.size();

			LinkedList< ValueWrapperType >& bucket = m_buckets[ index ];

			auto it = bucket.find( key );

			if ( it == bucket.end() )
				return nullptr;
			return &it->value();
		}

		constexpr bool contains( KeyType const& key )
		{
			auto index = t::hasher< KeyType >::hash( key ) % m_buckets.size();

			auto const& bucket = m_buckets[ index ];

			for ( auto const& wrapper : bucket )
			{
				if ( wrapper.key() == key )
					return true;
			}

			return false;
		}

		constexpr bool remove( KeyType const& key )
		{
			auto index = t::hasher< KeyType >::hash( key ) % m_buckets.size();

			auto& bucket = m_buckets[ index ];

			auto const removed = bucket.remove( key );

			m_size -= removed;

			return removed;
		}

		constexpr uint64 size() const { return m_size; }

	private:
		constexpr bool insertWrapper( ValueWrapperType&& wrapper, bool unchecked = false )
		{
			if ( atLoadFactor() )
			{
				rehash();
			}

			auto index = wrapper.hash() % m_buckets.size();

			auto& bucket = m_buckets[ index ];

			if ( !unchecked )
			{
				for ( auto const& w : bucket )
				{
					if ( w.key() == wrapper.key() )
						return false;
				}
			}

			bucket.pushFront( move( wrapper ) );
			++m_size;
			return true;
		}

		bool atLoadFactor() const
		{
			return m_size * 3 / 4 >= m_buckets.size();
		}

	public:
		void rehash()
		{
			auto const newsize = [ & ]()
			{
				if ( m_buckets.size()< 512 )
					return m_buckets.size() * 2;
				return m_buckets.size() * 8;
			}();
			HeapArray< BucketType > newBuckets( newsize );

			for ( auto& bucket : m_buckets )
			{
				for ( auto& wrapper : bucket )
				{
					auto index = wrapper.hash() % newsize;

					newBuckets[ index ].pushFront( std::move( wrapper ) );
				}
			}

			m_buckets = move( newBuckets );
		}

	private:
		HeapArray< BucketType > m_buckets;
		uint64 m_size = 0;
	};

	//namespace details
	//{
	//	template< class T, class U >
	//	struct WrapperWrapper
	//	{
	//		__BasicHashPair< T, U > m_wrapper = {};
	//		bool isOccupied = false;
	//	};
	//}
	//

	//template< class KeyTy, class ValTy >
	//class BasicVectorHashMap
	//{
	//public:
	//	using KeyType = KeyTy;
	//	using MappedType = ValTy;
	//	using ValueType = pair< type::add_const< KeyType >, MappedType >;

	//private:
	//	using ValueWrapperType = details::__BasicHashPair< KeyTy, ValTy >;
	//	using WrapperWrapperType = details::WrapperWrapper< KeyTy, ValTy >;
	//	using PairType = pair< KeyType, MappedType >;
	//public:
	//	BasicVectorHashMap():
	//		m_data( 32 ) {}

	//	bool insert( PairType&& pair )
	//	{
	//		ValueWrapperType wrapper( move( pair ) );

	//		uint64 hash = wrapper.hash();

	//		auto index = hash % ( m_data.size() / m_offset ) * m_offset;

	//		WrapperWrapperType* bucket = &m_data[ index ];

	//		while ( bucket->isOccupied && bucket->m_wrapper.hash() % (m_data.size() / m_offset) * m_offset <= index )
	//		{
	//			++bucket;
	//		}

	//		if ( bucket->isOccupied || bucket == &m_data[ 0 ] + m_data.size() )
	//			throw std::runtime_error("afoasn");

	//		//bucket->m_wrapper.pair() = move( wrapper );
	//	}
	//private:
	//	HeapArray< WrapperWrapperType > m_data;
	//	uint64 m_offset = 4;
	//	uint64 m_size = 0;
	//};
}

