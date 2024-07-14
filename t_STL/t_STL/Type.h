#pragma once

#include "Tint.h"

#include <type_traits>
#include <bit>

namespace t
{
    struct type
    {
    public:
        template< class T, class U >
        static constexpr bool is_same = std::is_same_v< T, U >;

    private:
        template< class... T >
        struct any_of;

        template< class T, class... U >
        struct any_of< T, U... > : any_of< U... >
        {
            using MyType = T;
            using BaseType = any_of< U... >;
        };

        template< class Ty, class any >
        static constexpr bool is_any_of_types()
        {
            if constexpr ( is_same< typename any::MyType, void > )
            {
                return false;
            }
            else if constexpr ( is_same< Ty, typename any::MyType > )
            {
                return true;
            }
            else
            {
                return is_any_of_types< Ty, typename any::BaseType >();
            }
        }
    public:
        template< class T, class... U >
        static constexpr bool is_any_of = is_any_of_types< T, any_of< U... > >();

        template< class T, class... U >
        static constexpr bool is_none_of = !is_any_of< T, U... >;

    public:

        template< class T >
        static constexpr bool is_reference = std::is_reference_v< T >;

        template< class T >
        static constexpr bool is_lvalue_reference = std::is_lvalue_reference_v< T >;

        template< class T >
        static constexpr bool is_rvalue_reference = std::is_rvalue_reference_v< T >;

        template< class T >
        static constexpr bool is_pointer = std::is_pointer_v< T >;

        template< class T >
        static constexpr bool is_const = std::is_const_v< T >;

        template< class T >
        static constexpr bool is_array = std::is_array_v< T >;

        template< class T >
        static constexpr bool is_volatile = std::is_volatile_v< T >;

#ifdef _MSC_VER
        template< class T >
        static constexpr bool is_integer = std::template _Is_standard_integer< T >;
#else
        template< class T >
        static constexpr bool is_integer = std::template __is_standard_integer< T >::value;
#endif
        template< class T >
        static constexpr bool is_signed = std::is_signed_v< T >;

        template< class T >
        static constexpr bool is_unsigned = std::is_unsigned_v< T >;

        template< class T >
        static constexpr bool is_floating_point = std::is_floating_point_v< T >;

        template< class T >
        static constexpr bool is_function = std::is_function_v< T >;

        template< class T >
        static constexpr bool is_arithmetic = std::is_arithmetic_v< T >;

        template< class T >
        static constexpr bool is_plain_type = true;

        template< class T >
        static constexpr bool is_plain_type< T& > = false;

        template< class T >
        static constexpr bool is_plain_type< T const > = false;

        template< class T >
        static constexpr bool is_plain_type< volatile T > = false;

        template< class T >
        static constexpr bool is_plain_type< T* > = false;

        template< class T >
        static constexpr bool is_plain_type< T[] > = false;

        template< class T, uint64 N >
        static constexpr bool is_plain_type< T[ N ] > = false;

        template< class T >
        using remove_reference = std::remove_reference_t< T >;

        template< class T >
        using make_rvalue_reference = std::add_rvalue_reference_t< T >;

        template< class T >
        using make_const_reference = std::add_const_t< std::add_lvalue_reference_t< T > >;

        template< class T >
        using make_lvalue_reference = std::add_lvalue_reference_t< T >;

        template< class T >
        using remove_pointer = std::remove_pointer_t< T >;

        template< class T >
        using add_pointer = std::add_pointer_t< T >;

        template< class T >
        using remove_const = std::remove_const_t< T >;

        template< class T >
        using add_const = std::add_const_t< T >;

        template< class T >
        using remove_array = std::remove_all_extents_t< T >;

        template< class T >
        using remove_volatile = std::remove_volatile_t< T >;

        template< class T >
        using decay = std::decay_t< T >;

        template< bool Cond, class T, class U >
        using ternary = std::conditional_t< Cond, T, U >;

    private:
        enum class Removable
        {
            NONE,
            CONST,
            VOLATILE,
            REFERENCE,
            ARRAY,
            POINTER,
        };
        template< typename T >
        static consteval Removable getRemovableType()
        {
            if constexpr ( type::is_reference< T > )
            {
                return Removable::REFERENCE;
            }
            if constexpr ( type::is_const< T > )
            {
                return Removable::CONST;
            }
            if constexpr ( type::is_pointer< T > )
            {
                return Removable::POINTER;
            }
            if constexpr ( type::is_array< T > )
            {
                return Removable::ARRAY;
            }
            if constexpr ( type::is_volatile< T > )
            {
                return Removable::VOLATILE;
            }
            return Removable::NONE;
        }
        template< class T, Removable r >
        struct type_remover;

        template< class T >
        struct type_remover< T, Removable::NONE >
        {
            using type = T;
        };

        template< class T >
        struct type_remover< T, Removable::CONST >
        {
            using temp = type::remove_const< T >;
            using type = type_remover< temp, getRemovableType< temp >() >::type;
        };

        template< class T >
        struct type_remover< T, Removable::REFERENCE >
        {
            using temp = type::remove_reference< T >;
            using type = type_remover< temp, getRemovableType< temp >() >::type;
        };

        template< class T >
        struct type_remover< T, Removable::POINTER >
        {
            using temp = type::remove_pointer< T >;
            using type = type_remover< temp, getRemovableType< temp >() >::type;
        };

        template< class T >
        struct type_remover< T, Removable::ARRAY >
        {
            using temp = type::remove_array< T >;
            using type = type_remover< temp, getRemovableType< temp >() >::type;
        };

        template< class T >
        struct type_remover< T, Removable::VOLATILE >
        {
            using temp = type::remove_volatile< T >;
            using type = type_remover< temp, getRemovableType< temp >() >::type;
        };

    public:

        template< class T >
        using plain = type_remover< T, getRemovableType< T >() >::type;

        template< class T >
        static constexpr bool is_expensive_hash = is_none_of< T,
            char,
            int8,  int16,  int32,  int64,
            uint8, uint16, uint32, uint64,
            float, double >;

        template< bool Cond, class T = void >
        using enable_if = std::enable_if< Cond, T >::type;
    };

    template<>
    struct type::any_of<>
    {
        using MyType = void;
        using BaseType = void;
    };

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
    struct hasher< char >
    {
        constexpr static uint64 hash( char val )
        {
            return uint64( val );
        }
    };

    template<>
    struct hasher< float >
    {
        constexpr static inline uint64 hash( float val )
        {
            return uint64( ::std::bit_cast< int32 >( val ) );
        }
    };

    template<>
    struct hasher< double >
    {
        constexpr static inline uint64 hash( double val )
        {
            return uint64( ::std::bit_cast< int64 >( val ) );
        }
    };
}
