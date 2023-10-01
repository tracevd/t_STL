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
        std::is_same_v< T, DynamicArray< int8 > > ||\
        std::is_same_v< T, DynamicArray< int16 > > ||\
        std::is_same_v< T, DynamicArray< int32 > > ||\
        std::is_same_v< T, DynamicArray< int64 > > ||\
        std::is_same_v< T, DynamicArray< uint8 > > ||\
        std::is_same_v< T, DynamicArray< uint16 > > ||\
        std::is_same_v< T, DynamicArray< uint32 > > ||\
        std::is_same_v< T, DynamicArray< uint64 > > ||\
        std::is_same_v< T, DynamicArray< float > > ||\
        std::is_same_v< T, DynamicArray< double > > ||\
        std::is_same_v< T, DynamicArray< String > > ||\
        std::is_same_v< T, DynamicArray< t::variant::Map > >

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
DefineType( DynamicArray< int8 >, INT8_LIST, "i8[]" ) \
DefineType( DynamicArray< int16 >, INT16_LIST, "i16[]" ) \
DefineType( DynamicArray< int32 >, INT32_LIST, "i32[]" ) \
DefineType( DynamicArray< int64 >, INT64_LIST, "i64[]" ) \
DefineType( DynamicArray< uint8 >, UINT8_LIST, "u8[]" ) \
DefineType( DynamicArray< uint16 >, UINT16_LIST, "u16[]" ) \
DefineType( DynamicArray< uint32 >, UINT32_LIST, "u32[]" ) \
DefineType( DynamicArray< uint64 >, UINT64_LIST, "u64[]" ) \
DefineType( DynamicArray< float >, FLOAT_LIST, "float[]" ) \
DefineType( DynamicArray< double >, DOUBLE_LIST, "double[]" ) \
DefineType( DynamicArray< String >, STRING_LIST, "String[]" )
