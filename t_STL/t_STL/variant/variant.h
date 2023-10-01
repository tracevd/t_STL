#pragma once

#include "../Type.h"
#include "../HashMap.h"

#include "Types.h"


namespace t
{
	namespace variant
	{
		class Map;

		class Base
		{
		public:
			constexpr virtual ~Base() = 0;
			constexpr virtual SharedPtr< Base > clone() const = 0;
		};

		constexpr Base::~Base() {};

		template< class T >
		class Derived : public Base
		{
		public:
			constexpr Derived( T&& data ):
				m_data( std::move( data ) ) {}

			constexpr Derived( T const& data ) :
				m_data( data ) {}

			constexpr virtual ~Derived() override = default;

			constexpr virtual SharedPtr< Base > clone() const final override
			{
				return SharedPtr< Base >( new Derived( m_data ) );
			}

			T m_data;
		};

		class Value
		{
		public:
			constexpr Value() = default;

			constexpr Value( Value&& other ) noexcept:
				m_ptr( std::move( other.m_ptr ) ),
				m_type( other.m_type )
			{
				other.m_type = Type::VOID;
			}

			constexpr Value( Value const& other ):
				m_ptr( other.m_ptr ),
				m_type( other.m_type ) {}

			constexpr Value( const char* str ):
				m_ptr( new Derived< String >( str ) ),
				m_type( templateToVariantType< String >() ) {}

			template< class T >
			constexpr explicit Value( T&& );

			template< class T >
			constexpr explicit Value( T const& );

			template< class T >
			constexpr explicit Value( T& );

			constexpr ~Value() = default;

			constexpr Value& operator=( Value&& rhs ) noexcept
			{
				m_ptr = std::move( rhs.m_ptr );
				m_type = rhs.m_type;
				rhs.m_type = Type::VOID;
				return *this;
			}

			constexpr Value& operator=( Value const& rhs )
			{
				m_ptr = rhs.m_ptr;
				m_type = rhs.m_type;
				return *this;
			}

			constexpr Value& operator=( const char* str )
			{
				m_ptr = new Derived< String >( str );
				m_type = templateToVariantType< String >();
				return *this;
			}

			template< class T >
			constexpr Value& operator=( T&& );

			template< class T >
			constexpr Value& operator=( T const& );

			template< class T >
			constexpr Value& operator=( T& );

			constexpr Value clone() const
			{
				Value val;
				if ( m_ptr )
					val.m_ptr = m_ptr->clone();
				return val;
			}

			constexpr Type getType() const { return m_type; }

			template< class T, class = type::enable_if<
				type::is_same< T, type::decay< T > >
				|| type::is_const< type::remove_reference< T > > > >
			constexpr T As() const;

			template< class T, class = type::enable_if<
				type::is_lvalue_reference< T > > >
			constexpr T As();

			constexpr bool operator==( Value const& ) const;

			constexpr bool operator!=( Value const& rhs ) const { return !(*this == rhs); }
		private:
			SharedPtr< Base > m_ptr;
			Type m_type = Type::VOID;
		};

		class Map : public HashMap< String, Value >
		{
		public:
			/*constexpr Value& operator[]( String const& key )
			{
				return m_data[ key ];
			}

			constexpr Value& operator[]( String&& key )
			{
				return m_data[ std::move( key ) ];
			}

			constexpr void insert( hashmap::pair< const String, Value >&& pair )
			{
				m_data.insert( std::move( pair ) );
			}

			constexpr uint64 size() const { return m_data.size(); }

			constexpr auto begin() { return m_data.begin(); }
			constexpr auto end() { return m_data.end(); }

			constexpr auto cbegin() const { return m_data.cbegin(); }
			constexpr auto cend() const { return m_data.cend(); }*/

			constexpr bool operator==( Map const& rhs ) const;
			
		/*public:
			HashMap< String, Value > m_data;*/
		};

		template< class T >
		constexpr Value::Value( T&& data ):
			m_ptr( new Derived< T >( std::move( data ) ) ),
			m_type( templateToVariantType< T >() )
		{
			static_assert( templateToVariantType< T >() != Type::VOID );
		}

		template< class T >
		constexpr Value::Value( T const& data ):
			m_ptr( new Derived< T >( data ) ),
			m_type( templateToVariantType< T >() )
		{
			static_assert( templateToVariantType< T >() != Type::VOID );
		}

		template< class T >
		constexpr Value::Value( T& data )
		{
			*this = Value( const_cast< T const& >( data ) );
		}

		template< class T >
		constexpr Value& Value::operator=( T&& data )
		{
			*this = Value( std::move( data ) );
			return *this;
		}

		template< class T >
		constexpr Value& Value::operator=( T const& data )
		{
			*this = Value( data );
			return *this;
		}

