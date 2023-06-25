#pragma once

#include <type_traits>
#include <exception>

namespace t
{
    class String;
    template< typename T >
    class Vector;

    namespace v
    {
        inline namespace v2
        {
            class Map;
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

            enum Type : uint8_t
            {
                VOID,
                INT8, INT16, INT32, INT64,
                UINT8, UINT16, UINT32, UINT64,
                FLOAT, DOUBLE,
                STRING,
                MAP,
                VECTOR = 0x10,
                INT8_VECTOR = INT8 | VECTOR,
                INT16_VECTOR = INT16 | VECTOR,
                INT32_VECTOR = INT32 | VECTOR,
                INT64_VECTOR = INT64 | VECTOR,
                UINT8_VECTOR = UINT8 | VECTOR,
                UINT16_VECTOR = UINT16 | VECTOR,
                UINT32_VECTOR = UINT32 | VECTOR,
                UINT64_VECTOR = UINT64 | VECTOR,
                FLOAT_VECTOR = FLOAT | VECTOR,
                DOUBLE_VECTOR = DOUBLE | VECTOR,
                STRING_VECTOR = STRING | VECTOR,
                MAP_VECTOR = MAP | VECTOR,
            };

            template< typename T >
            constexpr Type templateToVariantType()
            {
                if constexpr ( std::is_same_v< T, int8_t > )
                    return Type::INT8;
                if constexpr ( std::is_same_v< T, int16_t > )
                    return Type::INT16;
                if constexpr ( std::is_same_v< T, int32_t > )
                    return Type::INT32;
                if constexpr ( std::is_same_v< T, int64_t > )
                    return Type::INT64;
                if constexpr ( std::is_same_v< T, uint8_t > )
                    return Type::UINT8;
                if constexpr ( std::is_same_v< T, uint16_t > )
                    return Type::UINT16;
                if constexpr ( std::is_same_v< T, uint32_t > )
                    return Type::UINT32;
                if constexpr ( std::is_same_v< T, uint64_t > )
                    return Type::UINT64;
                if constexpr ( std::is_same_v< T, float> )
                    return Type::FLOAT;
                if constexpr ( std::is_same_v< T, double> )
                    return Type::DOUBLE;
                if constexpr ( std::is_same_v< T, String> )
                    return Type::STRING;
                if constexpr ( std::is_same_v< T, Map > )
                    return Type::MAP;
                if constexpr ( std::is_same_v< T, Vector< int8_t > > )
                    return Type::INT8_VECTOR;
                if constexpr ( std::is_same_v< T, Vector< int16_t > > )
                    return Type::INT16_VECTOR;
                if constexpr ( std::is_same_v< T, Vector< int32_t > > )
                    return Type::INT32_VECTOR;
                if constexpr ( std::is_same_v< T, Vector< int64_t > > )
                    return Type::INT64_VECTOR;
                if constexpr ( std::is_same_v< T, Vector< uint8_t > > )
                    return Type::UINT8_VECTOR;
                if constexpr ( std::is_same_v< T, Vector< uint16_t > > )
                    return Type::UINT16_VECTOR;
                if constexpr ( std::is_same_v< T, Vector< uint32_t > > )
                    return Type::UINT32_VECTOR;
                if constexpr ( std::is_same_v< T, Vector< uint64_t > > )
                    return Type::UINT64_VECTOR;
                if constexpr ( std::is_same_v< T, Vector< String > > )
                    return Type::STRING_VECTOR;
                if constexpr ( std::is_same_v< T, Vector< Map > > )
                    return Type::MAP_VECTOR;
                return Type::VOID;
            }

            template< typename T >
            constexpr bool checkValidType()
            {
                constexpr auto type = templateToVariantType< T >();
                return type != VOID;
            }
        }
        namespace v1
        {
            class Map;

            enum Type : uint8_t
            {
                VOID,
                INT8, INT16, INT32, INT64,
                UINT8, UINT16, UINT32, UINT64,
                FLOAT, DOUBLE,
                STRING,
                MAP,
                VECTOR = 0x10,
                INT8_VECTOR = INT8 | VECTOR,
                INT16_VECTOR = INT16 | VECTOR,
                INT32_VECTOR = INT32 | VECTOR,
                INT64_VECTOR = INT64 | VECTOR,
                UINT8_VECTOR = UINT8 | VECTOR,
                UINT16_VECTOR = UINT16 | VECTOR,
                UINT32_VECTOR = UINT32 | VECTOR,
                UINT64_VECTOR = UINT64 | VECTOR,
                FLOAT_VECTOR = FLOAT | VECTOR,
                DOUBLE_VECTOR = DOUBLE | VECTOR,
                STRING_VECTOR = STRING | VECTOR,
                MAP_VECTOR = MAP | VECTOR,
            };

            template< typename T >
            constexpr Type templateToVariantType()
            {
                if constexpr ( std::is_same_v< T, int8_t > )
                    return Type::INT8;
                if constexpr ( std::is_same_v< T, int16_t > )
                    return Type::INT16;
                if constexpr ( std::is_same_v< T, int32_t > )
                    return Type::INT32;
                if constexpr ( std::is_same_v< T, int64_t > )
                    return Type::INT64;
                if constexpr ( std::is_same_v< T, uint8_t > )
                    return Type::UINT8;
                if constexpr ( std::is_same_v< T, uint16_t > )
                    return Type::UINT16;
                if constexpr ( std::is_same_v< T, uint32_t > )
                    return Type::UINT32;
                if constexpr ( std::is_same_v< T, uint64_t > )
                    return Type::UINT64;
                if constexpr ( std::is_same_v< T, float> )
                    return Type::FLOAT;
                if constexpr ( std::is_same_v< T, double> )
                    return Type::DOUBLE;
                if constexpr ( std::is_same_v< T, String> )
                    return Type::STRING;
                if constexpr ( std::is_same_v< T, Map > )
                    return Type::MAP;
                if constexpr ( std::is_same_v< T, Vector< int8_t > > )
                    return Type::INT8_VECTOR;
                if constexpr ( std::is_same_v< T, Vector< int16_t > > )
                    return Type::INT16_VECTOR;
                if constexpr ( std::is_same_v< T, Vector< int32_t > > )
                    return Type::INT32_VECTOR;
                if constexpr ( std::is_same_v< T, Vector< int64_t > > )
                    return Type::INT64_VECTOR;
                if constexpr ( std::is_same_v< T, Vector< uint8_t > > )
                    return Type::UINT8_VECTOR;
                if constexpr ( std::is_same_v< T, Vector< uint16_t > > )
                    return Type::UINT16_VECTOR;
                if constexpr ( std::is_same_v< T, Vector< uint32_t > > )
                    return Type::UINT32_VECTOR;
                if constexpr ( std::is_same_v< T, Vector< uint64_t > > )
                    return Type::UINT64_VECTOR;
                if constexpr ( std::is_same_v< T, Vector< String > > )
                    return Type::STRING_VECTOR;
                if constexpr ( std::is_same_v< T, Vector< Map > > )
                    return Type::MAP_VECTOR;
                return Type::VOID;
            }

            template< typename T >
            constexpr bool checkValidType()
            {
                constexpr auto type = templateToVariantType< T >();
                return type != VOID;
            }
        }
    }
}
