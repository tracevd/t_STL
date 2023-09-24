#pragma once

#include "../Map.h"
#include "../../List.h"

namespace t
{
	namespace variant
	{
		using BufferType = List< uint8 >;

		inline namespace bitstream_v1
		{
			List< uint8_t > Serialize( Map const& map );
		}
	}
}
