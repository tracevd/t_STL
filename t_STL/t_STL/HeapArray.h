#include "Tint.h"

namespace t
{
	template< class T >
	class HeapArray
	{
	public:
		HeapArray() = delete;

		constexpr HeapArray( uint64 size ):
			m_data( new T[ size ]() ),
			m_size( size ) {}

		constexpr HeapArray( HeapArray const& arr ):
			m_data( new T[ arr.m_size ] ),
			m_size( arr.m_size )
		{
			for ( uint64 i = 0; i < m_size; ++i )
			{
				m_data[ i ] = arr.m_data[ i ];
			}
		}

		constexpr HeapArray( HeapArray&& arr ) noexcept:
			m_data( arr.m_data ),
			m_size( arr.m_size )
		{
			arr.m_data = nullptr;
			arr.m_size = 0;
		}

		constexpr HeapArray& operator=( HeapArray&& rhs ) noexcept
		{
			if ( this == &rhs )
				return *this;

			DestroyData();

			m_data = rhs.m_data;
			m_size = rhs.m_size;

			rhs.m_data = nullptr;
			rhs.m_size = 0;

			return *this;
		}

		constexpr HeapArray& operator=( HeapArray const& rhs )
		{
			if ( this == &rhs )
				return *this;

			DestroyData();

			m_data = new T[ rhs.m_size ];
			m_size = rhs.m_size;

			for ( uint64 i = 0; i < m_size; ++i )
				m_data[ i ] = rhs.m_data[ i ];

			return *this;
		}

		constexpr ~HeapArray()
		{
			DestroyData();
			m_data = nullptr;
			m_size = 0;
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
				throw Error( "Index too large!", 1 );
			return m_data[ index ];
		}

		constexpr T const& at( uint64 index ) const
		{
			if ( index >= m_size )
				throw Error( "Index too large!", 1 );
			return m_data[ index ];
		}

		constexpr T const* cbegin() const { return m_data; }
		constexpr T const* cend() const { return m_data + m_size; }

		constexpr T* begin() { return m_data; }
		constexpr T* end() { return m_data + m_size; }

		constexpr auto begin() const { return cbegin(); }
		constexpr auto end() const { return cend(); }

		constexpr uint64 size() const { return m_size; }

	private:
		constexpr void DestroyData()
		{
			delete[] m_data;
		}
	private:
		T* m_data;
		uint64 m_size;
	};
}