#pragma once

#include <assert.h>
#include "Type.h"
#include "DynamicArray.h"
#include "Memory.h"
#include "Array.h"
#include "Tuple.h"


namespace t
{
    template< class T >
    struct Hash;

    namespace details
    {
        template< class T >
        struct MultiValueLinkedList;

        template< class T >
        struct HashNode
        {
        public:
            constexpr HashNode() = default;

            constexpr HashNode( T val ):
                m_validIndexes( 1 )
            {
                m_data[ 0 ] = UniquePtr< T >( std::move( val ) );
            }

            constexpr HashNode& operator=( HashNode const& rhs )
            {
                m_validIndexes = rhs.m_validIndexes;
                for ( uint8 i = 0; i < 8; ++i )
                {
                    if ( rhs.m_data[ i ] )
                        m_data[ i ] = UniquePtr< T >( *rhs.m_data[ i ] );
                }

                if ( rhs.m_next )
                {
                    m_next = new HashNode();
                    *m_next = *rhs.m_next;
                }

                return *this;
            }

            constexpr ~HashNode() = default;

            constexpr HashNode* next() const { return m_next; }

            constexpr uint8 getNextValidIndex( uint8 start = 0 ) const
            {
                if ( (m_validIndexes >> start) == 0 || start >= INVALID_INDEX )
                    return INVALID_INDEX;

                for ( uint8 i = start; i < INVALID_INDEX; ++i )
                {
                    if ( (m_validIndexes >> i) & 1 )
                        return i;
                }
                return INVALID_INDEX;
            }

            /**
             * @brief Get the first empty index including the given parameter.
             * Returns INVALID_INDEX (8) if not found.
             */
            constexpr uint8 getNextEmptyIndex( uint8 start = 0 ) const
            {
                if ( (m_validIndexes >> start) == (limit< uint8 >::max >> start) || start >= INVALID_INDEX )
                {
                    return INVALID_INDEX;
                }
                for ( uint8 i = start; i < INVALID_INDEX; ++i )
                {
                    if ( !( ( m_validIndexes >> i ) & 1 ) )
                        return i;
                }
                return INVALID_INDEX;
            }
        public:
            constexpr static uint8 INVALID_INDEX = 8;
            t::Array< t::UniquePtr< T >, 8 > m_data;
            HashNode* m_next = nullptr;
            uint8 m_validIndexes = 0;
            friend MultiValueLinkedList< T >;
        };

        template< class T >
        struct MultiValueLinkedList
        {
        private:
            using Node_t = HashNode< T >;
        public:
            constexpr MultiValueLinkedList() = default;

            MultiValueLinkedList& operator=( MultiValueLinkedList const& rhs )
            {
                m_size = rhs.m_size;
                m_data = rhs.m_data;

                return *this;
            }

            constexpr ~MultiValueLinkedList()
            {
                auto next = m_data.m_next;

                while ( next != nullptr )
                {
                    auto copy = next;

                    next = next->m_next;

                    copy->m_next = nullptr;

                    delete copy;
                }
            }

            template< class U >
            constexpr T* insert_unchecked( U&& val )
            {
                Node_t* n = &m_data;
                while ( n )
                {
                    uint8 index = n->getNextEmptyIndex( 0 );
                    if ( index < INVALID_INDEX )
                    {
                        n->m_data[ index ] = UniquePtr< T >( T( std::move( val ) ) );
                        n->m_validIndexes ^= 1 << index;
                        ++m_size;
                        return n->m_data[ index ].get();
                    }
                    if ( n->m_next == nullptr )
                    {
                        n->m_next = new Node_t( T( std::move( val ) ) );
                        ++m_size;
                        return n->m_next->m_data[ 0 ].get();
                    }

                    n = n->m_next;
                }
                assert( false );
                return nullptr;
            }

