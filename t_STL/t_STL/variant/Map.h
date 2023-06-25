#pragma once
#include <unordered_map>
#include "variant.h"
#include "../String.h"

namespace t
{
	namespace v
	{
		inline namespace v2
		{
			class Map : public std::unordered_map< String, Value >
			{
			public:
				bool operator==( Map const& rhs ) const;
			};
		}
		namespace v1
		{
			class Map : public std::unordered_map< String, Value >
			{
			public:
				bool operator==( Map const& rhs ) const;
			};
		}
	}
}