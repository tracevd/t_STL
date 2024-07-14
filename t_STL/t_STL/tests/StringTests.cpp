#include "../String.h"

#include "TestAssert.h"

#include "../StaticArray.h"

static constexpr int testConstructor()
{
	{
		auto const str = t::String();

		test_assert( str.size() == 0 );
	}
	
	{
		auto const str = t::String("hello");

		test_assert( str.size() == 5 );
	}

	{
		auto const strv = t::StringView("hello");
		auto const str = t::String( strv );

		test_assert( str.size() == 5 );
	}

	{
		auto const str = t::String( 12345 );

		test_assert( str.size() == 5 );
		test_assert( str == "12345" );
	}

	return 0;
}

auto constexpr construct = testConstructor();

static constexpr int testAdding()
{
	{
		auto str = t::String("hello");

		auto str2 = str + " wow";

		test_assert( str2.size() == 9 );
		test_assert( str2 == "hello wow" );
	}

	{
		auto str = t::String("hello");

		str += " wow";

		test_assert( str.size() == 9 );
		test_assert( str == "hello wow" );
	}

	{
		auto str = t::String("hello");
		auto str2 = t::String(" wow");

		auto const str3 = str + str2;

		test_assert( str3.size() == 9 );
		test_assert( str3 == "hello wow" );
	}

	return 0;
}

static constexpr auto adding = testAdding();

static constexpr int testSplit()
{
	{
		auto const str = t::String(" hello this is a sentence ");

		auto const split = str.split(' ');

		test_assert( split.size() == 7 );

		t::StaticArray< t::String, 7 > expected = { t::String(), t::String("hello"), t::String("this"), t::String("is"), t::String("a"), t::String("sentence"), t::String()};

		for ( uint64 i = 0; i < split.size(); ++i )
		{
			test_assert( expected[ i ] == split[ i ] );
		}
	}

	return 0;
}

static constexpr int split = testSplit();

static constexpr int testReplace()
{
	{
		auto str = t::String("123_123_123");

		str.replace('1', '9');

		test_assert( str.size() == 11 );
		test_assert( str == "923_923_923" );
	}
	return 0;
}

static constexpr int replace = testReplace();

static constexpr int testReplaceFirst()
{
	{
		auto str = t::String( "123_123_123" );

		str.replaceFirst( '1', '9' );

		test_assert( str.size() == 11 );
		test_assert( str == "923_123_123" );
	}

	return 0;
}

static constexpr auto replaceFirst = testReplaceFirst();

static constexpr int testReplaceLast()
{
	{
		auto str = t::String( "123_123_123" );

		str.replaceLast( '1', '9' );

		test_assert( str.size() == 11 );
		test_assert( str == "123_123_923" );
	}

	return 0;
}

static constexpr auto replaceLast = testReplaceLast();

static constexpr int testIndexOf()
{
	auto str = t::String( "hello123" );

	auto const indexOfH = str.indexOf('h');

	test_assert( indexOfH == 0 );

	auto const indexOfL = str.indexOf('l');

	test_assert( indexOfL == 2 );

	auto const indexOf3 = str.indexOf('3');

	test_assert( indexOf3 == 7 );

	return 0;
}

static constexpr auto indexOf = testIndexOf();

static constexpr int testLastIndexOf()
{
	auto str = t::String( "hello123" );

	auto const indexOfH = str.lastIndexOf( 'h' );

	test_assert( indexOfH == 0 );

	auto const indexOfL = str.lastIndexOf( 'l' );

	test_assert( indexOfL == 3 );

	auto const indexOf3 = str.lastIndexOf( '3' );

	test_assert( indexOf3 == 7 );

	return 0;
}

static constexpr auto lastIndexOf = testLastIndexOf();