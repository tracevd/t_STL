#pragma once

#include "../Memory.h"
#include "variant.h"
#include "../String.h"
#include "../Vector.h"

#include <iostream>
#include <string>

namespace t
{
    namespace v
    {
        inline namespace v2
        {
            class Value
            {
            public:
                constexpr Value() = default;

                Value( Value const& data )
                {
                    *this = data.copy();
                }

                explicit Value( Value&& data ) noexcept:
                    m_data( data.m_data ),
                    m_type( data.m_type )
                {
                    data.m_data = nullptr;
                    data.m_type = VOID;
                }

                explicit Value( const char* str ):
                    m_data( new String( str ) ),
                    m_type( templateToVariantType< String >() ) {}

                template< typename T, typename = std::enable_if_t< !std::is_arithmetic_v< T > && ( ALLOWED_TYPES( std::decay_t< T > ) ) > >
                explicit Value( T const& data ):
                    m_data( new std::decay_t< T >{ data } ),
                    m_type( templateToVariantType< std::decay_t< T > >() ) {}
                
                template< typename T, typename = std::enable_if_t< !std::is_arithmetic_v< T > && ( ALLOWED_TYPES( std::decay< T > ) ) > >
                explicit Value( T&& data ):
                    m_data( new std::decay_t< T >{ std::move( data ) } ),
                    m_type( templateToVariantType< std::decay_t< T > >() ) {}

                template< typename T, typename = std::enable_if_t< std::is_arithmetic_v< T > && ( ALLOWED_TYPES( T ) ) > >
                explicit Value( T data ):
                    m_data( new T{ data } ),
                    m_type( templateToVariantType< T >() ) {}

                Value& operator=( Value const& rhs )
                {
                    if ( this == &rhs )
                        return *this;
                    
                    DestroyData();

                    *this = rhs.copy();

                    return *this;
                }

                Value& operator=( Value&& rhs ) noexcept
                {
                    if ( this == &rhs )
                        return *this;

                    DestroyData();

                    m_data = rhs.m_data;
                    m_type = rhs.m_type;

                    rhs.m_data = nullptr;
                    rhs.m_type = VOID;

                    return *this;
                }

                Value& operator=( const char* str )
                {
                    *this = Value( String( str ) );
                    return *this;
                }

                template< typename T, std::enable_if_t< !std::is_arithmetic_v< T > > >
                Value& operator=( T const& data )
                {
                    *this = Value( data );
                    return *this;
                }

                template< typename T, typename = std::enable_if_t< !std::is_arithmetic_v< T > > >
                Value& operator=( T&& data )
                {
                    if constexpr ( std::is_lvalue_reference_v< T > )
                        *this = Value( static_cast<T const&>(data) );
                    else
                        *this = Value( std::move( data ) );
                    return *this;
                }

                template< typename T, typename = std::enable_if_t< std::is_arithmetic_v< T > > >
                Value& operator=( T data )
                {
                    *this = Value( data );
                    return *this;
                }

                bool operator==( Value const& rhs ) const;

                bool operator!=( Value const& rhs ) const
                {
                    return !( *this == rhs );
                }

                Type getType() const { return m_type; }

                template< typename T >
                bool Is() const
                {
                    auto constexpr type = templateToVariantType< std::decay_t< T > >();

                    return m_type == type;
                }

                template< typename T >
                T As() const
                {
                    auto constexpr type = templateToVariantType< std::decay_t< T > >();

                    if constexpr ( type == VOID )
                        throw std::runtime_error("Cannot cast to this type!");

                    if ( m_data == nullptr )
                        throw std::runtime_error("Data is null!");

                    if ( type != m_type )
                        throw std::runtime_error( "Invalid type" );

                    return *static_cast< std::decay_t< T >* >( m_data );
                }

                ~Value()
                {
                    DestroyData();
                }
            private:
                void DestroyData();

                Value copy() const;

            private:
                void* m_data = nullptr;
                Type m_type = VOID;
            };
        }
        namespace v1
        {
            class Value
            {
            public:
                Value() = default;

                explicit Value( const char* ptr ):
                    m_data( new Derived< String >( ptr ) ),
                    m_type( templateToVariantType< String >() ) {}

                template< typename T, typename = std::enable_if_t< !std::is_arithmetic_v< std::decay_t< T > > && ( ALLOWED_TYPES( std::decay< T > ) ) > >
                explicit Value( const T& value ):
                    m_data( new Derived< std::decay_t< T > >{ value } ),
                    m_type( templateToVariantType< T >() ) {}

