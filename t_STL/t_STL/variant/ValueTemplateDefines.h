#pragma once

#define DefineTemplate( T ) \
template<> Value::Value< T, void >( T const& ); \
template<> Value::Value< T, void >( T&& ); \
template<> Value& Value::operator=< T, void >( T const& ); \
template<> Value& Value::operator=< T, void >( T&& ); \
template<> T& Value::As< T&, void >(); \
template<> T const& Value::As< T const&, void >(); \
template<> T Value::As< T, void >();

#define DefineBasicTemplate( T ) \
template<> Value::Value< T, void >( T ); \
template<> Value& Value::operator=< T, void >( T ); \
template<> T& Value::As< T&, void >(); \
template<> T const& Value::As< T const&, void >(); \
template<> T Value::As< T, void >();

#define DefineNormalAndVector( T ) \
DefineTemplate( T ) \
DefineTemplate( Vector< T > )

#define DefineBasicNormalAndVector( T ) \
DefineBasicTemplate( T ) \
DefineTemplate( Vector< T > )

#define DefineAll \
DefineBasicNormalAndVector( int8_t ) \
DefineBasicNormalAndVector( int16_t ) \
DefineBasicNormalAndVector( int32_t ) \
DefineBasicNormalAndVector( int64_t ) \
DefineBasicNormalAndVector( uint8_t ) \
DefineBasicNormalAndVector( uint16_t ) \
DefineBasicNormalAndVector( uint32_t ) \
DefineBasicNormalAndVector( uint64_t ) \
DefineBasicNormalAndVector( float ) \
DefineBasicNormalAndVector( double ) \
DefineNormalAndVector( String ) \
DefineNormalAndVector( Map ) 