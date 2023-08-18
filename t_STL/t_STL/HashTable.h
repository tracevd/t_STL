#pragma once

#include <assert.h>
#include "Type.h"
#include "Vector.h"
#include "LinkedList.h"
#include "Memory.h"
#include "Array.h"

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

	template< class HashTable >
	class HashTableIterator
	{
	private:
		using BucketList = HashTable::BucketList;
		using Node = HashTable::MultiValueLinkedList;
		using ValueType = HashTable::ValueType;
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
			return  *currentNode->pairs[ currentNodeIndex ];
		}

		constexpr ValueType* operator->()
		{
			return  currentNode->pairs[ currentNodeIndex ].get();
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
		constexpr void next( uint8 isInMiddleOfList = 1 )
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
		uint8 currentNodeIndex = 0;
	};

	template< class HashTable >
	class HashTableConstIterator
	{
	private:
		using BucketList = const HashTable::BucketList;
		using Node = const HashTable::MultiValueLinkedList;
		using ValueType = const HashTable::ValueType;
	public:
		constexpr HashTableConstIterator( BucketList* buckets ):
			buckets( buckets )
		{
			if ( buckets == nullptr || buckets->data() == nullptr )
				return;

			currentNode = buckets->data();

			next( 0 );
		}

		constexpr HashTableConstIterator( BucketList* buckets, uint64 bucketIndex, Node* node, uint8 nodeIndex ):
			buckets( buckets ),
			currentBucket( bucketIndex ),
			currentNode( node ),
			currentNodeIndex( nodeIndex ) {}

		constexpr HashTableConstIterator( HashTableConstIterator const& other ):
			buckets( other.buckets ),
			currentBucket( other.currentBucket ),
			currentNode( other.currentNode ),
			currentNodeIndex( other.currentNodeIndex ) {}

		constexpr HashTableConstIterator( HashTableConstIterator&& other ):
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

		constexpr HashTableConstIterator& operator=( HashTableConstIterator&& rhs )
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

		constexpr HashTableConstIterator& operator=( HashTableConstIterator const& rhs )
		{
			*this = HashTableIterator( rhs );
		}

		constexpr HashTableConstIterator operator++( int )
		{
			auto cpy = *this;
			next();
			return cpy;
		}

		constexpr HashTableConstIterator& operator++()
		{
			if ( buckets == nullptr || currentBucket == buckets->size() || currentNode == nullptr )
				throw std::runtime_error( "Invalid iterator!" );
			if ( currentBucket == buckets->size() )
				throw std::runtime_error( "Reached the end of the bucket list!" );

			next();

			return *this;
		}

		constexpr ValueType& operator*()
		{
			return  *currentNode->pairs[currentNodeIndex];
		}

		constexpr ValueType* operator->()
		{
			return  currentNode->pairs[currentNodeIndex].get();
		}

		constexpr bool operator==( HashTableConstIterator const& rhs ) const
		{
			return buckets == rhs.buckets
				&& currentBucket == rhs.currentBucket
				&& currentNode == rhs.currentNode
				&& currentNodeIndex == rhs.currentNodeIndex;
		}
		constexpr bool operator!=( HashTableConstIterator const& rhs ) const { return !(*this == rhs); }
	private:
		/**
		 * Advance to the next valid pair
		 * Set isInMiddleOfList to 1 (true) by default, meaning it will not include
		 * the current Node index when searching for the next valid index in a node
		 */
		constexpr void next( uint8 isInMiddleOfList = 1 )
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
		uint8 currentNodeIndex = 0;
	};

	template< typename KeyTy, typename ValTy >
	struct HashTable
	{
	public:
		using ValueType		= hashmap::Pair< type::add_const< KeyTy >, ValTy >;
		using Pair			= ValueType;
		using Iterator		= HashTableIterator< HashTable< KeyTy, ValTy > >;
		using ConstIterator = HashTableConstIterator< HashTable< KeyTy, ValTy > >;
	public:
		constexpr HashTable():
			m_size( 0 ),
			m_data( 10 ) {}

		constexpr explicit HashTable( uint64 numberOfBuckets ):
			m_size( 0 ),
			m_data( numberOfBuckets ) {}

		constexpr HashTable( HashTable&& other ):
			m_data( std::move( other.m_data ) ),
			m_size( other.m_size )
		{
			other.m_size = 0;
		}

		constexpr HashTable& operator=( HashTable&& rhs )
		{
			if ( this == &rhs )
				return *this;
			m_data = std::move( rhs.m_data );
			m_size = rhs.m_size;
			rhs.m_size = 0;
			return *this;
		}

		constexpr ~HashTable()
		{
			for ( auto& bucket : m_data )
			{
				auto next = bucket.next;
				while ( next != nullptr )
				{
					auto copy = next;

					next = next->next;

					copy->next = nullptr;

					delete copy;
				}
			}
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
			return ConstIterator( &m_data,
								  m_data.size(),
								  m_data.data() + m_data.size(),
								  uint8(8) );
		}

		constexpr bool contains( const KeyTy& key ) const
		{
			uint64 hash_ = hash( key );
			return m_data[ hash_ ].find( key ) != nullptr;
		}

		constexpr bool contains( KeyTy&& key ) const
		{
			return contains( key );
		}

		constexpr ValTy& at( const KeyTy& key )
		{
			uint64 hash_ = hash( key );
			return at( hash_, key );
		}

		constexpr const ValTy& at( const KeyTy& key ) const
		{
			return at( key );
		}

		constexpr ValTy& operator[] ( const KeyTy& key )
		{
			uint64 hash_ = hash( key );
			if ( !containsKey( hash_, key ) )
				return put( hash_, { key, ValTy() } )->val;
			return at( hash_, key );
		}

		constexpr ValTy& operator[] ( KeyTy&& key )
		{
			uint64 hash_ = hash( key );
			auto pair_ptr = m_data[ hash_ ].find( key );
			if ( pair_ptr == nullptr )
			{
				return put( hash_, { std::move( key ), ValTy() } ).val;
			}
			return pair_ptr->val;
		}

		constexpr void put( const KeyTy& key, const ValTy& val )
		{
			uint64 i = hash( key );
			put( i, Pair{ key, val } );
		}

		constexpr void put( Pair&& pair )
		{
			uint64 i = hash( pair.key );
			put( i, std::move( pair ) );
		}

		constexpr void remove( const KeyTy& key )
		{
			uint64 hash_ = hash( key );
			if ( m_data[ hash_ ].remove( key ) )
				m_size -= 1;
		}

		constexpr Pair* insert( Pair&& pair )
		{
			uint64 hash_ = hash( pair.key );
			if ( !m_data[ hash_ ].find( pair.key ) )
			{
				m_size += 1;
				return &m_data[ hash_ ].add( std::move( pair ) );
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

			constexpr MultiValueLinkedList( HashTable::ValueType&& pair ):
				valid_indexes( 1 )
			{
				pairs[ 0 ] = UniquePtr< ValueType >( std::move( pair ) );
			}

			constexpr ~MultiValueLinkedList() = default;

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
						if ( pairs[ i ].key == key )
							return { &pairs[ i ], this, i, } ;
					}
				}

				if ( next == nullptr )
					return { nullptr, nullptr, 0 };

				return next->find_w_index( key );
			}

			constexpr Pair* find( KeyTy const& key )
			{
				if ( valid_indexes != 0 )
				{
					for ( uint8 i = 0; i < pairs.size(); ++i )
					{
						if ( ( ( valid_indexes >> i ) & 1 ) == 0 )
							continue;
						if ( pairs[ i ]->key == key )
							return pairs[ i ].get();
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
							return true;
						}
					}
				}
				if ( next == nullptr )
					return false;

				return next->remove( key );
			}
			constexpr Pair& add( Pair&& pair )
			{
				auto i = getNextEmptyIndex( 0 );

				if ( i == pairs.size() )
				{
					if ( next == nullptr )
					{
						next = new MultiValueLinkedList( std::move( pair ) );
						return *(*next).pairs[ 0 ];
					}

					return next->add( std::move( pair ) );
				}

				valid_indexes ^= 1 << i;
				pairs[ i ] = UniquePtr< ValueType >( std::move( pair ) );
				return *pairs[ i ];
			}
			/*constexpr Pair* tryInsert( Pair&& pair )
			{
				if ( valid_indexes != 0 )
				{
					for ( uint8 i = 0; i < pairs.size(); ++i )
					{
						if ( ((valid_indexes >> i) & 1) == 0 )
							continue;
						if ( pairs[ i ]->key == key )
						{
							return nullptr;
							pairs[ i ] = Pair();
							valid_indexes ^= 1 << i;
							return true;
						}
					}
				}
				if ( next == nullptr )
					return false;

				return next->tryInsert( key );
			}*/
			constexpr uint8 getNextEmptyIndex( uint8 currentIndex ) const
			{
				if ( valid_indexes == limit< uint8 >::max )
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
			t::Array< UniquePtr< HashTable::ValueType >, 8 > pairs;
			MultiValueLinkedList* next = nullptr;
			friend HashTable< KeyTy, ValTy >;
			friend Iterator;
			friend ConstIterator;
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

		constexpr Pair& insert_unchecked( uint64 hash, Pair&& pair )
		{
			m_size += 1;
			return m_data[ hash ].add( std::move( pair ) );
		}

		constexpr inline Pair& put( uint64 hash, Pair&& pair )
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

		constexpr inline ValTy& at( uint64 hash, const KeyTy& key )
		{
			Pair* pair = m_data[ hash ].find( key );
			if ( pair == nullptr )
				throw std::runtime_error( "Cannot find key" );
			return pair->val;
		}

	private:
		uint64 m_size;
		BucketList m_data;
		friend Iterator;
		friend ConstIterator;
	};
}