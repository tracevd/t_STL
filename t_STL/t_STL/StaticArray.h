#pragma once

#include "ArrayIterator.h"

namespace t
{
	template< typename T, uint64 Size >
	class StaticArray
	{
	public:
		using ValueType = T;
		using SizeType  = uint64;
		using Iterator = ArrayIterator< T >;
		using ConstIterator = ArrayIterator< const T >;
		using ReverseIterator = ReverseArrayIterator< Iterator >;
		using ConstReverseIterator = ReverseArrayIterator< ConstIterator >;
	public:
		constexpr uint64 size() const { return Size; };

		constexpr T const& operator[]( uint64 i ) const { return m_data[ i ]; }
		constexpr T& operator[]( uint64 i ) { return m_data[ i ]; }

		constexpr T const& front() const { static_assert( Size > 0, "Array must have size greater than 0" ); return m_data[ 0 ]; }
		constexpr T& front() { static_assert( Size > 0, "Array must have size greater than 0" ); return m_data[0]; }

		constexpr T const& back() const { static_assert( Size > 0, "Array must have size greater than 0" ); return m_data[ Size - 1 ]; }
		constexpr T& back() { static_assert( Size > 0, "Array must have size greater than 0" ); return m_data[ Size - 1 ]; }

		constexpr T const* data() const { return m_data; }
		constexpr T* data() { return m_data; }

		constexpr auto cbegin() const { return ConstIterator( m_data ); }
		constexpr auto cend() const { return ConstIterator( m_data + Size ); }

		constexpr auto begin() { return Iterator( m_data ); }
		constexpr auto end() { return Iterator( m_data + Size ); }

		constexpr auto begin() const { return cbegin(); }
		constexpr auto end() const { return cend(); }

		constexpr auto rbegin() { return ReverseIterator( end() ); }
		constexpr auto rend() { return ReverseIterator( begin() ); }
		
		constexpr auto crbegin() const { return ConstReverseIterator( cend() ); }
		constexpr auto crend() const { return ConstReverseIterator( cbegin() ); }

		T m_data[ Size ];
	};

}
