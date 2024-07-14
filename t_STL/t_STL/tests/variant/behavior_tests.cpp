#include "../../variant/variant.h"

#include "../TestAssert.h"

void test()
{
	using namespace t::variant;
	using t::String;
	using t::StringView;

	auto map = Map({
		{ "string", Value("hello") },
		{ "uint32", Value(uint32(123)) }
	});

	auto const str = map.at( "string" ).As< String >();

	using namespace t::string_literals;

	test_assert( str == "hello"_sv );
	
	auto const uint = map.at("uint32").As< uint32 >();

	test_assert( uint == 123 );
}
