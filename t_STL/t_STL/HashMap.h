#pragma once

#include "Vector.h"
#include "LinkedList.h"
#include <algorithm>

namespace t
{
	namespace hashmap
	{
		template< typename KeyTy, typename ValTy >
		struct Pair
		{
			KeyTy key;
			ValTy val;
			inline bool operator==( const Pair& rhs ) const { return key == rhs.key; }
			inline bool operator!=( const Pair& rhs ) const { return !( *this == rhs ); }
		};
	}
	template< typename KeyTy, typename ValTy >
	struct HashMap
	{
	public:
		HashMap() : m_capacity( 20 ) { m_data.resize(20); }

		bool isEmpty() const { return size() == 0; }

		size_t size() const { return m_keys.size(); }

		bool containsKey( const KeyTy& key ) const
		{
			auto i = hash( key );
			return m_data[ i ].find( Pair{ key, ValTy() } ) != nullptr;
		}

		bool containsKey( KeyTy&& key ) const
		{
			return containsKey( key );
		}

		ValTy& at( const KeyTy& key )
		{
			size_t i = hash( key );
			return atImpl( i, key );
		}

		const ValTy& at( const KeyTy& key ) const
		{
			return at( key );
		}

		ValTy& operator[] ( const KeyTy& key )
		{
			auto hash_ = hash( key );
			if ( !containsKey( hash_, key ) )
				putImpl( hash_, { key, ValTy() } );
			return at( key );
		}

		ValTy& operator[] ( KeyTy&& key )
		{
			auto hash_ = hash( key );
			if ( !containsKey( hash_, key ) )
			{
				return putImpl( hash_, { std::move( key ), ValTy() } ).val;
			}
			return atImpl( hash_, key );
		}

		void put( const KeyTy& key, const ValTy& val )
		{
			size_t i = hash( key );
			putImpl( i, Pair{ key, val } );
		}

		void put( KeyTy&& key, ValTy&& val )
		{
			size_t i = hash( key );
			auto pair = Pair{ KeyTy(), ValTy() };
			pair.key = std::move( key );
			pair.val = std::move( val );
			putImpl( i, std::move( pair ) );
		}

		void remove( const KeyTy& key )
		{
			size_t i = hash( key );
			removeImpl( i, key );
		}

		const Vector< KeyTy* >& keys() const
		{
			return m_keys;
		}

		void printBucketSizes() const
		{
			for ( size_t i = 0; i < m_data.size(); i++ )
			{
				std::cout << std::to_string( i ) << ": " << std::to_string( m_data[i].size() ) << '\n';
			}
		}
		using Pair = hashmap::Pair< KeyTy, ValTy >;
	private:
		using Bucket = LinkedList< Pair >;
		static inline size_t hash_s( const KeyTy& key )
		{
			return std::hash< KeyTy >{}( key );
		}
		inline size_t hash( const KeyTy& key ) const
		{
			return hash_s( key ) % m_capacity;
		}
		inline size_t hash( const KeyTy& key, size_t capacity ) const
		{
			return std::hash< KeyTy >{}( key ) % capacity;
		}
		bool containsKey( size_t hash, const KeyTy& key  ) const
		{
			return m_data[ hash ].find( Pair{ key, ValTy() } ) != nullptr;
		}
		inline Pair* overwrite( size_t hash, Pair&& pair )
		{
			Pair* p = m_data[ hash ].find( pair );
			if ( p == nullptr )
				throw std::runtime_error("Unknown error occurred");
			*p = std::move( pair );
			return p;
		}
		inline Pair& putImpl( size_t& hash, Pair&& pair )
		{
			if ( containsKey( hash, pair.key ) )
			{
				Pair* pair_ = overwrite( hash, std::move( pair ) );
				return *pair_;
			}
			else
			{
				Pair& pair_ = m_data[ hash ].pushFront( std::move( pair ) );
				m_keys.pushBack( &( pair_.key ) );
				return pair_;
			}
		}
		inline ValTy& atImpl( size_t hash, const KeyTy& key )
		{
			Pair* pair = m_data[ hash ].find( Pair{ key, ValTy() } );
			if ( pair == nullptr )
				throw std::runtime_error( "Cannot find key" );
			return pair->val;
		}
		inline void removeImpl( size_t hash, const KeyTy& key )
		{
			if ( !containsKey( hash, key ) )
				return;
			auto pair = m_data[ hash ].find( Pair{ key, ValTy() } );
			auto keyptr = &( pair->key );
			m_keys.removeFirst( keyptr );
			m_data[ hash ].remove( Pair{ key, ValTy() } );	
		}
		static inline uint64_t keyPtr_toHandle( void* ptr )
		{
			return reinterpret_cast< uint64_t >( ptr );
		}
		static inline KeyTy& handle_toKey( uint64_t handle )
		{
			return *( reinterpret_cast< KeyTy* >( handle ) );
		}
		size_t m_capacity;
		Vector< Bucket > m_data;
		Vector< KeyTy* > m_keys;
	};

