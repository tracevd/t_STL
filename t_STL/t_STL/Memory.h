#pragma once

#include "Tint.h"
#include "Type.h"
#include "Array.h"

namespace t
{
	template< typename T >
	class UniquePtr
	{
	public:
		constexpr UniquePtr() = default;
		UniquePtr( const UniquePtr& al ) = delete;
		UniquePtr& operator=( const UniquePtr& rhs ) = delete;

		constexpr UniquePtr( T* allocation ):
			m_data( allocation ) {}

		constexpr UniquePtr( const T& in ) :
			m_data( new T( in ) ) {}

		constexpr UniquePtr( T&& in ) :
			m_data( new T( move( in ) ) ) {}

		constexpr UniquePtr( UniquePtr&& other ) noexcept
		{
			m_data = other.m_data;
			other.m_data = nullptr;
		}

		constexpr UniquePtr& operator=( UniquePtr&& rhs ) noexcept
		{
			if ( this == &rhs )
				return *this;
			DestroyData();
			m_data = rhs.m_data;
			rhs.m_data = nullptr;
			return *this;
		}

		constexpr UniquePtr& operator=( T* allocation ) noexcept
		{
			if ( allocation == m_data )
				return *this;
			DestroyData();

			m_data = allocation;
			return *this;
		}

		constexpr ~UniquePtr()
		{
			DestroyData();
		}

		constexpr T* release() { auto cpy_ptr = m_data; m_data = nullptr; return cpy_ptr; }

		constexpr T* get() const noexcept { return m_data; }

		constexpr const T* operator->() const noexcept { return m_data; }

		constexpr T* operator->() noexcept { return m_data; }

		constexpr T& operator*() { return *m_data; }

		constexpr T const& operator*() const { return *m_data; }

		constexpr bool operator==( const UniquePtr& rhs ) const { return m_data == rhs.m_data; }

		constexpr bool operator!=( const UniquePtr& rhs ) const { return m_data != rhs.m_data; }

		constexpr bool operator< ( const UniquePtr& rhs ) const { return m_data < rhs.m_data; }

		constexpr bool operator>=( const UniquePtr& rhs ) const { return m_data >= rhs.m_data; }

		constexpr bool operator> ( const UniquePtr& rhs ) const { return m_data > rhs.m_data; }

		constexpr bool operator<=( const UniquePtr& rhs ) const { return m_data <= rhs.m_data; }

		constexpr bool operator==( const void* const rhs ) const { return m_data == rhs; }

		constexpr bool operator!=( const void* const rhs ) const { return m_data != rhs; }

		constexpr bool operator< ( const void* const rhs ) const { return m_data < rhs; }

		constexpr bool operator>=( const void* const rhs ) const { return m_data >= rhs; }

		constexpr bool operator> ( const void* const rhs ) const { return m_data > rhs; }

		constexpr bool operator<=( const void* const rhs ) const { return m_data <= rhs; }

		constexpr operator bool() const { return m_data != nullptr; }
	private:
		constexpr void DestroyData()
		{
			if constexpr ( type::is_array< T > )
			{
				delete[] m_data;
			}
			else
			{
				delete m_data;
			}

			m_data = nullptr;
		}
	private:
		T* m_data = nullptr;
	};

	template< typename T >
	class SharedPtr
	{
	public:
		constexpr SharedPtr() = default;

		constexpr SharedPtr( const T& in ):
			m_data( new T( in ) ),
			m_refCount( new uint64( 1 ) ) {}

		constexpr SharedPtr( T&& in ):
			m_data( new T( move( in ) ) ),
			m_refCount( new uint64( 1 ) ) {}

		constexpr SharedPtr( T* allocation ):
			m_data( allocation ),
			m_refCount( new uint64( 1 ) ) {}

		constexpr SharedPtr( const SharedPtr& ptr ):
			m_data( ptr.m_data ),
			m_refCount( ptr.m_refCount )
		{
			if ( m_data != nullptr && m_refCount != nullptr )
			{
				*m_refCount += 1;
			}
		}

		constexpr SharedPtr( SharedPtr&& ptr ) noexcept:
			m_data( ptr.m_data ),
			m_refCount( ptr.m_refCount )
		{
			ptr.m_data = nullptr;
			ptr.m_refCount = nullptr;
		}

		constexpr ~SharedPtr()
		{
			DestroyData();
		}

