#pragma once

#include "Tint.h"
#include "ArrayIterator.h"

namespace t
{
	template< typename T >
	class ArrayView
	{
	public:
		using ValueType = T;
		using Iterator = ArrayIterator< T >;
		using ConstIterator = ArrayIterator< const T >;
		using ReverseIterator = ReverseArrayIterator< Iterator >;
		using ConstReverseIterator = ReverseArrayIterator< ConstIterator >;
	public:
		constexpr ArrayView() = default;

		constexpr ArrayView( T* data, uint64 size ):
			m_data( data ),
			m_size( size ) {}

		constexpr ArrayView( ArrayView const& other ):
			m_data( other.m_data ),
			m_size( other.m_size ) {}

		constexpr ArrayView& operator=( ArrayView const& rhs )
		{
			if ( this == &rhs )
				return *this;

			m_data = rhs.m_data;
			m_size = rhs.m_size;

			return *this;
		}

		constexpr T& operator[]( uint64 index )
		{
			return m_data[ index ];
		}

		constexpr T const& operator[]( uint64 index ) const
		{
			return m_data[ index ];
		}

		constexpr T& at( uint64 index )
		{
			if ( index >= m_size )
				throw Error( "Invalid index", 1 );
			return m_data[ index ];
		}

		constexpr T const& at( uint64 index ) const
		{
			if ( index >= m_size )
				throw Error( "invalid index", 1 );
			return m_data;
		}

		constexpr T* data() { return m_data; }
		constexpr const T* data() const { return m_data; }

		constexpr uint64 size() const { return m_size; }

		constexpr Iterator begin() { return Iterator( m_data ); }
		constexpr Iterator end() { return Iterator( m_data + m_size ); }

		constexpr ConstIterator cbegin() const { return ConstIterator( m_data ); }
		constexpr ConstIterator cend() const { return ConstIterator( m_data + m_size ); }

		constexpr auto begin() const { return cbegin(); }
		constexpr auto end() const { return cend(); }

		constexpr ReverseIterator rbegin() { return ReverseIterator( end() ); }
		constexpr ReverseIterator rend() { return ReverseIterator( begin() ); }

		constexpr ConstReverseIterator crbegin() { return ConstReverseIterator( cend() ); }
		constexpr ConstReverseIterator crend() { return ConstReverseIterator( cbegin() ); }
	private:
		T* m_data = nullptr;
		uint64 m_size = 0;;
	};
}
