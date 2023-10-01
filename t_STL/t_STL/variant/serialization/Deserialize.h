#pragma once

//#include "../Map.h"
#include "../variant.h"

namespace t
{
	namespace variant
	{
		Map Deserialize( DynamicArray< uint8 > const& buffer );

		Map Deserialize( const uint8_t* buffer, uint64 bufferSize );
	}
}
