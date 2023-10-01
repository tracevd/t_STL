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

#define DefineNormalAndList( template, T ) \
DefineTemplate( template, T ) \
DefineTemplate( template, DynamicArray< T > )

#define DefineAll( template ) \
DefineNormalAndList( template, int8 ) \
DefineNormalAndList( template, int16 ) \
DefineNormalAndList( template, int32 ) \
DefineNormalAndList( template, int64 ) \
DefineNormalAndList( template, uint8 ) \
DefineNormalAndList( template, uint16 ) \
DefineNormalAndList( template, uint32 ) \
DefineNormalAndList( template, uint64 ) \
DefineNormalAndList( template, float ) \
DefineNormalAndList( template, double ) \
DefineNormalAndList( template, String )
