#pragma once

#include <iostream>

#include "Tint.h"
#include "Lib.h"

namespace t
{
	class String
	{
	public:
		constexpr String() = default;
		constexpr String( const char* inStr, uint64 length )
		{
			m_data = new char[ length+1 ];
			m_size = length;
			memcpy( m_data, inStr, m_size );
			m_data[ m_size ] = '\0';
		}
		constexpr String( const char* inStr )
		{
			auto length = strlen( inStr );
			*this = String( inStr, length );
		}
		constexpr String( const String& inStr )
		{
			if ( inStr.m_size == 0 )
				return;
			m_size = inStr.m_size;
			m_data = new char[m_size+1];
			memcpy(m_data, inStr.m_data, m_size+1);
		}
		constexpr String( String&& inStr ) noexcept
		{
			this->m_size = inStr.m_size;
			this->m_data = inStr.m_data;
			inStr.m_size = 0;
			inStr.m_data = nullptr;
		}
		constexpr ~String()
		{
			delete[] m_data;
		}
		constexpr inline const bool isNull() const
		{
			return m_size == 0;
		}
		[[nodiscard]] constexpr const uint64 size() const
		{
			return m_size;
		}
		[[nodiscard]] constexpr const char* c_str() const
		{
			return m_data;
		}
		constexpr char* begin()
		{
			if ( m_data == nullptr )
				throw std::runtime_error( "Empty String!" );
			return m_data;
		}
		constexpr const char* cbegin() const
		{
			if ( m_data == nullptr )
				throw std::runtime_error( "Empty String!" );
			return m_data;
		}
		constexpr char* end()
		{
			if ( m_data == nullptr )
				throw std::runtime_error( "Empty String!" );
			return m_data + m_size;
		}
		constexpr const char* cend() const
		{
			if ( m_data == nullptr )
				throw std::runtime_error( "Empty String!" );
			return m_data + m_size;
		}
		[[nodiscard]] constexpr String substr( const uint64 begInd ) const
		{
			if ( begInd >= m_size )
				throw std::runtime_error( "Attempting to access memory outside of String" );

			const uint64 size = m_size - begInd + 1;

			auto toStr = new char[ size ];

			uint64 i = begInd;

			for ( ; m_data[ i ] != '\0'; i++ )
				toStr[ i - begInd ] = m_data[ i ];

			toStr[ size - 1 ] = '\0';
		
			String retStr = toStr;
			delete[] toStr;
			return retStr;
		}
		[[nodiscard]] constexpr String substr( const uint64 begin, const uint64 end ) const
		{
			if ( begin >= m_size || end >= m_size )
				return "";

			if ( begin == end )
				throw std::runtime_error("Invalid indexes given to substr; 'end' was the same as 'begin'\n");

			if ( begin > end )
				throw std::runtime_error( "Invalid indexes given to substr; 'end' was less than 'begin'" );

			const uint64 size = end - begin + 1;

			auto toStr = new char[ size ];

			uint64 i = begin;

			for ( ; i < end; i++ )
				toStr[ i - begin ] = m_data[ i ];

			toStr[ size - 1 ] = '\0';

			return String( toStr, size-1 );
		}
		constexpr String& operator+=( const String& rhs )
		{
			auto temp = m_data;
			m_size += rhs.m_size;
			m_data = new char[ m_size+1 ];
			memcpy( m_data, temp, m_size- rhs.m_size );
			memcpy( m_data + m_size-rhs.m_size, rhs.m_data, rhs.m_size );
			delete[] temp;

			return *this;
		}
		constexpr String& operator+=( const char* rhs )
		{
			auto temp = m_data;
			auto rhslength = strlen( rhs );
			m_data = new char[ m_size + rhslength + 1 ];
			memcpy( m_data, temp, m_size );
			memcpy( m_data + m_size, rhs, rhslength );
			m_size += rhslength;

			delete[] temp;
			return *this;
		}
		constexpr String& operator+=( const char c )
		{
			auto temp = m_data;
			++m_size;
			m_data = new char[ m_size + 1 ];
			memcpy( m_data, temp, m_size-1 );
			m_data[ m_size-1 ] = c;
			m_data[ m_size ] = '\0';
			delete[] temp;

			return *this;
		}
		constexpr String operator+( const char* const ptr ) const
		{
			auto othersize = strlen( ptr );
			char* newstr = new char[ m_size + othersize + 1 ];
			memcpy( newstr, m_data, m_size );
			memcpy( newstr + m_size, ptr, othersize );
			return String( newstr, m_size + othersize );
		}
		constexpr String operator+( const String& inStr ) const
		{
			char* newstr = new char[ m_size+inStr.m_size+1 ];
			memcpy( newstr, m_data, m_size );
			memcpy( newstr + m_size, inStr.m_data, inStr.m_size );
			return String( newstr, m_size + inStr.m_size );
		}
		constexpr String operator+( const char c ) const
		{
			char* newstr = new char[ m_size + 2 ];
			memcpy( newstr, m_data, m_size );
			newstr[ m_size ] = c;
			newstr[ m_size+1 ] = '\0';
			return String( newstr, m_size + 1 );
		}
		constexpr String& operator=( const String& inStr )
		{
			if ( &inStr == this )
				return *this;
			*this = String( inStr );
			return *this;
		}
		constexpr String& operator=( String&& inStr ) noexcept
		{
			if ( &inStr == this )
				return *this;
			delete[] m_data;
			m_size = inStr.m_size;
			m_data = inStr.m_data;

			inStr.m_size = 0;
			inStr.m_data = nullptr;
		
			return *this;
		}
		constexpr String& operator=( const char* ptr )
		{
			*this = String( ptr );
			return *this;
		}
		constexpr const char& operator[]( const uint64 i ) const { return m_data[ i ]; }
		constexpr char& operator[]( const uint64 i ) { return m_data[ i ]; }
		constexpr const char& at( uint64 i ) const
		{
			if ( i >= m_size )
				throw std::runtime_error("Invalid index");
			return operator[]( i );
		}
		constexpr char& at( uint64 i )
		{
			if ( i >= m_size )
				throw std::runtime_error("Invalid index");
			return operator[]( i );
		}
		[[nodiscard]] constexpr bool operator==( const String& cmpStr ) const
		{
			if ( cmpStr.m_size != this->m_size )
				return false;
			for( uint64 i = 0; i < m_size; i++ )
				if ( cmpStr[ i ] != m_data[ i ] )
					return false;
			return true;
		}
		[[nodiscard]] constexpr bool operator==( const char* cmpStr ) const
		{
			if ( strlen( cmpStr ) != this->m_size )
				return false;
			for ( uint64 i = 0; i < m_size; i++ )
				if ( cmpStr[ i ] != m_data[ i ] )
					return false;
			return true;
		}
		[[nodiscard]] constexpr bool operator!=( const String& cmpStr ) const noexcept
		{
			return !( *this == cmpStr );
		}
		[[nodiscard]] constexpr bool operator!=( const char* cmpStr ) const noexcept
		{
			return !( *this == cmpStr );
		}
		[[nodiscard]] constexpr String toUpperCase() const
		{
			constexpr char diff = 'a' - 'A';
			char* tmp = new char[ m_size + 1 ];

			for ( uint64 i = 0; i < m_size; i++ )
			{
				if ( m_data[ i ] >= 'a' && m_data[i] <= 'z' )
					tmp[ i ] = m_data[ i ] - diff;
				else
					tmp[ i ] = m_data[ i ];
			}
			return String( tmp, m_size );
		}
		[[nodiscard]] constexpr String toLowerCase() const
		{
			constexpr char diff = 'a' - 'A';
			char* tmp = new char[ m_size + 1 ];

			for ( uint64 i = 0; i < m_size; i++ )
			{
				if ( m_data[ i ] >= 'A' && m_data[ i ] <= 'Z' )
					tmp[ i ] = m_data[ i ] + diff;
				else
					tmp[ i ] = m_data[ i ];
			}
			return String( tmp, m_size );
		}
		friend std::istream& operator>>( std::istream& in, t::String& str )
		{
			constexpr uint64 buffSz = 128;
			char buff[ buffSz ];
			in.get( buff, buffSz, '\n' );
			str = buff;
			return in;
		}
		friend std::ostream& operator<<( std::ostream& os, const String& str )
		{
			return os << str.c_str();
		}
		friend constexpr t::String operator+( const char* const lhs, const t::String& rhs )
		{
			auto lhslength = strlen( lhs );
			auto newstrBuffSize = lhslength + rhs.size() + 1;
			auto newstr = new char[ lhslength + rhs.size() + 1 ];
			
			memcpy( newstr, lhs, lhslength );
			memcpy( newstr + lhslength -1, rhs.c_str(), rhs.size() + 1 );

			return ::t::String( newstr, lhslength + rhs.size() );
		}
		/**
		 * @brief Does not copy buffer, but instead takes buffer as 
		 * it's own data, therefore must be an allocated buffer
		 */
		static constexpr String makeString( char* const allocbuffer, uint64 size )
		{
			return String( allocbuffer, size );
		}
	private:
		/**
		 * @brief Constructor that does not copy 
		 * allocated buffer, but takes it as it's own
		 */
		constexpr String( char* const allocbuffer, uint64 size ):
			m_data( allocbuffer ),
			m_size( size ) {}
	private:
		char* m_data = nullptr;
		uint64 m_size = 0;
	};

