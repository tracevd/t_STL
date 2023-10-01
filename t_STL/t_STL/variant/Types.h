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
            case INT8_LIST:
                return "int8[]";
            case INT16_LIST:
                return "int16[]";
            case INT32_LIST:
                return "int32[]";
            case INT64_LIST:
                return "int64[]";
            case UINT8_LIST:
                return "uint8[]";
            case UINT16_LIST:
                return "uint16[]";
            case UINT32_LIST:
                return "uint32[]";
            case UINT64_LIST:
                return "uint64[]";
            case FLOAT_LIST:
                return "float[]";
            case DOUBLE_LIST:
                return "double[]";
            case STRING_LIST:
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
