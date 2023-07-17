#pragma once
#include "../Tint.h"
#include <unordered_map>
#include "variant.h"
#include "../String.h"

namespace t
{
	namespace variant
	{
		class Map : public std::unordered_map< String, Value >
		{
		public:
			Map() 					         = default;
			Map( Map&& ) 				     = default;
			Map& operator=( Map&& ) 		 = default;

			Map( Map const& other )
			{
				*this = other.QuickClone( 0 );
			}

			Map& operator=( Map const& rhs )
			{
				*this = rhs.QuickClone( 0 );
				return *this;
			}

			/**
			 * Returns a completely unique copy of this map. Every value will have a unique address.
			 */
			Map Clone() const;
			/**
			 * Returns a semi-unique copy of this map. Only necessary values will have a unique address.
			 */
			Map QuickClone() const;

			bool operator==( Map const& rhs ) const;

		private:
			Map QuickClone( uint64 depth ) const;
			friend Value;
		};
	}
}