                template< typename T, typename = std::enable_if_t< ALLOWED_TYPES( std::decay< T > ) > >
                Value( T&& value ):
                    m_data( new Derived< std::decay_t< T > >{ std::move( value ) } ),
                    m_type( templateToVariantType< std::decay_t< T > >() ) {}

                template< typename T, typename = std::enable_if_t< std::is_arithmetic_v< T > && ( ALLOWED_TYPES( T ) ) > >
                explicit Value( T data ):
                    m_data( new Derived< T >( data ) ),
                    m_type( templateToVariantType< T >() ) {}

                explicit Value( const Value& other )
                {
                    if ( other.m_data )
                    {
                        m_type = other.m_type;
                        m_data = other.m_data->clone();
                    }
                    else
                    {
                        m_data = nullptr;
                        m_type = Type::VOID;
                    }
                }

                explicit Value( Value&& other ) noexcept
                {
                    m_data = std::move( other.m_data );
                    m_type = other.m_type;
                    other.m_type = VOID;
                    other.m_data = nullptr;
                }

                Value& operator=( const Value& other )
                {
                    if ( other.m_data )
                    {
                        m_data = other.m_data.get()->clone();
                        m_type = other.m_type;
                    }
                    else
                    {
                        m_data = nullptr;
                        m_type = Type::VOID;
                    }
                    return *this;
                }

                Value& operator=( Value&& other ) noexcept
                {
                    m_data = std::move( other.m_data );
                    m_type = other.m_type;
                    return *this;
                }

                Value& operator=( const char* other )
                {
                    *this = Value( other );
                    return *this;
                }

                template< typename T, typename = std::enable_if_t< !std::is_arithmetic_v< T > > >
                Value& operator=( T const& data )
                {
                    *this = Value( data );
                    return *this;
                }

                template< typename T, typename = std::enable_if_t< std::is_arithmetic_v< T > > >
                Value& operator=( T data )
                {
                    *this = Value( data );
                    return *this;
                }

                template< typename T, typename = std::enable_if_t< !std::is_arithmetic_v< T > > >
                Value& operator=( T&& data )
                {
                    if constexpr ( std::is_lvalue_reference_v< T > )
                        *this = Value( static_cast< T const& >( data ) );
                    else
                        *this = Value( std::move( data ) );
                    return *this;
                }

                bool operator!=( const Value& rhs ) const
                {
                    return !(*this == rhs);
                }

                bool operator==( const Value& rhs ) const;

                template< typename T >
                static std::string templateToString()
                {
                    std::string str;
                    if constexpr ( std::is_const_v< T > )
                    {
                        str += "const ";
                    }
                    str += typeid(T).name();
                    if constexpr ( std::is_lvalue_reference_v< T > )
                    {
                        str += '&';
                    }
                    else if constexpr ( std::is_rvalue_reference_v< T > )
                    {
                        str += '&&';
                    }
                    return str;
                }

                template< typename T >
                T As() const
                {
                    if ( !checkValidType< std::decay_t< T > >() )
                        throw std::runtime_error( templateToString< T >() );
                    auto data = dynamic_cast< Derived< std::decay_t< T > >* >( m_data.get() );
                    if ( data == nullptr )
                        throw std::runtime_error( "Bad cast!" );
                    return data->data_;
                }

                ~Value() = default;

                inline bool isEmpty() const { return m_type == Type::VOID; }

                inline bool isNotEmpty() const { return m_type != Type::VOID; }

                inline bool isArray() const { return ((uint8_t) m_type >> 4) == 1; }

                inline bool isNotArray() const { return ((uint8_t) m_type >> 4) == 0; }

                Type getType() const { return m_type; }

                template< typename T >
                inline bool Is() const
                {
                    return dynamic_cast< Derived< T >* >( m_data.get() ) != nullptr;
                }

                template< typename T >
                inline bool IsNot() const
                {
                    return !Is< T >();
                }
            private:
                class Base
                {
                public:
                    Base() = default;
                    virtual ~Base() = default;

                    virtual UniquePtr< Base > clone() const = 0;
                };

                template< typename T >
                class Derived : public Base
                {
                public:
                    explicit Derived( const T& data ):
                        data_( data ) {}

                    explicit Derived( T&& value ):
                        data_( std::move( value ) ) {}

                    virtual UniquePtr< Base > clone() const override
                    {
                        return new Derived( data_ );
                    }

                    T data_;
                };
                UniquePtr< Base > m_data;
                Type m_type = Type::VOID;
                template< typename T >
                using rm_ref_and_const = std::remove_const_t< std::remove_reference_t< T > >;
            };
        }
    }
}