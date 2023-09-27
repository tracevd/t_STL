#pragma once

#include "Tint.h"

namespace t
{
	constexpr uint8 byteswap( uint8 data )
	{
		return data;
	}

	constexpr uint16 byteswap( uint16 data )
	{
		return (data >> 8 & 0xff) | (data & 0xff) << 8;
	}

	constexpr uint32 byteswap( uint32 data )
	{
		return (data >> 24 & 0xff) | (data >> 16 & 0xff) << 8 | (data >> 8 & 0xff) << 16 | (data & 0xff) << 24;
	}

	constexpr uint64 byteswap( uint64 data )
	{
		return (data >> 56 & 0xff) | (data >> 48 & 0xff) << 8 | (data >> 40 & 0xff) << 16 | (data >> 32 & 0xff) << 24 |
			(data >> 24 & 0xff) << 32 | (data >> 16 & 0xff) << 40 | (data >> 8 & 0xff) << 48 | (data & 0xff) << 56;
	}

	constexpr int8 byteswap( int8 data )
	{
		return data;
	}

	constexpr int16 byteswap( int16 data )
	{
		return (data >> 8 & 0xff) | (data & 0xff) << 8;
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
		auto dataInt = byteswap( *reinterpret_cast< uint32* >( static_cast< void* >( &data ) ) );
		return *reinterpret_cast< float* >( &dataInt );
	}

	inline double byteswap( double data )
	{
		auto dataInt = byteswap( *reinterpret_cast< uint64* >( static_cast< void* >( &data ) ) );
		return *reinterpret_cast< double* >( &dataInt );
	}

	static_assert( sizeof( float ) == sizeof( uint32 ) );
	static_assert( sizeof( double ) == sizeof( uint64 ) );
}