		constexpr SharedPtr& operator=( SharedPtr const& rhs )
		{
			if ( this == &rhs || m_data == rhs.m_data )
				return *this;
			DestroyData();

			m_data = rhs.m_data;
			m_refCount = rhs.m_refCount;

			if ( rhs.m_data != nullptr && m_refCount != nullptr )
				*m_refCount += 1;

			return *this;
		}

		constexpr SharedPtr& operator=( SharedPtr&& rhs ) noexcept
		{
			if ( this == &rhs )
				return *this;
			DestroyData();

			m_data = rhs.m_data;
			m_refCount = rhs.m_refCount;
			rhs.m_data = nullptr;
			rhs.m_refCount = nullptr;

			return *this;
		}

		constexpr bool isUnique() const noexcept
		{
			if ( m_data == nullptr || m_refCount == nullptr )
				return true;
			return *m_refCount == 1;
		}

		constexpr bool isShared() const noexcept
		{
			return !isUnique();
		}

		constexpr const T* get() const noexcept { return m_data; }

		constexpr T* get() noexcept { return m_data; }

		constexpr const T* operator->() const noexcept { return get(); }

		constexpr T* operator->() noexcept { return get(); }

		constexpr T& operator*() { return *m_data; }

		constexpr T const& operator*() const { return *m_data; }

		constexpr bool operator==( const SharedPtr& rhs ) const noexcept { return m_data == rhs.m_data; }

		constexpr bool operator!=( const SharedPtr& rhs ) const noexcept { return m_data != rhs.m_data; }

		constexpr bool operator< ( const SharedPtr& rhs ) const noexcept { return m_data < rhs.m_data; }

		constexpr bool operator>=( const SharedPtr& rhs ) const noexcept { return m_data >= rhs.m_data; }

		constexpr bool operator> ( const SharedPtr& rhs ) const noexcept { return m_data > rhs.m_data; }

		constexpr bool operator<=( const SharedPtr& rhs ) const noexcept { return m_data <= rhs.m_data; }

		constexpr bool operator==( const T* const rhs )   const noexcept { return getPointer() == rhs; }

		constexpr bool operator!=( const T* const rhs )   const noexcept { return getPointer() != rhs; }

		constexpr bool operator< ( const T* const rhs )   const noexcept { return getPointer() < rhs; }

		constexpr bool operator>=( const T* const rhs )   const noexcept { return getPointer() >= rhs; }

		constexpr bool operator> ( const T* const rhs )   const noexcept { return getPointer() > rhs; }

		constexpr bool operator<=( const T* const rhs )   const noexcept { return getPointer() <= rhs; }

		constexpr bool operator==( std::nullptr_t )       const noexcept { return m_data == nullptr; }

		constexpr bool operator!=( std::nullptr_t )       const noexcept { return m_data != nullptr; }

		constexpr operator bool() const noexcept { return m_data != nullptr; }
	private:
		constexpr void DestroyData()
		{
			if ( m_data == nullptr || m_refCount == nullptr )
				return;

			*m_refCount -= 1;

			if ( *m_refCount != 0 )
				return;
				
			delete m_refCount;

			if constexpr ( type::is_array< T > )
				delete[] m_data;
			else
				delete m_data;

			m_data = nullptr;
			m_refCount = nullptr;
		}
		constexpr inline const void* const getPointer() const { return m_data; }

	private:
		T* m_data = nullptr;
		uint64* m_refCount = nullptr;
	};

	template< class T >
	class ImmutableSharedPtr
	{
	public:
		constexpr ImmutableSharedPtr() = default;

		constexpr ImmutableSharedPtr( ImmutableSharedPtr&& other ) noexcept:
			m_data( other.m_data ),
			m_refCount( other.m_refCount )
		{
			other.m_data = nullptr;
			other.m_refCount = nullptr;
		}

		constexpr ImmutableSharedPtr( ImmutableSharedPtr const& other ):
			m_data( other.m_data ),
			m_refCount( other.m_refCount )
		{
			if ( m_refCount != nullptr )
				*m_refCount += 1;
		}

		constexpr ImmutableSharedPtr( T* allocation ):
			m_data( allocation ),
			m_refCount( new uint64( 1 ) ) {}

		constexpr ImmutableSharedPtr( T&& data ) noexcept:
			m_data( new T( std::move( data ) ) ),
			m_refCount( new uint64( 1 ) ) {};