            constexpr T* insert_ptr_unchecked( UniquePtr< T >&& ptr )
            {
                Node_t* n = &m_data;
                while ( n )
                {
                    uint8 index = n->getNextEmptyIndex( 0 );
                    if ( index < INVALID_INDEX )
                    {
                        n->m_data[ index ] = std::move( ptr );
                        n->m_validIndexes ^= 1 << index;
                        ++m_size;
                        return n->m_data[ index ].get();
                    }
                    if ( n->m_next == nullptr )
                    {
                        n->m_next = new Node_t();
                        n->m_next->m_data[ 0 ] = std::move( ptr );
                        n->m_next->m_validIndexes = 1;
                        ++m_size;
                        return n->m_next->m_data[ 0 ].get();
                    }

                    n = n->m_next;
                }
                assert( false );
                return nullptr;
            }

            /**
             * @brief Finds a value. Returns nullptr if not found.
             */
            template< class U >
            constexpr const T* find( U const& val ) const
            {
                const Node_t* n = &m_data;
                uint8 index = 0;
                while ( n )
                {
                    index = n->getNextValidIndex( index );

                    if ( index < INVALID_INDEX )
                    {
                        if ( *n->m_data[index] == val )
                            return n->m_data[ index ].get();
                        ++index;
                    }
                    else
                    {
                        if ( n->m_next == nullptr )
                            return nullptr;
                        n = n->m_next;
                        index = 0;
                    }
                }
                return nullptr;
            }

            /**
             * @brief Returns the location of the given value.
             * Always returns a valid node/index
             *
             * @param val
             * @return HashNode< T > - The node
             * @return uint8 - Index in the node's data
             * @return bool - True if the value was found, false if it was not
             */
            template< class U >
            constexpr tuple< Node_t*, uint8, bool > find_or_insert( U const& val )
            {
                tuple< Node_t*, uint8, bool > tuple_ = { nullptr, INVALID_INDEX, false };

                auto n = &m_data;

                while ( n )
                {
                    for ( uint8 i = 0; i < INVALID_INDEX; ++i )
                    {
                        if ( (n->m_validIndexes >> i) & 1 )
                        {
                            if ( *n->m_data[ i ] == val )
                                return { n, i, true };
                        }
                        else if ( std::get< 1 >( tuple_ ) == INVALID_INDEX )
                        {
                            std::get< 0 >( tuple_ ) = n;
                            std::get< 1 >( tuple_ ) = i;
                        }
                    }
                    if ( n->m_next != nullptr )
                    {
                        n = n->m_next;
                        continue;
                    }
                    if ( std::get< 1 >( tuple_ ) != INVALID_INDEX )
                    {
                        std::get< 0 >( tuple_ )->m_validIndexes ^= 1 << std::get< 1 >( tuple_ );
                        ++m_size;
                        return tuple_;
                    }
                    n->m_next = new Node_t( T( val ) );
                    ++m_size;
                    return { n->m_next, 0, false };
                }
            }

            template< class U >
            constexpr tuple< Node_t*, uint8, bool > find_or_insert( U&& val )
            {
                tuple< Node_t*, uint8, bool > tuple_ = { nullptr, INVALID_INDEX, false };

                auto n = &m_data;

                while ( n )
                {
                    for ( uint8 i = 0; i < INVALID_INDEX; ++i )
                    {
                        if ( ( n->m_validIndexes >> i ) & 1 )
                        {
                            if ( *n->m_data[ i ] == val )
                                return { n, i, true };
                        }
                        else if ( std::get< 1 >( tuple_ ) == INVALID_INDEX )
                        {
                            std::get< 0 >( tuple_ ) = n;
                            std::get< 1 >( tuple_ ) = i;
                        }
                    }
                    if ( n->m_next != nullptr )
                    {
                        n = n->m_next;
                        continue;
                    }
                    if ( std::get< 1 >( tuple_ ) != INVALID_INDEX )
                    {
                        auto ptr = std::get< 0 >( tuple_ );
                        ptr->m_data[ std::get< 1 >( tuple_ ) ] = UniquePtr< T >( T( std::move( val ) ) );
                        ptr->m_validIndexes ^= 1 << std::get< 1 >( tuple_ );
                        ++m_size;
                        return tuple_;
                    }
                    n->m_next = new Node_t( T( std::move( val ) ) );
                    ++m_size;
                    return { n->m_next, 0, false };
                }

                assert( std::get< 0 >( tuple_ ) != nullptr );

                return tuple_;
            }

