#pragma once

#include <compare>

#include "Tint.h"
#include "Type.h"
#include "Array.h"

namespace t
{
	namespace details
	{
		template< typename T >
		class UniquePtrBase
		{
		protected:
			using ValueType = type::remove_array< T >;
		protected:
			constexpr UniquePtrBase() = default;

			constexpr UniquePtrBase( ValueType* alloc ) noexcept:
				m_data( alloc ) {};

			constexpr UniquePtrBase( UniquePtrBase&& ptr ) noexcept:
				m_data( ptr.m_data )
			{
				ptr.m_data = nullptr;
			}

			constexpr UniquePtrBase& operator=( UniquePtrBase&& rhs ) noexcept
			{
				if ( this == &rhs )
					return *this;
				m_data = rhs.m_data;
				rhs.m_data = nullptr;
				return *this;
			}

			constexpr UniquePtrBase& operator=( ValueType* allocation ) noexcept
			{
				if ( allocation == m_data )
					return *this;
				this->DestroyData();

				this->m_data = allocation;
				return *this;
			}
		public:
			UniquePtrBase( UniquePtrBase const& ) = delete;
			UniquePtrBase& operator=( UniquePtrBase const& ) = delete;

			constexpr ~UniquePtrBase() { DestroyData(); }

			[[nodiscard]] constexpr ValueType* release() noexcept { auto cpy_ptr = m_data; m_data = nullptr; return cpy_ptr; }

			[[nodiscard]] constexpr ValueType* get() const noexcept { return m_data; }

			[[nodiscard]] constexpr const ValueType* operator->() const noexcept { return m_data; }

			[[nodiscard]] constexpr ValueType* operator->() noexcept { return m_data; }

			[[nodiscard]] constexpr ValueType& operator*() noexcept { return *m_data; }

			[[nodiscard]] constexpr ValueType const& operator*() const noexcept { return *m_data; }

			[[nodiscard]] constexpr auto operator<=>( UniquePtrBase const& rhs ) const noexcept { return m_data <=> rhs.m_data; }

			[[nodiscard]] constexpr bool operator==( const void* rhs ) const noexcept { return m_data == rhs; }
			[[nodiscard]] constexpr bool operator!=( const void* rhs ) const noexcept { return m_data != rhs; }
			[[nodiscard]] constexpr bool operator==( std::nullptr_t )  const { return m_data == nullptr; }
			[[nodiscard]] constexpr bool operator!=( std::nullptr_t )  const { return m_data != nullptr; }

			[[nodiscard]] constexpr operator bool() const { return m_data != nullptr; }
		protected:
			constexpr void DestroyData()
			{
				if constexpr ( type::is_array< T > )
					delete[] m_data;
				else
					delete m_data;
			}
		protected:
			ValueType* m_data = nullptr;
		};
	}

	template< typename T >
	class UniquePtr : public details::UniquePtrBase< T >
	{
	private:
		using BaseType = details::UniquePtrBase< T >;
	public:
		constexpr UniquePtr() = default;

		constexpr UniquePtr( const T& in ):
			BaseType( new T( in ) ) {}

		constexpr UniquePtr( T&& in ) noexcept:
			BaseType( new T( move( in ) ) ) {}

		constexpr UniquePtr( T* allocation ) noexcept:
			BaseType( allocation ) {}

		constexpr UniquePtr( UniquePtr&& other ) noexcept:
			BaseType( std::move( other ) ) {}

		constexpr UniquePtr& operator=( UniquePtr&& rhs ) noexcept
		{
			BaseType::operator=( std::move( rhs ) );
			return *this;
		}

		constexpr UniquePtr& operator=( T* alloc )
		{
			BaseType::operator=( alloc );
			return *this;
		}
	};

	template< class T >
	class UniquePtr< T[] > : public details::UniquePtrBase< T[] >
	{
	private:
		using BaseType = details::UniquePtrBase< T[] >;
		using ValueType = BaseType::ValueType;
	public:
		constexpr UniquePtr() = default;

		constexpr UniquePtr( ValueType* allocation ) noexcept:
			BaseType( allocation ) {}

		constexpr UniquePtr( UniquePtr&& other ) noexcept:
			BaseType( std::move( other ) ) {}

		constexpr UniquePtr& operator=( UniquePtr&& rhs ) noexcept
		{
			BaseType::operator=( std::move( rhs ) );
			return *this;
		}

		constexpr UniquePtr& operator=( ValueType* alloc )
		{
			BaseType::operator=( alloc );
			return *this;
		}