		constexpr ImmutableSharedPtr( T const& data ):
			m_data( new T( data ) ),
			m_refCount( new uint64( 1 ) ) {}

		constexpr ~ImmutableSharedPtr()
		{
			DestroyData();
		}

		constexpr ImmutableSharedPtr& operator=( ImmutableSharedPtr&& rhs ) noexcept
		{
			DestroyData();

			m_data = rhs.m_data;
			m_refCount = rhs.m_refCount;

			rhs.m_data = nullptr;
			rhs.m_refCount = nullptr;

			return *this;
		}

		constexpr ImmutableSharedPtr& operator=( ImmutableSharedPtr const& rhs )
		{
			DestroyData();

			m_data = rhs.m_data;
			m_refCount = rhs.m_refCount;

			if ( m_refCount != nullptr )
				*m_refCount += 1;

			return *this;
		}

		constexpr const T* get() const { return m_data; }

		constexpr T* get()
		{
			if ( m_data == nullptr || m_refCount == nullptr )
				return m_data;

			if ( *m_refCount == 1 )
				return m_data;

			auto cpy = *m_data;

			*m_refCount -= 1;

			m_refCount = new uint64( 1 );

			m_data = new T( std::move( cpy ) );

			return m_data;
		}

		constexpr T const& operator*() const { return *m_data; }

		constexpr T& operator*() { return *get(); }

		constexpr const T* operator->() const noexcept { return m_data; }

		constexpr T* operator->() noexcept { return get(); }

		constexpr bool isUnique() const noexcept
		{
			if ( m_data == nullptr || m_refCount == nullptr )
				return true;
			return *m_refCount == 1;
		}

		constexpr bool isShared() const noexcept
		{
			return !isUnique();
		}

	private:
		constexpr void DestroyData()
		{
			if ( m_data == nullptr || m_refCount == nullptr )
				return;

			*m_refCount -= 1;

			if ( *m_refCount != 0 )
				return;
			
			delete m_refCount;

			if constexpr ( type::is_array< T > )
				delete[] m_data;
			else
				delete m_data;

			m_data = nullptr;
			m_refCount = nullptr;
		}
	private:
		T* m_data = nullptr;
		uint64* m_refCount = nullptr;
	};

	template< class T, class... Args, class = type::enable_if< !type::is_array< T > > >
	constexpr UniquePtr< T > make_unique( Args&&... args )
	{
		return UniquePtr< T >( new T( std::forward< Args >( args )... ) );
	}

	template< class T, class... Elem, class = type::enable_if< type::is_array< T > > >
	constexpr auto make_unique( Elem&&... args )
	{
		using raw_t = type::remove_array< T >;
		constexpr uint64 size = sizeof( T ) / sizeof( raw_t );
		using ArrayTy = t::Array< raw_t, size >;
		return UniquePtr< ArrayTy >( new ArrayTy( { std::forward< Elem >( args )... } ) );
	}

	template< class T, class... Args, class = std::enable_if_t< !type::is_array< T > > >
	constexpr SharedPtr< T > make_shared( Args&&... args )
	{
		return SharedPtr< T >( new T( std::forward< Args >( args )... ) );
	}

	template< class T, class... Elem, class = type::enable_if< type::is_array< T > > >
	constexpr auto make_shared( Elem&&... args )
	{
		using raw_t = type::remove_array< T >;
		constexpr uint64 size = sizeof( T ) / sizeof( raw_t );
		using ArrayTy = t::Array< raw_t, size >;
		return SharedPtr< ArrayTy >( new ArrayTy( { std::forward< Elem >( args )... } ) );
	}

	template< class T, class... Args, class = type::enable_if< !type::is_array< T > > >
	constexpr ImmutableSharedPtr< T > make_immutable_shared( Args&&... args )
	{
		return ImmutableSharedPtr< T >( new T( std::forward< Args >( args  )... ) );
	}

	template< class T, class... Elem, class = type::enable_if< type::is_array< T > > >
	constexpr auto make_immutable_shared( Elem&&... args )
	{
		using raw_t = type::remove_array< T >;
		constexpr uint64 size = sizeof( T ) / sizeof( raw_t );
		using ArrayTy = t::Array< raw_t, size >;
		return ImmutableSharedPtr< ArrayTy >( new ArrayTy( { std::forward< Elem >( args )... } ) );
	}
}
