#pragma once

namespace t
{
	template< typename T >
	class ArrayIterator
	{
	public:
		using value_type = T;
	public:
		constexpr ArrayIterator() = default;

		constexpr ArrayIterator( T* ptr ):
			m_ptr( ptr ) {}

		constexpr ArrayIterator& operator++()
		{
			++m_ptr;
			return *this;
		}

		constexpr ArrayIterator operator++( int )
		{
			auto cpy = *this;
			++m_ptr;
			return cpy;
		}

		constexpr ArrayIterator& operator--()
		{
			--m_ptr;
			return *this;
		}

		constexpr ArrayIterator operator--( int )
		{
			auto cpy = *this;
			--m_ptr;
			return cpy;
		}

		constexpr T& operator*() { return *m_ptr; }

		constexpr T const& operator*() const { return *m_ptr; }

		constexpr T* get() { return m_ptr; }

		constexpr const T* get() const { return m_ptr; }

		constexpr auto* operator->() { return get(); }

		constexpr auto* operator->() const { return get(); }

		constexpr uint64 distance( ArrayIterator rhs ) const
		{
			return m_ptr > rhs.m_ptr ? m_ptr - rhs.m_ptr : rhs.m_ptr - m_ptr;
		}

		friend constexpr auto operator-( ArrayIterator lhs, ArrayIterator rhs )
		{
			return lhs.m_ptr - rhs.m_ptr;
		}

		constexpr ArrayIterator operator+( int64 offset ) const
		{
			return ArrayIterator( m_ptr + offset );
		}

		constexpr ArrayIterator operator-( int64 offset ) const
		{
			return ArrayIterator( m_ptr - offset );
		}

		friend constexpr bool operator==( ArrayIterator lhs, void* rhs )
		{
			return lhs.m_ptr == rhs;
		}

		friend constexpr bool operator==( void* lhs, ArrayIterator rhs )
		{
			return lhs == rhs.m_ptr;
		}

		friend constexpr auto operator<=>( ArrayIterator lhs, void* rhs )
		{
			return lhs.m_ptr <=> rhs;
		}
		friend constexpr auto operator<=>( void* lhs, ArrayIterator rhs )
		{
			return lhs <=> rhs.m_ptr;
		}
	protected:
		T* m_ptr = nullptr;
	};

	template< typename T >
	constexpr bool operator==( ArrayIterator< T > lhs, ArrayIterator< T > rhs )
	{
		return lhs.get() == rhs.get();
	}

	template< typename T >
	constexpr bool operator==( ArrayIterator< const T > lhs, ArrayIterator< T > rhs )
	{
		return lhs.get() == rhs.get();
	}

	template< typename T >
	constexpr bool operator==( ArrayIterator< T > lhs, ArrayIterator< const T > rhs )
	{
		return lhs.get() == rhs.get();
	}

	template< typename T >
	constexpr bool operator==( ArrayIterator< const T > lhs, ArrayIterator< const T > rhs )
	{
		return lhs.get() == rhs.get();
	}


	template< typename T >
	constexpr auto operator<=>( ArrayIterator< T > lhs, ArrayIterator< T > rhs )
	{
		return lhs.get() <=> rhs.get();
	}

	template< typename T >
	constexpr auto operator<=>( ArrayIterator< const T > lhs, ArrayIterator< T > rhs )
	{
		return lhs.get() <=> rhs.get();
	}

	template< typename T >
	constexpr auto operator<=>( ArrayIterator< T > lhs, ArrayIterator< const T > rhs )
	{
		return lhs.get() <=> rhs.get();
	}

	template< typename T >
	constexpr auto operator<=>( ArrayIterator< const T > lhs, ArrayIterator< const T > rhs )
	{
		return lhs.get() <=> rhs.get();
	}

	template< typename It >
	class ReverseArrayIterator
	{
	public:
		ReverseArrayIterator() = default;

