#pragma once

#include "Type.h"
#include "Tint.h"
#include "Tuple.h"
#include "Error.h"
#include "TypeArray.h"

namespace t
{
	namespace details
	{
		namespace variant
		{
			using DeleterType = void (*)( void* );
			using CopyerType  = void* (*)( void const* );

			struct CallBackStruct
			{
				DeleterType deleter;
				CopyerType copyer;
			};

			template< uint64 _index, typename... Types >
			struct _Callbacks;

			template< uint64 _index, typename T, typename... Types >
			struct _Callbacks< _index, T, Types... > : _Callbacks< _index + 1, Types... >
			{
				static constexpr uint64 index = _index;
				using Type = T;
				using BaseType = _Callbacks< _index + 1, Types... >;
				static constexpr bool valid = true;

				static constexpr void deleter( void* ptr )
				{
					delete static_cast< T* >( ptr );
				}

				static constexpr void* copyer( void const* ptr )
				{
					return new T( *static_cast< T const* >( ptr ) );
				}
			};

			template< uint64 _index >
			struct _Callbacks< _index >
			{
				static constexpr bool valid = false;
			};

			template< typename T, typename _callbacks >
			constexpr DeleterType getDeleter()
			{
				if constexpr ( t::type::is_same< T, _callbacks::Type > )
				{
					return &_callbacks::deleter;
				}

				return getDeleter< T, _callbacks::BaseType >();
			}

			template< typename T, typename _callbacks >
			constexpr CopyerType getCopyer()
			{
				if constexpr ( t::type::is_same< T, _callbacks::Type > )
				{
					return &_callbacks::copyer;
				}

				return getCopyer< T, _callbacks::BaseType >();
			}

			template< typename _callbacks >
			constexpr DeleterType getDeleter( uint64 index )
			{
				if constexpr ( _callbacks::valid )
				{
					if ( index == _callbacks::index )
					{
						return &_callbacks::deleter;
					}

					return getDeleter< typename _callbacks::BaseType >( index );
				}
				else
				{
					throw Error( "Bad", 1 );
				}
			}

			template< typename _callbacks >
			constexpr CopyerType getCopyer( uint64 index )
			{
				if constexpr ( _callbacks::valid )
				{
					if ( index == _callbacks::index )
					{
						return &_callbacks::copyer;
					}

					return getCopyer< typename _callbacks::BaseType >( index );
				}
				else
				{
					throw Error( "Bad", 1 );
				}
			}
		}
	}

	template< typename... Ts >
	class Variant
	{
	public:
		Variant() = default;

		Variant( Variant const& other ):
			m_ptr( [&](){
				if ( other.m_typeIndex < NumTypes )
				{
					return Callbacks[ other.m_typeIndex ].second( other.m_ptr );
				}
				return nullptr;
			}() ),
			m_typeIndex( other.m_typeIndex ) {}

		Variant( Variant&& other ) noexcept:
			m_ptr( exchange( other.m_ptr, nullptr ) ),
			m_typeIndex( exchange( other.m_typeIndex, NumTypes ) ) {}

		Variant& operator=( Variant const& rhs )
		{
			if ( this == &rhs )
			{
				return *this;
			}

			destroyData();

			if ( rhs.m_typeIndex < NumTypes )
			{
				m_ptr = Callbacks[ rhs.m_typeIndex ].copyer( rhs.m_ptr );
			}
			else
			{
				m_ptr = nullptr;
			}

			m_typeIndex = rhs.m_typeIndex;

			return *this;
		}

		Variant& operator=( Variant&& rhs ) noexcept
		{
			if ( this == &rhs )
			{
				return *this;
			}

			destroyData();

			m_ptr = exchange( rhs.m_ptr, nullptr );
			m_typeIndex = exchange( rhs.m_typeIndex, NumTypes );

			return *this;
		}

		template< typename T, class = t::type::enable_if< t::type::is_any_of< T, Ts... > > >
		Variant( T const& val ):
			m_ptr( make_value< T >( val ) ),
			m_typeIndex( get_type_index< T >() ) {}

		template< typename T, class = t::type::enable_if< t::type::is_any_of< T, Ts... > > >
		Variant( T&& val ):
			m_ptr( make_value< T >( std::move( val ) ) ),
			m_typeIndex( get_type_index< T >() ) {}

		template< typename T, class = t::type::enable_if< t::type::is_any_of< T, Ts... > > >
		Variant& operator=( T const& val )
		{
			*this = Variant( val );
			return *this;
		}

		template< typename T, class = t::type::enable_if< t::type::is_any_of< T, Ts... > > >
		Variant& operator=( T&& val )
		{
			*this = Variant( std::move( val ) );
			return *this;
		}

		~Variant()
		{
			static_assert( TypeArray< Ts... >::allUnique(), "All t::Variant types were not unique");
			destroyData();
		}

		template< typename T >
		T* tryGet()
		{
			auto constexpr index = get_type_index< T >();

			if ( index != m_typeIndex )
			{
				return nullptr;
			}

			return static_cast< T* >( m_ptr );
		}

		template< typename T >
		T const* tryGet() const
		{
			constexpr auto index = get_type_index< T >();

			if ( index != m_typeIndex )
			{
				return nullptr;
			}

			return static_cast< T const* >( m_ptr );
		}

	private:

		void destroyData()
		{
			if ( m_ptr )
			{
				Callbacks[ m_typeIndex ].deleter( m_ptr );
				m_typeIndex = NumTypes;
				m_ptr = nullptr;
			}
		}

	private:
		static constexpr uint64 NumTypes = sizeof...(Ts);

		using DeleterType = details::variant::DeleterType;
		using CopyerType = details::variant::CopyerType;

		using CallbackArrayType = t::StaticArray< details::variant::CallBackStruct, NumTypes >;

		template< typename T, typename... Args >
		static void* make_value( Args&&... args )
		{
			return new T( std::forward< Args >( args )... );
		}

		template< typename T >
		static consteval uint64 get_type_index()
		{
			return TypeArray< Ts... >::template indexOf< T >();
		}

	private:
		static consteval auto make_callbacks()
		{
			CallbackArrayType callbacks;

			for ( uint64 i = 0; i < NumTypes; ++i )
			{
				using namespace details::variant;
				callbacks[ i ].deleter = getDeleter< _Callbacks< 0, Ts... > >( i );
				callbacks[ i ].copyer  = getCopyer< _Callbacks< 0, Ts... > >( i );
			}

			return callbacks;
		}

		static inline constexpr CallbackArrayType Callbacks = make_callbacks();

	private:
		void* m_ptr = nullptr;
		uint64 m_typeIndex = NumTypes;
	};
}
