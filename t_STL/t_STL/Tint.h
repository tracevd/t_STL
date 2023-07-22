#pragma once

using int8   = signed char;
using int16  = signed short;
using int32  = signed int;
using int64  = signed long long;
using uint8  = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

#include <numeric>

namespace t
{
	template< class T >
	struct limit {};

	template<>
	struct limit< int8 >
	{
		using type = int8;
		static constexpr inline type max =   127;
		static constexpr inline type min = (-128);
		static constexpr bool isSigned = true;
	};

	template<>
	struct limit< int16 >
	{
		using type = int16;
		static constexpr type max =   32'767;
		static constexpr type min = (-32'768);
		static constexpr bool isSigned = true;
	};

	template<>
	struct limit< int32 >
	{
		using type = int32;
		static constexpr type max =  2'147'483'647;
		static constexpr type min = -2'147'483'648;
		static constexpr bool isSigned = true;
	};

	template<>
	struct limit< int64 >
	{
		using type = int64;
		static constexpr type max =   9'223'372'036'854'775'807;
		static constexpr type min = (-9'223'372'036'854'775'807 - 1);
		static constexpr bool isSigned = true;
	};

	template<>
	struct limit< uint8 >
	{
		using type = uint8;
		static constexpr type max = 255;
		static constexpr type min =   0;
		static constexpr bool isSigned = false;
	};

	template<>
	struct limit< uint16 >
	{
		using type = uint16;
		static constexpr type max = 65'535;
		static constexpr type min =      0;
		static constexpr bool isSigned = false;
	};

	template<>
	struct limit< uint32 >
	{
		using type = uint32;
		static constexpr type max = 4'294'967'295;
		static constexpr type min =             0;
		static constexpr bool isSigned = false;
	};

	template<>
	struct limit< uint64 >
	{
		using type = uint64;
		static constexpr type max = 18'446'744'073'709'551'615u;
		static constexpr type min =                          0;
		static constexpr bool isSigned = false;
	};
}

static_assert( sizeof( int8 )   == 1, "Invalid type size!" );
static_assert( sizeof( int16 )  == 2, "Invalid type size!");
static_assert( sizeof( int32 )  == 4, "Invalid type size!");
static_assert( sizeof( int64 )  == 8, "Invalid type size!");
static_assert( sizeof( uint8 )  == 1, "Invalid type size!");
static_assert( sizeof( uint16 ) == 2, "Invalid type size!");
static_assert( sizeof( uint32 ) == 4, "Invalid type size!");
static_assert( sizeof( uint64 ) == 8, "Invalid type size!");
