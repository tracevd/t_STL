#pragma once

#include <algorithm>

#include "utility.h"
#include "Pair.h"

namespace t
{
    template< class T >
    constexpr auto begin( T&& val )
    {
        return val.begin();
    }

    template< class T >
    constexpr auto end( T&& val )
    {
        return val.end();
    }

    template< class T >
    constexpr auto cbegin( T&& val )
    {
        return val.cbegin();
    }

    template< class T >
    constexpr auto cend( T&& val )
    {
        return val.cend();
    }

    template< class T, uint64 Size >
    constexpr auto begin( T( &arr )[ Size ] )
    {
        return &arr[ 0 ];
    }

    template< class T, uint64 Size >
    constexpr auto end( T( &arr )[ Size ] )
    {
        return ( &arr[ 0 ] ) + Size;
    }

    template< class T, uint64 Size >
    constexpr auto const* cbegin( T( &arr )[ Size ] )
    {
        return &arr[ 0 ];
    }

    template< class T, uint64 Size >
    constexpr auto const* cend( T( &arr )[ Size ] )
    {
        return ( &arr[ 0 ] ) + Size;
    }

    /*
     * Takes all of the elements from source and puts them into destination, converting them with func
     */
    template< class Source_It, class Dest_It, class Func >
    constexpr void transform( Source_It src_begin, Source_It const src_end, Dest_It dest_begin, Func func )
    {
        for( ; src_begin != src_end; ++src_begin, ++dest_begin )
        {
            *dest_begin = func( *src_begin );
        }
    }

    /*
     * Takes all of the elements from source and puts them into destination, converting them with func
     */
    template< class SourceContainer, class DestContainer, class Func >
    constexpr void transform( SourceContainer& src, DestContainer& dest, Func func )
    {
        transform( begin( src ), end( src ), begin( dest ), func );
    }

    /*
     * Takes all of the elements from both sources (assumed to be the same number of elements) and puts them into destination, converting them with func
     */
    template< class Source_It1, class SourceIt2, class Dest_It, class Func >
    constexpr void transform( Source_It1 begin1, Source_It1 const end1, SourceIt2 begin2, Dest_It dest_begin, Func func )
    {
        for ( ; begin1 != end1; ++begin1, ++begin2, ++dest_begin )
        {
            *dest_begin = func( *begin1, *begin2 );
        }
    }

    /*
     * Takes all of the elements from both sources (assumed to be the same number of elements) and puts them into destination, converting them with func
     */
    template< class T1, class T2, class U, class Func >
    constexpr void transform( T1& source1, T2& source2, U& destination, Func func )
    {
        transform( begin( source1 ), end( source1 ), begin( source2 ), begin( destination ), func );
    }

    /*
     * Finds the first element in the range that matches the value
     */
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

    /*
     * Finds the first element in the container that matches the value
     */
    template< class T, class Comp >
    constexpr auto find( T& container, Comp const& val )
    {
        return find( begin( container ), end( container ), val );
    }

    /*
     * Finds the first element in the range that matches the condition
     */
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

    /*
     * Finds the first element in the container that matches the condition
     */
    template< class T, class Func >
    constexpr auto findIf( T& container, Func condition )
    {
        return findIf( begin( container ), end( container ), condition );
    }

    /*
     * Applies a function to each element in the range
     */
    template< class It, class Func >
    constexpr void forEach( It begin, const It end, Func func )
    {
        for ( ; begin != end; ++begin )
        {
            func( *begin );
        }
    }

    /*
     * Applies a function to each element in the container
     */
    template< class T, class Func >
    constexpr void forEach( T& container, Func func )
    {
        forEach( begin( container ), end( container ), func );
    }

    /**
     * Edits the range such that elements where pred( element ) == true come first.
     * Returns the first element on the "false" side of the partition
     */
    template< class It, class Func >
    constexpr It partition( It begin, const It end, Func pred )
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

