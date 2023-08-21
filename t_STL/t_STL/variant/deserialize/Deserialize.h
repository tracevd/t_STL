#pragma once

#include "../Map.h"

namespace t
{
	namespace variant
	{
		using BufferType = Vector< uint8 >;

		Map Deserialize( BufferType const& buffer );

		Map Deserialize( const uint8_t* buffer, uint64 bufferSize );
	}
}