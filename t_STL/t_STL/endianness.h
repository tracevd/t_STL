#pragma once

#include <bit>

#include "Tint.h"

namespace t
{
	enum class endianness : uint8
	{
		little = 0,
		big    = 1,
		native = std::endian::native == std::endian::little ? little : big
	};

	constexpr uint8 byteswap( uint8 data )
	{
		return data;
	}

	constexpr uint16 byteswap( uint16 data )
	{
		return ((data >> 8) & 0xff) | ((data & 0xff) << 8);
	}

	constexpr uint32 byteswap( uint32 data )
	{
		return ((data >> 24) & 0xff) | (((data >> 16) & 0xff) << 8) | (((data >> 8) & 0xff) << 16) | ((data & 0xff) << 24);
	}

	constexpr uint64 byteswap( uint64 data )
	{
		return ((data >> 56) & 0xff) | (((data >> 48) & 0xff) << 8) | (((data >> 40) & 0xff) << 16) | (((data >> 32) & 0xff) << 24) |
			(((data >> 24) & 0xff) << 32) | (((data >> 16) & 0xff) << 40) | (((data >> 8) & 0xff) << 48) | ((data & 0xff) << 56);
	}

	constexpr int8 byteswap( int8 data )
	{
		return data;
	}

	constexpr int16 byteswap( int16 data )
	{
		return (data >> 8 & 0xff) | ((data & 0xff) << 8);
	}

	constexpr int32 byteswap( int32 data )
	{
		return (data >> 24 & 0xff) | (data >> 16 & 0xff) << 8 | (data >> 8 & 0xff) << 16 | (data & 0xff) << 24;
	}

	constexpr int64 byteswap( int64 data )
	{
		return (data >> 56 & 0xff) | (data >> 48 & 0xff) << 8 | (data >> 40 & 0xff) << 16 | (data >> 32 & 0xff) << 24 |
			(data >> 24 & 0xff) << 32 | (data >> 16 & 0xff) << 40 | (data >> 8 & 0xff) << 48 | (data & 0xff) << 56;
	}

	inline float byteswap( float data )
	{
		float result;
		constexpr auto n = sizeof( float );
		unsigned char* d = (uint8*) &data;
		for ( uint64 i = 0; i < n; ++i )
			( (uint8*) &result )[ i ] = d[ n - 1 - i ];
		return result;
	}

	inline double byteswap( double data )
	{
		double result;
		constexpr auto n = sizeof( double );
		unsigned char* d = (unsigned char*) &data;
		for ( uint64 i = 0; i < n; ++i )
			( (uint8*) &result )[ i ] = d[ n - 1 - i ];
		return result;
	}

	static_assert( sizeof( float ) == sizeof( uint32 ) );
	static_assert( sizeof( double ) == sizeof( uint64 ) );
}
