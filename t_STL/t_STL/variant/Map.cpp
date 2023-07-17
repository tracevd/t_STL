#include "Map.h"

#include "Value.h"

namespace t
{
    namespace variant
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
		Map Map::QuickClone() const
		{
			Map copy;

            for ( const auto& [ key, value ] : *this )
            {
				copy.insert( { key, value.QuickClone( 1 ) } );
            }
                
            return copy;
		}
		Map Map::QuickClone( uint64 depth ) const
		{
			if ( depth > 0 )
				return *this;
			
			Map copy;
			copy.reserve( size() );

            for ( const auto& [ key, value ] : *this )
            {
				copy.insert( { key, value.QuickClone( depth == 0 ? 0 : depth + 1 ) } );
            }
                
            return copy;
		}
		Map Map::Clone() const
		{
			Map copy;
			copy.reserve( size() );

            for ( const auto& [ key, value ] : *this )
            {
				copy.insert( { key, value.Clone() } );
            }
                
            return copy;
		}
    }
}