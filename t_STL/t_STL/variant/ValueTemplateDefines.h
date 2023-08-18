#pragma once

#ifdef _WIN32
#define DefineTemplate( template, T ) \
	template Value& Value::operator=< T >( T ); \
	template T& Value::As< T&, void >(); \
	template T const& Value::As< T const&, void >() const; \
	template T Value::As< T, void >() const;
#else
#define DefineTemplate( template, T ) \
	template Value::Value< T, void >( T ); \
	template Value& Value::operator=< T >( T ); \
	template T& Value::As< T&, void >(); \
	template T const& Value::As< T const&, void >() const; \
	template T Value::As< T, void >() const;
#endif

#define DefineNormalAndVector( template, T ) \
DefineTemplate( template, T ) \
DefineTemplate( template, Vector< T > )

#define DefineAll( template ) \
DefineNormalAndVector( template, int8 ) \
DefineNormalAndVector( template, int16 ) \
DefineNormalAndVector( template, int32 ) \
DefineNormalAndVector( template, int64 ) \
DefineNormalAndVector( template, uint8 ) \
DefineNormalAndVector( template, uint16 ) \
DefineNormalAndVector( template, uint32 ) \
DefineNormalAndVector( template, uint64 ) \
DefineNormalAndVector( template, float ) \
DefineNormalAndVector( template, double ) \
DefineNormalAndVector( template, String )
