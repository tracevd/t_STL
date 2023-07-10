#pragma once
#include <string.h>
#include <string>
#include <iostream>
#include <type_traits>
#include <string_view>

namespace t
{
	class String
	{
	public:
		String() = default;
		String( const char* inStr, size_t length )
		{
			m_data = new char[length+1];
			m_size = length;
			memcpy_s(m_data, m_size, inStr, m_size );
			m_data[ m_size ] = '\0';
		}
		String( const char* inStr )
		{
			auto length = strlen( inStr );
			*this = String( inStr, length );
		}
		String( const String& inStr )
		{
			if ( inStr.m_size == 0 )
				return;
			m_size = inStr.m_size;
			m_data = new char[m_size+1];
			memcpy_s(m_data, m_size+1, inStr.m_data, m_size+1);
		}
		String( String&& inStr ) noexcept
		{
			this->m_size = inStr.m_size;
			this->m_data = inStr.m_data;
			inStr.m_size = 0;
			inStr.m_data = nullptr;
		}
		~String()
		{
			delete[] m_data;
		}
		inline const bool isNull() const
		{
			return m_size == 0;
		}
		[[nodiscard]] const size_t size() const
		{
			return m_size;
		}
		[[nodiscard]] const char* c_str() const
		{
			return m_data;
		}
		char* begin()
		{
			if ( m_data == nullptr )
				throw std::runtime_error( "Empty String!" );
			return m_data;
		}
		const char* cbegin() const
		{
			if ( m_data == nullptr )
				throw std::runtime_error( "Empty String!" );
			return m_data;
		}
		char* end()
		{
			if ( m_data == nullptr )
				throw std::runtime_error( "Empty String!" );
			return m_data + m_size;
		}
		const char* cend() const
		{
			if ( m_data == nullptr )
				throw std::runtime_error( "Empty String!" );
			return m_data + m_size;
		}
		[[nodiscard]] String substr( const size_t begInd ) const
		{
			if ( begInd >= m_size )
				throw std::runtime_error( "Attempting to access memory outside of String" );

			const size_t size = m_size - begInd + 1;

			auto toStr = new char[ size ];

			size_t i = begInd;

			for ( ; m_data[ i ] != '\0'; i++ )
				toStr[ i - begInd ] = m_data[ i ];

			toStr[ size - 1 ] = '\0';
		
			String retStr = toStr;
			delete[] toStr;
			return retStr;
		}
		[[nodiscard]] String substr( const size_t begin, const size_t end ) const
		{
			if ( begin >= m_size || end >= m_size )
				return "";

			if ( begin == end )
				throw std::runtime_error("Invalid indexes given to substr; 'end' was the same as 'begin'\n");

			if ( begin > end )
				throw std::runtime_error( "Invalid indexes given to substr; 'end' was less than 'begin'" );

			const size_t size = end - begin + 1;

			auto toStr = new char[ size ];

			size_t i = begin;

			for ( ; i < end; i++ )
				toStr[ i - begin ] = m_data[ i ];

			toStr[ size - 1 ] = '\0';

			return String( toStr, size-1 );
		}
		String& operator+=( const String& inStr )
		{
			auto temp = m_data;
			m_size += inStr.m_size;
			m_data = new char[ m_size+1 ];
			sprintf_s( m_data, m_size+1, "%s%s\0", temp, inStr.m_data );
			delete[] temp;

			return *this;
		}
		String& operator+=( const char* inStr )
		{
			auto temp = m_data;
			m_size += strlen(inStr);
			m_data = new char[ m_size + 1 ];
			sprintf_s( m_data, m_size + 1, "%s%s\0", temp, inStr );

			delete[] temp;
			return *this;
		}
		String& operator+=( const char c )
		{
			auto temp = m_data;
			++m_size;
			m_data = new char[ m_size + 1 ];
			sprintf_s( m_data, m_size + 1, "%s%c\0", temp, c );
			delete[] temp;

			return *this;
		}
		String operator+( const char* const ptr ) const
		{
			auto othersize = strlen( ptr );
			char* newstr = new char[ m_size + othersize + 1 ];
			sprintf_s( newstr, m_size + othersize + 1, "%s%s\0", m_data, ptr );
			return String( newstr, m_size + othersize );
		}
		String operator+( const String& inStr ) const
		{
			char* newstr = new char[ m_size+inStr.m_size+1 ];
			sprintf_s( newstr, m_size+inStr.m_size+1, "%s%s\0", m_data, inStr.m_data );
			return String( newstr, m_size + inStr.m_size );
		}
		String operator+( const char c ) const
		{
			char* newstr = new char[ m_size + 2 ];
			sprintf_s( newstr, m_size + 2, "%s%c\0", m_data, c );
			return String( newstr, m_size + 1 );
		}
		String& operator=( const String& inStr )
		{
			if ( &inStr == this )
				return *this;
			*this = String( inStr );
			return *this;
		}
		String& operator=( String&& inStr ) noexcept
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
		String& operator=( const char* ptr )
		{
			*this = String( ptr );
			return *this;
		}
		const char& operator[]( const size_t i ) const { return m_data[ i ]; }
		char& operator[]( const size_t i ) { return m_data[ i ]; }
		const char& at( size_t i ) const
		{
			if ( i >= m_size )
				throw std::runtime_error("Invalid index");
			return operator[]( i );
		}
		char& at( size_t i )
		{
			if ( i >= m_size )
				throw std::runtime_error("Invalid index");
			return operator[]( i );
		}
		[[nodiscard]] bool operator==( const String& cmpStr ) const
		{
			if ( cmpStr.m_size != this->m_size )
				return false;
			for( size_t i = 0; i < m_size; i++ )
				if ( cmpStr[ i ] != m_data[ i ] )
					return false;
			return true;
		}
		[[nodiscard]] bool operator==( const char* cmpStr ) const
		{
			if ( strlen( cmpStr ) != this->m_size )
				return false;
			for ( size_t i = 0; i < m_size; i++ )
				if ( cmpStr[ i ] != m_data[ i ] )
					return false;
			return true;
		}
		[[nodiscard]] bool operator!=( const String& cmpStr ) const noexcept
		{
			return !( *this == cmpStr );
		}
		[[nodiscard]] bool operator!=( const char* cmpStr ) const noexcept
		{
			return !( *this == cmpStr );
		}
		[[nodiscard]] String toUpperCase() const
		{
			constexpr char diff = 'a' - 'A';
			char* tmp = new char[ m_size + 1 ];

			for ( size_t i = 0; i < m_size; i++ )
			{
				if ( m_data[ i ] >= 'a' && m_data[i] <= 'z' )
					tmp[ i ] = m_data[ i ] - diff;
				else
					tmp[ i ] = m_data[ i ];
			}
			return String( tmp, m_size );
		}
		[[nodiscard]] String toLowerCase() const
		{
			constexpr char diff = 'a' - 'A';
			char* tmp = new char[ m_size + 1 ];

			for ( size_t i = 0; i < m_size; i++ )
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
			constexpr size_t buffSz = 128;
			char buff[ buffSz ];
			in.get( buff, buffSz, '\n' );
			str = buff;
			return in;
		}
		friend std::ostream& operator<<( std::ostream& os, const String& str )
		{
			return os << str.c_str();
		}
		friend t::String operator+( const char* const lhs, const t::String& rhs )
		{
			auto lhslength = strlen( lhs );
			auto newstrBuffSize = lhslength + rhs.size() + 1;
			auto newstr = new char[ lhslength + rhs.size() + 1 ];
			memcpy_s( newstr, newstrBuffSize, lhs, lhslength );
			memcpy_s( newstr + lhslength -1, newstrBuffSize-lhslength, rhs.c_str(), rhs.size() + 1 );

			//sprintf_s( newstr, lhslength + rhs.size() + 1, "%s%s\0", lhs, rhs.c_str() );
			return String( newstr, lhslength + rhs.size() );
		}
		/**
		 * @brief Does not copy buffer, but instead takes buffer as 
		 * it's own data, therefore must be an allocated buffer
		 */
		static String makeString( char* const allocbuffer, size_t size )
		{
			return String( allocbuffer, size );
		}
	private:
		/**
		 * @brief Constructor that does not copy 
		 * allocated buffer, but takes it as it's own
		 */
		String( char* const allocbuffer, size_t size ):
			m_data( allocbuffer ),
			m_size( size ) {}
	private:
		char* m_data = nullptr;
		size_t m_size = 0;
	};

