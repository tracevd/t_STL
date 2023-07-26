#pragma once

#include <assert.h>
#include "Type.h"
#include "Vector.h"
#include "Memory.h"
#include "Array.h"

#define CONSTX constexpr

namespace t
{
    template< class T >
    struct Hash;

    namespace hash_impl
    {
        template< class T >
        class MultiValueLinkedList;

        template< class T >
        struct Node
        {
        public:
            CONSTX Node() = default;

            CONSTX Node( T val ):
                m_validIndexes( 1 )
            {
                m_data[ 0 ] = UniquePtr< T >( std::move( val ) );
            }

            CONSTX ~Node() = default;

            CONSTX Node* next() const { return m_next; }

            CONSTX uint8 getNextValidIndex( uint8 start = 0 ) const
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
            CONSTX uint8 getNextEmptyIndex( uint8 start = 0 ) const
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
            CONSTX static uint8 INVALID_INDEX = 8;
            t::Array< t::UniquePtr< T >, 8 > m_data;
            Node* m_next = nullptr;
            uint8 m_validIndexes = 0;
            friend MultiValueLinkedList< T >;
        };

        template< class T >
        class MultiValueLinkedList
        {
        private:
            using Node_t = Node< T >;
        public:
            CONSTX MultiValueLinkedList() = default;

            CONSTX ~MultiValueLinkedList()
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
            CONSTX T* insert_unchecked( U&& val )
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

            /**
             * @brief Finds a value. Returns nullptr if not found.
             */
            template< class U >
            CONSTX const T* find( U const& val ) const
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
             * @return Node< T > - The node
             * @return uint8 - Index in the node's data
             * @return bool - True if the value was found, false if it was not
             */
            template< class U >
            CONSTX std::tuple< Node_t*, uint8, bool > find_or_insert( U const& val )
            {
                std::tuple< Node_t*, uint8, bool > tuple = { nullptr, INVALID_INDEX, false };

                auto n = &m_data;

                while ( n )
                {
                    for ( uint8 i = 0; i < INVALID_INDEX; ++i )
                    {
                        if ( (n->m_validIndex >> i) & 1 )
                        {
                            if ( *n->m_data[i] == val )
                                return { n, i, true };
                        }
                        else if ( std::get< 1 >( tuple ) == INVALID_INDEX )
                        {
                            std::get< 0 >( tuple ) = n;
                            std::get< 1 >( tuple ) = i;
                        }
                    }
                    if ( n->m_next != nullptr )
                    {
                        n = n->m_next;
                        continue;
                    }
                    if ( std::get< 1 >( tuple ) != INVALID_INDEX )
                    {
                        std::get< 0 >( tuple )->m_validIndex ^= 1 << std::get< 1 >( tuple );
                        ++m_size;
                        return tuple;
                    }
                    n->m_next = new Node_t( T( val ) );
                    ++m_size;
                    return { n->m_next, 0, false };
                }
            }

            template< class U >
            CONSTX std::tuple< Node_t*, uint8, bool > find_or_insert( U&& val )
            {
                std::tuple< Node_t*, uint8, bool > tuple = { nullptr, INVALID_INDEX, false };

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
                        else if ( std::get< 1 >( tuple ) == INVALID_INDEX )
                        {
                            std::get< 0 >( tuple ) = n;
                            std::get< 1 >( tuple ) = i;
                        }
                    }
                    if ( n->m_next != nullptr )
                    {
                        n = n->m_next;
                        continue;
                    }
                    if ( std::get< 1 >( tuple ) != INVALID_INDEX )
                    {
                        auto ptr = std::get< 0 >( tuple );
                        ptr->m_data[ std::get< 1 >( tuple ) ] = UniquePtr< T >( T( std::move( val ) ) );
                        ptr->m_validIndexes ^= 1 << std::get< 1 >( tuple );
                        ++m_size;
                        return tuple;
                    }
                    n->m_next = new Node_t( T( std::move( val ) ) );
                    ++m_size;
                    return { n->m_next, 0, false };
                }

                assert( std::get< 0 >( tuple ) != nullptr );

