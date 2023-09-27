#pragma once

#include "Hash.h"

namespace t
{
    namespace hashmap
    {
        template< class T, class U >
        struct pair
        {
            constexpr pair( T&& val ):
                first( std::move( val ) ) {}
            constexpr pair( T&& key, U&& val ):
                first( std::move( key ) ),
                second( std::move( val ) ) {}
            constexpr pair( T const& key, T const& val ):
                first( key ),
                second( val ) {}

            pair()                         = delete;
            pair( pair&& )                 = default;
            pair( pair const& )            = default;
            pair& operator=( pair&& )      = delete;
            pair& operator=( pair const& ) = delete;

            constexpr bool operator==( pair const& rhs ) const
            {
                return first == rhs.first;
            }

            constexpr bool operator==( T const& key ) const
            {
                return first == key;
            }

            T first  {};
            U second {};
        };
    }

    template< class T, class U >
    struct HashMap
    {
    public:
        using ValueType = hashmap::pair< type::add_const< T >, U >;
    private:
        using BaseType = details::Hash< ValueType >;
    public:
        using ConstIterator = BaseType::ConstIterator;
        using Iterator = BaseType::Iterator;
    public:
        constexpr HashMap() = default;

        constexpr Iterator begin()
        {
            return m_data.begin();
        }

        constexpr Iterator end()
        {
            return m_data.end();
        }

        constexpr ConstIterator cbegin() const
        {
            return m_data.cbegin();
        }

        constexpr ConstIterator cend() const
        {
            return m_data.cend();
        }

        constexpr const U* find( T const& key ) const
        {
            auto hash_ = m_data.hash( key );
            return m_data.m_buckets.find( key );
        }

        constexpr U& at( T const& key )
        {
            auto hash_ = m_data.hash( key );
            return m_data.at( key, hash_ ).second;
        }

        constexpr U const& at( T const& key ) const
        {
            auto hash_ = m_data.hash( key );
            return m_data.at( key, hash_ ).second;
        }

        constexpr U& operator[]( T const& key )
        {
            auto hash_ = m_data.hash( key );

            auto [ node, index, found ] = m_data.m_buckets[ hash_ ].find_or_insert( key );

            return node->m_data[ index ]->second;
        }

        constexpr U& operator[]( T&& key )
        {
            auto hash_ = m_data.hash( key );

            auto [ node, index, found ] = m_data.m_buckets[ hash_ ].find_or_insert( std::move( key ) );

            return node->m_data[ index ]->second;
        }

        constexpr U const& operator[]( T const& key ) const
        {
            return at( key );
        }

        constexpr ValueType& insert( ValueType&& pair )
        {
            auto hash_ = m_data.hash( pair.first );
            return m_data.insert( std::move( pair ), hash_ );
        }

        constexpr ValueType& insert( ValueType const& pair )
        {
            auto pair_ = pair;
            return insert( std::move( pair_ ) );
        }

        template< class... Args >
        constexpr ValueType& place( Args&&... args )
        {
            auto ptr = make_unique< ValueType >( std::forward< Args >( args )... );

            auto hash_ = m_data.hash( ptr->first );

            if ( m_data.m_buckets[ hash_ ].find( ptr->first ) )
                throw std::runtime_error("Cannot overwrite value with insert!");
            
            return *m_data.m_buckets[ hash_ ].insert_ptr_unchecked( std::move( ptr ) );
        }

        constexpr bool remove( T const& key )
        {
            auto hash_ = m_data.hash( key );
            return m_data.remove( key );
        }

        constexpr uint64 size() const { return m_data.size(); }
    private:
        BaseType m_data;
        friend ConstIterator;
        friend Iterator;
    };
}
