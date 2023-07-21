#pragma once

namespace t
{
	template< typename T, uint64 Size >
	class Array
	{
	public:
		using ValueType = T;
		using SizeType  = uint64;
	public:
		constexpr Array() = default;

		constexpr Array( std::initializer_list< T > list )
		{
			auto it = list.begin();

			for ( uint64 i = 0; i < Size && it != list.end(); ++i, ++it )
			{
				m_data[ i ] = std::move( *it );
			}
		}

		Array& operator=( Array&& rhs )
		{
			for ( uint64 i = 0; i < Size; ++i )
			{
				m_data[ i ] = std::move( rhs[ i ] );
			}
			return *this;
		}

		Array& operator=( Array const& rhs )
		{
			for ( uint64 i = 0; i < Size; ++i )
			{
				m_data[ i ] = rhs[ i ];
			}
			return *this;
		}

		constexpr uint64 size() const { return Size; };

		constexpr const T& operator[]( uint64 i ) const { return m_data[ i ]; }
		constexpr T& operator[]( uint64 i ) { return m_data[ i ]; }

		constexpr const T& front() const { return m_data[ 0 ]; }
		constexpr T& front() { return m_data[ 0 ]; }

		constexpr T const& back() const { return m_data[ Size - 1 ]; }
		constexpr T& back() { return m_data[ Size - 1 ]; }

		constexpr const T* data() const { return m_data; }
		constexpr T* data() { return m_data; }

		constexpr const T* cbegin() const { return m_data; }
		constexpr const T* cend() const { return m_data + Size; }

		constexpr T* begin() { return m_data; }
		constexpr T* end()   { return m_data + Size; }
	private:
		T m_data[ Size ];
	};

}
