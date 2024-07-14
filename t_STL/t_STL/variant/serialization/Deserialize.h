#pragma once

#include "../variant.h"

namespace t
{
	namespace variant
	{
		Map Deserialize( Array< uint8 > const& buffer );

		Map Deserialize( const uint8_t* buffer, uint64 bufferSize );
	}
}
