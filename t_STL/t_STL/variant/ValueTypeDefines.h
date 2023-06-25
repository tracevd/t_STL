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
        std::is_same_v< T, Map > ||\
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
        std::is_same_v< T, Vector< Map > >

#define DefineType( T, type_ ) \
template<> \
struct TtoType< T > \
{ \
    static constexpr auto type = type_; \
};

#define TtoTypeDef \
\
template< typename T > \
struct TtoType \
{ \
    static constexpr auto type = VOID; \
}; \
\
DefineType( int8_t, INT8 ) \
DefineType( int16_t, INT16 ) \
DefineType( int32_t, INT32 ) \
DefineType( int64_t, INT64 ) \
DefineType( uint8_t, UINT8 ) \
DefineType( uint16_t, UINT16 ) \
DefineType( uint32_t, UINT32 ) \
DefineType( uint64_t, UINT64 ) \
DefineType( float, FLOAT ) \
DefineType( double, DOUBLE ) \
DefineType( String, STRING ) \
DefineType( Map, MAP ) \
DefineType( Vector< int8_t >, INT8_VECTOR ) \
DefineType( Vector< int16_t >, INT16_VECTOR ) \
DefineType( Vector< int32_t >, INT32_VECTOR ) \
DefineType( Vector< int64_t >, INT64_VECTOR ) \
DefineType( Vector< uint8_t >, UINT8_VECTOR ) \
DefineType( Vector< uint16_t >, UINT16_VECTOR ) \
DefineType( Vector< uint32_t >, UINT32_VECTOR ) \
DefineType( Vector< uint64_t >, UINT64_VECTOR ) \
DefineType( Vector< float >, FLOAT_VECTOR ) \
DefineType( Vector< double >, DOUBLE_VECTOR ) \
DefineType( Vector< String >, STRING_VECTOR ) \
DefineType( Vector< Map >, MAP_VECTOR )