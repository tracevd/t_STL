#pragma once

#include "Tint.h"

namespace t
{
	template< class List >
	class ListConstIterator;

	template< typename List >
	class ListIterator
	{
	public:
		using ValueType = List::ValueType;
	public:
		ListIterator() = delete;

		constexpr explicit ListIterator( ValueType* ptr ):
			m_ptr( ptr ) {}

		constexpr ListIterator& operator++()
		{
			m_ptr++;
			return *this;
		}
		constexpr ListIterator operator++(int)
		{
			ListIterator it = *this;
			++(*this);
			return it;
		}
		constexpr ListIterator& operator--()
		{
			m_ptr--;
			return *this;
		}
		constexpr ListIterator operator--( int )
		{
			ListIterator it = *this;
			--(*this);
			return it;
		}
		constexpr ListIterator operator+( uint64 offset )
		{
			return ListIterator( m_ptr + offset );
		}
		constexpr ValueType* operator->()
		{
			return m_ptr;
		}
		constexpr ValueType& operator*()
		{
			return *m_ptr;
		}
		constexpr bool operator==( const ListIterator& other ) const
		{
			return m_ptr == other.m_ptr;
		}
		constexpr bool operator!=( const ListIterator& other ) const
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
		friend ListConstIterator< List >;
	};

	template< typename List >
	class ListConstIterator
	{
	public:
		using ValueType = typename List::ValueType;
		using PointerType = ValueType const*;
		using ReferenceType = ValueType const&;
	private:
		PointerType m_ptr;
	public:
		ListConstIterator() = delete;

		constexpr explicit ListConstIterator( PointerType ptr ):
			m_ptr( ptr ) {}

		constexpr ListConstIterator( ListIterator< List > it ):
			m_ptr( it.m_ptr ) {}

		constexpr ListConstIterator& operator++()
		{
			m_ptr++;
			return *this;
		}
		constexpr ListConstIterator operator++( int )
		{
			ListIterator it = *this;
			++(*this);
			return it;
		}
		constexpr ListConstIterator& operator--()
		{
			m_ptr--;
			return *this;
		}
		constexpr ListConstIterator operator--( int )
		{
			ListIterator it = *this;
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
		constexpr bool operator==( const ListConstIterator& other ) const
		{
			return m_ptr == other.m_ptr;
		}
		constexpr bool operator!=( const ListConstIterator& other ) const
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
	class List
	{
	public:
		using ValueType = T;
		using Iterator = ListIterator< List< T > >;
		using Const_Iterator = ListConstIterator< List< T > >;
	public:
		constexpr List() = default;

		constexpr explicit List( uint64 size ):
			m_data( new T[ size ] ),
			m_size( size ),
			m_capacity( size ) {}

		constexpr List( List const& list ):
			m_size( list.m_size ),
			m_capacity( list.m_capacity )
		{
			if ( m_capacity == 0 ) return;
			m_data = new T[ m_capacity ];
			for ( uint64 i = 0; i < m_size; i++ )
				m_data[ i ] = list[ i ];
		}

		constexpr List( List&& list ) noexcept:
			m_data( list.m_data ),
			m_size( list.m_size ),
			m_capacity( list.m_capacity )
		{
			list.m_size = 0;
			list.m_capacity = 0;
			list.m_data = nullptr;
		}

		constexpr List( std::initializer_list< T >&& list ):
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

		constexpr ~List()
		{
			delete[] m_data;
			m_data = nullptr;
			m_size = 0;
			m_capacity = 0;
		}

		constexpr List& operator=( List const& list )
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

		constexpr List& operator=( List&& list )
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

		constexpr bool operator==( List const& rhs ) const
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
			if ( m_data != nullptr )
				delete[] m_data;
			m_data = new T[ capacity ];
			m_capacity = capacity;
			m_size = 0;
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
				throw std::runtime_error("Empty List!");
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
