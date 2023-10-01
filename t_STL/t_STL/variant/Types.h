#pragma once

#include "../Type.h"
#include "../Memory.h"

#include "../String.h"

namespace t
{
    template< typename T >
    class DynamicArray;

    template< typename T >
    struct is_vector
    {
        static constexpr bool value = false;
    };

    template< typename T >
    struct is_vector< DynamicArray< T > >
    {
        static constexpr bool value = true;
    };

    namespace variant
    {
        class Map;

        constexpr uint8 ARRAY = 0x10;

        enum Type : uint8
        {
            VOID,
            INT8, INT16, INT32, INT64,
            UINT8, UINT16, UINT32, UINT64,
            FLOAT, DOUBLE,
            STRING,
            MAP,
            INT8_ARRAY = INT8 | ARRAY,
            INT16_ARRAY = INT16 | ARRAY,
            INT32_ARRAY = INT32 | ARRAY,
            INT64_ARRAY = INT64 | ARRAY,
            UINT8_ARRAY = UINT8 | ARRAY,
            UINT16_ARRAY = UINT16 | ARRAY,
            UINT32_ARRAY = UINT32 | ARRAY,
            UINT64_ARRAY = UINT64 | ARRAY,
            FLOAT_ARRAY = FLOAT | ARRAY,
            DOUBLE_ARRAY = DOUBLE | ARRAY,
            STRING_ARRAY = STRING | ARRAY
        };

        #include "ValueTypeDefines.h"

        TtoTypeDef

        template< typename T >
        constexpr Type templateToVariantType()
        {
            return TtoType< T >::type;
        }

        constexpr const char* typeToString( Type type )
        {
            switch ( type )
            {
            case VOID:
                return "void";
            case INT8:
                return "int8";
            case INT16:
                return "int16";
            case INT32:
                return "int32";
            case INT64:
                return "int64";
            case UINT8:
                return "uint8";
            case UINT16:
                return "uint16";
            case UINT32:
                return "uint32";
            case UINT64:
                return "uint64";
            case FLOAT:
                return "float";
            case DOUBLE:
                return "double";
            case STRING:
                return "String";
            case MAP:
                return "Map";
            case INT8_ARRAY:
                return "int8[]";
            case INT16_ARRAY:
                return "int16[]";
            case INT32_ARRAY:
                return "int32[]";
            case INT64_ARRAY:
                return "int64[]";
            case UINT8_ARRAY:
                return "uint8[]";
            case UINT16_ARRAY:
                return "uint16[]";
            case UINT32_ARRAY:
                return "uint32[]";
            case UINT64_ARRAY:
                return "uint64[]";
            case FLOAT_ARRAY:
                return "float[]";
            case DOUBLE_ARRAY:
                return "double[]";
            case STRING_ARRAY:
                return "String[]";
            default:
                return "void";
            }
        }

        template< typename T >
        constexpr const char* templateToString()
        {
            return TtoType< T >::string;
        }
    }
}