		ReverseArrayIterator( It it ):
			m_it( it ) {}

		constexpr ReverseArrayIterator& operator++()
		{
			--m_it;
			return *this;
		}

		constexpr ReverseArrayIterator operator++( int )
		{
			auto cpy = *this;
			--m_it;
			return cpy;
		}

		constexpr ReverseArrayIterator& operator--()
		{
			++m_it;
			return *this;
		}

		constexpr ReverseArrayIterator operator--( int )
		{
			auto cpy = *this;
			++m_it;
			return cpy;
		}

		constexpr auto& operator*()
		{
			return *(m_it - 1);
		}

		constexpr auto* operator->()
		{
			return m_it.get() - 1;
		}

		constexpr auto* get()
		{
			return m_it.get() - 1;
		}

		constexpr uint64 distance( ReverseArrayIterator rhs ) const
		{
			return m_it.distance( rhs.m_it );
		}

		friend constexpr auto operator-( ReverseArrayIterator lhs, ReverseArrayIterator rhs )
		{
			return lhs.m_it - rhs.m_it;
		}

		constexpr ReverseArrayIterator operator+( int64 offset ) const
		{
			return ReverseArrayIterator( m_it - offset );
		}

		constexpr ReverseArrayIterator operator-( int64 offset ) const
		{
			return ReverseArrayIterator( m_it + offset );
		}

		friend constexpr bool operator==( ReverseArrayIterator lhs, void* rhs )
		{
			return lhs.get() == rhs;
		}

		friend constexpr bool operator==( void* lhs, ReverseArrayIterator rhs )
		{
			return lhs == rhs.get();
		}

		friend constexpr auto operator<=>( ReverseArrayIterator lhs, void* rhs )
		{
			return lhs.get() <=> rhs;
		}
		friend constexpr auto operator<=>( void* lhs, ReverseArrayIterator rhs )
		{
			return lhs <=> rhs.get();
		}
	protected:
		It m_it;
	};

	template< typename T >
	constexpr bool operator==( ReverseArrayIterator< ArrayIterator< T > > lhs, ReverseArrayIterator< ArrayIterator< T > > rhs )
	{
		return lhs.get() == rhs.get();
	}

	template< typename T >
	constexpr bool operator==( ReverseArrayIterator< ArrayIterator< const T > > lhs, ReverseArrayIterator< ArrayIterator< T > > rhs )
	{
		return lhs.get() == rhs.get();
	}

	template< typename T >
	constexpr bool operator==( ReverseArrayIterator< ArrayIterator< T > > lhs, ReverseArrayIterator< ArrayIterator< const T > > rhs )
	{
		return lhs.get() == rhs.get();
	}

	template< typename T >
	constexpr bool operator==( ReverseArrayIterator< ArrayIterator< const T > > lhs, ReverseArrayIterator< ArrayIterator< const T > > rhs )
	{
		return lhs.get() == rhs.get();
	}


	template< typename T >
	constexpr auto operator<=>( ReverseArrayIterator< ArrayIterator< T > > lhs, ReverseArrayIterator< ArrayIterator< T > > rhs )
	{
		return lhs.get() <=> rhs.get();
	}

	template< typename T >
	constexpr auto operator<=>( ReverseArrayIterator< ArrayIterator< const T > > lhs, ReverseArrayIterator< ArrayIterator< T > > rhs )
	{
		return lhs.get() <=> rhs.get();
	}

	template< typename T >
	constexpr auto operator<=>( ReverseArrayIterator< ArrayIterator< T > > lhs, ReverseArrayIterator< ArrayIterator< const T > > rhs )
	{
		return lhs.get() <=> rhs.get();
	}

	template< typename T >
	constexpr auto operator<=>( ReverseArrayIterator< ArrayIterator< const T > > lhs, ReverseArrayIterator< ArrayIterator< const T > > rhs )
	{
		return lhs.get() <=> rhs.get();
	}
}