#pragma once

#include "variant.h"

#include "../Tint.h"
#include <unordered_map>
#include "../String.h"
#include "Value.h"

namespace t
{
	namespace variant
	{
		class Map : public std::unordered_map< String, Value >
		{
		public:
			Map();
			Map( Map&& );
			Map( Map const& other );
			Map& operator=( Map&& );
			Map& operator=( Map const& rhs );

			/**
			 * Returns a completely unique copy of this map. Every value will have a unique address.
			 */
			Map Clone() const;
			/**
			 * Returns a semi-unique copy of this map. Only necessary values will have a unique address.
			 */
			Map QuickClone() const;

			bool operator==( Map const& rhs ) const;
		};
	}
}

#include "ValueTemplateDefines.h"

namespace t
{
	namespace variant
	{
		//extern template Value::Value< Map, void >( Map );
		extern template Value& Value::operator=< Map >( Map );
		extern template Map& Value::As< Map&, void >();
		extern template Map const& Value::As< Map const&, void >();
		extern template Map Value::As< Map, void >();
		//extern template bool Value::Is< Map >();
	}
}