	template< typename VectorHashMap >
	class VectorHashMapIterator
	{
	public:
		using ValueType = typename VectorHashMap::Pair;
		using SubValueIter = VectorIterator< Vector< Vector< ValueType > > >;
		using PointerType = ValueType*;
		using ReferenceType = ValueType&;
	private:
		SubValueIter m_currentVec;
		PointerType m_ptr;
	public:
		VectorHashMapIterator( SubValueIter vec, PointerType ptr ):
			m_currentVec( vec ),
			m_ptr( ptr ) {}
		VectorHashMapIterator& operator++()
		{
			if ( m_currentVec->end() != m_ptr + 1 )
			{
				m_ptr++;
				return *this;
			}
				
			++m_currentVec;
			m_ptr = &( *(*m_currentVec).begin() );
				
			return *this;
		}
		VectorHashMapIterator operator++( int )
		{
			VectorHashMapIterator it = *this;
			++( *this );
			return it;
		}
		VectorHashMapIterator& operator--()
		{
			m_ptr--;
			return *this;
		}
		VectorHashMapIterator operator--( int )
		{
			VectorHashMapIterator it = *this;
			--(*this);
			return it;
		}
		ReferenceType operator[]( int i )
		{
			return *( m_ptr[ i ] );
		}
		PointerType operator->()
		{
			return m_ptr;
		}
		ReferenceType operator*()
		{
			return *m_ptr;
		}
		bool operator==( const VectorHashMapIterator& other ) const
		{
			return m_ptr == other.m_ptr && m_currentVec == other.m_currentVec;
		}
		bool operator!=( const VectorHashMapIterator& other ) const
		{
			return !( *this == other );
		}
		bool operator==( const void* other ) const
		{
			return m_ptr == other;
		}
		bool operator!=( const void* other ) const
		{
			return !(*this == other);
		}
	};

	template< typename KeyTy, typename ValTy >
	struct VectorHashMap
	{
	public:
		using Iterator = VectorHashMapIterator< VectorHashMap< KeyTy, ValTy > >;
		VectorHashMap() : m_capacity( 20 ) { m_data.resize(20); }

		bool isEmpty() const { return size() == 0; }

		size_t size() const
		{
			size_t size = 0;
			for ( const auto& buck : m_data )
				size += buck.size();
			return size;
		}

		bool containsKey( const KeyTy& key ) const
		{
			auto i = hash( key );
			return m_data[ i ].find( Pair{ key, ValTy() } ) != nullptr;
		}

		bool containsKey( KeyTy&& key ) const
		{
			return containsKey( key );
		}

		ValTy& at( const KeyTy& key )
		{
			size_t i = hash( key );
			return atImpl( i, key );
		}

		const ValTy& at( const KeyTy& key ) const
		{
			return at( key );
		}

