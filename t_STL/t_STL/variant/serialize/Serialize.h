#pragma once

#include "../Map.h"
#include "../../Vector.h"

namespace t
{
	namespace variant
	{
		using BufferType = Vector< uint8 >;

		inline namespace bitstream_v1
		{
			Vector< uint8_t > Serialize( Map const& map );
		}
	}
}
