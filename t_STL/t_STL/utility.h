#pragma once

#include "Type.h"

namespace t
{
    template< typename T >
    inline constexpr T&& move( T& val ) noexcept
    {
        return static_cast< T&& >( val );
    }

    template< typename T >
    inline constexpr T&& forward( type::remove_reference< T >& val ) noexcept
    {
        return static_cast< T&& >( val );
    }

    template< typename T >
    inline constexpr T&& forward( type::remove_reference< T >&& val ) noexcept
    {
        static_assert( !type::is_lvalue_reference< T >, "bad forward" );

        return static_cast< T&& >( val );
    }

    /*
     * Swaps two values
     */
    template< class T >
    constexpr void swap( T& first, T& second ) noexcept
    {
        auto cpy = move( first );
        first = move( second );
        second = move( cpy );
    }

    /*
     * Moves newValue into out, and returns the old out value
     */
    template< class T, class U = T >
    constexpr T exchange( T& out, U&& newValue ) noexcept
    {
        auto temp = move( out );
        out = move( newValue );
        return temp;
    }
}