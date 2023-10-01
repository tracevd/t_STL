#pragma once

#include <iostream>

#include "Tint.h"
#include "Lib.h"
#include "DynamicArray.h"

namespace t
{
	template< class CharTy, class SizeTy >
	class GenericStringView;

	template< class CharTy, class SizeTy >
	class GenericString;

	template< class String >
	class StringReverseIterator
	{
	public:
		using CharType = typename String::CharType;
	public:
		constexpr StringReverseIterator& operator++()
		{
			--m_ptr;
			return *this;
		}

		constexpr CharType& operator*()
		{
			return *m_ptr;
		}

		constexpr bool operator==( StringReverseIterator const& rhs ) const
		{
			return m_ptr == rhs.m_ptr;
		}

		constexpr bool operator!=( StringReverseIterator const& rhs ) const
		{
			return !( *this == rhs );
		}
	private:
		constexpr StringReverseIterator( CharType* ptr ):
			m_ptr( ptr ) {}
	private:
		CharType* m_ptr = nullptr;
		friend String;
	};

	template< class String >
	class StringConstReverseIterator
	{
	public:
		using CharType = typename String::CharType const;
	public:
		constexpr StringConstReverseIterator& operator++()
		{
			--m_ptr;
			return *this;
		}

		constexpr CharType& operator*() const
		{
			return *m_ptr;
		}

		constexpr bool operator==( StringConstReverseIterator const& rhs ) const
		{
			return m_ptr == rhs.m_ptr;
		}

		constexpr bool operator!=( StringConstReverseIterator const& rhs ) const
		{
			return !(*this == rhs);
		}
	private:
		constexpr StringConstReverseIterator( CharType* ptr ):
			m_ptr( ptr ) {}
	private:
		CharType* m_ptr = nullptr;
		friend String;
	};

	template< class CharTy, class SizeTy >
	class GenericString
	{
	public:
		using CharType = CharTy;
		using SizeType = SizeTy;
	public:
		constexpr static SizeTy npos = t::limit< SizeTy >::max;
		using ReverseIterator = StringReverseIterator< GenericString >;
		using ConstReverseIterator = StringConstReverseIterator< GenericString >;
	public:
		constexpr GenericString() = default;

		//constexpr explicit GenericString( float number )
		//{
		//	constexpr double PRECISION = 0.00000000000001;
		//	constexpr uint32 MAX_NUMBER_STRING_SIZE = 32;

		//	// handle special cases
		//	if ( number != number ) // NaN
		//	{
		//		m_data = new char[ 4 ];
		//		m_size = m_capacity = 3;
		//		strcpy( m_data, "NaN", 4 );
		//		return;
		//	}
		//	else if ( number > FLT_MAX ) {
		//		m_data = new char[ 4 ];
		//		m_size = m_capacity = 3;
		//		strcpy( m_data, "inf", 4 );
		//		return;
		//	}
		//	else if ( number == 0.0 ) {
		//		m_data = new char[ 2 ];
		//		m_size = m_capacity = 1;
		//		strcpy( m_data, "0", 2 );
		//		return;
		//	}

		//	m_data = new char[ MAX_NUMBER_STRING_SIZE ];
		//	m_size = MAX_NUMBER_STRING_SIZE;
		//	m_capacity = MAX_NUMBER_STRING_SIZE;