	namespace fast
	{
		class String
		{
		public:
			constexpr String() = default;

			template< typename T, typename = std::enable_if_t< std::is_arithmetic_v< T > && !std::is_floating_point_v< T > > >
			explicit String( T number )
			{
				if constexpr ( std::is_same_v< T, int64_t > )
				{
					if ( number == std::numeric_limits< int64_t >::min() )
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

				*this = String( end, &buff[ 21 ] - end );
			}

			String( const char* str, uint32_t length )
			{
				m_size = length;
				m_capacity = m_size + DEFAULT_EXTRA_PADDING;
				m_data = new char[ m_capacity + 1 ];
				memcpy( m_data, str, m_size );
				m_data[ m_size ] = '\0';
			}

			String( const char* str )
			{
				uint32_t length = (uint32_t) strlen( str );
				*this = String( str, length );
			}

			String( String const& str )
			{
				m_size = str.m_size;
				m_capacity = m_size + DEFAULT_EXTRA_PADDING;
				m_data = new char[ m_capacity + 1 ];
				memcpy( m_data, str.m_data, m_size );
				m_data[ m_size ] = '\0';
			}

			String( String&& str ) noexcept
			{
				*this = std::move( str );
			}

			String& operator=( String&& rhs ) noexcept
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

			String& operator=( String const& rhs )
			{
				if ( m_capacity >= rhs.m_size && m_data != nullptr )
				{
					memcpy( m_data, rhs.m_data, rhs.m_size );
					m_size = rhs.m_size;
					m_data[ m_size ] = '\0';
					return *this;
				}
				*this = String( rhs );
				return *this;
			}

			String& operator=( const char* rhs )
			{
				uint32_t length = (uint32_t) strlen( rhs );
				if ( m_capacity >= length && m_data != nullptr )
				{
					memcpy( m_data, rhs, length );
					m_size = length;
					m_data[ m_size ] = '\0';
					return *this;
				}
				*this = String( rhs );
				return *this;
			}

			String& operator+=( String const& rhs )
			{
				if ( m_size + rhs.m_size > m_capacity )
				{
					if ( m_size + rhs.m_size < m_capacity * 2 )
						reallocate();
					else
						reallocate( m_size + rhs.m_size );
				}

				memcpy( &m_data[ m_size ], rhs.m_data, rhs.m_size + 1 );
				m_size += rhs.m_size;
				return *this;
			}

			String& operator+=( const char* rhs )
			{
				auto length = (uint32_t) strlen( rhs );

				if ( m_size + length > m_capacity )
				{
					if ( m_size + length < m_capacity * 2 )
						reallocate();
					else
						reallocate( m_size + length );
				}

				memcpy( &m_data[ m_size ], rhs, length );
				m_size += length;
				m_data[ m_size ] = '\0';

				return *this;
			}

			String& operator+=( char c )
			{
				if ( m_size + 1 > m_capacity )
				{
					reallocate();
				}

				m_data[ m_size ] = c;
				m_data[ ++m_size ] = '\0';

				return *this;
			}

			bool operator==( String const& rhs ) const
			{
				if ( m_size != rhs.m_size )
					return false;
				
				for ( uint32_t i = 0; i < m_size; ++i )
				{
					if ( m_data[ i ] != rhs.m_data[ i ] )
						return false;
				}

				return true;
			}

			uint32_t size() const { return m_size; }

			uint32_t capacity() const { return m_capacity; }

			void reserve( uint32_t capacity )
			{
				if ( m_capacity != 0 && m_capacity < capacity )
					throw std::runtime_error("Excepted a larger buffer to allocate");
				
				auto newdata = new char[ capacity ];

				if ( m_data != nullptr )
				{
					memcpy( newdata, m_data, m_size );
				}

				delete[] m_data;
				m_data = newdata;
				m_capacity = capacity;
			}

			char* release()
			{
				auto ptr = m_data;
				m_data = nullptr;
				m_capacity = 0;
				m_size = 0;
				return ptr;
			}

			char* c_str() { return m_data; }
			const char* c_str() const { return m_data; }

			static inline String makeString( char* allocbuffer, uint32_t stringSize )
			{
				return String( allocbuffer, stringSize, stringSize );
			}

			static inline String makeString( char* allocbuffer, uint32_t stringSize, uint32_t bufferCapacity )
			{
				return String( allocbuffer, stringSize, bufferCapacity );
			}

			friend std::istream& operator>>( std::istream& in, t::String& str )
			{
				constexpr size_t buffSz = 256;
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
			void reallocate( uint32_t newcap )
			{
				auto newdata = new char[ newcap + 1 ];
				memcpy( newdata, m_data, m_size+1 );
				delete[] m_data;
				m_data = newdata;
				m_capacity = newcap;
			}

			void reallocate()
			{
				reallocate( m_capacity * 2 );
			}

			String( char* allocbuffer, uint32_t bufferSize, uint32_t bufferCapacity ):
				m_data( allocbuffer ),
				m_size( bufferSize + 1 ),
				m_capacity( bufferCapacity ) {}

		private:
			char* m_data = nullptr;
			uint32_t m_size = 0;
			uint32_t m_capacity = 0;

			static constexpr uint32_t DEFAULT_EXTRA_PADDING = 7;
		};
	}