	namespace fast
	{
		class String
		{
		public:
			constexpr String() = default;

			template< typename T, typename = std::enable_if_t< std::is_arithmetic_v< T > && !std::is_floating_point_v< T > > >
			constexpr explicit String( T number )
			{
				if constexpr ( std::is_same_v< T, int64 > )
				{
					if ( number == limit< int64 >::min )
					{
						*this = String( "-9223372036854775808", 20 );
						return;
					}
				}

				char buff[ 21 ];
				auto end = &buff[ 21 ];

				auto _number = number;
				
				if constexpr ( std::is_signed_v< T > )
				{
					if ( number < 0 )
						number = 0 - number;
				}

				do
				{
					auto mod = number % 10;
					*--end = '0' + mod;
					number /= 10;
				} while( number );
				
				if constexpr ( std::is_signed_v< T > )
				{
					if ( _number < 0 )
					{
						*--end = '-';
					}
				}

				*this = String( end, static_cast< uint32 >( &buff[21] - end ) );
			}

			template< uint64 N >
			constexpr String( char const ( &str )[ N ] )
			{
				*this = String( str, static_cast< uint32 >( N-1 ) );
			}

			constexpr String( const char* str, uint32 length )
			{
				m_size = length;
				m_capacity = m_size + DEFAULT_EXTRA_PADDING;
				m_data = new char[ m_capacity + 1 ];
				strcpy< char >( m_data, str, m_size );
				m_data[ m_size ] = '\0';
			}

