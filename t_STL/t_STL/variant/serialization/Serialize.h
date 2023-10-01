#pragma once

//#include "../Map.h"
#include "../variant.h"
#include "../../DynamicArray.h"
#include "../../endianness.h"

namespace t
{
	namespace variant
	{
		inline namespace bitstream_v1
		{
			template< endianness = endianness::native >
			DynamicArray< uint8_t > Serialize( Map const& map );

			extern template DynamicArray< uint8_t > Serialize< endianness::little >( Map const& );
			extern template DynamicArray< uint8_t > Serialize< endianness::big >( Map const& );
		}
	}
}
