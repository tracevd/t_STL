#include "Map.h"

#include "Value.h"

namespace t
{
    namespace v
    {
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
		Map Map::SemiClone() const
		{
			Map copy;

            for ( const auto& [ key, value ] : *this )
            {
				copy.insert( { key, value.SemiClone() } );
            }
                
            return copy;
		}
		Map Map::Clone() const
		{
			Map copy;

            for ( const auto& [ key, value ] : *this )
            {
				copy.insert( { key, value.Clone() } );
            }
                
            return copy;
		}
    }
}