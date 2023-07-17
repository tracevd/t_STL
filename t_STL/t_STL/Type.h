#pragma once

#include "Tint.h"

#define TMPL_T template< class T >
#define TMPL template<>
#define INL_CONSTX_B inline constexpr static bool

namespace t
{
    struct type
    {
    public:
        template< class, class >
        INL_CONSTX_B is_same                = false;
        TMPL_T INL_CONSTX_B is_same< T, T > = true;

        TMPL_T INL_CONSTX_B is_reference              = false;
        TMPL_T INL_CONSTX_B is_reference< T& >        = true;
        TMPL_T INL_CONSTX_B is_reference< T&& >       = true;
        TMPL_T INL_CONSTX_B is_reference< T const& >  = true;
        TMPL_T INL_CONSTX_B is_reference< T const&& > = true;

        TMPL_T INL_CONSTX_B is_lvalue_reference             = false;
        TMPL_T INL_CONSTX_B is_lvalue_reference< T& >       = true;
        TMPL_T INL_CONSTX_B is_lvalue_reference< T const& > = true;

        TMPL_T INL_CONSTX_B is_rvalue_reference              = false;
        TMPL_T INL_CONSTX_B is_rvalue_reference< T&& >       = true;
        TMPL_T INL_CONSTX_B is_rvalue_reference< T const&& > = true;

        TMPL_T INL_CONSTX_B is_pointer                   = false;
        TMPL_T INL_CONSTX_B is_pointer< T* >             = true;
        TMPL_T INL_CONSTX_B is_pointer< T const* >       = true;
        TMPL_T INL_CONSTX_B is_pointer< T const* const > = true;

        TMPL_T INL_CONSTX_B is_const            = false;
        TMPL_T INL_CONSTX_B is_const< T const > = true;

        TMPL_T INL_CONSTX_B is_array        = false;
        TMPL_T INL_CONSTX_B is_array< T[] > = true;

        template< class T, size_t N >
        INL_CONSTX_B is_array< T[ N ] > = true;

        TMPL_T INL_CONSTX_B is_volatile               = false;
        TMPL_T INL_CONSTX_B is_volatile< volatile T > = true;

        TMPL_T INL_CONSTX_B is_integer         = false;
        TMPL INL_CONSTX_B is_integer< int8  >  = true;
        TMPL INL_CONSTX_B is_integer< int16 >  = true;
        TMPL INL_CONSTX_B is_integer< int32 >  = true;
        TMPL INL_CONSTX_B is_integer< int64 >  = true;
        TMPL INL_CONSTX_B is_integer< uint8 >  = true;
        TMPL INL_CONSTX_B is_integer< uint16 > = true;
        TMPL INL_CONSTX_B is_integer< uint32 > = true;
        TMPL INL_CONSTX_B is_integer< uint64 > = true;

        // only function types and reference types can't be const qualified 
        TMPL_T INL_CONSTX_B is_function = !is_const< const T > && !is_reference< T >;
    private:
        // References
        TMPL_T struct remove_reference_             { using type = T; };
        TMPL_T struct remove_reference_< T& >       { using type = T; };
        TMPL_T struct remove_reference_< T&& >      { using type = T; };
        TMPL_T struct remove_reference_< T const& > { using type = T const; };
    public:
        TMPL_T using remove_reference = typename remove_reference_< T >::type;
    private:
        TMPL_T struct make_reference_ { using type = T&; };
    public:
        TMPL_T using make_reference = typename make_reference_< T >::type;
    private:
        TMPL_T struct make_rvalue_reference_       { using type = T&&; };
        TMPL_T struct make_rvalue_reference_< T& > { using type = T&&; };
    public:
        TMPL_T using make_rvalue_reference = typename make_rvalue_reference_< T >::type;
    private:
        TMPL_T struct make_const_reference_        { using type = T const&; };
        TMPL_T struct make_const_reference_< T& >  { using type = T const&; };
        TMPL_T struct make_const_reference_< T&& > { using type = T const&; };
    public:
        TMPL_T using make_const_reference = typename make_const_reference_< T >::type;
    private:
        // Pointer
        TMPL_T struct remove_pointer_             { using type = T; };
        TMPL_T struct remove_pointer_< T* >       { using type = T; };
        TMPL_T struct remove_pointer_< T* const > { using type = T; };
    public:
        TMPL_T using remove_pointer = remove_pointer_< T >::type;
    private:
        TMPL_T struct add_pointer_ { using type = T*; };
    public:
        TMPL_T using add_pointer = typename add_pointer_< T >::type;
    private:
         // Const
        TMPL_T struct remove_const_              { using type = T; };
        TMPL_T struct remove_const_< T const >   { using type = T; };
        TMPL_T struct remove_const_< T const& >  { using type = T&; };
        TMPL_T struct remove_const_< T const&& > { using type = T&&; };
    public:
        TMPL_T using remove_const = typename remove_const_< T >::type;
    private:
        // Array
        TMPL_T struct remove_array_        { using type = T; };
        TMPL_T struct remove_array_< T[] > { using type = T; };
        template< class T, size_t N >
        struct remove_array_< T[N] >       { using type = T; };
    public:
        TMPL_T using remove_array = typename remove_array_< T >::type;
    private:
        // Volatile
        TMPL_T struct remove_volatile_               { using type = T; };
        TMPL_T struct remove_volatile_< volatile T > { using type = T; };
    public:
        TMPL_T using remove_volatile = typename remove_volatile_< T >::type;
    private:
        // Select
        template< bool, class T, class >
        struct ternary_                { using type = T; };
        template< class T, class U >
        struct ternary_< false, T, U > { using type = U; };
    public:
        template< bool b, class T, class U >
        using ternary = typename ternary_< b, T, U >::type;
    private:
        template< class T >
        struct plain_
        {
            using temp = remove_pointer< remove_reference< remove_const< remove_array< T > > > >;
            using type = ternary< is_pointer< temp > ||
                                  is_reference< temp > ||
                                  is_const< temp > ||
                                  is_array< temp >,
                                    typename plain_< temp >::type,
                                    temp >;
        };
    public:
        TMPL_T using plain = typename plain_< T >::type;
    private:
        template< class T >
        struct decay_
        {
            using t0 = remove_reference< T >;
            using t1 = ternary< is_function< t0 >,
                            add_pointer< remove_array< t0 > >,
                            remove_const< remove_volatile< t0 > > >;
            using type = typename ternary< is_array< t1 >,
                            add_pointer< remove_array< t1 > >,
                            t1 >;
        };
    public:
        TMPL_T using decay = decay_< T >::type;
    private:
        template< bool, class >
        struct enable_if_ {};
        template< class T >
        struct enable_if_< true, T > { using type = T; };
    public:
        template< bool case_, class T = void >
        using enable_if = enable_if_< case_, T >::type;
    };
    
    template< typename T >
    inline constexpr T&& move( T& val )
    {
        return static_cast<T&&>(val);
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
};

#undef TMPL_T
#undef INL_CONSTX_B
