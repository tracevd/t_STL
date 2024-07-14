#pragma once

#include "Tint.h"
#include "Type.h"

namespace t
{
	namespace details
	{
		template< uint64 _index, typename... Types >
		struct _TypeArray;

		template< uint64 _index, typename _Type, typename... Types >
		struct _TypeArray< _index, _Type, Types... > : _TypeArray< _index + 1, Types... >
		{
			constexpr static uint64 index = _index;
			using Type = _Type;
			using BaseType = _TypeArray< _index + 1, Types... >;
			static constexpr bool valid = true;
		};

		template< uint64 _index >
		struct _TypeArray< _index >
		{
			static constexpr bool valid = false;
		};

		template< typename T, class _typeArray >
		consteval uint64 getIndexOf()
		{
			if constexpr ( !_typeArray::valid )
			{
				throw Error( "blah", 1 );
			}
			else
			{
				if constexpr ( t::type::is_same< T, typename _typeArray::Type > )
				{
					return _typeArray::index;
				}
				return getIndexOf< T, typename _typeArray::BaseType >();
			}	
		}

		template< typename T, class _typeArray >
		consteval uint64 countOf()
		{
			if constexpr ( !_typeArray::valid )
			{
				return 0;
			}
			else
			{
				if constexpr ( t::type::is_same< T, typename _typeArray::Type > )
				{
					return 1 + countOf< T, typename _typeArray::BaseType >();
				}
				else
				{
					return countOf< T, typename _typeArray::BaseType >();
				}
			}
		}

		template< typename _typeArray, typename _typeIterator >
		consteval bool allUnique()
		{
			if constexpr ( _typeIterator::valid )
			{
				return countOf< typename _typeIterator::Type, _typeArray >() == 1 && allUnique< _typeArray, typename _typeIterator::BaseType >();
			}
			else
			{
				return true;
			}
		}
	}

	template< typename... Types >
	struct TypeArray
	{
	public:
		TypeArray() = delete;
		
		template< typename T >
		static consteval uint64 indexOf()
		{
			return details::getIndexOf< T, details::_TypeArray< 0, Types... > >();
		}

		static consteval bool allUnique()
		{
			using _Arr = details::_TypeArray< 0, Types... >;
			return details::allUnique< _Arr, _Arr >();
		}
	};
}