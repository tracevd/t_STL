#pragma once

namespace t
{
    template< typename Source_It, typename Dest_It, typename Func >
    constexpr void transform( Source_It src_begin, const Source_It src_end, Dest_It dest_begin, Func func )
    {
        for( ; src_begin != src_end; ++src_begin, ++dest_begin )
        {
            *dest_begin = func( *src_begin );
        }
    }

    template< typename It, typename Comp >
    constexpr It find( It begin, const It end, Comp const& val )
    {
        for ( ; begin != end; ++begin )
        {
            if ( *begin == val )
                return begin;
        }
        return begin;
    }

    template< typename It, typename Func >
    constexpr It findIf( It begin, const It end, Func func )
    {
        for ( ; begin != end; ++begin )
        {
            if ( func( *begin ) )
                return begin;
        }
        return begin;
    }
}