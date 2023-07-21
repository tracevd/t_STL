#include "Value.h"
#include "Map.h"

#include <iostream>

namespace t
{
    namespace variant
    {
        Value::Value( const char* str ):
            m_data( new Data< String >( String( str ) ) ),
            m_type( templateToVariantType< String >() ) {}

        template< class T, class >
        Value::Value( T data ):
            m_data( new Data< T >( std::is_arithmetic_v< T > ? data : std::move( data ) ) ),
            m_type( templateToVariantType< T >() ) {}
        
        Value& Value::operator=( const char* str )
        {
            *this = Value( str );
            return *this;
        }

        template< typename T >
        Value& Value::operator=( T data )
        {
            *this = Value( std::move( data ) );
            return *this;
        }

        static Vector< Map > CloneVectorOfMaps( Vector< Map > const& vec )
        {
            Vector< Map > copy{ vec.size() };

            for ( size_t i = 0; i < vec.size(); ++i )
            {
                copy[ i ] = vec[ i ].Clone();
            }

            return copy;
        }
        Value Value::QuickClone() const
        {
            switch ( m_type )
            {
            case MAP:
                return Value( static_cast< Data< Map >* >( m_data )->val.QuickClone() );
            case MAP_VECTOR:
                return Value( static_cast< Data< Vector< Map > >* >( m_data )->val );
            default:
                return *this;
            }
        }

        bool Value::isUnique() const
        {
            if ( m_data == nullptr )
                return true;
            return *static_cast< uint64_t* >( m_data ) == 1;
        }

        template< class T, class >
        T Value::As() const
        {
            auto constexpr type = templateToVariantType< type::decay< T > >();

            static_assert( type != VOID, "Value cannot be this type!" );

            if ( m_data == nullptr )
                throw std::runtime_error("Data is null!");

            if ( type != m_type )
            {
                throw std::runtime_error( "Invalid type" );
            }

            return static_cast< Data< type::decay< T > >* >( m_data )->val;
        }

        template< class T, class >
        T Value::As()
        {
            auto constexpr type = templateToVariantType< type::decay< T > >();

            static_assert( type != VOID, "Value cannot be this type!" );

            if ( m_data == nullptr )
                throw std::runtime_error("Data is null!");

            if ( type != m_type )
            {
                throw std::runtime_error( "Invalid type" );
            }

            auto data = static_cast< Data< type::decay< T > >* >( m_data );

            if constexpr ( std::is_reference_v< T > )
            {
                if ( data->references > 1 )
                {
                    if constexpr ( type::is_same< type::remove_reference< T >, Map > || type::is_same< type::remove_reference< T >, Vector< Map > > )
                        *this = QuickClone( 0 );
                    else
                        *this = Clone();
                }
            }

            return data->val;
        }

        DefineAll( template );
        DefineNormalAndVector( template, Map );

        Value Value::Clone() const
        {
            switch ( m_type )
            {
            case VOID:
                return Value();
            case INT8:
                return Value( static_cast< Data< int8 >* >( m_data )->val );
            case INT16:
                return Value( static_cast< Data< int16 >* >( m_data )->val );
            case INT32:
                return Value( static_cast< Data< int32 >* >( m_data )->val );
            case INT64:
                return Value( static_cast< Data< int64 >* >( m_data )->val );
            case UINT8:
                return Value( static_cast< Data< uint8 >* >( m_data )->val );
            case UINT16:
                return Value( static_cast< Data< uint16 >* >( m_data )->val );
            case UINT32:
                return Value( static_cast< Data< uint32 >* >( m_data )->val );
            case UINT64:
                return Value( static_cast< Data< uint64 >* >( m_data )->val );
            case FLOAT:
                return Value( static_cast< Data< float >* >( m_data )->val );
            case DOUBLE:
                return Value( static_cast< Data< double >* >( m_data )->val );
            case STRING:
                return Value( static_cast< Data< String >* >( m_data )->val );
            case MAP:
                return Value( static_cast< Data< Map >* >( m_data )->val.Clone() );
            case INT8_VECTOR:
                return Value( static_cast< Data< Vector< int8 > >* >( m_data )->val );
            case INT16_VECTOR:
                return Value( static_cast< Data< Vector< int16 > >* >( m_data )->val );
            case INT32_VECTOR:
                return Value( static_cast< Data< Vector< int32 > >* >( m_data )->val );
            case INT64_VECTOR:
                return Value( static_cast< Data< Vector< int64 > >* >( m_data )->val );
            case UINT8_VECTOR:
                return Value( static_cast< Data< Vector< uint8 > >* >( m_data )->val );
            case UINT16_VECTOR:
                return Value( static_cast< Data< Vector< uint16 > >* >( m_data )->val );
            case UINT32_VECTOR:
                return Value( static_cast< Data< Vector< uint32 > >* >( m_data )->val );
            case UINT64_VECTOR:
                return Value( static_cast< Data< Vector< uint64 > >* >( m_data )->val );
            case FLOAT_VECTOR:
                return Value( static_cast< Data< Vector< float > >* >( m_data )->val );
            case DOUBLE_VECTOR:
                return Value( static_cast< Data< Vector< double > >* >( m_data )->val );
            case STRING_VECTOR:
                return Value( static_cast< Data< Vector< String > >* >( m_data )->val );
            case MAP_VECTOR:
                return Value( CloneVectorOfMaps( static_cast< Data< Vector< Map > >* >( m_data )->val ) );
            default:
                throw std::runtime_error( "Unknown type attempting to be deleted" );
            }
        }

