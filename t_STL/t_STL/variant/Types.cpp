#include "Types.h"


namespace t
{
	namespace v
	{
		const char* typeToString( Type type )
        {
            switch ( type )
            {
            case VOID:
                return "void";
            case INT8:
                return "i8";
            case INT16:
                return "i16";
            case INT32:
                return "i32";
            case INT64:
                return "i64";
            case UINT8:
                return "u8";
            case UINT16:
                return "u16";
            case UINT32:
                return "u32";
            case UINT64:
                return "u64";
            case FLOAT:
                return "float";
            case DOUBLE:
                return "double";
            case STRING:
                return "String";
            case MAP:
                return "Map";
            case INT8_VECTOR:
                return "i8[]";
            case INT16_VECTOR:
                return "i16[]";
            case INT32_VECTOR:
                return "i32[]";
            case INT64_VECTOR:
                return "i64[]";
            case UINT8_VECTOR:
                return "u8[]";
            case UINT16_VECTOR:
                return "u16[]";
            case UINT32_VECTOR:
                return "u32[]";
            case UINT64_VECTOR:
                return "u64[]";
            case FLOAT_VECTOR:
                return "float[]";
            case DOUBLE_VECTOR:
                return "double[]";
            case STRING_VECTOR:
                return "String[]";
            case MAP_VECTOR:
                return "Map[]";
            default:
                return "void";
            }
        }
	}
}