		template< class T >
		constexpr Value& Value::operator=( T& data )
		{
			*this = Value( data );
			return *this;
		}

		template< class T, class >
		constexpr T Value::As() const
		{
			if ( m_type == Type::VOID || !m_ptr ) [[unlikely]]
				throw std::runtime_error( "Accessing data of void value!" );

			auto constexpr type = templateToVariantType< type::decay< T > >();

			if ( m_type != type ) [[unlikely]]
			{
				String msg = "Type did not match!\nFound: ";
				msg += typeToString( m_type );
				msg += ", but ";
				msg += templateToString< type::decay< T > >();
				msg += " was requested";
				throw std::runtime_error( msg.c_str() );
			}

			return static_cast< Derived< type::decay< T > >* >( m_ptr.get() )->m_data;
		}

		template< class T, class >
		constexpr T Value::As()
		{
			if ( m_type == Type::VOID || !m_ptr ) [[unlikely]]
				throw std::runtime_error( "Accessing data of void value!" );

			auto constexpr type = templateToVariantType< type::decay< T > >();

			if ( m_type != type ) [[unlikely]]
			{
				String msg = "Type did not match!\nFound: ";
				msg += typeToString( m_type );
				msg += ", but ";
				msg += templateToString< type::decay< T > >();
				msg += " was requested";
				throw std::runtime_error( msg.c_str() );
			}

			if ( m_ptr.isShared() )
			{
				*this = Value( static_cast< Derived< type::decay< T > > >( m_ptr.get() )->m_data );
			}

			return static_cast< Derived< type::decay< T > >* >( m_ptr.get() )->m_data;
		}

		constexpr bool Value::operator==( Value const& rhs ) const
		{
			if ( m_type != rhs.m_type )
				return false;

			switch ( m_type )
			{
			case Type::VOID:
				return true;
			case Type::INT8:
				return As< int8 >() == rhs.As< int8 >();
			case Type::INT16:
				return As< int16 >() == rhs.As< int16 >();
			case Type::INT32:
				return As< int32 >() == rhs.As< int32 >();
			case Type::INT64:
				return As< int64 >() == rhs.As< int64 >();
			case Type::UINT8:
				return As< uint8 >() == rhs.As< uint8 >();
			case Type::UINT16:
				return As< uint16 >() == rhs.As< uint16 >();
			case Type::UINT32:
				return As< uint32 >() == rhs.As< uint32 >();
			case Type::UINT64:
				return As< uint64 >() == rhs.As< uint64 >();
			case Type::FLOAT:
				return As< float >() == rhs.As< float >();
			case Type::DOUBLE:
				return As< double >() == rhs.As< double >();
			case Type::STRING:
				return As< String const& >() == As< String const& >();
			case Type::MAP:
				return As< Map const& >() == rhs.As< Map const& >();
			case Type::INT8_ARRAY:
				return As< DynamicArray< int8 > const& >() == rhs.As< DynamicArray< int8 > const& >();
			case Type::INT16_ARRAY:
				return As< DynamicArray< int16 > const& >() == rhs.As< DynamicArray< int16 > const& >();
			case Type::INT32_ARRAY:
				return As< DynamicArray< int32 > const& >() == rhs.As< DynamicArray< int32 > const& >();
			case Type::INT64_ARRAY:
				return As< DynamicArray< int64 > const& >() == rhs.As< DynamicArray< int64 > const& >();
			case Type::UINT8_ARRAY:
				return As< DynamicArray< uint8 > const& >() == rhs.As< DynamicArray< uint8 > const& >();
			case Type::UINT16_ARRAY:
				return As< DynamicArray< uint16 > const& >() == rhs.As< DynamicArray< uint16 > const& >();
			case Type::UINT32_ARRAY:
				return As< DynamicArray< uint32 > const& >() == rhs.As< DynamicArray< uint32 > const& >();
			case Type::UINT64_ARRAY:
				return As< DynamicArray< uint64 > const& >() == rhs.As< DynamicArray< uint64 > const& >();
			case Type::FLOAT_ARRAY:
				return As< DynamicArray< float > const& >() == rhs.As< DynamicArray< float > const& >();
			case Type::DOUBLE_ARRAY:
				return As< DynamicArray< double > const& >() == rhs.As< DynamicArray< double > const& >();
			case Type::STRING_ARRAY:
				return As< DynamicArray< String > const& >() == As< DynamicArray< String > const& >();
			}
		}

		constexpr bool Map::operator==( Map const& rhs ) const
		{
			if ( size() != rhs.size() )
				return false;

			for ( const auto& [key, value] : *this )
			{
				if ( rhs.find( key ) == nullptr )
					return false;
			}

			return true;
		}
	}
}
