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

        enum class Type : uint8
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
            case Type::VOID:
                return "void";
            case Type::INT8:
                return "int8";
            case Type::INT16:
                return "int16";
            case Type::INT32:
                return "int32";
            case Type::INT64:
                return "int64";
            case Type::UINT8:
                return "uint8";
            case Type::UINT16:
                return "uint16";
            case Type::UINT32:
                return "uint32";
            case Type::UINT64:
                return "uint64";
            case Type::FLOAT:
                return "float";
            case Type::DOUBLE:
                return "double";
            case Type::STRING:
                return "String";
            case Type::MAP:
                return "Map";
            case Type::INT8_ARRAY:
                return "int8[]";
            case Type::INT16_ARRAY:
                return "int16[]";
            case Type::INT32_ARRAY:
                return "int32[]";
            case Type::INT64_ARRAY:
                return "int64[]";
            case Type::UINT8_ARRAY:
                return "uint8[]";
            case Type::UINT16_ARRAY:
                return "uint16[]";
            case Type::UINT32_ARRAY:
                return "uint32[]";
            case Type::UINT64_ARRAY:
                return "uint64[]";
            case Type::FLOAT_ARRAY:
                return "float[]";
            case Type::DOUBLE_ARRAY:
                return "double[]";
            case Type::STRING_ARRAY:
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
