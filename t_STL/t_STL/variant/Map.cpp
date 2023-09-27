#include "Map.h"

#include <iostream>

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
			std::cout << "In comparision\n";
			if ( this == &rhs )
				return true;
			if ( size() != rhs.size() )
				return false;

			std::cout << size() << '\n';

			for ( const auto& [ key, value ] : *this )
			{
				std::cout << key << '\n';
				auto it = rhs.find( key );
				if ( it == rhs.cend() )
					return false;
				if ( it->second != value )
					return false;
			}
			std::cout << "Maps were equal\n";
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