            template< class U >
            constexpr bool remove( U const& val )
            {
                auto n = &m_data;

                uint8 index = 0;

                while ( n )
                {
                    index = n->getNextValidIndex( index );

                    if ( index == INVALID_INDEX )
                    {
                        if ( n->m_next == nullptr )
                            return false;

                        index = 0;
                        n = n->m_next;
                        continue;
                    }

                    if ( *n->m_data[ index ] == val )
                    {
                        n->m_data[ index ] = nullptr;
                        n->m_validIndexes ^= 1 << index;
                        --m_size;
                        return true;
                    }

                    ++index;
                }
                return false;
            }

            constexpr uint64 size() const { return m_size; }

            constexpr HashNode< T >* data() { return &m_data; }
            constexpr HashNode< T > const* data() const { return &m_data; }
        private:
            static constexpr auto INVALID_INDEX = HashNode< T >::INVALID_INDEX;
            HashNode< T > m_data;
            uint64 m_size = 0;
            friend Hash< T >;
        };
    }

    template< class Hash >
    class HashConstIterator;

    template< class Hash >
    class HashIterator
    {
    #ifdef _MSC_VER
    public:
        using ValueType = typename Hash::ValueType;
    private:
        using NodeType = typename Hash::NodeType;
        using BucketType = typename Hash::BucketType;
    #else
    public:
        using ValueType = Hash::ValueType;
    private:
        using NodeType = Hash::NodeType;
        using BucketType = Hash::BucketType;
    #endif
    public:
        constexpr HashIterator( BucketType* buckets, BucketType* bucketEnd, NodeType* node, uint8 index ):
            m_currentBucket( buckets ),
            m_endBucket( bucketEnd ),
            m_currentNode( node ),
            m_currentIndex( index ) {}

        constexpr HashIterator( BucketType* buckets, BucketType* bucketEnd ):
            m_currentBucket( buckets ),
            m_endBucket( bucketEnd )
        {
            m_currentNode = buckets->data();
            next( 0 );
        }

        constexpr bool operator==( HashIterator const& rhs ) const noexcept
        {
            return m_currentBucket == rhs.m_currentBucket &&
                m_currentNode == rhs.m_currentNode &&
                m_currentIndex == rhs.m_currentIndex;
        }

        constexpr bool operator!=( HashIterator const& rhs ) const noexcept
        {
            return !(*this == rhs);
        }

        constexpr HashIterator& operator++()
        {
            next();
            return *this;
        }

        constexpr ValueType* operator->() { return m_currentNode->m_data[m_currentIndex].get(); }

        constexpr ValueType& operator*() { return *m_currentNode->m_data[m_currentIndex]; }

    private:
        constexpr void next( uint8 inMiddleOfNode = 1 )
        {
            /*assert( m_currentBucket != nullptr );
            assert( m_currentNode != nullptr );*/

            while ( m_currentBucket != m_endBucket )
            {
                m_currentIndex = m_currentNode->getNextValidIndex( m_currentIndex + inMiddleOfNode );

                if ( m_currentIndex != NodeType::INVALID_INDEX )
                {
                    break;
                }
                inMiddleOfNode = 0;
                m_currentIndex = 0;
                if ( m_currentNode->m_next != nullptr )
                {
                    m_currentNode = m_currentNode->m_next;
                    continue;
                }
                ++m_currentBucket;
                m_currentNode = m_currentBucket->data();
                continue;
            }
            if ( m_currentBucket == m_endBucket )
                m_currentIndex = NodeType::INVALID_INDEX;
        }
    private:
        BucketType* m_currentBucket = nullptr;
        BucketType* m_endBucket = nullptr;
        NodeType* m_currentNode = nullptr;
        uint8 m_currentIndex = 0;
        friend HashConstIterator< Hash >;
    };

    template< class Hash >
    class HashConstIterator
    {
    #ifdef _MSC_VER
    public:
        using ValueType = typename const Hash::ValueType;
    private:
        using NodeType = typename const Hash::NodeType;
        using BucketType = typename const Hash::BucketType;
    #else
    public:
        using ValueType = const Hash::ValueType;
    private:
        using NodeType = const Hash::NodeType;
        using BucketType = const Hash::BucketType;
    #endif
    public:
        constexpr HashConstIterator( BucketType* buckets, BucketType* bucketEnd, NodeType* node, uint8 index ):
            m_currentBucket( buckets ),
            m_endBucket( bucketEnd ),
            m_currentNode( node ),
            m_currentIndex( index ) {}
        