		//	int digit, m, m1;
		//	char* c = m_data;
		//	int neg = (number < 0);
		//	if ( neg )
		//		number = -number;
		//	// calculate magnitude
		//	m = static_cast< int >( log10( number ) );
		//	int useExp = (m >= 14 || (neg && m >= 9) || m <= -9);
		//	if ( neg )
		//		*(c++) = '-';
		//	// set up for scientific notation
		//	if ( useExp ) {
		//		if ( m < 0 )
		//			m -= 1;
		//		number = number / static_cast< float >( pow( 10.0f, m ) );
		//		m1 = m;
		//		m = 0;
		//	}
		//	if ( m < 1.0 ) {
		//		m = 0;
		//	}
		//	// convert the number
		//	while ( number > PRECISION || m >= 0 ) {
		//		double weight = pow( 10.0, m );
		//		if ( weight > 0 && !isinf( weight ) ) {
		//			digit = static_cast< int >( floor( number / weight ) );
		//			number -= static_cast< float >(digit * weight);
		//			*(c++) = '0' + digit;
		//		}
		//		if ( m == 0 && number > 0 )
		//			*(c++) = '.';
		//		m--;
		//	}
		//	if ( useExp ) {
		//		// convert the exponent
		//		*(c++) = 'e';
		//		if ( m1 > 0 ) {
		//			*(c++) = '+';
		//		}
		//		else {
		//			*(c++) = '-';
		//			m1 = -m1;
		//		}
		//		m = 0;
		//		while ( m1 > 0 ) {
		//			*(c++) = '0' + m1 % 10;
		//			m1 /= 10;
		//			m++;
		//		}
		//		c -= m;
		//		for ( uint32 i = 0, j = m - 1; i < j; ++i, --j ) {
		//			// swap without temporary
		//			c[ i ] ^= c[ j ];
		//			c[ j ] ^= c[ i ];
		//			c[ i ] ^= c[ j ];
		//		}
		//		c += m;
		//	}
		//	*(c) = '\0';

		//	m_size = static_cast< uint32 >( c - m_data );
		//}

		template< typename T, typename = std::enable_if_t< std::is_arithmetic_v< T > && !std::is_floating_point_v< T > > >
		constexpr explicit GenericString( T number )
		{
			if constexpr ( std::is_same_v< T, int64 > )
			{
				if ( number == limit< int64 >::min )
				{
					*this = GenericString( "-9223372036854775808", 20 );
					return;
				}
			}

			CharTy buff[ 21 ];
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

			*this = GenericString( end, static_cast< SizeTy >( &buff[21] - end ) );
		}

		template< uint64 N >
		constexpr GenericString( char const ( &str )[ N ] )
		{
			*this = GenericString( str, static_cast< SizeTy >( N-1 ) );
		}

		constexpr GenericString( const CharTy* str, SizeTy length ):
			m_data( new CharTy[length + 1] ),
			m_size( length ),
			m_capacity( length )			
		{
			strcpy( m_data, str, m_size );
			m_data[ m_size ] = '\0';
		}

		constexpr GenericString( const CharTy* str )
		{
			SizeTy length = static_cast< SizeTy >( strlen( str ) );
			*this = GenericString( str, length );
		}

		constexpr GenericString( GenericString const& str ):
			m_data( new CharTy[ str.m_size + 1 ] ),
			m_size( str.m_size ),
			m_capacity( str.m_size )
		{
			strcpy( m_data, str.m_data, m_size );
			m_data[ m_size ] = '\0';
		}

		constexpr GenericString( GenericString&& str ) noexcept
		{
			*this = std::move( str );
		}

		constexpr ~GenericString()
		{
			delete[] m_data;
			m_data	   = nullptr;
			m_capacity = 0;
			m_size	   = 0;
		}

		constexpr GenericString& operator=( GenericString&& rhs ) noexcept
		{
			delete[] m_data;

			m_data     = rhs.m_data;
			m_size 	   = rhs.m_size;
			m_capacity = rhs.m_capacity;

			rhs.m_data     = nullptr;
			rhs.m_size     = 0;
			rhs.m_capacity = 0;

			return *this;
		}

		constexpr GenericString& operator=( GenericString const& rhs )
		{
			if ( m_capacity >= rhs.m_size && m_data != nullptr )
			{
				strcpy< char >( m_data, rhs.m_data, rhs.m_size );
				m_size = rhs.m_size;
				m_data[m_size] = '\0';
				return *this;
			}
			*this = GenericString( rhs );
			return *this;
		}

