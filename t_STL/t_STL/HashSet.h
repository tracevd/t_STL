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
		using BaseType = Hash< T >;
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

		constexpr bool remove( ValueType const& val )
		{
			return m_data.remove( val );
		}

		constexpr uint64 size() const { return m_data.size(); }
	private:
		Hash< T > m_data;
	};
}
