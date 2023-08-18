#pragma once

#include "Type.h"
#include "Tint.h"
#include "Pair.h"

namespace t
{
	template< class... Types >
	struct tuple;

	template<>
	struct tuple<>
	{
	};

	template< class T, class... Types >
	struct tuple< T, Types... > : private tuple< Types... >
	{
		T val {};

		using BaseType = tuple< Types... >;

		constexpr tuple( T const& val, Types&&... args ):
			val( val ),
			BaseType( std::forward< Types >( args )... ) {}

		template< uint64 N >
		constexpr auto& get()
		{
			if constexpr ( N == 0 )
				return val;
			else
				return ( *this )->template get< N - 1 >();
		}

		template< uint64 N >
		constexpr auto& get() const
		{
			if constexpr ( N == 0 )
				return val;
			else
				return ( *this )->template get< N - 1 >();
		}

		template< uint64 N, class TypeAlias, class = std::enable_if_t< !type::is_reference< TypeAlias > || type::is_const< type::remove_reference< TypeAlias > > > >
		constexpr TypeAlias getAs() const
		{
			if constexpr ( N == 0 )
				return static_cast< TypeAlias >( val );
			else
				return (*this)->template getAs< N - 1, TypeAlias >();
		}
		
		template< uint64 N, class TypeAlias >
		constexpr TypeAlias getAs()
		{
			if constexpr ( N == 0 )
				return static_cast< TypeAlias >( val );
			else
				return (*this)->template getAs< N-1, TypeAlias >();
		}

		template< class U, uint64 N, class TypeAlias = U >
		constexpr TypeAlias getNthOfType()
		{
			if constexpr ( !type::is_same< T, U > )
				return ( *this )->template getNthOfType< U, N, TypeAlias >();
			else if constexpr ( N == 0 )
			{
				if constexpr ( type::is_same< T, type::remove_const< type::remove_reference< TypeAlias > > > )
					return val;
				else
					return static_cast< TypeAlias >( val );
			}
			else
				return ( *this )->template getNthOfType< U, N - 1, TypeAlias >();
		}

		template< class U, uint64 N, class TypeAlias = U, class = std::enable_if_t< type::is_same< U, TypeAlias > || type::is_const< type::remove_reference< TypeAlias > > > >
		constexpr TypeAlias getNthOfType() const
		{
			if constexpr ( !type::is_same< T, U > )
				return ( *this )->template getNthOfType< U, N, TypeAlias >();

			else if constexpr ( N == 0 )
			{
				if constexpr ( type::is_same< T, type::remove_const< type::remove_reference< TypeAlias > > > )
					return val;
				else
					return static_cast< TypeAlias >( val );
			}
			else
				return ( *this )->template getNthOfType< U, N - 1, TypeAlias >();
		}

		constexpr BaseType* operator->()
		{
			return static_cast< BaseType* >( this );
		}

		constexpr const BaseType* operator->() const
		{
			return static_cast< const BaseType* >( this );
		}

		constexpr uint64 size() const
		{
			if constexpr ( type::is_same< BaseType, tuple<> > )
				return 1;
			else
				return 1 + ( *this )->template size();
		}
	};

	template< class... Types >
	constexpr auto make_tuple( Types&&... args )
	{
		return tuple< Types... >( std::forward< Types >( args )... );
	}
}

static_assert( sizeof( t::tuple< uint32, uint32, uint32 > ) == ( sizeof( uint32 ) * 3 ), "Tuple was not correct size!" );
