#pragma once

#include <cassert>

#include "Tint.h"
#include "Type.h"

namespace t
{
	template< class T >
	struct Allocation
	{
		constexpr T* operator->() { return ptr; }

		constexpr T const* operator->() const { return ptr; }

		constexpr T& operator*() { assert( ptr != nullptr ); return *ptr; }

		constexpr T const& operator*() const { assert( ptr != nullptr ); return *ptr; }

		constexpr void destroy()
		{
			delete ptr;
			ptr = nullptr;
		}

		constexpr operator bool() const { return ptr != nullptr; }
	private:
		T* ptr = nullptr;
	};

	template< class T >
	struct Allocation< T[] >
	{
		constexpr Allocation() = default;

		constexpr Allocation( uint64 numel )
		{
			*this = allocate( numel );
		}

		constexpr Allocation( Allocation const& ) = default;

		constexpr Allocation& operator=( Allocation const& rhs )
		{
			m_numel = rhs.m_numel;
			m_ptr = rhs.m_ptr;
			return *this;
		}

		constexpr T* ptr() { return m_ptr; }
		constexpr const T* ptr() const { return m_ptr; }
		constexpr uint64 numel() const { return m_numel; }

		constexpr void reallocate( uint64 newNumel )
		{
			auto newAlloc = allocate( newNumel );

			for ( uint64 i = 0; i < m_numel; ++i )
				newAlloc[ i ] = std::move( m_ptr[ i ] );

			destroy();

			*this = newAlloc;
		}

		constexpr T& operator[]( uint64 index )
		{
			assert( index < m_numel );
			return m_ptr[ index ];
		}

		constexpr T const& operator[]( uint64 index ) const
		{
			assert( index < m_numel );
			return m_ptr[ index ];
		}

		constexpr void destroy()
		{
			delete[] m_ptr;
			m_ptr = nullptr;
			m_numel = 0;
		}

	private:
		constexpr Allocation( T* alloc, uint64 numel ):
			m_ptr( alloc ),
			m_numel( numel ) {};

		constexpr Allocation allocate( uint64 numel )
		{
			return { new T[ numel ], numel };
		}
	private:
		T* m_ptr = nullptr;
		uint64 m_numel = 0;
	};

	template< class T >
	concept isArray = type::is_array< T >;

	template< class T >
	concept isNotArray = !type::is_array< T >;

	template< isNotArray T, class... Args >
	constexpr Allocation< T > allocate( Args&&... args )
	{
		return { .ptr = new T( std::forward< Args >( args )... ) };
	}
}
