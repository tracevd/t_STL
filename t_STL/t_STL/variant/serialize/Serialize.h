#pragma once

#include <bit>

#include "../Map.h"
#include "../../List.h"

namespace t
{
	namespace variant
	{
		using BufferType = List< uint8 >;

		inline namespace bitstream_v1
		{
			template< std::endian endianness = std::endian::native >
			List< uint8_t > Serialize( Map const& map );

			extern template List< uint8_t > Serialize< std::endian::big >( Map const& );
			extern template List< uint8_t > Serialize< std::endian::little >( Map const& );
		}
	}
}
