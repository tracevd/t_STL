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
        std::is_same_v< T, Array< int8 > > ||\
        std::is_same_v< T, Array< int16 > > ||\
        std::is_same_v< T, Array< int32 > > ||\
        std::is_same_v< T, Array< int64 > > ||\
        std::is_same_v< T, Array< uint8 > > ||\
        std::is_same_v< T, Array< uint16 > > ||\
        std::is_same_v< T, Array< uint32 > > ||\
        std::is_same_v< T, Array< uint64 > > ||\
        std::is_same_v< T, Array< float > > ||\
        std::is_same_v< T, Array< double > > ||\
        std::is_same_v< T, Array< String > > ||\
        std::is_same_v< T, Array< t::variant::Map > >

#define DefineType( T, type_, str ) \
template<> \
struct TtoType< T > \
{ \
    static constexpr ::t::variant::Type type = type_; \
    static constexpr StringView string  = str; \
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
    static constexpr t::variant::Type type = Type::VOID; \
    static constexpr auto string = StringView("void"); \
}; \
template< Type t > \
struct TypeToT; \
\
DefineType( int8, Type::INT8, "i8" ) \
DefineType( int16, Type::INT16, "i16" ) \
DefineType( int32, Type::INT32, "i32" ) \
DefineType( int64, Type::INT64, "i64" ) \
DefineType( uint8, Type::UINT8, "u8" ) \
DefineType( uint16, Type::UINT16, "u16" ) \
DefineType( uint32, Type::UINT32, "u32" ) \
DefineType( uint64, Type::UINT64, "u64" ) \
DefineType( float, Type::FLOAT, "float" ) \
DefineType( double, Type::DOUBLE, "double" ) \
DefineType( String, Type::STRING, "String" ) \
DefineType( ::t::variant::Map, Type::MAP, "Map" ) \
DefineType( Array< int8 >, Type::INT8_ARRAY, "i8[]" ) \
DefineType( Array< int16 >, Type::INT16_ARRAY, "i16[]" ) \
DefineType( Array< int32 >, Type::INT32_ARRAY, "i32[]" ) \
DefineType( Array< int64 >, Type::INT64_ARRAY, "i64[]" ) \
DefineType( Array< uint8 >, Type::UINT8_ARRAY, "u8[]" ) \
DefineType( Array< uint16 >, Type::UINT16_ARRAY, "u16[]" ) \
DefineType( Array< uint32 >, Type::UINT32_ARRAY, "u32[]" ) \
DefineType( Array< uint64 >, Type::UINT64_ARRAY, "u64[]" ) \
DefineType( Array< float >, Type::FLOAT_ARRAY, "float[]" ) \
DefineType( Array< double >, Type::DOUBLE_ARRAY, "double[]" ) \
DefineType( Array< String >, Type::STRING_ARRAY, "String[]" )