		constexpr GenericString& operator=( const CharTy* rhs )
		{
			SizeTy length = (SizeTy) strlen( rhs );
			if ( m_capacity >= length && m_data != nullptr )
			{
				strcpy< CharTy >( m_data, rhs, length );
				m_size = length;
				m_data[m_size] = '\0';
				return *this;
			}
			*this = GenericString( rhs, length );
			return *this;
		}

		constexpr CharTy const* cbegin() const { return m_data; }
		constexpr CharTy const* cend() const { return m_data + m_size; }
		constexpr CharTy* begin() { return m_data; }
		constexpr CharTy* end() { return m_data + m_size; }
		constexpr CharTy const* begin() const { return cbegin(); }
		constexpr CharTy const* end() const { return cend(); }

		constexpr auto crbegin() const { return ConstReverseIterator( m_data + m_size - 1 ); }
		constexpr auto crend() const { return ConstReverseIterator( m_data - 1 ); }
		constexpr auto rbegin() { return ReverseIterator( m_data + m_size - 1 ); }
		constexpr auto rend() { return ReverseIterator( m_data - 1 ); }
		constexpr auto rbegin() const { return crbegin(); }
		constexpr auto rend() const { return crend(); }

		constexpr GenericString substr( SizeTy start, SizeTy end = npos ) const
		{
			if ( end <= start )
				throw std::runtime_error("End cannot be less than or equal to start!");

			auto const size_ = m_size;

			if ( size_ == 0 )
				throw std::runtime_error("Empty string!");

			if ( end > size_ )
				end = size_;

			return GenericString( m_data + start, end - start );
		}

		constexpr GenericStringView< CharTy, SizeTy > substrv( SizeTy start, SizeTy end = npos ) const;

		constexpr void replace( CharTy from, CharTy to )
		{
			if ( m_data == nullptr )
				return;

			for ( SizeTy i = 0; i < m_size; ++i )
			{
				if ( m_data[ i ] == from )
				{
					m_data[ i ] = std::move( to );
				}
			}
		}

		constexpr void replaceFirst( CharTy from, CharTy to )
		{
			if ( m_data == nullptr )
				return;

			for ( SizeTy i = 0; i < m_size; ++i )
			{
				if ( m_data[ i ] == from )
				{
					m_data[ i ] = std::move( to );
					return;
				}
			}
		}

		constexpr void replaceLast( CharTy from, CharTy to )
		{
			if ( m_data == nullptr )
				return;

			const CharTy* const b4begin = m_data - 1;

			for ( auto _data = m_data + m_size-1; _data > b4begin; --_data )
			{
				if ( *_data == from )
				{
					*_data = to;
					return;
				}
			}
		}

		constexpr SizeTy indexOf( CharTy c ) const
		{
			if ( m_data == nullptr )
				return npos;

			for ( SizeTy i = 0; i < m_size; ++i )
			{
				if ( m_data[ i ] == c )
					return i;
			}

			return npos;
		}

		constexpr CharTy lastIndexOf( CharTy c ) const
		{
			if ( m_data == nullptr )
				return npos;

			const CharTy* const b4begin = m_data - 1;

			for ( auto _data = m_data + m_size - 1; _data > b4begin; --_data )
			{
				if ( *_data == c )
					return _data - m_data;
			}

			return npos;
		}

		constexpr DynamicArray< GenericString > split( CharTy c ) const
		{
			if ( m_data == nullptr )
				return {};

			DynamicArray< SizeTy > indexes;

			for ( SizeTy i = 0; i < m_size; ++i )
			{
				if ( m_data[ i ] == c )
					indexes.pushBack( i );
			}

			if ( indexes.isEmpty() )
				return {};

			indexes.pushBack( m_size );

			DynamicArray< GenericString > strings( indexes.size() );

			auto stringptr = strings.data();

			for ( SizeTy i = 0; i < indexes.size()-1; ++i, ++stringptr )
			{
				*stringptr = GenericString( &m_data[ indexes[ i ] ], indexes[ i+1 ] - indexes[ i ] );
			}

			return strings;
		}

