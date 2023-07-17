#pragma once

#include "Tint.h"
#include "Type.h"

namespace t
{
	inline constexpr uint64 strlen( const char* str )
	{
		uint64 length = 0;

		while( *(str++) ) ++length;

		return length;
	}

	inline constexpr void memcpy( void* destination, const void* source, uint64 numBytes )
	{
		auto dest = static_cast< char* >( destination );
		auto end = static_cast<const char* const>(dest) + numBytes;
		auto src  = static_cast< const char* >( source );
		
		for ( ; dest < end; ++dest, ++src )
		{
			*dest = *src;
		}
	}
}