		ValTy& operator[] ( const KeyTy& key )
		{
			Hash hash_ = hash( key );
			if ( !containsKey( hash_, key ) )
				putImpl( hash_, { key, ValTy() } );
			return at( key );
		}

		ValTy& operator[] ( KeyTy&& key )
		{
			Hash hash_ = hash( key );
			if ( !containsKey( hash_, key ) )
			{
				return putImpl( hash_, { std::move( key ), ValTy() } ).val;
			}
			return atImpl( hash_, key );
		}

		void put( const KeyTy& key, const ValTy& val )
		{
			Hash i = hash( key );
			putImpl( i, Pair{ key, val } );
		}

		void put( KeyTy&& key, ValTy&& val )
		{
			Hash i = hash( key );
			auto pair = Pair{ KeyTy(), ValTy() };
			pair.key = std::move( key );
			pair.val = std::move( val );
			putImpl( i, std::move( pair ) );
		}

		void remove( const KeyTy& key )
		{
			Hash i = hash( key );
			removeImpl( i, key );
		}

		void printBucketSizes() const
		{
			for ( size_t i = 0; i < m_data.size(); i++ )
			{
				std::cout << std::to_string( i ) << ": " << std::to_string( m_data[ i ].size() ) << '\n';
			}
		}
		Iterator begin()
		{
			auto x = (*(m_data.begin())).begin();
			return Iterator( m_data.begin(), &*x );
		}
		Iterator end()
		{
			auto x = ( *(m_data.end()) ).end();
			return Iterator( m_data.end(), static_cast< Pair* >( &*x ) );
		}
		using Pair = hashmap::Pair< KeyTy, ValTy >;
	private:
		using Hash = uint64_t;
		using Bucket = Vector< Pair >;
		static inline Hash s_hash( const KeyTy& key )
		{
			return std::hash< KeyTy >{}( key );
		}
		inline Hash hash( const KeyTy& key ) const
		{
			return s_hash( key ) % m_capacity;
		}
		static inline Hash hash( const KeyTy& key, size_t capacity )
		{
			return s_hash( key ) % capacity;
		}
		bool containsKey( Hash hash, const KeyTy& key  ) const
		{
			return m_data[ hash ].find( Pair{ key, ValTy() } ) != nullptr;
		}
		inline Pair* overwrite( Hash hash, Pair&& pair )
		{
			Pair* p = m_data[ hash ].find( pair );
			if ( p == nullptr )
				throw std::runtime_error("Unknown error occurred");
			*p = std::move( pair );
			return p;
		}
		inline Pair& putImpl( Hash& hash, Pair&& pair )
		{
			if ( containsKey( hash, pair.key ) )
			{
				Pair* pair_ = overwrite( hash, std::move( pair ) );
				return *pair_;
			}
			else
			{
				Pair& pair_ = m_data[ hash ].pushBack( std::move( pair ) );
				return pair_;
			}
		}
		inline ValTy& atImpl( Hash hash, const KeyTy& key )
		{
			Pair* pair = m_data[ hash ].find( Pair{ key, ValTy() } );
			if ( pair == nullptr )
				throw std::runtime_error( "Cannot find key" );
			return pair->val;
		}
		inline void removeImpl( Hash hash, const KeyTy& key )
		{
			if ( !containsKey( hash, key ) )
				return;
			auto pair = m_data[ hash ].find( Pair{ key, ValTy() } );
			m_data[ hash ].remove( Pair{ key, ValTy() } );
		}
		static inline uint64_t keyPtr_toHandle( void* ptr )
		{
			return reinterpret_cast< uint64_t >( ptr );
		}
		static inline KeyTy& handle_toKey( uint64_t handle )
		{
			return *( reinterpret_cast< KeyTy* >( handle ) );
		}
		size_t m_capacity;
		Vector< Bucket > m_data;
	};
}