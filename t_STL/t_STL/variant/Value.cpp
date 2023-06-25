#include "Value.h"
#include "Map.h"

#include <iostream>

namespace t
{
    namespace v
    {
        inline namespace v2
        {
            void Value::DestroyData()
            {
                #define delete_as( type ) delete static_cast< type* >( m_data ); m_data = nullptr; return
                #define delete_as_vector( type ) delete static_cast< Vector< type >* >( m_data ); m_data = nullptr; return
                if ( m_refs == nullptr )
                    return;
                *m_refs -= 1;
                if ( *m_refs > 0 )
                    return;

                std::cout << "Deleting data: ";
                std::cout << typeToString( m_type ) << "\n\n";

                delete m_refs;
                switch ( m_type )
                {
                case VOID:
                    return;
                case INT8:
                    delete_as( int8_t );
                case INT16:
                    delete_as( int16_t );
                case INT32:
                    delete_as( int32_t );
                case INT64:
                    delete_as( int64_t );
                case UINT8:
                    delete_as( uint8_t );
                case UINT16:
                    delete_as( uint16_t );
                case UINT32:
                    delete_as( uint32_t );
                case UINT64:
                    delete_as( uint64_t );
                case FLOAT:
                    delete_as( float );
                case DOUBLE:
                    delete_as( double );
                case STRING:
                    delete_as( String );
                case MAP:
                    delete_as( Map );
                case INT8_VECTOR:
                    delete_as_vector( int8_t );
                case INT16_VECTOR:
                    delete_as_vector( int16_t );
                case INT32_VECTOR:
                    delete_as_vector( int32_t );
                case INT64_VECTOR:
                    delete_as_vector( int64_t );
                case UINT8_VECTOR:
                    delete_as_vector( uint8_t );
                case UINT16_VECTOR:
                    delete_as_vector( uint16_t );
                case UINT32_VECTOR:
                    delete_as_vector( uint32_t );
                case UINT64_VECTOR:
                    delete_as_vector( uint64_t );
                case FLOAT_VECTOR:
                    delete_as_vector( float );
                case DOUBLE_VECTOR:
                    delete_as_vector( double );
                case STRING_VECTOR:
                    delete_as_vector( String );
                case MAP_VECTOR:
                    delete_as_vector( Map );
                default:
                    throw std::runtime_error( "Unknown type attempting to be deleted" );
                }
                #undef delete_as
                #undef delete_as_vector
            }

            bool Value::operator==( Value const& rhs ) const
            {
                if ( m_type != rhs.m_type )
                    return false;
                if ( this == &rhs )
                    return true;
                switch ( m_type )
                {
                case INT8:
                    return As< int8_t >() == rhs.As< int8_t >();
                case INT16:
                    return As< int16_t >() == rhs.As< int16_t >();
                case INT32:
                    return As< int32_t >() == rhs.As< int32_t >();
                case INT64:
                    return As< int64_t >() == rhs.As< int64_t >();
                case UINT8:
                    return As< uint8_t >() == rhs.As< uint8_t >();
                case UINT16:
                    return As< uint16_t >() == rhs.As< uint16_t >();
                case UINT32:
                    return As< uint32_t >() == rhs.As< uint32_t >();
                case UINT64:
                    return As< uint64_t >() == rhs.As< uint64_t >();
                case FLOAT:
                    return As< float >() == rhs.As< float >();
                case DOUBLE:
                    return As< double >() == rhs.As< double >();
                case STRING:
                    return As< String const& >() == rhs.As< String const& >();
                case MAP:
                    return As< Map const& >() == rhs.As< Map const& >();
                case INT8_VECTOR:
                    return As< Vector< int8_t > const& >() == rhs.As< Vector< int8_t > const& >();
                case INT16_VECTOR:
                    return As< Vector< int16_t > const& >() == rhs.As< Vector< int16_t > const& >();
                case INT32_VECTOR:
                    return As< Vector< int32_t > const& >() == rhs.As< Vector< int32_t > const& >();
                case INT64_VECTOR:
                    return As< Vector< int64_t > const& >() == rhs.As< Vector< int64_t > const& >();
                case UINT8_VECTOR:
                    return As< Vector< uint8_t > const& >() == rhs.As< Vector< uint8_t > const& >();
                case UINT16_VECTOR:
                    return As< Vector< uint16_t > const& >() == rhs.As< Vector< uint16_t > const& >();
                case UINT32_VECTOR:
                    return As< Vector< uint32_t > const& >() == rhs.As< Vector< uint32_t > const& >();
                case UINT64_VECTOR:
                    return As< Vector< uint64_t > const& >() == rhs.As< Vector< uint64_t > const& >();
                case FLOAT_VECTOR:
                    return As< Vector< float > const& >() == rhs.As< Vector< float > const& >();
                case DOUBLE_VECTOR:
                    return As< Vector< double > const& >() == rhs.As< Vector< double > const& >();
                case STRING_VECTOR:
                    return As< Vector< String > const& >() == rhs.As< Vector< String > const& >();
                case MAP_VECTOR:
                    return As< Vector< Map > const& >() == rhs.As< Vector< Map > const& >();
                default:
                    return false;
                }
            }
        }
        namespace v1
        {
            bool Value::operator==( Value const& rhs ) const
            {
                if ( m_type != rhs.m_type )
                    return false;
                if ( this == &rhs )
                    return true;
                switch ( m_type )
                {
                case INT8:
                    return As< int8_t >() == rhs.As< int8_t >();
                case INT16:
                    return As< int16_t >() == rhs.As< int16_t >();
                case INT32:
                    return As< int32_t >() == rhs.As< int32_t >();
                case INT64:
                    return As< int64_t >() == rhs.As< int64_t >();
                case UINT8:
                    return As< uint8_t >() == rhs.As< uint8_t >();
                case UINT16:
                    return As< uint16_t >() == rhs.As< uint16_t >();
                case UINT32:
                    return As< uint32_t >() == rhs.As< uint32_t >();
                case UINT64:
                    return As< uint64_t >() == rhs.As< uint64_t >();
                case FLOAT:
                    return As< float >() == rhs.As< float >();
                case DOUBLE:
                    return As< double >() == rhs.As< double >();
                case STRING:
                    return As< String const& >() == rhs.As< String const& >();
                case MAP:
                    return As< Map const& >() == rhs.As< Map const& >();
                case INT8_VECTOR:
                    return As< Vector< int8_t > const& >() == rhs.As< Vector< int8_t > const& >();
                case INT16_VECTOR:
                    return As< Vector< int16_t > const& >() == rhs.As< Vector< int16_t > const& >();
                case INT32_VECTOR:
                    return As< Vector< int32_t > const& >() == rhs.As< Vector< int32_t > const& >();
                case INT64_VECTOR:
                    return As< Vector< int64_t > const& >() == rhs.As< Vector< int64_t > const& >();
                case UINT8_VECTOR:
                    return As< Vector< uint8_t > const& >() == rhs.As< Vector< uint8_t > const& >();
                case UINT16_VECTOR:
                    return As< Vector< uint16_t > const& >() == rhs.As< Vector< uint16_t > const& >();
                case UINT32_VECTOR:
                    return As< Vector< uint32_t > const& >() == rhs.As< Vector< uint32_t > const& >();
                case UINT64_VECTOR:
                    return As< Vector< uint64_t > const& >() == rhs.As< Vector< uint64_t > const& >();
                case FLOAT_VECTOR:
                    return As< Vector< float > const& >() == rhs.As< Vector< float > const& >();
                case DOUBLE_VECTOR:
                    return As< Vector< double > const& >() == rhs.As< Vector< double > const& >();
                case STRING_VECTOR:
                    return As< Vector< String > const& >() == rhs.As< Vector< String > const& >();
                case MAP_VECTOR:
                    return As< Vector< Map > const& >() == rhs.As< Vector< Map > const& >();
                default:
                    return false;
                }
            }
        }
    }
}
