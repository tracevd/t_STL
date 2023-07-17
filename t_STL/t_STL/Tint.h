#pragma once

using int8   = char;
using int16  = short;
using int32  = int;
using int64  = long long;
using uint8  = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

inline constexpr int8   int8_MAX   =                        127i8;
inline constexpr int8   int8_MIN   =                       -128i8;
inline constexpr int16  int16_MAX  =                     32'767i16;
inline constexpr int16  int16_MIN  =                    -32'768i16;
inline constexpr int32  int32_MAX  =              2'147'483'647i32;
inline constexpr int32  int32_MIN  =             -2'147'483'648i32;
inline constexpr int64  int64_MAX  =  9'223'372'036'854'775'807i64;
inline constexpr int64  int64_MIN  = -9'223'372'036'854'775'808i64;
inline constexpr uint8  uint8_MAX  =                        256ui8;
inline constexpr uint8  uint8_MIN  =                          0ui8;
inline constexpr uint16 uint16_MAX =                     65'535ui16;
inline constexpr uint16 uint16_MIN =                          0ui16;
inline constexpr uint32 uint32_MAX =              4'294'967'295ui32;
inline constexpr uint32 uint32_MIN =                          0ui32;
inline constexpr uint64 uint64_MAX = 18'446'744'073'709'551'615ui64;

static_assert( sizeof( int8 )   == 1, "Invalid type size!" );
static_assert( sizeof( int16 )  == 2, "Invalid type size!");
static_assert( sizeof( int32 )  == 4, "Invalid type size!");
static_assert( sizeof( int64 )  == 8, "Invalid type size!");
static_assert (sizeof( uint8 )  == 1, "Invalid type size!");
static_assert (sizeof( uint16 ) == 2, "Invalid type size!");
static_assert (sizeof( uint32 ) == 4, "Invalid type size!");
static_assert (sizeof( uint64 ) == 8, "Invalid type size!");
