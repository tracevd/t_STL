#pragma once

#include <algorithm>

#include "Pair.h"

namespace t
{
    template< class T >
    constexpr void swap( T& first, T& second )
    {
        auto cpy( std::move( first ) );
        first = std::move( second );
        second = std::move( cpy );
    }

    template< class Source_It, class Dest_It, class Func >
    constexpr void transform( Source_It src_begin, const Source_It src_end, Dest_It dest_begin, Func func )
    {
        for( ; src_begin != src_end; ++src_begin, ++dest_begin )
        {
            *dest_begin = func( *src_begin );
        }
    }

    template< class Source_It1, class SourceIt2, class Dest_It, class Func >
    constexpr void transform( Source_It1 begin1, Source_It1 const end1, SourceIt2 begin2, Dest_It dest_begin, Func func )
    {
        for ( ; begin1 != end1; ++begin1, ++begin2, ++dest_begin )
        {
            *dest_begin = func( *begin1, *begin2 );
        }
    }

    template< class It, class Comp >
    constexpr It find( It begin, const It end, Comp const& val )
    {
        for ( ; begin != end; ++begin )
        {
            if ( *begin == val )
                return begin;
        }
        return begin;
    }

    template< class It, class Func >
    constexpr It findIf( It begin, const It end, Func condition )
    {
        for ( ; begin != end; ++begin )
        {
            if ( condition( *begin ) )
                return begin;
        }
        return begin;
    }

    template< class It, class Func >
    constexpr void forEach( It begin, const It end, Func func )
    {
        for ( ; begin != end; ++begin )
        {
            func( *begin );
        }
    }

    template< class It, class Func >
    constexpr void partition( It begin, const It end, Func pred )
    {
        auto oneAfterPartition( begin );
        for ( ; begin != end; ++begin )
        {
            if ( pred( *begin ) )
            {
                if ( begin != oneAfterPartition )
                {
                    swap( *oneAfterPartition, *begin );
                }

                ++oneAfterPartition;
            }
        }
    }

    template< class It >
    constexpr void sort( It begin, It end )
    {
        return std::sort( begin, end );
    }

    template< class It >
    constexpr bool isSorted( It begin, It const end )
    {
        if ( begin == end )
            return true;

        auto next = begin;
        ++next;

        for ( ; next != end; ++begin, ++next )
        {
            if ( *begin > *next )
                return false;
        }

        return true;
    }
}
