#pragma once

#include "../Map.h"
#include "../../Vector.h"

namespace t
{
	namespace v
	{
		Vector< uint8_t > Serialize( Map const& map );
		Map Deserialize( Vector< uint8_t > const& buffer );
	}
}