                return tuple;
            }

            template< class U >
            CONSTX bool remove( U const& val )
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

            CONSTX uint64 size() const { return m_size; }

            CONSTX Node< T >* data() { return &m_data; }
            CONSTX Node< T > const* data() const { return &m_data; }
        private:
            static constexpr auto INVALID_INDEX = Node< T >::INVALID_INDEX;
            Node< T > m_data;
            uint64 m_size = 0;
            friend Hash< T >;
        };
    }

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

    template< class T >
    struct Hash
    {
    public:
        using ValueType = T;
        using NodeType = hash_impl::Node< T >;
        using BucketType = hash_impl::MultiValueLinkedList< T >;
        using BucketListType = Vector< BucketType >;

        using Iterator = HashIterator< Hash< T > >;
        using ConstIterator = HashConstIterator< Hash< T > >;
    public:
        CONSTX Hash():
            m_buckets( 8 ) {}

        constexpr Iterator begin()
        {
            if ( size() == 0 )
                throw std::runtime_error("Creating empty iterator!");
            
            return Iterator( m_buckets.data(), m_buckets.data() + m_buckets.size() );
        }

        constexpr Iterator end()
        {
            if ( size() == 0 )
                throw std::runtime_error("Creating empty iterator!");
            
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
                throw std::runtime_error("Creating empty iterator!");
            
            return ConstIterator( m_buckets.data(), m_buckets.data() + m_buckets.size() );
        }

        constexpr ConstIterator cend() const
        {
            if ( size() == 0 )
                throw std::runtime_error("Creating empty iterator!");
            
            const BucketType* begin = m_buckets.data();
            auto const size = m_buckets.size();
            return ConstIterator( begin + size,
                                  begin + size,
                                  ( begin + size )->data(),
                                  uint8(8) );
        }

        template< class U >
        CONSTX ValueType& at( U const& val )
        {
            auto hash_ = hash( val );
            return at( val, hash_ );
        }

        template< class U >
        CONSTX ValueType const& at( U const& val ) const
        {
            auto hash_ = hash( val );

            return at( val, hash_ );
        }

        template< class U >
        CONSTX ValueType const& at( U const& val, uint64 hash_ ) const
        {
            auto found = m_buckets[ hash_ ].find( val );

            if ( found == nullptr )
                throw std::runtime_error( "Could not find value!" );

            return *found;
        }

        template< class U >
        CONSTX ValueType& at( U const& val, uint64 hash_ )
        {
            auto found = m_buckets[hash_].find( val );

            if ( found == nullptr )
                throw std::runtime_error( "Could not find value!" );

            return *found;
        }

        CONSTX ValueType& insert( ValueType&& val )
        {
            auto hash_ = hash( val );
            
            return insert( std::move( val ), hash_ );
        }

        CONSTX ValueType& insert( ValueType const& val )
        {
            auto newval = val;
            
            return insert( std::move( newval ) );
        }

        CONSTX ValueType& insert( ValueType&& val, uint64 hash_ )
        {
            if ( m_buckets[ hash_ ].find( val ) )
                throw std::runtime_error( "Cannot overwrite value with insert!" );

            return *m_buckets[ hash_ ].insert_unchecked( std::move( val ) );
        }

        template< class U >
        CONSTX bool remove( U const& val )
        {
            auto hash_ = hash( val );

            return remove( val, hash_ );
        }

        template< class U >
        CONSTX bool remove( U const& val, uint64 hash_ )
        {
            return m_buckets[hash_].remove( val );
        }

        CONSTX uint64 size() const
        {
            uint64 size_ = 0;

            for ( uint64 i = 0; i < m_buckets.size(); ++i )
                size_ += m_buckets[ i ].size();

            return size_;
        }

        uint64 hash( ValueType const& val ) const { return t::hasher< type::remove_const< ValueType > >::hash( val ) % m_buckets.size(); }

        template< class U >
        uint64 hash( U const& val ) const { return t::hasher< U >::hash( val ) % m_buckets.size(); }

        BucketListType m_buckets;
    };
}