		[[nodiscard]] constexpr ValueType& operator[]( uint64 index ) noexcept { return this->m_data[ index ]; }
		[[nodiscard]] constexpr ValueType const& operator[]( uint64 index ) const noexcept { return this->m_data[ index ]; }
	};

	namespace details
	{
		template< class T >
		class SharedPtrBase
		{
		protected:
			using ValueType = type::remove_array< T >;
		protected:
			constexpr SharedPtrBase() = default;

			constexpr SharedPtrBase( ValueType* alloc ) noexcept:
				m_data( alloc ),
				m_refCount( new uint64( 1 ) ) {};

			constexpr SharedPtrBase( SharedPtrBase const& ptr ):
				m_data( ptr.m_data ),
				m_refCount( ptr.m_refCount )
			{
				if ( m_refCount )
					*m_refCount += 1;
			}

			constexpr SharedPtrBase( SharedPtrBase&& ptr ) noexcept:
				m_data( ptr.m_data ),
				m_refCount( ptr.m_refCount )
			{
				ptr.m_data = nullptr;
				ptr.m_refCount = nullptr;
			}

			constexpr void operator=( SharedPtrBase const& rhs )
			{
				if ( this == &rhs )
					return;

				DestroyData();

				m_data = rhs.m_data;
				m_refCount = rhs.m_refCount;

				if ( m_refCount )
					*m_refCount += 1;
			}
			
			constexpr void operator=( SharedPtrBase&& rhs ) noexcept
			{
				if ( this == &rhs )
					return;

				DestroyData();

				m_data = rhs.m_data;
				m_refCount = rhs.m_refCount;

				rhs.m_data = nullptr;
				rhs.m_refCount = nullptr;
			}

			constexpr void operator=( ValueType* alloc )
			{
				DestroyData();

				m_data = alloc;
				if ( m_data == nullptr )
					return;
				m_refCount = new uint64( 1 );
			}
		public:
			constexpr ~SharedPtrBase() { DestroyData(); }

			[[nodiscard]] constexpr bool isUnique() const noexcept
			{
				if ( m_data == nullptr || m_refCount == nullptr )
					return true;
				return *m_refCount == 1;
			}

			[[nodiscard]] constexpr bool isShared() const noexcept
			{
				return !isUnique();
			}

			[[nodiscard]] constexpr ValueType* get() const noexcept { return m_data; }

			[[nodiscard]] constexpr const ValueType* operator->() const noexcept { return m_data; }

			[[nodiscard]] constexpr ValueType* operator->() noexcept { return m_data; }

			[[nodiscard]] constexpr ValueType& operator*() noexcept { return *m_data; }

			[[nodiscard]] constexpr ValueType const& operator*() const noexcept { return *m_data; }

			[[nodiscard]] constexpr auto operator<=>( SharedPtrBase const& rhs ) const noexcept { return m_data <=> rhs.m_data; }

			[[nodiscard]] constexpr bool operator==( const void* rhs ) const noexcept { return m_data == rhs; }
			[[nodiscard]] constexpr bool operator!=( const void* rhs ) const noexcept { return m_data != rhs; }
			[[nodiscard]] constexpr bool operator==( std::nullptr_t )  const { return m_data == nullptr; }
			[[nodiscard]] constexpr bool operator!=( std::nullptr_t )  const { return m_data != nullptr; }

			[[nodiscard]] constexpr operator bool() const { return m_data != nullptr; }
		protected:
			constexpr void DestroyData()
			{
				if ( m_refCount == nullptr )
					return;
				*m_refCount -= 1;
				if ( *m_refCount )
				{
					m_refCount = nullptr;
					m_data = nullptr;
					return;
				}
				delete m_refCount;

				if constexpr ( type::is_array< T > )
					delete[] m_data;
				else
					delete m_data;

				m_data = nullptr;
				m_refCount = nullptr;
			}
		protected:
			ValueType* m_data = nullptr;
			uint64* m_refCount = nullptr;
		};
	}

	template< typename T >
	class SharedPtr : public details::SharedPtrBase< T >
	{
	private:
		using BaseType = details::SharedPtrBase< T >;
	public:
		constexpr SharedPtr() = default;

		constexpr SharedPtr( const T& in ):
			BaseType( new T( in ) ) {}

		constexpr SharedPtr( T&& in ):
			BaseType( new T( move( in ) ) ) {}

		constexpr SharedPtr( T* allocation ):
			BaseType( allocation ) {}

		constexpr SharedPtr( const SharedPtr& ptr ):
			BaseType( ptr ) {}

		constexpr SharedPtr( SharedPtr&& ptr ) noexcept:
			BaseType( move( ptr ) ) {}

