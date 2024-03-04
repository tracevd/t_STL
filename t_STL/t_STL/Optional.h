#pragma once

#include <utility>
#include <memory>

template< typename T >
class Optional
{
public:
	constexpr Optional():
		m_data( Data() ),
		m_valid( false ) {}

	constexpr Optional( Optional&& other ) noexcept:
		m_data( [&other](){
			if ( other )
			{
				return Data( std::move( other.m_data.data ) );
			}
			return Data();
		}() ),
		m_valid( other.m_valid ) {}

	constexpr Optional( Optional const& other ):
		m_data( [&other](){
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

	constexpr Optional( T const& val ):
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
			throw std::runtime_error("Optional was null!");
		}

		return m_data.data;
	}

	constexpr T& value()
	{
		if ( !hasValue() )
		{
			throw std::runtime_error( "Optional was null!" );
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

		constexpr Data() {}
		constexpr ~Data() {}
	} m_data;
	bool m_valid;
};