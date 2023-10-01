#pragma once

#include <stdexcept>

#include "Tint.h"

namespace t
{
	template< class DynamicArray >
	class DynamicArrayConstIterator;

	template< typename DynamicArray >
	class DynamicArrayIterator
	{
	public:
		using ValueType = DynamicArray::ValueType;
	public:
		DynamicArrayIterator() = delete;

		constexpr explicit DynamicArrayIterator( ValueType* ptr ):
			m_ptr( ptr ) {}

		constexpr DynamicArrayIterator& operator++()
		{
			m_ptr++;
			return *this;
		}
		constexpr DynamicArrayIterator operator++(int)
		{
			DynamicArrayIterator it = *this;
			++(*this);
			return it;
		}
		constexpr DynamicArrayIterator& operator--()
		{
			m_ptr--;
			return *this;
		}
		constexpr DynamicArrayIterator operator--( int )
		{
			DynamicArrayIterator it = *this;
			--(*this);
			return it;
		}
		constexpr DynamicArrayIterator operator+( uint64 offset )
		{
			return DynamicArrayIterator( m_ptr + offset );
		}
		constexpr ValueType* operator->()
		{
			return m_ptr;
		}
		constexpr ValueType& operator*()
		{
			return *m_ptr;
		}
		constexpr bool operator==( const DynamicArrayIterator& other ) const
		{
			return m_ptr == other.m_ptr;
		}
		constexpr bool operator!=( const DynamicArrayIterator& other ) const
		{
			return !( *this == other );
		}
		constexpr bool operator==( const void* other ) const
		{
			return m_ptr == other;
		}
		constexpr bool operator!=( const void* other ) const
		{
			return !(*this == other);
		}
	private:
		ValueType* m_ptr;
		friend DynamicArrayConstIterator< DynamicArray >;
	};

	template< typename DynamicArray >
	class DynamicArrayConstIterator
	{
	public:
		using ValueType = typename DynamicArray::ValueType;
		using PointerType = ValueType const*;
		using ReferenceType = ValueType const&;
	private:
		PointerType m_ptr;
	public:
		DynamicArrayConstIterator() = delete;

		constexpr explicit DynamicArrayConstIterator( PointerType ptr ):
			m_ptr( ptr ) {}

		constexpr DynamicArrayConstIterator( DynamicArrayIterator< DynamicArray > it ):
			m_ptr( it.m_ptr ) {}

		constexpr DynamicArrayConstIterator& operator++()
		{
			m_ptr++;
			return *this;
		}
		constexpr DynamicArrayConstIterator operator++( int )
		{
			DynamicArrayConstIterator it = *this;
			++(*this);
			return it;
		}
		constexpr DynamicArrayConstIterator& operator--()
		{
			m_ptr--;
			return *this;
		}
		constexpr DynamicArrayConstIterator operator--( int )
		{
			DynamicArrayConstIterator it = *this;
			--(*this);
			return it;
		}
		constexpr PointerType operator->()
		{
			return m_ptr;
		}
		constexpr ReferenceType operator*()
		{
			return *m_ptr;
		}
		constexpr bool operator==( const DynamicArrayConstIterator& other ) const
		{
			return m_ptr == other.m_ptr;
		}
		constexpr bool operator!=( const DynamicArrayConstIterator& other ) const
		{
			return !(*this == other);
		}
		constexpr bool operator==( const void* other ) const
		{
			return m_ptr == other;
		}
		constexpr bool operator!=( const void* other ) const
		{
			return !(*this == other);
		}
	};

	template< typename T >
	class DynamicArray
	{
	public:
		using ValueType = T;
		using Iterator = DynamicArrayIterator< DynamicArray< T > >;
		using Const_Iterator = DynamicArrayConstIterator< DynamicArray< T > >;
	public:
		constexpr DynamicArray() = default;

		constexpr explicit DynamicArray( uint64 size ):
			m_data( new T[ size ] ),
			m_size( size ),
			m_capacity( size ) {}

		constexpr DynamicArray( DynamicArray const& list ):
			m_size( list.m_size ),
			m_capacity( list.m_capacity )
		{
			if ( m_capacity == 0 ) return;
			m_data = new T[ m_capacity ];
			for ( uint64 i = 0; i < m_size; i++ )
				m_data[ i ] = list[ i ];
		}

		constexpr DynamicArray( DynamicArray&& list ) noexcept:
			m_data( list.m_data ),
			m_size( list.m_size ),
			m_capacity( list.m_capacity )
		{
			list.m_size = 0;
			list.m_capacity = 0;
			list.m_data = nullptr;
		}

		constexpr DynamicArray( std::initializer_list< T >&& list ):
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

		constexpr ~DynamicArray()
		{
			delete[] m_data;
			m_data = nullptr;
			m_size = 0;
			m_capacity = 0;
		}

		constexpr DynamicArray& operator=( DynamicArray const& list )
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

		constexpr DynamicArray& operator=( DynamicArray&& list )
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

		constexpr bool operator==( DynamicArray const& rhs ) const
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

		constexpr const T* data() const { return m_data; }
		constexpr T* data() { return m_data; }

		constexpr uint64 size() const { return m_size; }
		constexpr uint64 length() const { return size(); }

		constexpr T& pop()
		{
			if ( m_size == 0 )
				throw std::runtime_error("Empty DynamicArray!");
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
				throw std::range_error("Invalid index!");
			return m_data[ i ];
		}

		constexpr T const& at( uint64 i ) const
		{
			if ( i >= m_size )
				throw std::range_error("Invalid index!");
			return m_data[ i ];
		}

		constexpr const auto cbegin() const
		{
			return Const_Iterator( m_data );
		}

		constexpr const auto cend() const
		{
			return Const_Iterator( m_data + m_size );
		}

		constexpr auto begin()
		{
			return Iterator( m_data );
		}

		constexpr auto end()
		{
			return Iterator( m_data + m_size );
		}
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
