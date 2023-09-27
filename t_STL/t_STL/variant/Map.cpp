#include "Map.h"

namespace t
{
    namespace variant
    {
		Map::Map() = default;
		Map::Map( Map&& ) = default;
		Map& Map::operator=( Map&& ) = default;

		Map::Map( Map const& other )
		{
			*this = other.QuickClone();
		}

		Map& Map::operator=( Map const& rhs )
		{
			*this = rhs.QuickClone();
			return *this;
		}

		bool Map::operator==( Map const& rhs ) const
		{
			if ( this == &rhs )
				return true;
			if ( size() != rhs.size() )
				return false;

			for ( const auto& [ key, value ] : *this )
			{
				auto it = rhs.find( key );
				if ( it == rhs.cend() )
					return false;
				if ( it->second != value )
					return false;
			}
			return true;
		}
		Map Map::QuickClone() const
		{
			Map copy;
			copy.reserve( size() );

			for ( const auto& [ key, value ] : *this )
			{
				copy.insert( { key, value.QuickClone() } );
			}

			return copy;
		}

		Map Map::Clone() const
		{
			Map copy;
			copy.reserve( size() );

			for ( const auto& [ key, value ] : *this )
			{
				copy.insert( { key,  value.Clone() } );
			}

			return copy;
		}
    }
}
