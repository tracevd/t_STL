#pragma once

#include "../Type.h"
#include "../Memory.h"

#include "../String.h"

namespace t
{
    template< typename T >
    class List;

    template< typename T >
    struct is_vector
    {
        static constexpr bool value = false;
    };

    template< typename T >
    struct is_vector< List< T > >
    {
        static constexpr bool value = true;
    };

    namespace variant
    {
        class Map;

        enum Type : uint8
        {
            VOID,
            INT8, INT16, INT32, INT64,
            UINT8, UINT16, UINT32, UINT64,
            FLOAT, DOUBLE,
            STRING,
            MAP,
            LIST = 0x10,
            INT8_LIST = INT8 | LIST,
            INT16_LIST = INT16 | LIST,
            INT32_LIST = INT32 | LIST,
            INT64_LIST = INT64 | LIST,
            UINT8_LIST = UINT8 | LIST,
            UINT16_LIST = UINT16 | LIST,
            UINT32_LIST = UINT32 | LIST,
            UINT64_LIST = UINT64 | LIST,
            FLOAT_LIST = FLOAT | LIST,
            DOUBLE_LIST = DOUBLE | LIST,
            STRING_LIST = STRING | LIST
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
