#include "Types.h"


namespace t
{
	namespace variant
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
            case INT8_LIST:
                return "i8[]";
            case INT16_LIST:
                return "i16[]";
            case INT32_LIST:
                return "i32[]";
            case INT64_LIST:
                return "i64[]";
            case UINT8_LIST:
                return "u8[]";
            case UINT16_LIST:
                return "u16[]";
            case UINT32_LIST:
                return "u32[]";
            case UINT64_LIST:
                return "u64[]";
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
	}
}