        constexpr HashConstIterator( BucketType* buckets, BucketType* bucketEnd ):
            m_currentBucket( buckets ),
            m_endBucket( bucketEnd )
        {
            m_currentNode = buckets->data();
            next( 0 );
        }

        constexpr HashConstIterator( HashIterator< Hash > const& it ):
            m_currentBucket( it.m_currentBucket ),
            m_endBucket( it.m_endBucket ),
            m_currentNode( it.m_currentNode ),
            m_currentIndex( it.m_currentIndex ) {}

        constexpr bool operator==( HashConstIterator const& rhs ) const noexcept
        {
            return m_currentBucket == rhs.m_currentBucket &&
                   m_currentNode   == rhs.m_currentNode &&
                   m_currentIndex  == rhs.m_currentIndex;
        }

        constexpr bool operator!=( HashConstIterator const& rhs ) const noexcept
        {
            return !( *this == rhs );
        }

        constexpr HashConstIterator& operator++()
        {
            next();
            return *this;
        }

        constexpr ValueType* operator->() { return m_currentNode->m_data[ m_currentIndex ].get(); }

        constexpr ValueType& operator*() { return *m_currentNode->m_data[ m_currentIndex ]; }
        
    private:
        constexpr void next( uint8 inMiddleOfNode = 1 )
        {
            assert( m_currentBucket != nullptr );
            assert( m_currentNode != nullptr );

            while ( m_currentBucket != m_endBucket )
            {
                m_currentIndex = m_currentNode->getNextValidIndex( m_currentIndex + inMiddleOfNode );

                if ( m_currentIndex != NodeType::INVALID_INDEX )
                {
                    break;
                }
                inMiddleOfNode = 0;
                m_currentIndex = 0;
                if ( m_currentNode->m_next != nullptr )
                {
                    m_currentNode = m_currentNode->m_next;
                    continue;
                }
                ++m_currentBucket;
                m_currentNode = m_currentBucket->data();
                continue;
            }
            if ( m_currentBucket == m_endBucket )
                m_currentIndex = NodeType::INVALID_INDEX;
        }
    private:
        BucketType* m_currentBucket = nullptr;
        BucketType* m_endBucket = nullptr;
        NodeType* m_currentNode = nullptr;
        uint8 m_currentIndex = 0;
    };

    namespace details
    {
        /**
         * @breif TODO: Implement rehashing
         * @brief TODO: Store hashes along with data for faster re-hashes?
         */
        template< class T >
        struct Hash
        {
        public:
            using ValueType = T;
            using NodeType = details::HashNode< T >;
            using BucketType = details::MultiValueLinkedList< T >;
            using BucketListType = DynamicArray< BucketType >;

            using Iterator = HashIterator< Hash< T > >;
            using ConstIterator = HashConstIterator< Hash< T > >;
        public:
            constexpr Hash():
                m_buckets( 8 ) {}

            constexpr Hash( Hash&& other ):
                m_buckets( std::move( other.m_buckets ) ) {}

            constexpr Hash( Hash const& other ):
                m_buckets( 8 )
            {
                for ( auto it = other.cbegin(); it != other.cend(); ++it )
                {
                    auto hash_ = hash( *it );

                    m_buckets[ hash_ ].insert_unchecked( *it );
                }
            }

            constexpr Hash& operator=( Hash const& ) = delete;

            constexpr Iterator begin()
            {
                if ( size() == 0 )
                    throw std::runtime_error( "Creating empty iterator!" );

                return Iterator( m_buckets.data(), m_buckets.data() + m_buckets.size() );
            }

            constexpr Iterator end()
            {
                if ( size() == 0 )
                    throw std::runtime_error( "Creating empty iterator!" );

                BucketType* begin = m_buckets.data();
                auto const size = m_buckets.size();

                return Iterator( begin + size,
                    begin + size,
                    (begin + size)->data(),
                    uint8( 8 ) );
            }

