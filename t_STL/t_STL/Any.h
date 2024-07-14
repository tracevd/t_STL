#pragma once

#include <typeinfo>
#include "Type.h"
#include "Tint.h"
#include "Error.h"
#include "Optional.h"
#include "utility.h"

namespace t
{
	class Any
	{
	public:
		Any() = default;

		Any( Any const& other ):
			m_ptr( other.m_copyer( other.m_ptr ) ),
			m_deleter( other.m_deleter ),
			m_copyer( other.m_copyer ),
			m_type( other.m_type ) {}

		Any( Any&& other ) noexcept:
			m_ptr( exchange( other.m_ptr, nullptr ) ),
			m_deleter( exchange( other.m_deleter, nullptr ) ),
			m_copyer( exchange( other.m_copyer, empty_copyer ) ),
			m_type( exchange( other.m_type, nullptr ) ) {}

		Any& operator=( Any const& rhs )
		{
			if ( this == &rhs )
				return *this;

			destroyData();

			m_ptr     = rhs.m_copyer( rhs.m_ptr );
			m_deleter = rhs.m_deleter;
			m_copyer  = rhs.m_copyer;
			m_type    = rhs.m_type;

			return *this;
		}

		Any& operator=( Any&& rhs ) noexcept
		{
			if ( this == &rhs )
				return *this;

			destroyData();
			
			m_ptr     = exchange( rhs.m_ptr, nullptr );
			m_deleter = exchange( rhs.m_deleter, nullptr );
			m_copyer  = exchange( rhs.m_copyer, empty_copyer );
			m_type    = exchange( rhs.m_type, nullptr );

			return *this;
		}

		~Any()
		{
			if ( m_deleter && m_ptr )
			{
				m_deleter( m_ptr );
			}
		}

		template< typename T >
		explicit Any( T const& val ) noexcept( std::is_nothrow_copy_constructible_v< T > ):
			m_ptr ( make_value< T >( val ) ),
			m_deleter( make_deleter< T >() ),
			m_copyer( make_copyer< T >() ),
			m_type( &typeid( T ) )
		{
			static_assert( !t::type::is_array< t::type::remove_reference< T > >, "Cannot make t::Any from an array" );
		}

		template< typename T >
		explicit Any( T&& val ) noexcept( std::is_nothrow_move_constructible_v< T > ):
			m_ptr( make_value< T >( std::move( val ) ) ),
			m_deleter( make_deleter< T >() ),
			m_copyer( make_copyer< T >() ),
			m_type( &typeid( T ) )
		{
			static_assert(!t::type::is_array< t::type::remove_reference< T > >, "Cannot make t::Any from an array");
		}

		template< typename T >
		Any& operator=( T const& val )
		{
			static_assert( !t::type::is_array< t::type::remove_reference< T > >, "Cannot make t::Any from an array" );

			if ( m_ptr == &val )
			{
				return *this;
			}

			destroyData();

			*this = Any( val );

			return *this;
		}

		template< typename T >
		Any& operator=( T&& val )
		{
			static_assert(!t::type::is_array< t::type::remove_reference< T > >, "Cannot make t::Any from an array");

			if ( m_ptr == &val )
			{
				return *this;
			}

			destroyData();

			*this = Any( std::move( val ) );

			return *this;
		}

		template< typename T >
		T& get()
		{
			if ( m_type && typeid( T ) != *m_type )
			{
				throw Error( "bad any cast", 1 );
			}

			return *static_cast< T* >( m_ptr );
		}

		template< typename T >
		T const& get() const
		{
			if ( m_type && typeid(T) != *m_type )
			{
				throw Error( "bad any cast", 1 );
			}

			return *static_cast< T const* >( m_ptr );
		}

		template< typename T >
		T* tryGet()
		{
			if ( m_type && typeid( T ) != *m_type )
			{
				return nullptr;
			}

			return static_cast< T* >( m_ptr );
		}

		template< typename T >
		T const* tryGet() const
		{
			if ( m_type && typeid( T ) != *m_type )
			{
				return nullptr;
			}

			return static_cast< T const* >( m_ptr );
		}

		void clear()
		{
			destroyData();

			m_ptr = nullptr;
			m_deleter = nullptr;
			m_type = nullptr;
		}

	private:
		void destroyData()
		{
			if ( m_deleter && m_ptr )
			{
				m_deleter( m_ptr );
			}

			m_ptr = nullptr;
		}

		template< typename T, typename... Args >
		static auto make_value( Args&&... args )
		{
			return new T( std::forward< Args >( args )... );
		}

		template< typename T >
		static consteval auto make_deleter()
		{
			return []( void* val )
			{
				delete static_cast< T* >( val );
			};
		}

		template< typename T >
		static consteval auto make_copyer()
		{
			return []( void const* ptr )
			{
				return static_cast< void* >( new T( *static_cast< T const* >( ptr ) ) );
			};
		}

	private:
		static constexpr auto empty_copyer = []( void const* ) -> void* { return nullptr; };

	private:
		void* m_ptr = nullptr;
		void ( *m_deleter )( void* ) = nullptr;
		void* ( *m_copyer )( void const* ) = empty_copyer;
		std::type_info const* m_type = nullptr;
	};
}