        void Value::DestroyData()
        {
            #define delete_as( T ) delete static_cast< Data< T >* >( m_data ); m_data = nullptr; return
            #define delete_as_vector( T ) delete static_cast< Data< Vector< T > >* >( m_data ); m_data = nullptr; return
            if ( m_data == nullptr )
                return;
            *static_cast< uint64_t* >( m_data ) -= 1;
            if ( *static_cast< uint64_t* >( m_data ) > 0 )
                return;

            switch ( m_type )
            {
            case VOID:
                return;
            case INT8:
                delete_as( int8 );
            case INT16:
                delete_as( int16 );
            case INT32:
                delete_as( int32 );
            case INT64:
                delete_as( int64 );
            case UINT8:
                delete_as( uint8 );
            case UINT16:
                delete_as( uint16 );
            case UINT32:
                delete_as( uint32 );
            case UINT64:
                delete_as( uint64 );
            case FLOAT:
                delete_as( float );
            case DOUBLE:
                delete_as( double );
            case STRING:
                delete_as( String );
            case MAP:
                delete_as( Map );
            case INT8_VECTOR:
                delete_as_vector( int8 );
            case INT16_VECTOR:
                delete_as_vector( int16 );
            case INT32_VECTOR:
                delete_as_vector( int32 );
            case INT64_VECTOR:
                delete_as_vector( int64 );
            case UINT8_VECTOR:
                delete_as_vector( uint8 );
            case UINT16_VECTOR:
                delete_as_vector( uint16 );
            case UINT32_VECTOR:
                delete_as_vector( uint32 );
            case UINT64_VECTOR:
                delete_as_vector( uint64 );
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
            if ( this == &rhs || m_data == rhs.m_data )
                return true;

            switch ( m_type )
            {
            case INT8:
                return As< int8 >() == rhs.As< int8 >();
            case INT16:
                return As< int16 >() == rhs.As< int16 >();
            case INT32:
                return As< int32 >() == rhs.As< int32 >();
            case INT64:
                return As< int64 >() == rhs.As< int64 >();
            case UINT8:
                return As< uint8 >() == rhs.As< uint8 >();
            case UINT16:
                return As< uint16 >() == rhs.As< uint16 >();
            case UINT32:
                return As< uint32 >() == rhs.As< uint32 >();
            case UINT64:
                return As< uint64 >() == rhs.As< uint64 >();
            case FLOAT:
                return As< float >() == rhs.As< float >();
            case DOUBLE:
                return As< double >() == rhs.As< double >();
            case STRING:
                return As< String const& >() == rhs.As< String const& >();
            case MAP:
                return As< Map const& >() == rhs.As< Map const& >();
            case INT8_VECTOR:
                return As< Vector< int8 > const& >() == rhs.As< Vector< int8 > const& >();
            case INT16_VECTOR:
                return As< Vector< int16 > const& >() == rhs.As< Vector< int16 > const& >();
            case INT32_VECTOR:
                return As< Vector< int32 > const& >() == rhs.As< Vector< int32 > const& >();
            case INT64_VECTOR:
                return As< Vector< int64 > const& >() == rhs.As< Vector< int64 > const& >();
            case UINT8_VECTOR:
                return As< Vector< uint8 > const& >() == rhs.As< Vector< uint8 > const& >();
            case UINT16_VECTOR:
                return As< Vector< uint16 > const& >() == rhs.As< Vector< uint16 > const& >();
            case UINT32_VECTOR:
                return As< Vector< uint32 > const& >() == rhs.As< Vector< uint32 > const& >();
            case UINT64_VECTOR:
                return As< Vector< uint64 > const& >() == rhs.As< Vector< uint64 > const& >();
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
