#pragma once
#include <unordered_map>
#include "variant.h"
#include "../String.h"

namespace t
{
	namespace v
	{
		class Map : public std::unordered_map< String, Value >
		{
		public:
			bool operator==( Map const& rhs ) const;
			Map Clone() const;
			Map SemiClone() const;
			Map() = default;
			Map( Map const& other )
			{
				*this = other.Clone();
			}
			Map( Map&& ) = default;
			Map& operator=( Map&& ) = default;
			Map& operator=( Map const& rhs )
			{
				*this = rhs.SemiClone();
				return *this;
			}
		};
	}
}