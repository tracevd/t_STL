#pragma once

#include "Tint.h"
#include "Type.h"

namespace t
{
	template< class CharTy >
	inline constexpr uint64 strlen( const CharTy* str )
	{
		uint64 length = 0;

		while ( *( str + length ) )
			++length;

		return length;
	}

	template< class T, class U >
	inline constexpr void memcpy( T* destination, const U* source, uint64 numel )
	{
		const T* const end = destination + numel;
		
		for ( ; destination < end; ++destination, ++source )
		{
			*destination = *source;
		}
	}

	template< class CharTy >
	inline constexpr void strcpy( CharTy* destination, const CharTy* source, uint64 length )
	{
		const CharTy* const end = destination + length;

		for ( ; destination != end; ++destination, ++source )
		{
			*destination = *source;
		}
	}
}
