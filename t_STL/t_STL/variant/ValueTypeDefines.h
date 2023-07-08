#pragma once

#define ALLOWED_TYPES( T ) \
        std::is_same_v< T, String > ||\
        std::is_same_v< T, int8_t > ||\
        std::is_same_v< T, int16_t > ||\
        std::is_same_v< T, int32_t > ||\
        std::is_same_v< T, int64_t > ||\
        std::is_same_v< T, uint8_t > ||\
        std::is_same_v< T, uint16_t > ||\
        std::is_same_v< T, uint32_t > ||\
        std::is_same_v< T, uint64_t > ||\
        std::is_same_v< T, double > ||\
        std::is_same_v< T, float > ||\
        std::is_same_v< T, t::v::Map > ||\
        std::is_same_v< T, Vector< int8_t > > ||\
        std::is_same_v< T, Vector< int16_t > > ||\
        std::is_same_v< T, Vector< int32_t > > ||\
        std::is_same_v< T, Vector< int64_t> > ||\
        std::is_same_v< T, Vector< uint8_t> > ||\
        std::is_same_v< T, Vector< uint16_t> > ||\
        std::is_same_v< T, Vector< uint32_t > > ||\
        std::is_same_v< T, Vector< uint64_t > > ||\
        std::is_same_v< T, Vector< float > > ||\
        std::is_same_v< T, Vector< double > > ||\
        std::is_same_v< T, Vector< String > > ||\
        std::is_same_v< T, Vector< t::v::Map > >

#define DefineType( T, type_, str ) \
template<> \
struct TtoType< T > \
{ \
    static constexpr t::v::Type type          = type_; \
    static constexpr const char* string = str; \
};

#define TtoTypeDef \
\
template< typename T > \
struct TtoType \
{ \
    static constexpr t::v::Type type          = VOID; \
    static constexpr const char* string = "void"; \
}; \
\
DefineType( int8_t, INT8, "i8" ) \
DefineType( int16_t, INT16, "i16" ) \
DefineType( int32_t, INT32, "i32" ) \
DefineType( int64_t, INT64, "i64" ) \
DefineType( uint8_t, UINT8, "u8" ) \
DefineType( uint16_t, UINT16, "u16" ) \
DefineType( uint32_t, UINT32, "u32" ) \
DefineType( uint64_t, UINT64, "u64" ) \
DefineType( float, FLOAT, "float" ) \
DefineType( double, DOUBLE, "double" ) \
DefineType( String, STRING, "String" ) \
DefineType( Map, MAP, "Map" ) \
DefineType( Vector< int8_t >, INT8_VECTOR, "i8[]" ) \
DefineType( Vector< int16_t >, INT16_VECTOR, "i16[]" ) \
DefineType( Vector< int32_t >, INT32_VECTOR, "i32[]" ) \
DefineType( Vector< int64_t >, INT64_VECTOR, "i64[]" ) \
DefineType( Vector< uint8_t >, UINT8_VECTOR, "u8[]" ) \
DefineType( Vector< uint16_t >, UINT16_VECTOR, "u16[]" ) \
DefineType( Vector< uint32_t >, UINT32_VECTOR, "u32[]" ) \
DefineType( Vector< uint64_t >, UINT64_VECTOR, "u64[]" ) \
DefineType( Vector< float >, FLOAT_VECTOR, "float[]" ) \
DefineType( Vector< double >, DOUBLE_VECTOR, "double[]" ) \
DefineType( Vector< String >, STRING_VECTOR, "String[]" ) \
DefineType( Vector< Map >, MAP_VECTOR, "Map[]" )