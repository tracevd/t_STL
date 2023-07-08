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
	private:
		char* m_data = nullptr;
		size_t m_size = 0;
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
			auto newstr = new char[ lhslength + rhs.size() + 1 ];
			sprintf_s( newstr, lhslength + rhs.size() + 1, "%s%s\0", lhs, rhs.c_str() );
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
	};

}

template<>
struct std::hash< t::String >
{
	size_t operator()( const t::String& str ) const
	{
		return hash< std::string_view >()( str.c_str() );
	}
};