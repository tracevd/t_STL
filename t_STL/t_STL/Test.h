#pragma once

#include <unordered_map>

#include "Memory.h"
#include "String.h"

namespace t
{
	namespace test
	{
		class Map;

		enum class Type : uint8
		{
			VOID = 0,
			UINT8,
			INT8,
			MAP
		};

		template< class T >
		struct TypeInfo
		{
			constexpr static auto str = "void";
			constexpr static Type type = Type::VOID;
		};

#define type_info( t, enum_ ) \
		template<> \
		struct TypeInfo< t > \
		{ \
			constexpr static auto str = #t; \
			constexpr static Type type = enum_; \
		}

		type_info( uint8, Type::UINT8 );
		type_info( int8, Type::INT8 );
		type_info( Map, Type::MAP );

		template< class T >
		constexpr Type templateToType()
		{
			return TypeInfo< T >::type;
		}

		template< class T >
		constexpr const char* templateToString()
		{
			return TypeInfo< T >::str;
		}

		constexpr const char* typeToString( Type type )
		{
			switch ( type )
			{
			case Type::VOID:
				return TypeInfo< void >::str;
			case Type::UINT8:
				return TypeInfo< uint8 >::str;
			case Type::INT8:
				return TypeInfo< int8 >::str;
			case Type::MAP:
				return TypeInfo< Map >::str;
			default:
				return TypeInfo< void >::str;
			}
		}

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

			constexpr Derived( T const& data ):
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

			constexpr bool operator!=( Value const& rhs ) const { return !( *this == rhs ); }
		private:
			SharedPtr< Base > m_ptr;
			Type m_type = Type::VOID;
		};

		class Map : public std::unordered_map< String, Value >
		{

		};

		template< class T >
		constexpr Value::Value( T&& data ):
			m_ptr( new Derived< T >( std::move( data ) ) ),
			m_type( templateToType< T >() )
		{
			static_assert( templateToType< T >() != Type::VOID );
		}

		template< class T >
		constexpr Value::Value( T const& data ):
			m_ptr( new Derived< T >( data ) ),
			m_type( templateToType< T >() )
		{
			static_assert( templateToType< T >() != Type::VOID );
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
				throw std::runtime_error("Accessing data of void value!");

			auto constexpr type = templateToType< type::decay< T > >();

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

			auto constexpr type = templateToType< type::decay< T > >();

			if ( m_type != type ) [[unlikely]]
			{
				String msg = "Type did not match!\nFound: ";
				msg += typeToString( m_type );
				msg += ", but ";
				msg += templateToString< type::decay< T > >();
				msg += " was requested";
				throw std::runtime_error( msg.c_str() );
			}

			return static_cast<Derived< type::decay< T > >*>(m_ptr.get())->m_data;
		}

		constexpr bool Value::operator==( Value const& rhs ) const
		{
			if ( m_type != rhs.m_type )
				return false;
			
			switch ( m_type )
			{
			case Type::VOID:
				return true;
			case Type::UINT8:
				return As< uint8 >() == rhs.As< uint8 >();
			case Type::INT8:
				return As< int8 >() == rhs.As< int8 >();
			case Type::MAP:
				return As< Map const& >() == rhs.As< Map const& >();
			}
		}
	}
}