#pragma once

//#include "../Map.h"
#include "../variant.h"
#include "../../Array.h"
#include "../../endianness.h"

namespace t
{
	namespace variant
	{
		inline namespace bitstream_v1
		{
			template< endianness = endianness::native >
			Array< uint8_t > Serialize( Map const& map );

			extern template Array< uint8_t > Serialize< endianness::little >( Map const& );
			extern template Array< uint8_t > Serialize< endianness::big >( Map const& );
		}
	}
}
