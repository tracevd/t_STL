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

    template< typename Source_It1, typename SourceIt2, typename Dest_It, typename Func >
    constexpr void transform( Source_It1 begin1, Source_It1 const end1, SourceIt2 begin2, Dest_It dest_begin, Func func )
    {
        for ( ; begin1 != end1; ++begin1, ++begin2, ++dest_begin )
        {
            *dest_begin = func( *begin1, *begin2 );
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
    constexpr It findIf( It begin, const It end, Func condition )
    {
        for ( ; begin != end; ++begin )
        {
            if ( condition( *begin ) )
                return begin;
        }
        return begin;
    }

    template< typename It, typename Func >
    constexpr void forEach( It begin, const It end, Func func )
    {
        for ( ; begin != end; ++begin )
        {
            func( *begin );
        }
    }
}