#pragma once

#include "Hash.h"

namespace t
{
	template< class T >
	class HashSet
	{
	public:
		using ValueType = type::add_const< T >;
	private:
		using BaseType = Hash< ValueType >;
	public:
		using ConstIterator = BaseType::ConstIterator;
	public:
		HashSet() = default;

		constexpr ConstIterator cbegin() const
		{
			return m_data.cbegin();
		}

		constexpr ConstIterator cend() const
		{
			return m_data.cend();
		}

		constexpr ValueType& at( ValueType const& val ) const
		{
			return m_data.at( val );
		}

		constexpr ValueType& insert( type::remove_const< ValueType >&& val )
		{
			return m_data.insert( std::move( val ) );
		}

		constexpr ValueType& insert( ValueType const& val )
		{
			return m_data.insert( val );
		}

		template< class... Args >
        constexpr ValueType& place( Args&&... args )
        {
            auto ptr = make_unique< ValueType >( std::forward< Args >( args )... );

            auto hash_ = m_data.hash( *ptr );

            if ( m_data.m_buckets[ hash_ ].find( *ptr ) )
                throw std::runtime_error("Cannot overwrite value with insert!");
            
            return *m_data.m_buckets[ hash_ ].insert_ptr_unchecked( std::move( ptr ) );
        }

		constexpr bool remove( ValueType const& val )
		{
			return m_data.remove( val );
		}

		constexpr uint64 size() const { return m_data.size(); }
	private:
		Hash< ValueType > m_data;
	};
}
