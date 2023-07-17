#pragma once

#include "Tint.h"

namespace t
{
	template< typename Vector >
	class VectorIterator
	{
	public:
		using ValueType = Vector::ValueType;
	private:
		ValueType* m_ptr;
	public:
		VectorIterator( ValueType* ptr )
			: m_ptr(ptr) {}
		VectorIterator& operator++()
		{
			m_ptr++;
			return *this;
		}
		VectorIterator operator++(int)
		{
			VectorIterator it = *this;
			++(*this);
			return it;
		}
		VectorIterator& operator--()
		{
			m_ptr--;
			return *this;
		}
		VectorIterator operator--( int )
		{
			VectorIterator it = *this;
			--(*this);
			return it;
		}
		VectorIterator operator+( uint64 offset )
		{
			return VectorIterator( m_ptr + offset );
		}
		ValueType& operator[]( int i )
		{
			return *( m_ptr[i] );
		}
		ValueType* operator->()
		{
			return m_ptr;
		}
		ValueType& operator*()
		{
			return *m_ptr;
		}
		bool operator==( const VectorIterator& other ) const
		{
			return m_ptr == other.m_ptr;
		}
		bool operator!=( const VectorIterator& other ) const
		{
			return !( *this == other );
		}
		bool operator==( const void* other ) const
		{
			return m_ptr == other;
		}
		bool operator!=( const void* other ) const
		{
			return !(*this == other);
		}
	};
	template< typename Vector >
	class VectorIteratorConst
	{
	public:
		using ValueType = typename Vector::ValueType;
		using PointerType = ValueType* const;
		using ReferenceType = ValueType const&;
	private:
		PointerType m_ptr;
	public:
		VectorIteratorConst( PointerType ptr )
			: m_ptr( ptr ) {}
		VectorIteratorConst& operator++()
		{
			m_ptr++;
			return *this;
		}
		VectorIteratorConst operator++( int )
		{
			VectorIterator it = *this;
			++(*this);
			return it;
		}
		VectorIteratorConst& operator--()
		{
			m_ptr--;
			return *this;
		}
		VectorIteratorConst operator--( int )
		{
			VectorIterator it = *this;
			--(*this);
			return it;
		}
		ReferenceType operator[]( int i )
		{
			return *( m_ptr[ i ] );
		}
		PointerType operator->()
		{
			return m_ptr;
		}
		ReferenceType operator*()
		{
			return *m_ptr;
		}
		bool operator==( const VectorIteratorConst& other ) const
		{
			return m_ptr == other.m_ptr;
		}
		bool operator!=( const VectorIteratorConst& other ) const
		{
			return !(*this == other);
		}
		bool operator==( const void* other ) const
		{
			return m_ptr == other;
		}
		bool operator!=( const void* other ) const
		{
			return !(*this == other);
		}
	};
template< typename T >
class Vector
{
public:
	using ValueType = T;
	using Iterator = VectorIterator< Vector< T > >;
	using Const_Iterator = VectorIteratorConst< Vector< T > >;
public:
	constexpr Vector()
	{
		m_capacity = 2;
		m_data = new T[ m_capacity ];
		m_size = 0;
	}
	constexpr explicit Vector( uint64 size )
	{
		m_data = new T[ size ];
		m_size = size;
		m_capacity = size;
	}
	constexpr Vector( const Vector& vec )
	{
		m_data = new T[ vec.m_capacity ];
		for ( uint64 i = 0; i < vec.m_size; i++ )
			m_data[ i ] = vec[ i ];
		m_size = vec.m_size;
		m_capacity = vec.m_capacity;
	}
	constexpr Vector( Vector&& vec ) noexcept
	{
		m_data = vec.m_data;
		m_size = vec.m_size;
		m_capacity = vec.m_capacity;
		vec.m_size = 0;
		vec.m_capacity = 0;
		vec.m_data = nullptr;
	}
	constexpr Vector( std::initializer_list< T > list )
	{
		resize( list.size() );
		uint64 i = 0;
		for ( auto it = list.begin(); it != list.end(); ++it, ++i )
		{
			m_data[ i ] = std::move( *it );
		}
	}
	constexpr ~Vector()
	{
		if ( m_size != 0 )
			delete[] m_data;
	}
	constexpr Vector& operator=( Vector& vec )
	{
		if ( this == &vec )
			return *this;
		delete[] m_data;
		m_data = new T[ vec.m_capacity ];
		for ( uint64 i = 0; i < vec.m_size; i++ )
			m_data[ i ] = vec[ i ];
		m_size = vec.m_size;
		m_capacity = vec.m_capacity;

		return *this;
	}
	constexpr Vector& operator=( Vector&& vec )
	{
		m_data = vec.m_data;
		m_size = vec.m_size;
		m_capacity = vec.m_capacity;
		vec.m_data = nullptr;

		return *this;
	}
	constexpr bool operator==( Vector const& rhs ) const
	{
		if ( m_size != rhs.m_size )
			return false;
		for ( uint64 i = 0; i < m_size; ++i )
		{
			if ( m_data[ i ] != rhs.m_data[ i ] )
				return false;
		}
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
		T* newData = new T[ size ];
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
		{
			newdata[ i ] = std::move( m_data[ i ] );
		}
		m_capacity = m_size;
		delete[] m_data;
		m_data = newdata;
	}

