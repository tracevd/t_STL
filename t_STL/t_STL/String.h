#pragma once

#include <iostream>

#include "Tint.h"
#include "Lib.h"
#include "Array.h"
#include "Algorithm.h"
#include "Error.h"
#include "Optional.h"

namespace t
{
	class Char
	{
	public:
		explicit constexpr Char( char c ):
			data( c ) {}

		constexpr auto operator<=>( Char c )
		{
			return data <=> c.data;
		}

		constexpr Char operator&( Char rhs )
		{
			return Char( data & rhs.data );
		}

		constexpr Char operator|( Char rhs )
		{
			return Char( data | rhs.data );
		}

		constexpr Char operator^( Char rhs )
		{
			return Char( data ^ rhs.data );
		}
	private:
		char data;
	};

	template< typename CharTy >
	struct CharTraits
	{
	public:
		static constexpr bool equal( CharTy c1, CharTy c2 )
		{
			return c1 == c2;
		}

		static constexpr CharTy NullTerminator = CharTy( '\0' );


	};

	template< class CharTy >
	class GenericStringView;

	template< class CharTy >
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
			return *( m_ptr - 1 );
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
			return *( m_ptr - 1 );
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

	template< class CharTy >
	class GenericString
	{
	public:
		using CharType = CharTy;
		using SizeType = uint64;
	public:
		constexpr static SizeType npos = t::limit< SizeType >::max;
		using ReverseIterator = StringReverseIterator< GenericString >;
		using ConstReverseIterator = StringConstReverseIterator< GenericString >;
	public:
		constexpr GenericString() = default;

		template< typename T, typename = type::enable_if< type::is_arithmetic< T > && !type::is_floating_point< T > > >
		constexpr explicit GenericString( T number )
		{
			if constexpr ( type::is_same< T, int64 > )
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
				
			if constexpr ( type::is_signed< T > )
			{
				if ( number < 0 )
					number = 0 - number;
			}

			do
			{
				auto mod = number % 10;
				*--end = static_cast< CharTy >( '0' ) + static_cast< CharTy >(mod);
				number /= 10;
			} while( number );
				
			if constexpr ( type::is_signed< T > )
			{
				if ( _number < 0 )
				{
					*--end = '-';
				}
			}

			*this = GenericString( end, static_cast< SizeType >( &buff[ 21 ] - end ) );
		}

		template< uint64 N >
		constexpr GenericString( char const ( &str )[ N ] )
		{
			*this = GenericString( str, static_cast< SizeType >( N-1 ) );
		}

		constexpr GenericString( const CharType* str, SizeType length ):
			m_data( length ? new CharTy[ length + 1 ] : nullptr ),
			m_size( length ),
			m_capacity( length )
		{
			if ( length )
			{
				strcpy( m_data, str, m_size );
				m_data[ m_size ] = '\0';
			}
		}

		constexpr explicit GenericString( const CharType* str )
		{
			SizeType length = static_cast< SizeType >( strlen( str ) );
			*this = GenericString( str, length );
		}

		constexpr GenericString( GenericString const& str ):
			m_data( str.m_size ? new CharTy[ str.m_size + 1 ] : nullptr ),
			m_size( str.m_size ),
			m_capacity( str.m_size )
		{
			if ( m_size )
			{
				strcpy( m_data, str.m_data, m_size );
				m_data[ m_size ] = '\0';
			}
		}

		constexpr GenericString( GenericString&& str ) noexcept
		{
			*this = move( str );
		}

		constexpr explicit GenericString( GenericStringView< CharTy > strv );

		constexpr ~GenericString()
		{
			delete[] m_data;
			m_data	   = nullptr;
			m_capacity = 0;
			m_size	   = 0;
		}

		constexpr GenericString& operator=( GenericString&& rhs ) noexcept
		{
			/*if ( this == &rhs )
				return *this;*/

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
			if ( m_capacity < rhs.m_size || m_data == nullptr || rhs.m_size == 0 )
			{
				*this = GenericString( rhs );
				return *this;
			}

			strcpy( m_data, rhs.m_data, rhs.m_size );
			m_size = rhs.m_size;
			m_data[ m_size ] = '\0';
			return *this;
		}