			constexpr String( const char* str )
			{
				uint32 length = static_cast< uint32 >( strlen( str ) );
				*this = String( str, length );
			}

			constexpr String( String const& str )
			{
				m_size = str.m_size;
				m_capacity = m_size + DEFAULT_EXTRA_PADDING;
				m_data = new char[ m_capacity + 1 ];
				strcpy< char >( m_data, str.m_data, m_size );
				m_data[ m_size ] = '\0';
			}

			constexpr String( String&& str ) noexcept
			{
				*this = std::move( str );
			}

			constexpr ~String()
			{
				delete[] m_data;
				m_data	   = nullptr;
				m_capacity = 0;
				m_size	   = 0;
			}

			constexpr String& operator=( String&& rhs ) noexcept
			{
				if ( rhs.m_size <= m_capacity )
				{
					delete[] m_data;
				}

				m_data     = rhs.m_data;
				m_size 	   = rhs.m_size;
				m_capacity = rhs.m_capacity;

				rhs.m_data     = nullptr;
				rhs.m_size     = 0;
				rhs.m_capacity = 0;

				return *this;
			}

			constexpr const char* data() const { return m_data; }
			constexpr char* data() { return m_data; }

			constexpr String& operator=( String const& rhs )
			{
				if ( m_capacity >= rhs.m_size && m_data != nullptr )
				{
					strcpy< char >( m_data, rhs.m_data, rhs.m_size );
					m_size = rhs.m_size;
					m_data[ m_size ] = '\0';
					return *this;
				}
				*this = String( rhs );
				return *this;
			}

			constexpr String& operator=( const char* rhs )
			{
				uint32 length = (uint32) strlen( rhs );
				if ( m_capacity >= length && m_data != nullptr )
				{
					::t::strcpy< char >( m_data, rhs, length );
					m_size = length;
					m_data[ m_size ] = '\0';
					return *this;
				}
				*this = String( rhs, length );
				return *this;
			}

			constexpr String& operator+=( String const& rhs )
			{
				if ( m_size + rhs.m_size > m_capacity )
				{
					if ( m_size + rhs.m_size < m_capacity * 2 )
						reallocate();
					else
						reallocate( m_size + rhs.m_size );
				}

				strcpy< char >( &m_data[ m_size ], rhs.m_data, rhs.m_size + 1 );
				m_size += rhs.m_size;
				return *this;
			}

			constexpr String& operator+=( const char* rhs )
			{
				auto length = (uint32) strlen( rhs );

				if ( m_size + length > m_capacity )
				{
					if ( m_size + length < m_capacity * 2 )
						reallocate();
					else
						reallocate( m_size + length );
				}

				strcpy< char >( &m_data[ m_size ], rhs, length );
				m_size += length;
				m_data[ m_size ] = '\0';

				return *this;
			}

			constexpr String& operator+=( char c )
			{
				if ( m_size + 1 > m_capacity )
				{
					reallocate();
				}

				m_data[ m_size ] = c;
				m_data[ ++m_size ] = '\0';

				return *this;
			}