		constexpr const CharTy* data() const { return m_data; }
		constexpr CharTy* data() { return m_data; }

		constexpr CharTy const& operator[]( SizeTy index ) const
		{
			return m_data[ index ];
		}

		constexpr CharTy& operator[]( SizeTy index )
		{
			return m_data[ index ];
		}

		constexpr CharTy const& at( SizeTy index ) const
		{
			if ( index >= m_size )
				throw std::runtime_error("Past string length!");
			return m_data[ index ];
		}

		constexpr CharTy& at( SizeTy index )
		{
			if ( index >= m_size )
				throw std::runtime_error( "Past string length!" );
			return m_data[ index ];
		}

		constexpr bool operator<( GenericString const& rhs )
		{
			if ( m_size < rhs.m_size )
				return true;
				
			for ( SizeTy i = 0; i < rhs.m_size; ++i )
			{
				if ( m_data[ i ] >= rhs.m_data[ i ] )
					return false;
			}

			return true;
		}

		constexpr bool operator>( GenericString const& rhs )
		{
			if ( m_size > rhs.m_size )
				return true;
				
			for ( SizeTy i = 0; i < rhs.m_size; ++i )
			{
				if ( m_data[ i ] <= rhs.m_data[ i ] )
					return false;
			}

			return true;
		}

		constexpr GenericString& operator+=( GenericString const& rhs )
		{
			if ( m_size + rhs.m_size > m_capacity )
			{
				if ( m_size + rhs.m_size < m_capacity * 2 )
					reallocate();
				else
					reallocate( m_size + rhs.m_size );
			}

			strcpy( &m_data[ m_size ], rhs.m_data, rhs.m_size + 1 );
			m_size += rhs.m_size;
			return *this;
		}

		constexpr GenericString& operator+=( const CharTy* rhs )
		{
			auto length = static_cast< SizeTy >( strlen( rhs ) );

			if ( m_size + length > m_capacity )
			{
				if ( m_size + length < m_capacity * 2 )
					reallocate();
				else
					reallocate( m_size + length );
			}

			strcpy( &m_data[ m_size ], rhs, length );
			m_size += length;
			m_data[ m_size ] = '\0';

			return *this;
		}

		constexpr GenericString& operator+=( CharTy c )
		{
			if ( m_size + 1 > m_capacity )
			{
				reallocate();
			}

			m_data[ m_size ] = std::move( c );
			m_data[ ++m_size ] = '\0';

			return *this;
		}

		constexpr bool operator==( GenericString const& rhs ) const
		{
			if ( this == &rhs )
				return true;

			if ( m_size != rhs.m_size )
				return false;

			if ( m_size == 0 )
				return true;
				
			for ( SizeTy i = 0; i < m_size; ++i )
			{
				if ( m_data[ i ] != rhs.m_data[ i ] )
					return false;
			}

			return true;
		}

		constexpr bool operator!=( GenericString const& rhs ) const
		{
			return !( *this == rhs );
		}

		constexpr SizeTy size() const { return m_size; }

		constexpr SizeTy capacity() const { return m_capacity; }

		constexpr void reserve( SizeTy capacity )
		{
			if ( m_capacity >= capacity )
				throw std::runtime_error("Excepted a larger buffer to allocate");
				
			auto newdata = new CharTy[ capacity ];

			if ( m_data != nullptr )
			{
				strcpy( newdata, m_data, m_size );
				delete[] m_data;
			}

			m_data = newdata;
			m_capacity = capacity;
		}

		constexpr CharTy* release()
		{
			auto ptr = m_data;
			m_data = nullptr;
			m_capacity = 0;
			m_size = 0;
			return ptr;
		}