        return oneAfterPartition;
    }

    /**
     * Edits the range such that elements where pred( element ) == true come first.
     * Returns an iterator to the first element on the "false" side of the partition
     */
    template< class T, class Func >
    constexpr auto partition( T& container, Func pred )
    {
        return partition( begin( container ), end( container ), pred );
    }

    /**
     * Predicate is a function that accepts two arguments:
     *     The first is an element of the range, the second is the pivot element.
     * The predicate should return true if the element should appear before the pivot,
     * and false if it should appear after the pivot.
     * 
     * The range will be modified as follows:
     *     All elements where pred( e, *pivot ) == true
     *     pivot element
     *     All elements where pred( e, *pivot ) == false
     * 
     * Returns an iterator to the pivot. Assumes pivot is an element in the range
     */
    template< class It, class Func >
    constexpr auto partitionByPivot( It begin, It const end, It pivot, Func pred )
    {
        // empty or one element
        if ( begin == end || begin == end - 1 )
            return pivot;

        // edit the range as follows:
        // [begin, ..., pivotElem, end)
        auto pivotElem = end - 1;
        swap( *pivotElem, *pivot );

        // two elements
        if ( pivotElem == begin + 1 )
        {
            if ( pred( *begin, *pivotElem ) )
            {
                return pivotElem;
            }

            swap( *begin, *pivotElem );
            return begin;
        }

        auto oneAfterPartition( begin );

        // partition over range [begin, pivotElem)
        for ( ; begin != pivotElem; ++begin )
        {
            if ( pred( *begin, *pivotElem ) )
            {
                if ( begin != oneAfterPartition )
                {
                    swap( *oneAfterPartition, *begin );
                }

                ++oneAfterPartition;
            }
        }

        // put the pivot element in the "middle" of the partitioned "halves"
        swap( *pivotElem, *oneAfterPartition );

        return oneAfterPartition;
    }

    /**
     * Predicate is a function that accepts two arguments:
     *     The first is an element of the range, the second is the pivot element.
     * The predicate should return true if the element should appear before the pivot,
     * and false if it should appear after the pivot.
     *
     * The range will be modified as follows:
     *     All elements where pred( e, *pivot ) == true
     *     pivot element
     *     All elements where pred( e, *pivot ) == false
     *
     * Returns an iterator to the pivot. Assumes pivot is an iterator to an element in the container
     */
    template< class T, class It, class Func >
    constexpr auto partitionByPivot( T& container, It pivot, Func pred )
    {
        return partitionByPivot( begin( container ), end( container ), pivot, pred );
    }

    namespace details::algorithm
    {        
        template< typename T >
        constexpr auto iteratorDistance( T it1, T it2 )
        {
            return it1 > it2 ? it1 - it2 : it2 - it1;
        }

        template< typename It >
        constexpr It getMiddle( It begin, It const end )
        {
            auto const distance = details::algorithm::iteratorDistance( begin, end );

            return begin + distance / 2;
        }

        template< typename It, typename Func >
        constexpr void quickSort( It begin, It end, Func elementsAreSorted )
        {
            auto const distance = details::algorithm::iteratorDistance( begin, end );

            if ( distance <= 1 )
                return;

            if ( distance == 2 )
            {
                auto elem1 = begin + 1;

                if ( !elementsAreSorted( *begin, *elem1 ) )
                {
                    swap( *begin, *elem1 );
                }
            }

            if ( distance == 3 )
            {
                auto elem1 = begin + 1;
                auto elem2 = begin + 2;

                bool swappedLastTwo = false;

                if ( !elementsAreSorted( *begin, *elem1 ) )
                {
                    swap( *begin, *elem1 );
                }

                if ( !elementsAreSorted( *elem1, *elem2 ) )
                {
                    swap( *elem1, *elem2 );
                    swappedLastTwo = true;
                }

                if ( swappedLastTwo )
                {
                    if ( !elementsAreSorted( *begin, *elem1 ) )
                    {
                        swap( *begin, *elem1 );
                    }
                }

                return;
            }

            auto pivot = details::algorithm::getMiddle( begin, end );

            pivot = partitionByPivot( begin, end, pivot, elementsAreSorted );

            details::algorithm::quickSort( begin, pivot, elementsAreSorted );
            details::algorithm::quickSort( pivot + 1, end, elementsAreSorted );
        }
    }

    /**
     * Sorts the given range based on the function provided.
     * The function should take two arguments, and the function should return true if the elements are in sorted order
     */
    template< class It, class Func >
    constexpr void quickSort( It begin, It end, Func elementsAreSorted )
    {
        details::algorithm::quickSort( begin, end, elementsAreSorted );
    }

    /**
     * Sorts the range in ascending order
     */
    template< class It >
    constexpr void quickSort( It begin, It end )
    {
        details::algorithm::quickSort( begin, end, []( auto const& lhs, auto const& rhs ){ return lhs < rhs; } );
    }

    /**
     * Sorts the given container based on the function provided.
     * The function should take two arguments, and the function should return true if the elements are in sorted order
     */
    template< class T, class Func >
    constexpr void quickSort( T& container, Func elementsAreSorted )
    {
        details::algorithm::quickSort( begin( container ), end( container ), elementsAreSorted );
    }

    /**
     * Sorts the container in ascending order
     */
    template< class T >
    constexpr void quickSort( T& container )
    {
        details::algorithm::quickSort( begin( container ), end( container ), []( auto const& lhs, auto const& rhs ){ return lhs < rhs; } );
    }

    namespace details::algorithm
    {
        template< class It, class Func >
        constexpr void sort( It begin, It const end, Func elementsAreSorted )
        {
            auto const distance = details::algorithm::iteratorDistance( begin, end );

            if ( distance <= 1 )
                return;

            if ( distance < 16 )
            {
                // insertion sort
                for ( int64 i = 1; i < distance; ++i )
                {
                    auto val = std::move( *( begin + i ) );
                    auto it = begin + ( i - 1 );
                    while ( it >= begin && !elementsAreSorted( *it, *( it + 1 ) ) )
                    {
                        swap( *it, *( it + 1 ) );
                        --it;
                    }
                    *( it + 1 ) = std::move( val );
                }
            }
            else // qsort
            {
                auto pivot = details::algorithm::getMiddle( begin, end );

                pivot = partitionByPivot( begin, end, pivot, elementsAreSorted );

                details::algorithm::sort( begin, pivot, elementsAreSorted );
                details::algorithm::sort( pivot + 1, end, elementsAreSorted );
            }
        }
    }

    template< class It, class Func >
    constexpr void sort( It begin, It const end, Func elementsAreSorted )
    {
        details::algorithm::sort( begin, end, elementsAreSorted );
    }

    /*
     * Checks if the range is sorted based on the comparator
     * Comparator expects two arguments, and should return true if the elements are in sorted order.
     */
    template< class It, class Func >
    constexpr bool isSorted( It begin, It const end, Func func )
    {
        if ( begin == end )
            return true;

        auto next = begin;
        ++next;

        for ( ; next != end; ++begin, ++next )
        {
            if ( !func( *begin, *next ) )
                return false;
        }

        return true;
    }

    /**
     * Checks if the range is sorted in ascending order
     */
    template< class It >
    constexpr bool isSorted( It begin, It const end )
    {
        return isSorted( begin, end, []( auto const& lhs, auto const& rhs ){ return lhs <= rhs; } );
    }

    /*
     * Checks if the container is sorted based on the comparator
     * Comparator expects two arguments, and should return true if the elements are in sorted order.
     */
    template< typename T, class Func >
    constexpr bool isSorted( T const& container, Func func )
    {
        return isSorted( begin( container ), end( container ), func );
    }

    /*
     * Checks if the container is sorted in ascending order
     */
    template< typename T >
    constexpr bool isSorted( T const& container )
    {
        return isSorted( cbegin( container ), cend( container ), []( auto const& lhs, auto const& rhs ){ return lhs <= rhs; } );
    }

    template< class It, class T >
    constexpr void replace( It begin, It const end, T const& toReplace, T const& newValue )
    {
        for ( ; begin != end; ++begin )
        {
            if ( *begin == toReplace )
            {
                *begin = newValue;
            }
        }
    }

    template< class T, class U >
    constexpr void replace( T& container, U const& toReplace, U const& newValue )
    {
        replace( begin( container ), end( container ), toReplace, newValue );
    }

    template< class It, class Func, class T >
    constexpr void replaceIf( It begin, It const end, Func condition, T const& newValue )
    {
        for ( ; begin != end; ++begin )
        {
            if ( condition( *begin ) )
            {
                *begin = newValue;
            }
        }
    }

    template< class T, class Func, class U >
    constexpr void replaceIf( T& container, Func condition, U const& newValue )
    {
        replace_if( begin( container ), end( container ), condition, newValue );
    }

    template< class It, class T, class U >
    constexpr void replaceFirst( It begin, It const end, T const& toReplace, U&& newValue )
    {
        for ( ; begin != end; ++begin )
        {
            if ( *begin == toReplace )
            {
                *begin = std::forward< U >( newValue );
                return;
            }
        }
    }

    template< class T, class U, class V >
    constexpr void replaceFirst( T& container, U const& toReplace, V&& newValue )
    {
        replaceFirst( begin( container ), end( container ), toReplace, std::forward< V >( newValue ) );
    }
}
