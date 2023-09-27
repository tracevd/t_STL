#pragma once

#include "../Map.h"
#include "../../List.h"
#include "../../endianness.h"

namespace t
{
	namespace variant
	{
		using BufferType = List< uint8 >;

		inline namespace bitstream_v1
		{
			template< endianness = endianness::native >
			List< uint8_t > Serialize( Map const& map );

			extern template List< uint8_t > Serialize< endianness::little >( Map const& );
			extern template List< uint8_t > Serialize< endianness::big >( Map const& );
		}
	}
}
