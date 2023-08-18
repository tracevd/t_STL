#pragma once

#include "Type.h"
#include "Tint.h"

namespace t
{
	template< class T, class U >
	struct pair
	{
		T first {};
		U second {};

		template< uint64 N >
		constexpr auto get()
		{
			static_assert( N < 2, "Pairs only have 2 elements!" );

			if constexpr ( N == 0 )
				return first;
			else
				return second;
		}
	};
}