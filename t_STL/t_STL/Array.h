#pragma once

#include "Tint.h"
#include "Error.h"
#include "ArrayView.h"

namespace t
{
	template< typename T >
	class Array
	{
	public:
		using ValueType = T;
		using Iterator = ArrayIterator< T >;
		using ConstIterator = ArrayIterator< const T >;
		using ReverseIterator = ReverseArrayIterator< Iterator >;
		using ConstReverseIterator = ReverseArrayIterator< ConstIterator >;
	public:
		constexpr Array() = default;

		constexpr explicit Array( uint64 size ):
			m_data( new T[ size ] ),
			m_size( size ),
			m_capacity( size ) {}

		constexpr Array( Array const& list ):
			m_size( list.m_size ),
			m_capacity( list.m_capacity )
		{
			if ( m_capacity == 0 ) return;
			m_data = new T[ m_capacity ];
			for ( uint64 i = 0; i < m_size; i++ )
				m_data[ i ] = list[ i ];
		}

		constexpr Array( Array&& list ) noexcept:
			m_data( list.m_data ),
			m_size( list.m_size ),
			m_capacity( list.m_capacity )
		{
			list.m_size = 0;
			list.m_capacity = 0;
			list.m_data = nullptr;
		}

		constexpr Array( std::initializer_list< T >&& list ):
			m_data( new T[ list.size() ] ),
			m_size( list.size() ),
			m_capacity( list.size() )
		{
			uint64 i = 0;

			for ( auto it = list.begin(); i < m_size; ++it, ++i )
			{
				m_data[ i ] = std::move( *it );
			}
		}

		constexpr ~Array()
		{
			delete[] m_data;
			m_data = nullptr;
			m_size = 0;
			m_capacity = 0;
		}

		constexpr Array& operator=( Array const& list )
		{
			if ( this == &list )
				return *this;
			delete[] m_data;
			m_data = new T[ list.m_capacity ];
			for ( uint64 i = 0; i < list.m_size; i++ )
				m_data[ i ] = list[ i ];
			m_size = list.m_size;
			m_capacity = list.m_capacity;

			return *this;
		}

		constexpr Array& operator=( Array&& list )
		{
			if ( this == &list )
				return *this;

			m_data = list.m_data;
			m_size = list.m_size;
			m_capacity = list.m_capacity;
			list.m_data = nullptr;
			list.m_size = 0;
			list.m_capacity = 0;

			return *this;
		}

		constexpr bool operator==( Array const& rhs ) const
		{
			if ( m_size != rhs.m_size )
				return false;

			for ( uint64 i = 0; i < m_size; ++i )
				if ( m_data[ i ] != rhs.m_data[ i ] )
					return false;

			return true;
		}

		constexpr void reserve( uint64 capacity )
		{
			auto newdata = new T[ capacity ];

			for ( uint64 i = 0; i < m_size && i < capacity; ++i )
				newdata[ i ] = std::move( m_data[ i ] );

			delete[] m_data;
			m_data = newdata;
			m_capacity = capacity;
		}

		constexpr void resize( uint64 size )
		{
			auto newData = new T[ size ];

			for ( uint64 i = 0; i < m_size; ++i )
				newData[ i ] = std::move( m_data[ i ] );

			delete[] m_data;
			m_data = newData;
			m_size = size;
			m_capacity = size;
		}

		constexpr void shrinkToFit()
		{
			auto newdata = new T[ m_size ];

			for ( uint64 i = 0; i < m_size; ++i )
				newdata[ i ] = std::move( m_data[ i ] );

			m_capacity = m_size;
			delete[] m_data;
			m_data = newdata;
		}

		constexpr bool isEmpty() const { return m_data == nullptr || m_size == 0; }
		constexpr operator bool() const { return !isEmpty(); }

		constexpr const T* data() const { return m_data; }
		constexpr T* data() { return m_data; }

		constexpr uint64 size() const { return m_size; }
		constexpr uint64 length() const { return size(); }

		constexpr T& pop()
		{
			if ( m_size == 0 )
				throw Error( "Empty Array!", 1 );
			return m_data[ --m_size ];
		}

		constexpr T& pushBack( const T& in )
		{
			if ( m_size == m_capacity )
				realloc();
			m_data[ m_size ] = in;
			return m_data[ m_size++ ];
		}

		constexpr T& pushBack( T&& in )
		{
			if ( m_size == m_capacity )
				realloc();
			m_data[ m_size ] = std::move( in );
			return m_data[ m_size++ ];
		}

		constexpr T& operator[]( uint64 i ) { return m_data[ i ]; }

		constexpr const T& operator[]( uint64 i ) const { return m_data[ i ]; }

		constexpr T& at( uint64 i )
		{
			if ( i >= m_size )
				throw Error( "Invalid index!", 1 );
			return m_data[ i ];
		}

		constexpr T const& at( uint64 i ) const
		{
			if ( i >= m_size )
				throw Error( "Invalid index!", 1 );
			return m_data[ i ];
		}

		constexpr auto cbegin() const
		{
			return ConstIterator( m_data );
		}

		constexpr auto cend() const
		{
			return ConstIterator( m_data + m_size );
		}

		constexpr auto begin()
		{
			return Iterator( m_data );
		}

		constexpr auto end()
		{
			return Iterator( m_data + m_size );
		}

		constexpr auto begin() const { return cbegin(); }
		constexpr auto end() const { return cend(); }

		constexpr auto rbegin() { return ReverseIterator( end() ); }
		constexpr auto rend() { return ReverseIterator( begin() ); }

		constexpr auto crbegin() const { return ConstReverseIterator( cend() ); }
		constexpr auto crend() const { return ConstReverseIterator( cbegin() ); }
	private:
		constexpr void realloc()
		{
			uint64 newcap = m_capacity ? m_capacity * 2 : 4;
			auto temp = m_data;
			m_data = new T[ newcap ];
			for ( uint64 i = 0; i < m_size; i++ )
				m_data[ i ] = std::move( temp[ i ] );
			m_capacity = newcap;
			delete[] temp;
		}
	private:
		T* m_data = nullptr;
		uint64 m_size = 0;
		uint64 m_capacity = 0;
	};
}
