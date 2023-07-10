#pragma once

#include "../Map.h"
#include "../../Vector.h"

namespace t
{
	namespace variant
	{
		using BufferType = Vector< uint8_t >;

		inline namespace bitstream_v1
		{
			Vector< uint8_t > Serialize( Map const& map );
		}
		
		Map Deserialize( BufferType const& buffer );

		Map Deserialize( const uint8_t* buffer, size_t bufferSize );
	}
}
