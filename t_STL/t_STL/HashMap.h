#pragma once

#include "Type.h"
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
			constexpr inline bool operator==( const Pair& rhs ) const { return key == rhs.key; }
			constexpr inline bool operator!=( const Pair& rhs ) const { return !( *this == rhs ); }
			using KeyType   = KeyTy;
			using ValueType = ValTy;
		};
	}
	template< typename KeyTy, typename ValTy >
	struct HashMap
	{
		using Pair = hashmap::Pair< KeyTy, ValTy >;
	public:
		HashMap() : m_capacity( 20 ) { m_data.resize(20); }

		bool isEmpty() const { return size() == 0; }

		uint64 size() const { return m_keys.size(); }

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
			uint64 i = hash( key );
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
			uint64 i = hash( key );
			putImpl( i, Pair{ key, val } );
		}

		void put( KeyTy&& key, ValTy&& val )
		{
			uint64 i = hash( key );
			auto pair = Pair{ KeyTy(), ValTy() };
			pair.key = std::move( key );
			pair.val = std::move( val );
			putImpl( i, std::move( pair ) );
		}

		void remove( const KeyTy& key )
		{
			uint64 i = hash( key );
			removeImpl( i, key );
		}

		const Vector< KeyTy* >& keys() const
		{
			return m_keys;
		}
	private:
		using Bucket = LinkedList< Pair >;
		static inline uint64 hash_s( const KeyTy& key )
		{
			return std::hash< KeyTy >{}( key );
		}
		inline uint64 hash( const KeyTy& key ) const
		{
			return hash_s( key ) % m_capacity;
		}
		inline uint64 hash( const KeyTy& key, uint64 capacity ) const
		{
			return std::hash< KeyTy >{}( key ) % capacity;
		}
		bool containsKey( uint64 hash, const KeyTy& key  ) const
		{
			return m_data[ hash ].find( Pair{ key, ValTy() } ) != nullptr;
		}
		inline Pair* overwrite( uint64 hash, Pair&& pair )
		{
			Pair* p = m_data[ hash ].find( pair );
			if ( p == nullptr )
				throw std::runtime_error("Unknown error occurred");
			*p = std::move( pair );
			return p;
		}
		inline Pair& putImpl( uint64& hash, Pair&& pair )
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
		inline Pair& atImpl( uint64 hash, const KeyTy& key )
		{
			Pair* pair = m_data[ hash ].find( Pair{ key, ValTy() } );
			if ( pair == nullptr )
				throw std::runtime_error( "Cannot find key" );
			return pair;
		}
		inline void removeImpl( uint64 hash, const KeyTy& key )
		{
			if ( !containsKey( hash, key ) )
				return;
			auto pair = m_data[ hash ].find( Pair{ key, ValTy() } );
			auto keyptr = &( pair->key );
			m_keys.removeFirst( keyptr );
			m_data[ hash ].remove( Pair{ key, ValTy() } );	
		}
		static inline uint64 keyPtroHandle( void* ptr )
		{
			return reinterpret_cast< uint64 >( ptr );
		}
		static inline KeyTy& handleoKey( uint64 handle )
		{
			return *( reinterpret_cast< KeyTy* >( handle ) );
		}
		uint64 m_capacity;
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

	template< class HashTable, bool isConst >
	class HashTableIterator
	{
	private:
		using BucketList = type::ternary< isConst, typename const HashTable::BucketList, typename HashTable::BucketList >;
		using Node = type::ternary< isConst, typename const HashTable::MultiValueLinkedList, typename HashTable::MultiValueLinkedList >;
		using ValueType = type::ternary< isConst, typename const HashTable::ValueType, typename HashTable::ValueType >;
	public:
		constexpr HashTableIterator( BucketList* buckets ):
			buckets( buckets )
		{
			if ( buckets == nullptr || buckets->data() == nullptr )
				return;
			
			currentNode = buckets->data();

			next( 0 );
		}

		constexpr HashTableIterator( BucketList* buckets, uint64 bucketIndex, Node* node, uint8 nodeIndex ):
			buckets( buckets ),
			currentBucket( bucketIndex ),
			currentNode( node ),
			currentNodeIndex( nodeIndex ) {}

		constexpr HashTableIterator( HashTableIterator const& other ):
			buckets( other.buckets ),
			currentBucket( other.currentBucket ),
			currentNode( other.currentNode ),
			currentNodeIndex( other.currentNodeIndex ) {}

		constexpr HashTableIterator( HashTableIterator&& other ):
			buckets( other.buckets ),
			currentBucket( other.currentBucket ),
			currentNode( other.currentNode ),
			currentNodeIndex( other.currentNodeIndex )
		{
			other.buckets = nullptr;
			other.currentBucket = 0;
			other.currentNode = nullptr;
			other.currentNodeIndex = 0;
		}

		constexpr HashTableIterator& operator=( HashTableIterator&& rhs )
		{
			buckets = rhs.buckets;
			currentBucket = rhs.currentBucket;
			currentNode = rhs.currentNode;
			currentNodeIndex = rhs.currentNodeIndex;

			rhs.buckets = nullptr;
			rhs.currentBucket = 0;
			rhs.currentNode = nullptr;
			rhs.currentNodeIndex = 0;
		}

		constexpr HashTableIterator& operator=( HashTableIterator const& rhs )
		{
			*this = HashTableIterator( rhs );
		}

		constexpr HashTableIterator operator++( int )
		{
			auto cpy = *this;
			next();
			return cpy;
		}
		
		constexpr HashTableIterator& operator++()
		{
			if ( buckets == nullptr || currentBucket == buckets->size() || currentNode == nullptr )
				throw std::runtime_error("Invalid iterator!");
			if ( currentBucket == buckets->size() )
				throw std::runtime_error("Reached the end of the bucket list!");
			
			next();
			
			return *this;
		}

		constexpr ValueType& operator*()
		{
			return  reinterpret_cast< ValueType& >( currentNode->pairs[ currentNodeIndex ] );
		}

		constexpr ValueType* operator->()
		{
			return  reinterpret_cast< ValueType* >( &currentNode->pairs[ currentNodeIndex ] );
		}

		constexpr bool operator==( HashTableIterator const& rhs ) const
		{
			return buckets == rhs.buckets
			    && currentBucket == rhs.currentBucket
				&& currentNode == rhs.currentNode
				&& currentNodeIndex == rhs.currentNodeIndex;
		}
		constexpr bool operator!=( HashTableIterator const& rhs ) const { return !( *this == rhs ); }
	private:
		/**
		 * Advance to the next valid pair
		 * Set isInMiddleOfList to 1 (true) by default, meaning it will not include
		 * the current Node index when searching for the next valid index in a node
		 */
		void next( uint8 isInMiddleOfList = 1 )
		{
			uint8 nextIndex = INVALID_INDEX;

			while ( currentBucket < buckets->size() )
			{
				nextIndex = currentNode->getNextValidIndex( currentNodeIndex + isInMiddleOfList );

				if ( nextIndex != currentNode->pairs.size() )
				{
					currentNodeIndex = nextIndex;
					return;
				}
				
				isInMiddleOfList = 0;
				currentNodeIndex = 0;
				if ( currentNode->next == nullptr )
				{
					++currentBucket;
					currentNode = buckets->data() + currentBucket;
				}
				else
				{
					currentNode = currentNode->next;
				}
			}
			if ( currentBucket == buckets->size() && nextIndex == INVALID_INDEX )
				currentNodeIndex = nextIndex;
		}
	private:
		constexpr static inline uint8 INVALID_INDEX = 8;
		BucketList* buckets = nullptr;
		uint64 currentBucket = 0;
		Node* currentNode = nullptr;
		uint8 currentNodeIndex;
	};

	template< typename KeyTy, typename ValTy >
	struct HashTable
	{
	public:
		using ValueType		= hashmap::Pair< type::add_const< KeyTy >, ValTy >;
		using Pair			= ValueType;
		using Iterator		= HashTableIterator< HashTable< KeyTy, ValTy >, false >;
		using ConstIterator = HashTableIterator< HashTable< KeyTy, ValTy >, true >;
	private:
		/**
		 * pair keys need to be mutable internally
		 * as they are preallocated in batches of 8
		 */
		using InnerPair = hashmap::Pair< KeyTy, ValTy >;
	public:
		constexpr HashTable():
			m_size(0)
		{
			m_data.resize( 20 );
		}

		constexpr explicit HashTable( uint64 size ): m_size(0)
		{
			m_data.resize(size);
		}

		constexpr HashTable( HashTable&& other ):
			m_data( std::move( other.m_data ) ),
			m_size( other.m_size )
		{
			other.m_size = 0;
		}

		constexpr HashTable& operator=( HashTable&& rhs )
		{
			m_data = std::move( rhs.m_data );
			m_size = rhs.m_size;
			rhs.m_size = 0;
			return *this;
		}

		constexpr bool isEmpty() const { return size() == 0; }

		constexpr uint64 size() const
		{
			return m_size;
		}

		constexpr Iterator begin()
		{
			return Iterator( &m_data );
		}

		constexpr Iterator end()
		{
			return Iterator( &m_data, m_data.size(), m_data.data() + m_data.size(), uint8(8) );
		}

		constexpr ConstIterator cbegin() const
		{
			return ConstIterator( &m_data );
		}

		constexpr ConstIterator cend() const
		{
			return ConstIterator( reinterpret_cast<const Vector< Bucket >*>(&m_data),
								  m_data.size(),
								  reinterpret_cast< const Bucket* >( m_data.data() + m_data.size() ),
								  uint8(8) );
		}

		constexpr bool contains( const KeyTy& key ) const
		{
			uint64 i = hash( key );
			return m_data[ i ].find( key ) != nullptr;
		}

		constexpr bool contains( KeyTy&& key ) const
		{
			return contains( key );
		}

		constexpr ValTy& at( const KeyTy& key )
		{
			uint64 i = hash( key );
			return atImpl( i, key );
		}

		constexpr const ValTy& at( const KeyTy& key ) const
		{
			return at( key );
		}

		constexpr ValTy& operator[] ( const KeyTy& key )
		{
			uint64 hash_ = hash( key );
			if ( !containsKey( hash_, key ) )
				putImpl( hash_, { key, ValTy() } );
			return at( key );
		}

		constexpr ValTy& operator[] ( KeyTy&& key )
		{
			uint64 hash_ = hash( key );
			auto pair_ptr = m_data[ hash_ ].find( key );
			if ( pair_ptr == nullptr )
			{
				return putImpl( hash_, { std::move( key ), ValTy() } ).val;
			}
			return pair_ptr->val;
		}

		constexpr void put( const KeyTy& key, const ValTy& val )
		{
			uint64 i = hash( key );
			putImpl( i, Pair{ key, val } );
		}

		constexpr void put( Pair&& pair )
		{
			uint64 i = hash( pair.key );
			putImpl( i, std::move( pair ) );
		}

		constexpr void remove( const KeyTy& key )
		{
			uint64 hash_ = hash( key );
			if ( m_data[ hash_ ].remove( key ) )
				m_size -= 1;
		}

		constexpr Pair* insert( Pair&& pair )
		{
			uint64 hash = hash( pair.key );
			if ( m_data[ hash ].find( pair.key ) )
			{
				m_size += 1;
				return &m_data[ hash ].add( std::move( pair ) );
			}
			return nullptr;
		}

		constexpr void setNumberOfBuckets( uint64 size )
		{
			if ( m_size != 0 )
				throw std::runtime_error("Cannot set NumberOfBuckets after inserting elements");
			m_data.resize( size );
		}

		constexpr Iterator find( KeyTy const& key ) const
		{
			auto hash_ = hash( key );
			auto [ ptr, node, index ] = m_data[ hash_ ].find_node( key );
			if ( ptr == nullptr )
				return end();
			return Iterator( &m_data, hash_, (MultiValueLinkedList*) node, (uint8) index );
		}
	private:
		struct MultiValueLinkedList
		{
			constexpr MultiValueLinkedList() = default;

			constexpr MultiValueLinkedList( HashTable::InnerPair pair ):
				pairs( { std::move( pair ) } ),
				valid_indexes( 1 ) {}

			constexpr ~MultiValueLinkedList()
			{
				while( next != nullptr )
				{
					auto copy = next;

					next = next->next;

					copy->next = nullptr;

					delete copy;
				}
			}

			constexpr MultiValueLinkedList& operator=( MultiValueLinkedList const& ) = delete;

			constexpr MultiValueLinkedList& operator=( MultiValueLinkedList&& rhs ) noexcept
			{
				valid_indexes = rhs.valid_indexes;
				rhs.valid_indexes = 0;
				pairs = std::move( rhs.pairs );
				next = rhs.next;
				rhs.next = nullptr;
				return *this;
			}

			constexpr const HashTable::Pair* find( KeyTy const& key ) const
			{
				if ( valid_indexes != 0 )
				{
					for ( uint8 i = 0; i < pairs.size(); ++i )
					{
						if ( ( ( valid_indexes >> i ) & 1 ) == 0 )
							continue;
						if ( pairs[ i ].key == key )
							return &pairs[ i ];
					}
				}

				if ( next == nullptr )
					return nullptr;
				
				return next->find( key );
			}

			constexpr std::tuple< Pair*, MultiValueLinkedList*, uint8 > find_node( const KeyTy& key ) const
			{
				if ( valid_indexes != 0 )
				{
					for ( uint8 i = 0; i < pairs.size(); ++i )
					{
						if ( ((valid_indexes >> i) & 1) == 0 )
							continue;
						if ( pairs[i].key == key )
							return { &pairs[i], this, i, } ;
					}
				}

				if ( next == nullptr )
					return { nullptr, nullptr, 0 };

				return next->find_w_index( key );
			}

			constexpr InnerPair* find( KeyTy const& key )
			{
				if ( valid_indexes != 0 )
				{
					for ( uint8 i = 0; i < pairs.size(); ++i )
					{
						if ( ( ( valid_indexes >> i ) & 1 ) == 0 )
							continue;
						if ( pairs[ i ].key == key )
							return &pairs[ i ];
					}
				}

				if ( next == nullptr )
					return nullptr;

				return next->find( key );
			}
			constexpr bool remove( KeyTy const& key )
			{
				if ( valid_indexes != 0 )
				{
					for ( uint8 i = 0; i < pairs.size(); ++i )
					{
						if ( ( ( valid_indexes >> i ) & 1 ) == 0 )
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
			constexpr Pair& add( InnerPair&& pair )
			{
				auto i = getNextEmptyIndex( 0 );

				if ( i == pairs.size() )
				{
					if ( next == nullptr )
					{
						next = new MultiValueLinkedList( std::move( pair ) );
						return reinterpret_cast< Pair& >( (*next).pairs[ 0 ] );
					}

					return next->add( std::move( pair ) );
				}

				valid_indexes ^= 1 << i;
				pairs[ i ] = std::move( pair );
				return  reinterpret_cast< Pair& >( pairs[ i ] );
			}
			constexpr uint8 getNextEmptyIndex( uint8 currentIndex ) const
			{
				if ( valid_indexes == uint8_MAX )
					return INVALID_INDEX;
				for ( ; currentIndex < pairs.size(); ++currentIndex )
				{
					if ( ( (valid_indexes >> currentIndex) & 1 ) == 0 )
						return currentIndex;
				}

				return INVALID_INDEX;
			}
			constexpr uint8 getNextValidIndex( uint8 currentIndex ) const
			{
				if ( valid_indexes == 0 || currentIndex >= pairs.size() )
					return INVALID_INDEX;
				for ( uint8 i = currentIndex; i < pairs.size(); ++i )
				{
					if ( ( ( valid_indexes >> i ) & 1 ) == 1 )
						return i;
				}
				return INVALID_INDEX;
			}
		private:
			static constexpr uint8 INVALID_INDEX = 8;
			uint8 valid_indexes = 0;
			t::Array< HashTable::InnerPair, 8 > pairs;
			MultiValueLinkedList* next = nullptr;
			friend HashTableIterator< HashTable< KeyTy, ValTy >, true >;
			friend HashTableIterator< HashTable< KeyTy, ValTy >, false >;
		};

		using Bucket = MultiValueLinkedList;
		using BucketList = Vector< Bucket >;

		constexpr inline uint64 hash( const KeyTy& key ) const
		{
			return std::hash< KeyTy >{}( key ) % m_data.size();
		}

		constexpr bool containsKey( uint64 hash, const KeyTy& key  ) const
		{
			return m_data[ hash ].find( key ) != nullptr;
		}

		constexpr inline Pair& putImpl( uint64 hash, InnerPair&& pair )
		{
			auto pair_ptr = m_data[ hash ].find( pair.key );

			if ( pair_ptr == nullptr )
			{
				m_size += 1;
				return m_data[ hash ].add( std::move( pair ) );
			}

			pair_ptr->val = std::move( pair.val );
			return *reinterpret_cast< Pair* >( pair_ptr );
		}

		constexpr inline ValTy& atImpl( uint64 hash, const KeyTy& key )
		{
			Pair* pair = m_data[ hash ].find( key );
			if ( pair == nullptr )
				throw std::runtime_error( "Cannot find key" );
			return pair->val;
		}

	private:
		uint64 m_size;
		BucketList m_data;
		friend HashTableIterator< HashTable< KeyTy, ValTy >, true >;
		friend HashTableIterator< HashTable< KeyTy, ValTy >, false >;
	};
}