	constexpr const T* data() const { return m_data; }
	constexpr T* data() { return m_data; }

	constexpr [[nodiscard]] uint64 size() const
	{
		return m_size;
	}
	constexpr T& pop()
	{
		if ( m_size == 0 )
			throw std::runtime_error("Empty Vector!");
		return m_data[ --m_size ];
	}
	constexpr void removeFirst( const T& elem )
	{
		if ( m_size == 1 )
		{
			m_data[ 0 ] = T();
			--m_size;
			return;
		}
		bool deleted = false;
		for ( uint64 i = 0; i < m_size; i++ )
		{
			if ( deleted )
			{
				m_data[ i - 1 ] = std::move( m_data[ i ] );
				continue;
			}
			if ( m_data[ i ] == elem )
			{
				m_data[ i ] = T();
				deleted = true;
			}
		}
		if ( deleted )
			--m_size;
	}
	constexpr Iterator begin()
	{
		return Iterator( m_data );
	}
	constexpr const Const_Iterator cbegin() const
	{
		return Const_Iterator( m_data );
	}
	constexpr Iterator end()
	{
		return Iterator( m_data + m_size );
	}
	constexpr const Const_Iterator cend() const
	{
		return Const_Iterator( m_data + m_size );
	}
	constexpr T& pushBack( const T& in )
	{
		if ( m_size == m_capacity )
			realloc();
		return m_data[ m_size++ ] = in;
	}
	constexpr T& pushBack( T&& in )
	{
		if ( m_size == m_capacity )
			realloc();
		return m_data[ m_size++ ] = std::move( in );
	}
	constexpr T& operator[]( uint64 i )
	{
		if ( i > m_size )
			throw std::runtime_error("Exceeded Max Array Bounds");
		return m_data[ i ];
	}
	constexpr const T& operator[]( uint64 i ) const
	{
		if ( i > m_size )
			throw std::runtime_error("Exceeded Max Array Bounds");
		return m_data[ i ];
	}
	constexpr T* find( const T& elem )
	{
		if ( m_size == 0 )
			return nullptr;
		if ( m_size < 5 )
		{
			return find_w_linearSweep( elem );
		}
		return find_w_convergingSweep( elem );
	}
	constexpr const T* const find( const T& elem ) const
	{
		if ( m_size == 0 )
			return nullptr;
		if ( m_size < 5 )
		{
			return find_w_linearSweep( elem );
		}
		return find_w_convergingSweep( elem );
	}
	constexpr void remove( const T& elem )
	{
		T* toRemove = find( elem );
		if ( toRemove == nullptr )
			return;
		uint64_t offset = toRemove - m_data;
		while ( offset < m_size-1 )
		{
			m_data[ offset ] = std::move( m_data[ ++offset ] );
		}
		--m_size;
	}
private:
	T* m_data = nullptr;
	uint64 m_size = 0;
	uint64 m_capacity = 0;
private:
	constexpr T* find_w_linearSweep( const T& elem )
	{
		for ( uint64 i = 0; i < m_size; i++ )
		{
			if ( m_data[ i ] == elem )
				return m_data + i;
		}
		return nullptr;
	}
	constexpr const T* const find_w_linearSweep( const T& elem ) const
	{
		for ( uint64 i = 0; i < m_size; i++ )
		{
			if ( m_data[i] == elem )
				return m_data + i;
		}
		return nullptr;
	}
	constexpr T* find_w_convergingSweep( const T& elem )
	{
		for ( uint64 i = 0, j = m_size-1; i < m_size && i <= j; i++, j-- )
		{
			if ( m_data[ i ] == elem )
				return m_data + i;
			if ( m_data[ j ] == elem )
				return m_data + j;
		}
		return nullptr;
	}
	constexpr const T* const find_w_convergingSweep( const T& elem ) const
	{
		for ( uint64 i = 0, j = m_size - 1; i < m_size && i <= j; i++, j-- )
		{
			if ( m_data[i] == elem )
				return m_data + i;
			if ( m_data[j] == elem )
				return m_data + j;
		}
		return nullptr;
	}
	constexpr void realloc()
	{
		uint64 newSize = m_capacity * 2;
		// std::cout << "Realloc Vector: " << m_capacity << " -> " << newSize << "\n";
		auto temp = m_data;
		m_data = new T[ newSize ];
		for ( uint64 i = 0; i < m_size; i++ )
			m_data[ i ] = std::move( temp[ i ] );
		m_capacity = newSize;
		delete[] temp;
	}
	constexpr void realloc( uint64 newSize )
	{
		// std::cout << "Realloc Vector: " << m_capacity << " -> " << newSize << "\n";
		auto temp = m_data;
		m_data = new T[newSize];
		for ( uint64 i = 0; i < m_size; i++ )
			m_data[ i ] = std::move( temp[i] );
		m_capacity = newSize;
		delete[] temp;
	}
};
}