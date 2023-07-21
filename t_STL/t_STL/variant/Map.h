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
		DefineNormalAndVector( extern template, Map );
	}
}