		constexpr SharedPtr& operator=( SharedPtr const& rhs )
		{
			BaseType::operator=( rhs );
			return *this;
		}

		constexpr SharedPtr& operator=( SharedPtr&& rhs ) noexcept
		{
			BaseType::operator=( move( rhs ) );
			return *this;
		}

		constexpr SharedPtr& operator=( T* alloc )
		{
			BaseType::operator=( alloc );
			return *this;
		}
	};

	template< typename T >
	class SharedPtr< T[] > : public details::SharedPtrBase< T[] >
	{
	private:
		using BaseType = details::SharedPtrBase< T[] >;
	public:
		constexpr SharedPtr() = default;

		constexpr SharedPtr( T* allocation ):
			BaseType( allocation ) {}

		constexpr SharedPtr( const SharedPtr& ptr ):
			BaseType( ptr ) {}

		constexpr SharedPtr( SharedPtr&& ptr ) noexcept:
			BaseType( ptr ) {}

		constexpr SharedPtr& operator=( SharedPtr const& rhs )
		{
			BaseType::operator=( rhs );
			return *this;
		}

		constexpr SharedPtr& operator=( SharedPtr&& rhs ) noexcept
		{
			BaseType::operator=( move( rhs ) );
			return *this;
		}

		constexpr SharedPtr& operator=( T* alloc )
		{
			BaseType::operator=( alloc );
			return *this;
		}

		[[nodiscard]] T& operator[]( uint64 index ) noexcept { return this->m_data[ index ]; }
		[[nodiscard]] T const& operator[]( uint64 index ) const noexcept { return this->m_data[ index ]; }
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

		[[nodiscard]] constexpr const T* get() const { return m_data; }

		[[nodiscard]] constexpr T* get()
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

		[[nodiscard]] constexpr T const& operator*() const { return *m_data; }

		[[nodiscard]] constexpr T& operator*() { return *get(); }

		[[nodiscard]] constexpr const T* operator->() const noexcept { return m_data; }

		[[nodiscard]] constexpr T* operator->() noexcept { return get(); }

		[[nodiscard]] constexpr auto operator<=>( ImmutableSharedPtr const& rhs ) const { return m_data <=> rhs.m_data; }

		[[nodiscard]] constexpr auto operator<=>( const T* const rhs ) const { return m_data <=> rhs; }

		[[nodiscard]] constexpr bool isUnique() const noexcept
		{
			if ( m_data == nullptr || m_refCount == nullptr )
				return true;
			return *m_refCount == 1;
		}

		[[nodiscard]] constexpr bool isShared() const noexcept
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
			delete m_data;

			m_data = nullptr;
			m_refCount = nullptr;
		}
	private:
		T* m_data = nullptr;
		uint64* m_refCount = nullptr;
	};

	template< class T >
	class ImmutableSharedPtr< T[] >
	{
		ImmutableSharedPtr() = delete;
	};

	template< class T, class... Args, class = type::enable_if< !type::is_array< T > > >
	[[nodiscard]] constexpr UniquePtr< T > make_unique( Args&&... args )
	{
		return UniquePtr< T >( new T( std::forward< Args >( args )... ) );
	}

	template< class T, class = std::enable_if_t< type::is_array< T > > >
	[[nodiscard]] constexpr auto make_unique( std::initializer_list< type::remove_array< T > >&& args )
	{
		UniquePtr< T > ptr = new type::remove_array< T >[ args.size() ];
		auto raw = ptr.get();
		for ( auto it = args.begin(); it != args.end(); ++it, ++raw )
			*raw = std::move( *it );
		return ptr;
	}

	template< class T, class... Args, class = std::enable_if_t< !type::is_array< T > > >
	[[nodiscard]] constexpr SharedPtr< T > make_shared( Args&&... args )
	{
		return SharedPtr< T >( new T( std::forward< Args >( args )... ) );
	}

	template< class T, class = std::enable_if_t< type::is_array< T > > >
	[[nodiscard]] constexpr auto make_shared( std::initializer_list< type::remove_array< T > >&& args )
	{
		SharedPtr< T > ptr = new type::remove_array< T >[ args.size() ];
		auto raw = ptr.get();
		for ( auto it = args.begin(); it != args.end(); ++it, ++raw )
			*raw = std::move( *it );
		return ptr;
	}

	template< class T, class... Args >
	[[nodiscard]] constexpr ImmutableSharedPtr< T > make_immutable_shared( Args&&... args )
	{
		return ImmutableSharedPtr< T >( new T( std::forward< Args >( args  )... ) );
	}
}
