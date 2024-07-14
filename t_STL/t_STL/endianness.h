#pragma once

#include <bit>

#include "Tint.h"

namespace t
{
	enum class endianness : uint8
	{
		little = 0,
		big    = 1,
		native = std::endian::native == std::endian::little ? little : big,
		not_native = std::endian::native == std::endian::little ? big : little
	};

	template< typename T >
	constexpr T byteswap( T data )
	{
		if constexpr ( sizeof( T ) == 1 )
		{
			return data;
		}
		else if constexpr ( sizeof( T ) == 2 )
		{
			return ((data >> 8) & 0xff) | ((data & 0xff) << 8);
		}
		else if constexpr ( sizeof( T ) == 4 )
		{
			return ((data >> 24) & 0xff) | (((data >> 16) & 0xff) << 8) | (((data >> 8) & 0xff) << 16) | ((data & 0xff) << 24);
		}
		else if constexpr ( sizeof( T ) == 8 )
		{
			return ((data >> 56) & 0xff) | (((data >> 48) & 0xff) << 8) | (((data >> 40) & 0xff) << 16) | (((data >> 32) & 0xff) << 24) |
				(((data >> 24) & 0xff) << 32) | (((data >> 16) & 0xff) << 40) | (((data >> 8) & 0xff) << 48) | ((data & 0xff) << 56);
		}
		else
		{
			static_assert( !sizeof( T ), "Bad type" );
		}
	}

	constexpr float byteswap( float data )
	{
		return std::bit_cast< float >( byteswap( std::bit_cast< uint32 >( data ) ) );
	}

	inline double byteswap( double data )
	{
		return std::bit_cast< double >( byteswap( std::bit_cast< uint64 >( data ) ) );
	}

	static_assert( sizeof( float )  == sizeof( uint32 ) );
	static_assert( sizeof( double ) == sizeof( uint64 ) );
}
