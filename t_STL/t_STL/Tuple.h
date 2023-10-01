#pragma once

#include <tuple>

#include "Type.h"
#include "Tint.h"
#include "Pair.h"

namespace t
{
	template< class... Types >
	using tuple = std::tuple< Types... >;

	template< class... Types >
	constexpr auto make_tuple( Types&&... args )
	{
		return tuple< Types... >( std::forward< Types >( args )... );
	}

	template< uint64 N, class... Types >
	constexpr auto get( tuple< Types... > const& tup )
	{
		return std::get< N >( tup );
	}
}

static_assert( sizeof( t::tuple< uint32, uint32, uint32 > ) == ( sizeof( uint32 ) * 3 ), "Tuple was not correct size!" );
