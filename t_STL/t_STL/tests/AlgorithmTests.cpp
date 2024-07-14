#include "../Algorithm.h"
#include "../Array.h"

#include "TestAssert.h"

static constexpr bool testFind()
{
	{
		t::Array< int > arr{ 1, 2, 3, 4, 5 };

		auto it = t::find( arr, 3 );

		test_assert( it.get() == arr.data() + 2 );
	}

	{
		t::Array< int > arr{ 1, 2, 3, 4, 5 };

		auto it = t::find( arr, 6 );

		test_assert( it == arr.end() );
	}

	return true;
}

static constexpr auto find = testFind();

static constexpr auto testFindIf()
{
	{
		t::Array< int > arr{ 1, 2, 3, 4, 5 };

		auto it = t::findIf( arr, []( auto const val ){ return val > 2; } );

		test_assert( it.get() == arr.data() + 2 );
	}

	{
		t::Array< int > arr{ 1, 2, 3, 4, 5 };

		auto it = t::findIf( arr, []( auto const val ){ return val > 5; } );

		test_assert( it == arr.end() );
	}

	return true;
}

static constexpr auto findIf = testFindIf();

static consteval bool testPartition()
{
	t::Array< int > arr{ 5, 4, 9, 5, 6, 8 };

	t::partition( arr.begin(), arr.end(), []( int const val ){ return val <= 6; } );

	test_assert( ( arr == t::Array< int >{ 5, 4, 5, 6, 9, 8 } ) );
	
	return true;
}

static constexpr auto partition = testPartition();

static constexpr bool testPartitionByPivot()
{
	{
		t::Array< int > arr{ 4, 2, 1 };

		auto pivot = t::partitionByPivot( arr.begin(), arr.end(), arr.begin() + 1,
			[]( auto const elem, auto const pivot ) { return elem < pivot; } );

		test_assert(( arr == t::Array< int >{ 1, 2, 4 } ));
		test_assert( *pivot == 2 );
	}
	
	{
		t::Array< int > arr{ 4, 2 };

		auto pivot = t::partitionByPivot( arr.begin(), arr.end(), arr.begin(),
			[]( auto const elem, auto const pivot ) { return elem < pivot; } );

		test_assert(( arr == t::Array< int >{ 2, 4 } ));
		test_assert( *pivot == 4 );
	}

	{
		t::Array< int > arr{ 2, 4 };

		auto pivot = t::partitionByPivot( arr.begin(), arr.end(), arr.begin(),
			[]( auto const elem, auto const pivot ) { return elem < pivot; } );

		test_assert(( arr == t::Array< int >{ 2, 4 } ));
		test_assert( *pivot == 2 );
	}

	return true;
}

static constexpr auto partitionByPivot = testPartitionByPivot();

static consteval bool testTransform()
{
	t::Array< int32 > arr( 9 );

	for ( uint64 i = 0; i < arr.size(); ++i )
	{
		arr[ i ] = static_cast< int32 >( i );
	}

	t::Array< int32 > dest( 9 );

	t::transform( arr, dest, []( int32 val ){ return val; } );

	for ( uint64 i = 0; i < arr.size(); ++i )
	{
		test_assert( arr[ i ] == dest[ i ] );
	}

	return true;
}

static constexpr auto transform = testTransform();

static constexpr bool testQuickSort()
{
	{
		auto arr = t::Array< int >{ 5, 2, 1, 3, 7, 4, 9 };
		auto expected = t::Array< int >{ 1, 2, 3, 4, 5, 7, 9 };

		t::quickSort( arr );

		test_assert( arr == expected );
	}

	{
		auto arr = t::Array< int >{ 1, 8, 4, 3, 7, 4, 8, 4, 2 };
		auto expected = t::Array< int >{ 8, 8, 7, 4, 4, 4, 3, 2, 1 };

		t::quickSort( arr, []( auto lhs, auto rhs ){ return lhs > rhs; } );

		test_assert( arr == expected );
	}

	return true;
}

static constexpr bool quickSort = testQuickSort();

static constexpr bool testIsSorted()
{
	{
		auto arr = t::Array< int >{ 1, 2, 3, 4, 5, 6, 7 };
		test_assert( t::isSorted( arr ) );
	}

	{
		auto arr = t::Array< int >{ 5, 4, 3, 2, 1 };
		test_assert( t::isSorted( arr, []( auto lhs, auto rhs ){ return lhs >= rhs; } ) );
	}

	{
		auto arr = t::Array< int >{ 1, 2, 3, 4, 1 };
		test_assert( !t::isSorted( arr ) );
	}

	return true;
}

static constexpr bool isSorted = testIsSorted();