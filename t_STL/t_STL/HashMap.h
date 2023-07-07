#pragma once

#include "Vector.h"
#include "LinkedList.h"
#include "Memory.h"

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
		using Pair = hashmap::Pair< KeyTy, ValTy >;
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

		/**
		 * @brief Finds a value at a key. Returns null if not found
		 */
		Pair* find( KeyTy const& key )
		{
			auto hash = hash( key );
			if ( !containsKey( hash, key ) )
			{
				return nullptr;
			}
			return &atImpl( hash, key );
		}

		ValTy& at( const KeyTy& key )
		{
			size_t i = hash( key );
			return atImpl( i, key ).val;
		}

		const ValTy& at( const KeyTy& key ) const
		{
			return at( key );
		}

		ValTy& operator[] ( const KeyTy& key )
		{
			auto hash_ = hash( key );
			if ( !containsKey( hash_, key ) )
				return putImpl( hash_, { key, ValTy() } ).val;
			return at( key );
		}

		ValTy& operator[] ( KeyTy&& key )
		{
			auto hash_ = hash( key );
			if ( !containsKey( hash_, key ) )
			{
				return putImpl( hash_, { std::move( key ), ValTy() } ).val;
			}
			return atImpl( hash_, key ).val;
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
		inline Pair& atImpl( size_t hash, const KeyTy& key )
		{
			Pair* pair = m_data[ hash ].find( Pair{ key, ValTy() } );
			if ( pair == nullptr )
				throw std::runtime_error( "Cannot find key" );
			return pair;
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

	#include "Array.h"

	template< typename KeyTy, typename ValTy >
	struct HashTable
	{
		using Pair = hashmap::Pair< KeyTy, ValTy >;
	public:
		HashTable() { m_data.resize( 20 ); }

		bool isEmpty() const { return size() == 0; }

		size_t size() const
		{
			return m_size;
		}

		bool containsKey( const KeyTy& key ) const
		{
			uint64_t i = hash( key );
			return m_data[ i ].find( key ) != nullptr;
		}

		bool containsKey( KeyTy&& key ) const
		{
			return containsKey( key );
		}

		ValTy& at( const KeyTy& key )
		{
			uint64_t i = hash( key );
			return atImpl( i, key );
		}

		const ValTy& at( const KeyTy& key ) const
		{
			return at( key );
		}

		ValTy& operator[] ( const KeyTy& key )
		{
			uint64_t hash_ = hash( key );
			if ( !containsKey( hash_, key ) )
				putImpl( hash_, { key, ValTy() } );
			return at( key );
		}

		ValTy& operator[] ( KeyTy&& key )
		{
			uint64_t hash_ = hash( key );
			auto pair_ptr = m_data[ hash_ ].find( key );
			if ( pair_ptr == nullptr )
			{
				return putImpl( hash_, { std::move( key ), ValTy() } ).val;
			}
			return pair_ptr->val;
		}

		void put( const KeyTy& key, const ValTy& val )
		{
			uint64_t i = hash( key );
			putImpl( i, Pair{ key, val } );
		}

		void put( Pair&& pair )
		{
			uint64_t i = hash( pair.key );
			putImpl( i, std::move( pair ) );
		}

		void remove( const KeyTy& key )
		{
			uint64_t hash_ = hash( key );
			if ( m_data[ hash_ ].remove( key ) )
				m_size -= 1;
		}

		Pair* insert( Pair&& pair )
		{
			uint64_t hash = hash( pair.key );
			if ( m_data[ hash ].find( pair.key ) )
			{
				m_size += 1;
				return &m_data[ hash ].add( std::move( pair ) );
			}
			return nullptr;
		}

		void printBucketSizes() const
		{
			for ( size_t i = 0; i < m_data.size(); ++i )
			{
				std::cout << std::to_string( i ) << ": " << std::to_string( m_data[ i ].size() ) << '\n';
			}
		}
	private:
		struct MultiValueLinkedList
		{
			MultiValueLinkedList() = default;
			MultiValueLinkedList( HashTable::Pair pair ):
				pairs( std::move( pair ) ),
				valid_indexes( 1 ) {}
			~MultiValueLinkedList()
			{
				while( next != nullptr )
				{
					*this = std::move( *next );
				}
			}

			MultiValueLinkedList& operator=( MultiValueLinkedList const& ) = delete;

			MultiValueLinkedList& operator=( MultiValueLinkedList&& rhs ) noexcept
			{
				valid_indexes = rhs.valid_indexes;
				rhs.valid_indexes = 0;
				pairs = std::move( rhs.pairs );
				next = std::move( rhs.next );
				return *this;
			}

			const HashTable::Pair* find( KeyTy const& key ) const
			{
				if ( valid_indexes != 0 )
				{
					for ( uint8_t i = 0; i < pairs.size(); ++i )
					{
						if ( valid_indexes >> i == 0 )
							continue;
						if ( pairs[ i ].key == key )
							return &pairs[ i ];
					}
				}

				if ( next == nullptr )
					return nullptr;
				
				return next->find( key );
			}

			Pair* find( KeyTy const& key )
			{
				if ( valid_indexes != 0 )
				{
					for ( uint8_t i = 0; i < pairs.size(); ++i )
					{
						if ( valid_indexes >> i == 0 )
							continue;
						if ( pairs[ i ].key == key )
							return &pairs[ i ];
					}
				}

				if ( next == nullptr )
					return nullptr;

				return next->find( key );
			}
			bool remove( KeyTy const& key )
			{
				if ( valid_indexes != 0 )
				{
					for ( uint8_t i = 0; i < pairs.size(); ++i )
					{
						if ( valid_indexes >> i == 0 )
							continue;
						if ( pairs[ i ].key == key )
						{
							pairs[ i ] = Pair();
							valid_indexes ^= 1 << i;
							if ( valid_indexes == 0 && next != nullptr )
								*this = std::move( *next );
							return true;
						}
					}
				}
				if ( next == nullptr )
					return false;

				return next->remove( key );
			}
			Pair& add( Pair&& pair )
			{
				for ( uint8_t i = 0; i < pairs.size(); ++i )
				{
					if ( valid_indexes >> i == 0 )
					{
						valid_indexes ^= 1 << i;
						pairs[ i ] = std::move( pair );
						return pairs[ i ];
					}
				}
				return next->add( std::move( pair ) );
			}
		private:
			uint8_t valid_indexes = 0;
			Array< HashTable::Pair, 8 > pairs;
			UniquePtr< MultiValueLinkedList > next = nullptr;
		};

		using Bucket = MultiValueLinkedList;

		inline uint64_t hash( const KeyTy& key ) const
		{
			return std::hash< KeyTy >{}( key ) % m_data.size();
		}

		bool containsKey( uint64_t hash, const KeyTy& key  ) const
		{
			return m_data[ hash ].find( key ) != nullptr;
		}

		inline Pair& putImpl( uint64_t hash, Pair&& pair )
		{
			auto pair_ptr = m_data[ hash ].find( pair.key );

			if ( pair_ptr == nullptr )
			{
				m_size += 1;
				return m_data[ hash ].add( std::move( pair ) );
			}

			pair_ptr->val = std::move( pair.val );
			return *pair_ptr;
		}

		inline ValTy& atImpl( uint64_t hash, const KeyTy& key )
		{
			Pair* pair = m_data[ hash ].find( key );
			if ( pair == nullptr )
				throw std::runtime_error( "Cannot find key" );
			return pair->val;
		}

		HashTable( size_t capacity )
		{

		}

		HashTable rehash()
		{

		}
	private:
		size_t m_size;
		Vector< Bucket > m_data;
	};
}