#pragma once

//#include <stdexcept>

#include "../Error.h"

namespace t::test
{
	class Error : public t::Error
	{
	public:
		Error( const char* message ):
			t::Error( message ) {}
	};

#define stringify_detail( thing ) \
	#thing
#define stringify( thing ) \
	stringify_detail( thing )

#define test_assert( cond ) \
	if ( !( cond ) ) throw t::test::Error("Bad assert: " __FILE__ " on line " stringify(__LINE__))
}