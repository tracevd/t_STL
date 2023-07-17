#pragma once

namespace t
{
	template< typename T, uint64 Size >
	class Array
	{
	private:
		T m_data[ Size ];
	public:
		constexpr Array() = default;

		constexpr Array( std::initializer_list< T > list )
		{

			auto it = list.begin();

			for ( size_t i = 0; i < Size && it != list.end(); ++i, ++it )
			{
				m_data[ i ] = std::move( *it );
			}
		}

		Array& operator=( Array&& rhs )
		{
			for ( size_t i = 0; i < Size; ++i )
			{
				m_data[ i ] = std::move( rhs[ i ] );
			}
			return *this;
		}

		Array& operator=( Array const& rhs )
		{
			for ( size_t i = 0; i < Size; ++i )
			{
				m_data[ i ] = rhs[ i ];
			}
			return *this;
		}

		constexpr uint64 size() const { return Size; };

		constexpr const T& operator[]( size_t i ) const
		{
			return m_data[ i ];
		}

		constexpr T& operator[]( size_t i ) 
		{ 
			return m_data[ i ];
		}

		constexpr const T& front() const 
		{ 
			return m_data[ 0 ]; 
		}

		constexpr T& front() 
		{ 
			return m_data[ 0 ]; 
		}

		constexpr T const& back() const
		{
			return m_data[ Size - 1 ];
		}

		constexpr T& back()
		{
			return m_data[ Size - 1 ];
		}

		constexpr T* data() { return m_data; }
		constexpr const T* data() const { return m_data; }

		constexpr const T* cbegin() const { return m_data; }
		constexpr const T* cend() const { return m_data + Size; }

		constexpr T* begin() const { return m_data; }
		constexpr T* end() const { return m_data + Size; }
	};

}