			constexpr bool operator==( String const& rhs ) const
			{
				if ( m_size != rhs.m_size )
					return false;
				
				for ( uint32 i = 0; i < m_size; ++i )
				{
					if ( m_data[ i ] != rhs.m_data[ i ] )
						return false;
				}

				return true;
			}

			constexpr uint32 size() const { return m_size; }

			constexpr uint32 capacity() const { return m_capacity; }

			constexpr void reserve( uint32 capacity )
			{
				if ( m_capacity != 0 && m_capacity < capacity )
					throw std::runtime_error("Excepted a larger buffer to allocate");
				
				auto newdata = new char[ capacity ];

				if ( m_data != nullptr )
				{
					strcpy< char >( newdata, m_data, m_size );
				}

				delete[] m_data;
				m_data = newdata;
				m_capacity = capacity;
			}

			constexpr char* release()
			{
				auto ptr = m_data;
				m_data = nullptr;
				m_capacity = 0;
				m_size = 0;
				return ptr;
			}

			constexpr char* c_str() { return m_data; }
			constexpr const char* c_str() const { return m_data; }

			constexpr static inline String makeString( char* allocbuffer, uint32 stringSize )
			{
				return String( allocbuffer, stringSize, stringSize );
			}

			constexpr static inline String makeString( char* allocbuffer, uint32 stringSize, uint32 bufferCapacity )
			{
				return String( allocbuffer, stringSize, bufferCapacity );
			}
			
			friend std::istream& operator>>( std::istream& in, t::String& str )
			{
				constexpr uint64 buffSz = 256;
				char buff[ buffSz ];
				in.get( buff, buffSz, '\n' );
				str = buff;
				return in;
			}

			friend std::ostream& operator<<( std::ostream& os, const String& str )
			{
				return os << str.c_str();
			}

		private:
			constexpr void reallocate( uint32 newcap )
			{
				auto newdata = new char[ newcap + 1 ];
				strcpy< char >( newdata, m_data, m_size+1 );
				delete[] m_data;
				m_data = newdata;
				m_capacity = newcap;
			}

			constexpr void reallocate()
			{
				reallocate( m_capacity * 2 );
			}

			constexpr String( char* allocbuffer, uint32 bufferSize, uint32 bufferCapacity ):
				m_data( allocbuffer ),
				m_size( bufferSize + 1 ),
				m_capacity( bufferCapacity ) {}

		private:
			char* m_data = nullptr;
			uint32 m_size = 0;
			uint32 m_capacity = 0;

			static constexpr uint32 DEFAULT_EXTRA_PADDING = 7;
		};
	}

	class StringView
	{
	public:
		StringView() = delete;

		template< uint64 n >
		constexpr StringView( char const ( &str )[ n ] ):
			m_data( str ),
			m_size( n - 1 ) {}
		
		constexpr StringView( const char* str, uint64 length ):
			m_data( str ),
			m_size( length ) {}

		constexpr StringView( String const& str ):
			m_data( str.c_str() ),
			m_size( str.size() ) {}

		constexpr StringView( fast::String const& str ):
			m_data( str.c_str() ),
			m_size( str.size() ) {}

		constexpr const char* data() const { return m_data; }
		constexpr uint64 size() const { return m_size; }

		friend std::ostream& operator<<( std::ostream& os, StringView const& str )
		{
			return os << str.data();
		}

	private:
		const char* const m_data;
		const uint64 m_size;
	};

}

template<>
struct std::hash< t::String >
{
	uint64_t operator()( const t::String& str ) const
	{
		return std::hash< std::string_view >{}( str.c_str() );
	}
};

template<>
struct std::hash< t::fast::String >
{
	constexpr uint64_t operator()( t::fast::String const& str ) const
	{
		const uint64_t size = str.size();

		uint64_t hash = 0x0f'0f'0f'0f'0f;

		auto half = size / 2u;

		auto quarter = half / 2u;

		auto _3quarters = quarter * 3u;

		auto data = str.c_str();

		if ( data == nullptr )
			return 0;

		uint64 i = 0;

		for ( ; i < quarter; ++i )
		{
			hash += data[ i ];
		}

		auto cpy = hash;

		hash *= 1011u;
		hash ^= ( cpy / 2 ) << 56u;

		for ( ; i < half; ++i )
		{
			hash += data[ i ];
		}

		hash *= 1234u;

		for ( ; i < _3quarters; ++i )
		{
			hash += data[ i ];
		}

		auto cpy2 = hash;
		hash *= 4325u;
		hash ^= cpy2 << 45u;

		for ( ; i < size; ++i )
		{
			hash += data[ i ];
		}

		hash ^= static_cast< uint64_t >( *data ) << 56u;

		return hash;
	}
};
