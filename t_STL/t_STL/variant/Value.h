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
        template< typename T >
        using decay = std::decay_t< T >;

        template< typename T >
        using rm_ref = std::remove_reference_t< T >;

        class Value
        {
        public:
            constexpr Value() = default;

            Value( Value const& data ):
                m_data( data.m_data ),
                m_type( data.m_type )
            {
                if ( m_data == nullptr )
                    return;
                *static_cast< uint64_t* >( m_data ) += 1;
            }

            Value( Value&& data ) noexcept:
                m_data( data.m_data ),
                m_type( data.m_type )
            {
                data.m_data = nullptr;
                data.m_type = VOID;
            }

            /**
             * @brief Create a Value containing a t::String
             */
            explicit Value( const char* str ):
                m_data( new Data< String >( str ) ),
                m_type( templateToVariantType< String >() ) {}

            /**
             * @brief Create a Value with a copy of the input data
             */
            template< typename T, typename = std::enable_if_t< !std::is_arithmetic_v< T > && ( ALLOWED_TYPES( decay< T > ) ) > >
            explicit Value( T const& data ):
                m_data( new Data< decay< T > >( data ) ),
                m_type( templateToVariantType< decay< T > >() ) {}

            /**
             * @brief Need this overload (as far as I know) to be able to move data into the constructor. Analagous to T&& constructor
             * --- ACTUALLY MOVES DATA ---
             */
            template< typename T >
            explicit Value( T const&& data ):
                m_data( new Data< decay< T > >( const_cast< T&& >( data ) ) ),
                m_type( templateToVariantType< decay< T > >() ) {}

            // useless ?
            // template< typename T, typename = std::enable_if_t< !std::is_lvalue_reference_v< T > && !std::is_arithmetic_v< std::decay_t< T > > && (ALLOWED_TYPES( std::decay< T > )) > >
            // explicit Value( T&& data ):
            //     m_data( new std::decay_t< T >{ std::move( data ) } ),
            //     m_refs( new uint64_t( 1 ) ),
            //     m_type( templateToVariantType< std::decay_t< T > >() ) {}

            /**
             * @brief Create a Value containing a stdint or float/double
             */
            template< typename T, typename = std::enable_if_t< std::is_arithmetic_v< T > && ( ALLOWED_TYPES( T ) ) > >
            explicit Value( T data ):
                m_data( new Data< T >{ data } ),
                m_type( templateToVariantType< T >() ) {}

            /**
             * @brief Copy a value
             */
            Value& operator=( Value const& rhs )
            {
                if ( this == &rhs )
                    return *this;

                DestroyData();

                m_data = rhs.m_data;
                m_type = rhs.m_type;

                if ( m_data == nullptr )
                    return *this;

                *static_cast< uint64_t* >( m_data ) += 1;

                return *this;
            }

            /**
             * @brief Move a value
             */
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

            /**
             * @brief Assign the value to be a t::String
             */
            Value& operator=( const char* str )
            {
                *this = Value( str );
                return *this;
            }

            /**
             * @brief Assign the value to be copy of the given data
             */
            template< typename T, typename = std::enable_if_t< !std::is_arithmetic_v< decay< T > > > >
            Value& operator=( T const& data )
            {
                *this = Value( data );
                return *this;
            }

            /**
             * @brief Assign the value to a moved object (unless T&& is interpreted as T&, in which case it will be copied)
             */
            template< typename T, typename = std::enable_if_t< !std::is_arithmetic_v< decay< T > > > >
            Value& operator=( T&& data )
            {
                if constexpr ( std::is_lvalue_reference_v< T > )
                    *this = Value( static_cast< T const& >( data ) );
                else
                    *this = Value( std::move( data ) );
                return *this;
            }

            /**
             * @brief Assign the value to be a stdint or float/double
             */
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

            /**
             * @brief Return this value's type
             */
            Type getType() const { return m_type; }

            /**
             * @brief Return if this value is empty or not
             */
            bool isEmpty() const { return m_type == VOID; }

            /**
             * @brief Returns whether or not this value's data is unique. If empty, returns true
             */
            bool isUnique() const
            {
                if ( m_data == nullptr )
                    return true;
                return *static_cast< uint64_t* >( m_data ) == 1;
            }

            /**
             * @brief Returns whether or not this value's data is shared. If empty, returns false
             */
            bool isShared() const { return !isUnique(); }

            /**
             * @brief Check the type of the value
             */
            template< typename T >
            bool Is() const
            {
                auto constexpr type = templateToVariantType< decay< T > >();

                static_assert( type != VOID, "Value cannot be of this type" );

                return m_type == type;
            }

            /**
             * Cast to mutable reference. If underlying data is shared, create unique copy to be changed
             */
            template< typename T, typename = std::enable_if_t< std::is_reference_v< T > && !std::is_const_v< rm_ref< T > > > >
            T As()
            {
                auto constexpr type = templateToVariantType< decay< T > >();

                static_assert( type != VOID, "Value cannot be this type!" );

                if ( m_data == nullptr )
                    throw std::runtime_error("Data is null!");

                if ( type != m_type )
                    throw std::runtime_error( "Invalid type" );

                auto data = static_cast< Data< decay< T > >* >( m_data );

                if constexpr ( std::is_reference_v< T > )
                {
                    if ( data->references > 1 )
                        *this = Clone();
                }

                return data->val;
            }

            /**
             * Cast to const reference or literal
             */
            template< typename T, typename = std::enable_if_t< std::is_const_v< rm_ref< T > > || !std::is_reference_v< T > > >
            T As() const
            {
                auto constexpr type = templateToVariantType< decay< T > >();

                static_assert( type != VOID, "Value cannot be this type!" );

                if ( m_data == nullptr )
                    throw std::runtime_error("Data is null!");

                if ( type != m_type )
                    throw std::runtime_error( "Invalid type" );

                return static_cast< Data< decay< T > >* >(m_data)->val;
            }

            /**
             * @brief Return a completely unique copy of this value
             */
            Value Clone() const;

            ~Value()
            {
                DestroyData();
            }

        private:
            void DestroyData();

            /**
             * Used to store the data for t::v::Value in one allocation
             */
            template< typename T >
            struct Data
            {
            public:
                Data() = delete;
                template< typename = std::enable_if_t< !std::is_arithmetic_v< T > > >
                Data( T const& data ):
                    val( data ),
                    references( 1 ) {}
                template< typename = std::enable_if_t< !std::is_arithmetic_v< T > > >
                Data( T&& data ):
                    val( std::move( data ) ),
                    references( 1 ) {}
                template< typename = std::enable_if_t< std::is_arithmetic_v< T > > >
                Data( T data ):
                    val( data ),
                    references( 1 ) {}
                ~Data() = default;
            public:
                uint64_t references;
                T val;
            };

            template<>
            struct Data< String >
            {
                Data() = delete;
                Data( const char* str ):
                    val( str ),
                    references( 1 ) {}
                Data( String const& str ):
                    val( str ),
                    references( 1 ) {}
                Data( String&& str ):
                    val( std::move( str ) ),
                    references( 1 ) {}
                uint64_t references;
                String val;
            };

        private:
            void*     m_data = nullptr;
            Type      m_type = VOID;
        };
    }
}