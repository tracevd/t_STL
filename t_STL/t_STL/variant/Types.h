#pragma once

#include <type_traits>

namespace t
{
    class String;
    template< typename T >
    class Vector;

    template< typename T >
    struct is_vector
    {
        static constexpr bool value = false;
    };

    template< typename T >
    struct is_vector< Vector< T > >
    {
        static constexpr bool value = true;
    };

    namespace variant
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

        #include "ValueTypeDefines.h"

        TtoTypeDef

        template< typename T >
        constexpr Type templateToVariantType()
        {
            return TtoType< T >::type;
        }

        const char* typeToString( Type type );

        template< typename T >
        constexpr const char* templateToString()
        {
            return TtoType< T >::string;
        }
    }
}
