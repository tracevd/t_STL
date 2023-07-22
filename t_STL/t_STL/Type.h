#pragma once

#include "Tint.h"

#include <type_traits>

#define TMPL_T template< class T >
#define TMPL template<>
#define INL_CONSTX_B inline constexpr bool
#define INL_CONSTX_S_B inline constexpr static bool

namespace t
{
    struct type
    {
    public:
        template< class T, class U >
        INL_CONSTX_S_B is_same = std::is_same_v< T, U >;

        TMPL_T INL_CONSTX_S_B is_reference = std::is_reference_v< T >;

        TMPL_T INL_CONSTX_S_B is_lvalue_reference = std::is_lvalue_reference_v< T >;

        TMPL_T INL_CONSTX_S_B is_rvalue_reference = std::is_rvalue_reference_v< T >;

        TMPL_T INL_CONSTX_S_B is_pointer = std::is_pointer_v< T >;

        TMPL_T INL_CONSTX_S_B is_const = std::is_const_v< T >;

        TMPL_T INL_CONSTX_S_B is_array = std::is_array_v< T >;

        TMPL_T INL_CONSTX_S_B is_volatile = std::is_volatile_v< T >;

#ifdef _MSC_VER
        TMPL_T INL_CONSTX_S_B is_integer = std::template _Is_standard_integer< T >;
#else
        TMPL_T INL_CONSTX_S_B is_integer = std::template __is_standard_integer< T >::value;
#endif
        TMPL_T INL_CONSTX_S_B is_signed = std::is_signed_v< T >;

        TMPL_T INL_CONSTX_S_B is_unsigned = std::is_unsigned_v< T >;

        TMPL_T INL_CONSTX_S_B is_floating_point = std::is_floating_point_v< T >;

        TMPL_T INL_CONSTX_S_B is_function = std::is_function_v< T >;

        TMPL_T using remove_reference = std::remove_reference_t< T >;

        TMPL_T using make_rvalue_reference = std::add_rvalue_reference_t< T >;

        TMPL_T using make_const_reference = std::add_const_t< std::add_lvalue_reference_t< T > >;

        TMPL_T using make_lvalue_reference = std::add_lvalue_reference_t< T >;

        TMPL_T using remove_pointer = std::remove_pointer_t< T >;

        TMPL_T using add_pointer = std::add_pointer_t< T >;

        TMPL_T using remove_const = std::remove_const_t< T >;

        TMPL_T using add_const = std::add_const_t< T >;

        TMPL_T using remove_array = std::remove_all_extents_t< T >;

        TMPL_T using remove_volatile = std::remove_volatile_t< T >;

        template< bool Cond, class T, class U >
        using ternary = std::conditional_t< Cond, T, U >;
    /*private:
        template< class T >
        struct plain_
        {
            using no_array = remove_array< T >;
            using no_const = remove_const< no_array >;
            using no_reference = remove_reference< no_const >;
            using temp = remove_pointer< no_reference >;
            using type = ternary<
                            is_pointer< temp > || is_reference< temp > || is_const< temp > || is_array< temp >,
                                plain_< temp >::type,
                                temp >;
        };
    public:
        TMPL_T using plain = plain_< T >::type;*/

        TMPL_T using decay = std::decay_t< T >;

        template< bool case_, class T = void >
        using enable_if = std::enable_if_t< case_, T >;
    };
    
    template< typename T >
    inline constexpr T&& move( T& val )
    {
        return static_cast< T&& >(val);
    }
    
    template< typename T >
    inline constexpr T&& forward( T&& val )
    {
        if constexpr ( type::is_lvalue_reference< T > )
            return static_cast< T& >( val );
        return static_cast< T&& >( val );
    }

    template< typename T >
    inline constexpr T&& forward( T& val )
    {
        if constexpr ( type::is_lvalue_reference< T > )
            return static_cast< T& >( val );
        return static_cast< T&& >( val );
    }

    template< class T >
    struct hasher;

    template<>
    struct hasher< uint64 >
    {
        constexpr static inline uint64 hash( uint64 val )
        {
            return val;
        }
    };

    template<>
    struct hasher< uint32 >
    {
        constexpr static inline uint64 hash( uint32 val )
        {
            return uint64( val );
        }
    };

    template<>
    struct hasher< uint16 >
    {
        constexpr static inline uint64 hash( uint16 val )
        {
            return uint64( val );
        }
    };

    template<>
    struct hasher< uint8 >
    {
        constexpr static inline uint64 hash( uint8 val )
        {
            return uint64( val );
        }
    };

    template<>
    struct hasher< int64 >
    {
        constexpr static inline uint64 hash( int64 val )
        {
            return uint64( val );
        }
    };

    template<>
    struct hasher< int32 >
    {
        constexpr static inline uint64 hash( int32 val )
        {
            return uint64( val );
        }
    };

    template<>
    struct hasher< int16 >
    {
        constexpr static inline uint64 hash( int16 val )
        {
            return uint64( val );
        }
    };

    template<>
    struct hasher< int8 >
    {
        constexpr static inline uint64 hash( int8 val )
        {
            return uint64( val );
        }
    };

    template<>
    struct hasher< float >
    {
        constexpr static inline uint64 hash( float val )
        {
            return uint64( int64( val * 27.7f ) );
        }
    };

    template<>
    struct hasher< double >
    {
        constexpr static inline uint64 hash( double val )
        {
            return uint64( int64( val * 27.7 ) );
        }
    };
};

#undef TMPL
#undef TMPL_T
#undef INL_CONSTX_S_B
#undef INL_CONSTX_B
