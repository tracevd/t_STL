#include "Value.h"
#include "Map.h"

#include <iostream>

namespace t
{
    namespace variant
    {
        template< class T >
        using DerivedPtr = value::Derived< T >*;

        template< class T >
        using ConstDerivedPtr = const value::Derived< T >*;

        Value::Value( const char* str ):
            m_data( new value::Derived< String >( String( str ) ) ),
            m_type( templateToVariantType< String >() ) {}

        template< class T, class >
        Value::Value( T data ):
            m_data( new value::Derived< T >( std::is_arithmetic_v< T > ? data : std::move( data ) ) ),
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

        Value Value::QuickClone() const
        {
            switch ( m_type )
            {
            case MAP:
                return Value( static_cast< value::Derived< Map > const* >( m_data.get() )->val.QuickClone() );
            default:
                return *this;
            }
        }

        template< class T, class >
        T Value::As() const
        {
            auto constexpr type = templateToVariantType< type::decay< T > >();

            static_assert( type != VOID, "Value cannot be this type!" );

            if ( m_data == nullptr )
                throw std::runtime_error("Data is null!");

            if ( type != m_type )
                throw std::runtime_error( "Invalid type" );

            return static_cast< const value::Derived< type::decay< T > >* >( m_data.get() )->val;
        }

        template< class T, class >
        T Value::As()
        {
            auto constexpr type = templateToVariantType< type::decay< T > >();

            static_assert( type != VOID, "Value cannot be this type!" );

            if ( m_data == nullptr )
                throw std::runtime_error("Data is null!");

            if ( type != m_type )
                throw std::runtime_error( "Invalid type" );

            auto data = static_cast< DerivedPtr< type::decay< T > > >( m_data.get() );

            if constexpr ( std::is_reference_v< T > )
            {
                if ( m_data.isShared() )
                {
                    if constexpr ( type::is_same< type::remove_reference< T >, Map > )
                        *this = QuickClone();
                    else
                        *this = Clone();

                    data = static_cast< DerivedPtr< type::decay< T > > >( m_data.get() );
                }
            }

            return data->val;
        }

        DefineAll( template );
        DefineTemplate( template, Map );

        Value Value::Clone() const
        {
            switch ( m_type )
            {
            case VOID:
                return Value();
            case INT8:
                return Value( static_cast< ConstDerivedPtr< int8 > >( m_data.get() )->val );
            case INT16:
                return Value( static_cast< ConstDerivedPtr< int16 > >( m_data.get() )->val );
            case INT32:
                return Value( static_cast< ConstDerivedPtr< int32 > >( m_data.get() )->val );
            case INT64:
                return Value( static_cast< ConstDerivedPtr< int64 > >( m_data.get() )->val );
            case UINT8:
                return Value( static_cast< ConstDerivedPtr< uint8 > >( m_data.get() )->val );
            case UINT16:
                return Value( static_cast< ConstDerivedPtr< uint16 > >( m_data.get() )->val );
            case UINT32:
                return Value( static_cast< ConstDerivedPtr< uint32 > >( m_data.get() )->val );
            case UINT64:
                return Value( static_cast< ConstDerivedPtr< uint64 > >( m_data.get() )->val );
            case FLOAT:
                return Value( static_cast< ConstDerivedPtr< float > >( m_data.get() )->val );
            case DOUBLE:
                return Value( static_cast< ConstDerivedPtr< double > >( m_data.get() )->val );
            case STRING:
                return Value( static_cast< ConstDerivedPtr< String > >( m_data.get() )->val );
            case MAP:
                return Value( static_cast< ConstDerivedPtr< Map > >( m_data.get() )->val.Clone() );
            case INT8_LIST:
                return Value( static_cast< ConstDerivedPtr< List< int8 > > >( m_data.get() )->val );
            case INT16_LIST:
                return Value( static_cast< ConstDerivedPtr< List< int16 > > >( m_data.get() )->val );
            case INT32_LIST:
                return Value( static_cast< ConstDerivedPtr< List< int32 > > >( m_data.get() )->val );
            case INT64_LIST:
                return Value( static_cast< ConstDerivedPtr< List< int64 > > >( m_data.get() )->val );
            case UINT8_LIST:
                return Value( static_cast< ConstDerivedPtr< List< uint8 > > >( m_data.get() )->val );
            case UINT16_LIST:
                return Value( static_cast< ConstDerivedPtr< List< uint16 > > >( m_data.get() )->val );
            case UINT32_LIST:
                return Value( static_cast< ConstDerivedPtr< List< uint32 > > >( m_data.get() )->val );
            case UINT64_LIST:
                return Value( static_cast< ConstDerivedPtr< List< uint64 > > >( m_data.get() )->val );
            case FLOAT_LIST:
                return Value( static_cast< ConstDerivedPtr< List< float > > >( m_data.get() )->val );
            case DOUBLE_LIST:
                return Value( static_cast< ConstDerivedPtr< List< double > > >( m_data.get() )->val );
            case STRING_LIST:
                return Value( static_cast< ConstDerivedPtr< List< String > > >( m_data.get() )->val );
            default:
                throw std::runtime_error( "Unknown type attempting to be deleted" );
            }
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
            case INT8_LIST:
                return As< List< int8 > const& >() == rhs.As< List< int8 > const& >();
            case INT16_LIST:
                return As< List< int16 > const& >() == rhs.As< List< int16 > const& >();
            case INT32_LIST:
                return As< List< int32 > const& >() == rhs.As< List< int32 > const& >();
            case INT64_LIST:
                return As< List< int64 > const& >() == rhs.As< List< int64 > const& >();
            case UINT8_LIST:
                return As< List< uint8 > const& >() == rhs.As< List< uint8 > const& >();
            case UINT16_LIST:
                return As< List< uint16 > const& >() == rhs.As< List< uint16 > const& >();
            case UINT32_LIST:
                return As< List< uint32 > const& >() == rhs.As< List< uint32 > const& >();
            case UINT64_LIST:
                return As< List< uint64 > const& >() == rhs.As< List< uint64 > const& >();
            case FLOAT_LIST:
                return As< List< float > const& >() == rhs.As< List< float > const& >();
            case DOUBLE_LIST:
                return As< List< double > const& >() == rhs.As< List< double > const& >();
            case STRING_LIST:
                return As< List< String > const& >() == rhs.As< List< String > const& >();
            default:
                return false;
            }
        }
    }
}
