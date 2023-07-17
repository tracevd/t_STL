#pragma once

#include "Tint.h"
#include "Type.h"

namespace t
{
	inline constexpr uint64 strlen( const char* str )
	{
		uint64 length = 0;

		while ( *( str + length ) )
			++length;

		return length;
	}

	inline constexpr void memcpy( void* destination, const void* source, uint64 numBytes )
	{
		auto dest = static_cast< uint8* >( destination );
		auto end = static_cast< const uint8* const>( dest ) + numBytes;
		auto src  = static_cast< const uint8* >( source );
		
		for ( ; dest < end; ++dest, ++src )
		{
			*dest = *src;
		}
	}
}