		constexpr CharTy* c_str() { return m_data; }
		constexpr const CharTy* c_str() const { return m_data; }

		constexpr static inline GenericString makeString( CharTy* allocbuffer, SizeTy stringSize )
		{
			return GenericString( allocbuffer, stringSize, stringSize );
		}

		constexpr static inline GenericString makeString( CharTy* allocbuffer, SizeTy stringSize, SizeTy bufferCapacity )
		{
			return GenericString( allocbuffer, stringSize, bufferCapacity );
		}
		
		friend std::istream& operator>>( std::istream& in, GenericString& str )
		{
			constexpr SizeTy buffSz = 256;
			CharTy buff[ buffSz ] = { 0 };
			in.get( buff, buffSz, '\n' );
			str = buff;
			return in;
		}

		friend std::ostream& operator<<( std::ostream& os, const GenericString& str )
		{
			if ( str.c_str() == nullptr )
				return os << "";
			return os << str.c_str();
		}

	private:
		constexpr void reallocate( SizeTy newcap )
		{
			auto newdata = new CharTy[ newcap + 1 ];
			if ( m_data != nullptr )
			{
				strcpy< CharTy >( newdata, m_data, m_size + 1 );
				delete[] m_data;
			}
				
			m_data = newdata;
			m_capacity = newcap;
		}

		constexpr void reallocate()
		{
			if ( m_capacity == 0 )
			{
				reallocate( 10 );
			}
			reallocate( m_capacity * 2 );
		}

		constexpr GenericString( CharTy* allocbuffer, SizeTy bufferSize, SizeTy bufferCapacity ):
			m_data( allocbuffer ),
			m_size( bufferSize + 1 ),
			m_capacity( bufferCapacity ) {}

	private:
		CharTy* m_data = nullptr;
		SizeTy m_size = 0;
		SizeTy m_capacity = 0;
	};

	using String = GenericString< char, uint64 >;

	template< class CharTy, class SizeTy >
	class GenericStringView
	{
	public:
		static constexpr auto npos = t::limit< SizeTy >::max;
	public:
		using CharType = CharTy;
		using SizeType = SizeTy;
	public:
		GenericStringView() = delete;

		template< SizeTy n >
		constexpr GenericStringView( char const ( &str )[ n ] ):
			m_data( str ),
			m_size( n - 1 ) {}
		
		constexpr GenericStringView( const CharTy* str, SizeTy length ):
			m_data( str ),
			m_size( length ) {}

		template< class Size >
		constexpr GenericStringView( GenericString< CharTy, Size > const& str ):
			m_data( str.c_str() ),
			m_size( str.size() ) {}

		constexpr const CharTy* data() const { return m_data; }
		constexpr SizeTy size() const { return m_size; }

		constexpr GenericStringView substrv( SizeTy begin, SizeTy end = npos ) const
		{
			if ( end <= begin )
				throw std::runtime_error( "End cannot be less than or equal to start!" );

			auto const size_ = m_size;

			if ( size_ == 0 )
				throw std::runtime_error( "Empty string!" );

			if ( end > size_ )
				end = size_;

			return GenericStringView( m_data + begin, end - begin );
		}

		constexpr char operator[]( SizeTy index ) const
		{
			return m_data[ index ];
		}

		friend std::ostream& operator<<( std::ostream& os, GenericStringView const& str )
		{
			return os << str.data();
		}

	private:
		const CharTy* const m_data;
		const uint64 m_size;
	};

	using StringView = GenericStringView< char, uint64 >;

	template< class CharTy, class SizeTy >
	constexpr GenericStringView< CharTy, SizeTy > GenericString< CharTy, SizeTy >::substrv( SizeTy start, SizeTy end ) const
	{
		if ( end <= start )
			throw std::runtime_error( "End cannot be less than or equal to start!" );

		auto const size_ = m_size;

		if ( size_ == 0 )
			throw std::runtime_error( "Empty string!" );

		if ( end > size_ )
			end = size_;

		return GenericStringView( m_data + start, end - start );
	}
}

