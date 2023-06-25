#include "Map.h"

#include "Value.h"

namespace t
{
    namespace v
    {
		bool Map::operator==( const Map& rhs ) const
		{
			if ( this->size() != rhs.size() )
				return false;

			for ( const auto& [key, value] : *this )
			{
				auto it = rhs.find( key );
				if ( it == rhs.cend() )
					return false;
				if ( it->second != value )
					return false;
			}
			return true;
		}
		Map Map::Clone() const
		{
			Map copy;

            for ( const auto& [ key, value ] : *this )
            {
				if ( value.Is< Map >() || value.Is< Vector< Map > >() )
					copy.insert( { key, value.Clone() } );
				else
					copy.insert( { key, value } );
            }
                
            return copy;
		}
    }
}