		constexpr GenericString& operator=( const CharTy* rhs )
		{
			SizeType length = strlen( rhs );
			if ( m_capacity >= length && m_data != nullptr )
			{
				strcpy( m_data, rhs, length );
				m_size = length;
				m_data[ m_size ] = '\0';
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

		constexpr auto crbegin() const { return ConstReverseIterator( m_data + m_size ); }
		constexpr auto crend() const { return ConstReverseIterator( m_data ); }
		constexpr auto rbegin() { return ReverseIterator( m_data + m_size ); }
		constexpr auto rend() { return ReverseIterator( m_data ); }
		constexpr auto rbegin() const { return crbegin(); }
		constexpr auto rend() const { return crend(); }

		constexpr GenericString substr( SizeType start, SizeType end = npos ) const
		{
			if ( end <= start )
				throw Error( "End cannot be less than or equal to start!", 1 );

			if ( m_size == 0 )
				throw Error( "Empty string!", 1 );

			if ( end > m_size )
				end = m_size;

			return GenericString( m_data + start, end - start );
		}

		constexpr GenericStringView< CharTy > substrv( SizeType start, SizeType end = npos ) const;

		constexpr void replace( CharTy from, CharTy to )
		{
			t::replace( *this, from, to );
		}

		constexpr void replaceFirst( CharTy from, CharTy to )
		{
			t::replaceFirst( *this, from, to );
		}

		constexpr void replaceLast( CharTy from, CharTy to )
		{
			t::replaceFirst( rbegin(), rend(), from, to );
		}

		constexpr SizeType indexOf( CharTy c ) const
		{
			auto it = t::find( *this, c );

			if ( it == cend() )
			{
				return npos;
			}

			return &*it - m_data;
		}

		constexpr SizeType lastIndexOf( CharTy c ) const
		{
			auto it = t::find( crbegin(), crend(), c );

			if ( it == crend() )
			{
				return npos;
			}

			return &*it - m_data;
		}

		constexpr Array< GenericString > split( CharTy delimeter ) const
		{
			if ( m_size == 0 )
				return {};

			Array< SizeType > indexes;

			for ( SizeType i = 0; i < m_size; ++i )
			{
				if ( m_data[ i ] == delimeter )
					indexes.pushBack( i );
			}

			if ( indexes.size() == 0 )
				return {};

			Array< GenericString > strings( indexes.size() + 1 );

			auto it = strings.data();

			*it = GenericString( m_data, indexes[ 0 ] );
			++it;

			for ( uint64 i = 0; i < indexes.size() - 1; ++i, ++it )
			{
				auto const currentIndex = indexes[ i ];
				auto const nextIndex    = indexes[ i + 1 ];
				*it = GenericString( &m_data[ currentIndex ] + 1, nextIndex - currentIndex - 1 );
			}

			*it = GenericString( &m_data[ indexes[ indexes.size() - 1 ] ] + 1, m_size - indexes[ indexes.size() - 1 ] - 1 );

			return strings;
		}

		constexpr const CharTy* data() const { return m_data; }
		constexpr CharTy* data() { return m_data; }

		constexpr CharTy const& operator[]( SizeType index ) const
		{
			return m_data[ index ];
		}

		constexpr CharTy& operator[]( SizeType index )
		{
			return m_data[ index ];
		}

		constexpr CharTy const& at( SizeType index ) const
		{
			if ( index >= m_size )
				throw Error( "Past string length!", 1 );
			return m_data[ index ];
		}

		constexpr CharTy& at( SizeType index )
		{
			if ( index >= m_size )
				throw Error( "Past string length!", 1 );
			return m_data[ index ];
		}

		constexpr bool operator<( GenericString const& rhs )
		{
			if ( m_size < rhs.m_size )
				return true;
				
			for ( SizeType i = 0; i < rhs.m_size; ++i )
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
				
			for ( SizeType i = 0; i < rhs.m_size; ++i )
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
			auto length = static_cast< SizeType >( strlen( rhs ) );

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

			m_data[ m_size ]   = c;
			m_data[ ++m_size ] = '\0';

			return *this;
		}

		constexpr GenericString& operator+=( GenericStringView< CharTy > strv );

		template< SizeType n >
		constexpr bool operator==( char const( &arr )[ n ] ) const
		{
			if ( n-1 != size() )
				return false;

			for ( SizeType i = 0; i < size(); ++i )
			{
				if ( arr[ i ] != m_data[ i ] )
					return false;
			}

			return true;
		}

		constexpr bool operator==( GenericString const& rhs ) const
		{
			if ( m_size != rhs.m_size )
				return false;

			if ( m_size == 0 )
				return true;
				
			for ( SizeType i = 0; i < m_size; ++i )
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

		constexpr SizeType size() const { return m_size; }
		constexpr SizeType length() const { return size(); }

		constexpr bool isEmpty() const { return size() == 0; }
		constexpr operator bool() const { return !isEmpty(); }

		constexpr SizeType capacity() const { return m_capacity; }

		constexpr void reserve( SizeType capacity )
		{
			if ( m_capacity >= capacity )
				return;
				
			auto newdata = new CharTy[ capacity + 1 ];

			if ( m_data != nullptr )
			{
				strcpy( newdata, m_data, m_size );
				delete[] m_data;
			}

			newdata[ m_size ] = '\0';

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

		constexpr static inline GenericString makeString( CharTy* allocbuffer, SizeType stringSize )
		{
			return GenericString( allocbuffer, stringSize, stringSize );
		}

		constexpr static inline GenericString makeString( CharTy* allocbuffer, SizeType stringSize, SizeType bufferCapacity )
		{
			return GenericString( allocbuffer, stringSize, bufferCapacity );
		}
		
		friend std::istream& operator>>( std::istream& in, GenericString& str )
		{
			constexpr SizeType buffSz = 256;
			CharTy buff[ buffSz ] = { 0 };
			in.getline( buff, buffSz, '\n' );
			auto const count = in.gcount();
			str = GenericString( buff, count - 1 );
			return in;
		}

		friend std::ostream& operator<<( std::ostream& os, const GenericString& str )
		{
			if ( str.c_str() == nullptr )
				return os << "";
			return os << str.c_str();
		}

	private:
		constexpr void reallocate( SizeType newcap )
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
				return;
			}
			reallocate( m_capacity * 2 );
		}

		constexpr GenericString( CharTy* allocbuffer, SizeType bufferSize, SizeType bufferCapacity ):
			m_data( allocbuffer ),
			m_size( bufferSize + 1 ),
			m_capacity( bufferCapacity ) {}

	private:
		CharTy* m_data = nullptr;
		SizeType m_size = 0;
		SizeType m_capacity = 0;
	};

	using String = GenericString< char >;

	template< class CharTy >
	class GenericStringView
	{
	public:
		using CharType = CharTy;
		using SizeType = uint64;
		using ReverseIterator = StringConstReverseIterator< GenericStringView >;
		using ConstReverseIterator = ReverseIterator;
	public:
		static constexpr auto npos = t::limit< SizeType >::max;
	public:
		GenericStringView()										 = default;
		GenericStringView( GenericStringView const& )			 = default;
		GenericStringView( GenericStringView&& )				 = default;
		GenericStringView& operator=( GenericStringView const& ) = default;
		GenericStringView& operator=( GenericStringView&& )		 = default;

		template< SizeType n >
		constexpr GenericStringView( char const ( &str )[ n ] ):
			m_data( str ),
			m_size( n - 1 ) {}
		
		constexpr GenericStringView( const CharTy* str, SizeType length ):
			m_data( str ),
			m_size( length ) {}

		constexpr explicit GenericStringView( GenericString< CharTy > const& str ):
			m_data( str.data() ),
			m_size( str.size() ) {}

		constexpr const CharTy* data() const { return m_data; }
		constexpr SizeType size() const { return m_size; }
		constexpr auto length() const { return size(); }

		constexpr bool isEmpty() const { return size() == 0; }
		constexpr operator bool() const { return !isEmpty(); }

		constexpr CharTy const* begin() const { return m_data; }
		constexpr CharTy const* end() const { return m_data + m_size; }

		constexpr CharTy const* cbegin() const { return begin(); }
		constexpr CharTy const* cend() const { return end(); }

		constexpr auto rbegin() const { return ReverseIterator( m_data + m_size ); }
		constexpr auto rend() const { return ReverseIterator( m_data ); }

		constexpr auto crbegin() const { return rbegin(); }
		constexpr auto crend() const { return rend(); }

		constexpr SizeType indexOf( CharTy c ) const
		{
			auto it = t::find( *this, c );

			if ( it == cend() )
			{
				return npos;
			}

			return &*it - m_data;
		}

		constexpr SizeType lastIndexOf( CharTy c ) const
		{
			auto it = t::find( crbegin(), crend(), c );

			if ( it == crend() )
			{
				return npos;
			}

			return &*it - m_data;
		}

		constexpr GenericStringView substrv( SizeType begin, SizeType end = npos ) const
		{
			if ( end <= begin )
				throw Error( "End cannot be less than or equal to start!", 1 );

			auto const size_ = m_size;

			if ( size_ == 0 )
				throw Error( "Empty string!", 1 );

			if ( end > size_ )
				end = size_;

			return GenericStringView( m_data + begin, end - begin );
		}

		constexpr char const& at( SizeType index ) const
		{
			if ( index >= m_size )
			{
				throw Error( "String index was too large!", 1 );
			}

			return m_data[ index ];
		}

		constexpr char const& operator[]( SizeType index ) const
		{
			return m_data[ index ];
		}

		friend std::ostream& operator<<( std::ostream& os, GenericStringView const& str )
		{
			os.write( str.data(), str.size() );
			return os;
		}

	private:
		const CharTy* m_data;
		uint64 m_size;
	};

	template< class CharTy >
	constexpr GenericString< CharTy >::GenericString( GenericStringView< CharTy > strv ):
		GenericString( strv.data(), strv.size() ) {}

	template< typename CharTy >
	constexpr bool operator==( GenericString< CharTy > const& lhs, GenericStringView< CharTy > rhs )
	{
		auto const size = rhs.size();

		if ( lhs.size() != size )
			return false;

		for ( typename GenericString< CharTy >::SizeType i = 0; i < size; ++i )
		{
			if ( lhs[ i ] != rhs[ i ] )
				return false;
		}

		return true;
	}

	template< typename CharTy >
	constexpr bool operator==( GenericStringView< CharTy > lhs, GenericString< CharTy > const& rhs )
	{
		return rhs == lhs;
	}

	template< typename CharTy >
	constexpr bool operator!=( GenericString< CharTy > const& lhs,  GenericStringView< CharTy > rhs )
	{
		return !( lhs == rhs );
	}

	template< typename CharTy >
	constexpr bool operator!=( GenericStringView< CharTy > lhs, GenericString< CharTy > const& rhs )
	{
		return !( lhs == rhs );
	}

	template< typename CharTy >
	constexpr GenericString< CharTy > operator+( GenericStringView< CharTy > lhs, GenericString< CharTy > const& rhs )
	{
		GenericString< CharTy > str;

		str.reserve( lhs.size() + rhs.size() );

		str += lhs;

		str += rhs;

		return str;
	}

	template< typename CharTy >
	constexpr GenericString< CharTy > operator+( GenericString< CharTy > const& lhs, GenericString< CharTy > const& rhs )
	{
		GenericString< CharTy > str;

		str.reserve( lhs.size() + rhs.size() );

		str += lhs;
		
		str += rhs;

		return str;
	}

	template< typename CharTy >
	constexpr GenericString< CharTy > operator+( GenericString< CharTy > const& lhs, GenericStringView< CharTy > rhs )
	{
		GenericString< CharTy > str;

		str.reserve( lhs.size() + rhs.size() );

		str += lhs;

		str += rhs;

		return str;
	}

	template< typename CharTy, uint64 N >
	constexpr GenericString< CharTy > operator+( GenericString< CharTy > const& lhs, CharTy const(&arr)[ N ] )
	{
		return lhs + GenericStringView< CharTy >( arr );
	}

	using StringView = GenericStringView< char >;

	namespace string_literals
	{
		consteval StringView operator "" _sv( const char* str, size_t length ) noexcept
		{
			return StringView( str, length );
		}

		constexpr String operator "" _s( const char* str, size_t length )
		{
			return String( str, uint64( length ) );
		}
	}	

	template< class CharTy >
	constexpr GenericStringView< CharTy > GenericString< CharTy >::substrv( SizeType start, SizeType end ) const
	{
		if ( end <= start )
			throw Error( "End cannot be less than or equal to start!", 1 );

		auto const size_ = m_size;

		if ( size_ == 0 )
			throw Error( "Empty string!", 1 );

		if ( end > size_ )
			end = size_;

		return GenericStringView( m_data + start, end - start );
	}

	template< class CharTy >
	constexpr GenericString< CharTy >& GenericString< CharTy >::operator+=( GenericStringView< CharTy > strv )
	{
		if ( m_size + strv.size() > m_capacity )
		{
			if ( m_size + strv.size() < m_capacity * 2 )
				reallocate();
			else
				reallocate( m_size + strv.size() );
		}

		strcpy( &m_data[ m_size ], strv.data(), strv.size() );
		m_size += strv.size();
		m_data[ m_size ] = '\0';
		return *this;
	}

	template< typename T, typename StringTy, typename = type::enable_if< type::is_integer< T > && !type::is_reference< T > > >
	Optional< T > fromString( StringTy const& str )
	{
		bool isNeg = false;

		if ( str.size() == 0 )
		{
			return {};
		}

		uint64 index = 0;

		if constexpr ( type::is_signed< T > )
		{
			if ( str[ index ] == '-' )
			{
				isNeg = true;
				++index;
			}

			if ( str.size() == index )
			{
				return {};
			}
		}

		T val = 0;

		for ( ; index < str.size(); ++index )
		{
			auto const c = str[ index ];

			if ( c < '0' || c > '9' )
				return {};

			val *= 10;
			val += c - '0';
		}

		if constexpr ( type::is_signed< T > )
		{
			if ( isNeg )
			{
				val *= -1;
			}
		}

		return Optional< T >( val );
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

		/*const uint64_t size = str.size();

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

		return hash;*/
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
				case '[': throw Error( "nope", 1 );
				case '!': {
					if ( patternIndex >= pattern.size() - 1 )
						throw Error( "Invalid syntax!", 1 );
					++patternIndex;
					if ( pattern[ patternIndex ] == string[ stringIndex ] )
						return false;
					++patternIndex;
					++stringIndex;
				} break;
				case '\\': {
					if ( patternIndex >= pattern.size() - 1 )
						throw Error( "Invalid syntax!", 1 );
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
