#pragma once

#define ALLOWED_TYPES( T ) \
        std::is_same_v< T, String > ||\
        std::is_same_v< T, int8 > ||\
        std::is_same_v< T, int16 > ||\
        std::is_same_v< T, int32 > ||\
        std::is_same_v< T, int64 > ||\
        std::is_same_v< T, uint8 > ||\
        std::is_same_v< T, uint16 > ||\
        std::is_same_v< T, uint32 > ||\
        std::is_same_v< T, uint64 > ||\
        std::is_same_v< T, double > ||\
        std::is_same_v< T, float > ||\
        std::is_same_v< T, t::variant::Map > ||\
        std::is_same_v< T, Vector< int8 > > ||\
        std::is_same_v< T, Vector< int16 > > ||\
        std::is_same_v< T, Vector< int32 > > ||\
        std::is_same_v< T, Vector< int64 > > ||\
        std::is_same_v< T, Vector< uint8 > > ||\
        std::is_same_v< T, Vector< uint16 > > ||\
        std::is_same_v< T, Vector< uint32 > > ||\
        std::is_same_v< T, Vector< uint64 > > ||\
        std::is_same_v< T, Vector< float > > ||\
        std::is_same_v< T, Vector< double > > ||\
        std::is_same_v< T, Vector< String > > ||\
        std::is_same_v< T, Vector< t::variant::Map > >

#define DefineType( T, type_, str ) \
template<> \
struct TtoType< T > \
{ \
    static constexpr t::variant::Type type = type_; \
    static constexpr const char* string    = str; \
}; \
template<> \
struct TypeToT< type_ > \
{ \
    using type = T; \
};

#define TtoTypeDef \
template< typename U > \
struct TtoType \
{ \
    static constexpr t::variant::Type type = VOID; \
    static constexpr const char* string    = "void"; \
}; \
template< Type t > \
struct TypeToT; \
\
DefineType( int8, INT8, "i8" ) \
DefineType( int16, INT16, "i16" ) \
DefineType( int32, INT32, "i32" ) \
DefineType( int64, INT64, "i64" ) \
DefineType( uint8, UINT8, "u8" ) \
DefineType( uint16, UINT16, "u16" ) \
DefineType( uint32, UINT32, "u32" ) \
DefineType( uint64, UINT64, "u64" ) \
DefineType( float, FLOAT, "float" ) \
DefineType( double, DOUBLE, "double" ) \
DefineType( String, STRING, "String" ) \
DefineType( ::t::variant::Map, MAP, "Map" ) \
DefineType( Vector< int8 >, INT8_VECTOR, "i8[]" ) \
DefineType( Vector< int16 >, INT16_VECTOR, "i16[]" ) \
DefineType( Vector< int32 >, INT32_VECTOR, "i32[]" ) \
DefineType( Vector< int64 >, INT64_VECTOR, "i64[]" ) \
DefineType( Vector< uint8 >, UINT8_VECTOR, "u8[]" ) \
DefineType( Vector< uint16 >, UINT16_VECTOR, "u16[]" ) \
DefineType( Vector< uint32 >, UINT32_VECTOR, "u32[]" ) \
DefineType( Vector< uint64 >, UINT64_VECTOR, "u64[]" ) \
DefineType( Vector< float >, FLOAT_VECTOR, "float[]" ) \
DefineType( Vector< double >, DOUBLE_VECTOR, "double[]" ) \
DefineType( Vector< String >, STRING_VECTOR, "String[]" ) \
DefineType( Vector< ::t::variant::Map >, MAP_VECTOR, "Map[]" )