#include "Map.h"

#include "Value.h"

namespace t
{
    namespace v
    {
		inline namespace v2
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
		}
		namespace v1
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
		}
    }
}