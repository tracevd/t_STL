#pragma once

#include "../Memory.h"
#include "variant.h"
#include "../String.h"
#include "../List.h"

#include "ValueImpl.h"

#include "../Type.h"

#include <iostream>
#include <string>

namespace t
{
    namespace variant
    {
        class Value
        {
        public:
            constexpr Value() = default;

            Value( Value const& data ):
                m_data( data.m_data ),
                m_type( data.m_type )
            {
            }

            Value( Value&& data ) noexcept:
                m_data( std::move( data.m_data ) ),
                m_type( data.m_type )
            {
                data.m_data = nullptr;
                data.m_type = VOID;
            }

            /**
             * @brief Create a Value containing a t::String
             */
            explicit Value( const char* str );

            /**
             * @brief Create a Value containing a stdint or float/double
             */
            template< typename T, typename = type::enable_if< ALLOWED_TYPES( T ) > >
            explicit Value( T data );

            /**
             * @brief Copy a value
             */
            Value& operator=( Value const& rhs )
            {
                if ( this == &rhs )
                    return *this;

                m_data = rhs.m_data;
                m_type = rhs.m_type;

                return *this;
            }

            /**
             * @brief Move a value
             */
            Value& operator=( Value&& rhs ) noexcept
            {
                if ( this == &rhs )
                    return *this;

                m_data = std::move( rhs.m_data );
                m_type = rhs.m_type;

                rhs.m_type = VOID;

                return *this;
            }

            /**
             * @brief Assign the value to be a t::String
             */
            Value& operator=( const char* str );

            template< typename T >
            Value& operator=( T data );

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
            bool isUnique() const;

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
                auto constexpr type = templateToVariantType< type::decay< T > >();

                static_assert( type != VOID, "Value cannot be of this type" );

                return m_type == type;
            }

            /**
             * Cast to mutable reference. If underlying data is shared, create unique copy to be changed
             */
            template< typename T, typename = type::enable_if< type::is_reference< T > && !type::is_const< type::remove_reference< T > > > >
            T As();

            /**
             * Cast to const reference or plain type
             */
            template< typename T, typename = type::enable_if< type::is_const< type::remove_reference< T > > || !type::is_reference< T > > >
            T As() const;

            Value Clone() const;

            Value QuickClone() const;

            ~Value() = default;

        private:
            SharedPtr< value::Base > m_data;
            Type m_type = VOID;
        };
    }
}

#include "ValueTemplateDefines.h"

namespace t
{
    namespace variant
    {
        DefineAll( extern template );
    }
}
