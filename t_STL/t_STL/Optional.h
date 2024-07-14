#pragma once

#include <utility>
#include <memory>

namespace t
{
	template< typename T >
	class Optional
	{
	public:
		constexpr Optional():
			m_data( Data() ),
			m_valid( false ) {}

		constexpr Optional( Optional&& other ) noexcept:
			m_data( [&other]() {
			if ( other )
			{
				return Data( std::move( other.m_data.data ) );
			}
			return Data();
				}() ),
			m_valid( other.m_valid ) {}

		constexpr Optional( Optional const& other ):
			m_data( [&other]() {
			if ( other )
			{
				return Data( other.m_data.data );
			}
			return Data();
				}() ),
			m_valid( other.m_valid ) {}

		constexpr Optional( T&& value ):
			m_data( std::move( value ) ),
			m_valid( true ) {}

		constexpr Optional( T const& val ) :
			m_data( val ),
			m_valid( true ) {}

		constexpr Optional& operator=( Optional&& rhs ) noexcept
		{
			if ( this == &rhs )
				return *this;

			if ( rhs.m_valid )
			{
				if ( m_valid )
				{
					m_data = std::move( rhs.m_data.data );
				}
				else
				{
					CreateData( std::move( rhs.m_data.data ) );
				}
			}

			m_valid = rhs.m_valid;

			return *this;
		}

		constexpr Optional& operator=( Optional const& rhs )
		{
			if ( this == &rhs )
				return *this;

			if ( rhs.m_valid )
			{
				if ( m_valid )
				{
					m_data = rhs.m_data.data;
				}
				else
				{
					CreateData( rhs.m_data.data );
				}
			}

			m_valid = rhs.m_valid;

			return *this;
		}

		constexpr Optional& operator=( T&& rhs )
		{
			if ( m_valid )
			{
				m_data.data = std::move( rhs );
			}
			else
			{
				CreateData( std::move( rhs ) );
			}

			return *this;
		}

		constexpr Optional& operator=( T const& rhs )
		{
			if ( m_valid )
			{
				m_data.data = rhs;
			}
			else
			{
				CreateData( rhs );
			}

			return *this;
		}

		constexpr bool hasValue() const { return m_valid; }

		constexpr operator bool() const { return hasValue(); }

		constexpr T const& value() const
		{
			if ( !hasValue() )
			{
				throw Error( "Optional was null!", 1 );
			}

			return m_data.data;
		}

		constexpr T& value()
		{
			if ( !hasValue() )
			{
				throw Error( "Optional was null!", 1 );
			}

			return m_data.data;
		}

		constexpr T valueOr( T&& other )
		{
			return hasValue() ? m_data.data : T( std::move( other ) );
		}

		constexpr T& valueOr( T& other )
		{
			return hasValue() ? m_data.data : other;
		}

		constexpr T const& valueOr( T const& other ) const
		{
			return hasValue() ? m_data.data : other;
		}

		constexpr ~Optional()
		{
			DestroyData();
			m_valid = false;
		}

	private:
		template< typename... Args >
		constexpr void CreateData( Args&&... args )
		{
			std::construct_at( &m_data.data, std::forward< Args >( args )... );
		}

		constexpr void DestroyData()
		{
			if ( m_valid )
			{
				m_data.data.~T();
			}
		}
	private:
		union Data {
			T data;
			bool __dummy = false;

			constexpr Data( T&& v ):
				data( std::move( v ) ) {}

			constexpr Data( T const& v ):
				data( v ) {}

			constexpr Data() {}
			constexpr ~Data() {}
		} m_data;
		bool m_valid;
	};

	template< typename T >
	class Optional< T& >
	{
	public:
		constexpr Optional() = default;

		constexpr Optional( T& value ):
			m_data( &value ) {}

		constexpr Optional( Optional&& other ):
			m_data( other.m_data ) {}

		constexpr Optional( Optional const& other ):
			m_data( other.m_data ) {}

		constexpr Optional& operator=( Optional&& rhs )
		{
			m_data = rhs.m_data;

			return *this;
		}

		constexpr Optional& operator=( Optional const& rhs )
		{
			m_data = rhs.m_data;

			return *this;
		}

		constexpr Optional& operator=( T&& value )
		{
			if ( m_data == &value )
				return *this;

			if ( !m_data )
			{
				m_data = &value;
				return *this;
			}

			*m_data = move( value );

			return *this;
		}

		constexpr Optional& operator=( T const& value )
		{
			if ( m_data == &value )
				return *this;

			if ( !m_data )
			{
				m_data = &value;
				return *this;
			}

			*m_data = value;

			return *this;
		}

		constexpr bool hasValue() const { return m_data; }
		constexpr operator bool() const { return m_data; }

		constexpr T& value()
		{
			if ( !m_data )
				throw Error( "Bad optional access", 1 );
			return *m_data;
		}

		constexpr T const& value() const
		{
			if ( !m_data )
				throw Error( "Bad optional access", 1 );
			return *m_data;
		}

		constexpr T valueOr( T&& val )
		{
			return m_data ? *m_data : val;
		}

		constexpr T& valueOr( T& val )
		{
			return m_data ? *m_data : val;
		}

		constexpr T const& valueOr( T const& other ) const
		{
			return m_data ? m_data.data : other;
		}

	private:
		T* m_data;
	};
}
