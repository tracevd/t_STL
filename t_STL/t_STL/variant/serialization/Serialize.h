#pragma once

#include "../Map.h"
#include "../../Buffer.h"
#include "../../endianness.h"

namespace t
{
	namespace variant
	{
		using BufferType = Buffer< uint8 >;

		inline namespace bitstream_v1
		{
			template< endianness = endianness::native >
			Buffer< uint8_t > Serialize( Map const& map );

			extern template Buffer< uint8_t > Serialize< endianness::little >( Map const& );
			extern template Buffer< uint8_t > Serialize< endianness::big >( Map const& );
		}
	}
}