	class StringView
	{
	public:
		StringView() = delete;

		template< size_t n >
		constexpr StringView( char const ( &str )[ n ] ):
			m_data( str ),
			m_size( n - 1 ) {}
		
		StringView( const char* str, size_t length ):
			m_data( str ),
			m_size( length ) {}

		StringView( String const& str ):
			m_data( str.c_str() ),
			m_size( str.size() ) {}

		StringView( fast::String const& str ):
			m_data( str.c_str() ),
			m_size( str.size() ) {}

		constexpr const char* data() const { return m_data; }
		constexpr size_t size() const { return m_size; }

		friend std::ostream& operator<<( std::ostream& os, StringView const& str )
		{
			return os << str.data();
		}

	private:
		const char* const m_data;
		const size_t m_size;
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
	uint64_t operator()( t::fast::String const& str ) const
	{
		const uint64_t size = str.size();

		uint64_t hash = 0x0f'0f'0f'0f'0f;

		auto half = size / 2u;

		auto quarter = half / 2u;

		auto _3quarters = quarter * 3u;

		auto data = str.c_str();

		if ( data == nullptr )
			return 0;

		size_t i = 0;

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

		hash ^= static_cast< uint64_t >( *reinterpret_cast< const uint8_t* >( data ) ) << 56u;

		return hash;
	}
};