//template<>
//struct std::hash< t::String >
//{
//	uint64_t operator()( const t::String& str ) const
//	{
//		return std::hash< std::string_view >{}( str.c_str() );
//	}
//};

constexpr uint64 hash_fstring( t::String const& str )
{
	uint64 hash = 2166136261;
	//constexpr uint64 FNVMultiple = 16777619;

	if ( str.size() == 0 )
		return 0;

	for ( t::String::SizeType i = 0; i < str.size(); ++i )
	{
		const auto c = str[ i ];
		hash += c;
		//hash ^= ( hash & 255 ) << ( c & 55 );
		hash ^= hash << 1;
		//hash *= FNVMultiple;
	}
	/*uint64 hash = 0x7deadbfabeef;

	auto data = str.data();

	if ( data == nullptr )
		return 0;

	for ( ; *data; ++data )
	{
		auto const c = *data;

		hash *= ( uint64( 0x7bd00f ) << ( c & 55 ) ) + 1;

		hash += c;
	}*/

	return hash;
}

template<>
struct std::hash< t::String >
{
	constexpr uint64_t operator()( t::String const& str ) const
	{
		return hash_fstring( str );

		const uint64_t size = str.size();

		uint64_t hash = 0x7f'0f'7f'0f'7f;

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

template<>
struct t::hasher< t::String >
{
	constexpr static inline uint64 hash( t::String const& str )
	{
		return std::hash< t::String >{}( str );
	}
};

namespace t
{
	namespace string
	{
		/**
		 * Special characters:
		 *   '*'     -> Any character 0-inf times
		 *   '?'     -> Any character 1 time
		 *   '[...]' -> Group of characters
		 *       Where:
		 *         [a-z] is a range
		 *         [abc] is a group
		 *   '!'     -> Not the following character
		 *   '\'     -> Escape next special character
		 */
		template< class StrTy1, class StrTy2 >
		constexpr inline bool match( StrTy1 const& pattern, StrTy2 const& string )
		{
			if ( pattern.size() == 0 )
				return false;
			if ( string.size() == 0 )
				return false;

			uint32 patternIndex = 0;
			uint32 stringIndex  = 0;

			while ( patternIndex < pattern.size() && stringIndex < string.size() )
			{
				switch ( pattern[ patternIndex ] )
				{
				case '?':
					++stringIndex;
					++patternIndex;
					break;
				case '*': {
					while ( patternIndex != pattern.size() && stringIndex != string.size() )
					{
						if ( patternIndex == pattern.size() - 1 )
							return true;
						if ( match( StringView( pattern.data() + patternIndex + 1, pattern.size() - patternIndex - 1 ), StringView( string.data() + stringIndex, string.size() - stringIndex ) ) )
							return true;
						++stringIndex;
					}
					return false;
				}
				case '[': throw std::runtime_error("nope");
				case '!': {
					if ( patternIndex >= pattern.size() - 1 )
						throw std::runtime_error("Invalid syntax!");
					++patternIndex;
					if ( pattern[ patternIndex ] == string[ stringIndex ] )
						return false;
					++patternIndex;
					++stringIndex;
				} break;
				case '\\': {
					if ( patternIndex >= pattern.size() - 1 )
						throw std::runtime_error( "Invalid syntax!" );
					++patternIndex;
					if ( pattern[ patternIndex ] != string[ stringIndex ] )
						return false;
					++patternIndex;
					++stringIndex;
				}
				default:
					if ( pattern[ patternIndex ] != string[ stringIndex ] )
						return false;
					++patternIndex;
					++stringIndex;
				}
			}

			return stringIndex == string.size() && patternIndex == pattern.size();
		}
	}
}