            constexpr ConstIterator cbegin() const
            {
                if ( size() == 0 )
                    throw std::runtime_error( "Creating empty iterator!" );

                return ConstIterator( m_buckets.data(), m_buckets.data() + m_buckets.size() );
            }

            constexpr ConstIterator cend() const
            {
                if ( size() == 0 )
                    throw std::runtime_error( "Creating empty iterator!" );

                const BucketType* begin = m_buckets.data();
                auto const size = m_buckets.size();
                return ConstIterator( begin + size,
                    begin + size,
                    (begin + size)->data(),
                    uint8( 8 ) );
            }

            template< class U >
            constexpr ValueType& at( U const& val )
            {
                auto hash_ = hash( val );
                return at( val, hash_ );
            }

            template< class U >
            constexpr ValueType const& at( U const& val ) const
            {
                auto hash_ = hash( val );

                return at( val, hash_ );
            }

            template< class U >
            constexpr ValueType const& at( U const& val, uint64 hash_ ) const
            {
                auto found = m_buckets[hash_].find( val );

                if ( found == nullptr )
                    throw std::runtime_error( "Could not find value!" );

                return *found;
            }

            template< class U >
            constexpr ValueType& at( U const& val, uint64 hash_ )
            {
                auto found = m_buckets[hash_].find( val );

                if ( found == nullptr )
                    throw std::runtime_error( "Could not find value!" );

                return *found;
            }

            constexpr ValueType& insert( ValueType&& val )
            {
                auto hash_ = hash( val );

                return insert( std::move( val ), hash_ );
            }

            constexpr ValueType& insert( ValueType const& val )
            {
                auto newval = val;

                return insert( std::move( newval ) );
            }

            constexpr ValueType& insert( ValueType&& val, uint64 hash_ )
            {
                if ( m_buckets[hash_].find( val ) )
                    throw std::runtime_error( "Cannot overwrite value with insert!" );

                return *m_buckets[hash_].insert_unchecked( std::move( val ) );
            }

            template< class U >
            constexpr bool remove( U const& val )
            {
                auto hash_ = hash( val );

                return remove( val, hash_ );
            }

            template< class U >
            constexpr bool remove( U const& val, uint64 hash_ )
            {
                return m_buckets[hash_].remove( val );
            }

            constexpr uint64 size() const
            {
                uint64 size_ = 0;

                for ( uint64 i = 0; i < m_buckets.size(); ++i )
                    size_ += m_buckets[i].size();

                return size_;
            }

            constexpr static uint64 fast_mod( uint64 val, uint64 mod )
            {
                //return val % mod;
                return val & (mod - 1);
            }

            void rehash( uint64 cap )
            {
                BucketListType newdata( cap );

                for ( auto it = begin(); it != end(); ++it )
                {
                    auto hash_ = fast_mod( t::hasher< type::remove_const< ValueType > >::hash( *it ), newdata.size() );

                    newdata[ hash_ ].insertUnchecked( std::move( *it ) );
                }

                m_buckets = std::move( newdata );
            }

            constexpr uint64 hash( ValueType const& val ) const
            {
                return fast_mod( t::hasher< type::remove_const< ValueType > >::hash( val ), m_buckets.size() );
            }

            template< class U >
            constexpr uint64 hash( U const& val ) const
            {
                return fast_mod( t::hasher< U >::hash( val ), m_buckets.size() );
            }

            BucketListType m_buckets;
        };
    }
}

template< class Hash >
constexpr bool operator==( t::HashIterator< Hash > lhs, t::HashConstIterator< Hash > rhs )
{
    return lhs.operator->() == rhs.operator->();
}

template< class Hash >
constexpr bool operator!=( t::HashIterator< Hash > lhs, t::HashConstIterator< Hash > rhs )
{
    return lhs.operator->() != rhs.operator->();
}

template< class Hash >
constexpr bool operator==( t::HashConstIterator< Hash > lhs, t::HashIterator< Hash > rhs )
{
    return lhs.operator->() == rhs.operator->();
}

template< class Hash >
constexpr bool operator!=( t::HashConstIterator< Hash > lhs, t::HashIterator< Hash > rhs )
{
    return lhs.operator->() != rhs.operator->();
}
