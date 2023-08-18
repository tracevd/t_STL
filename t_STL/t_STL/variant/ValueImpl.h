#pragma once

#include "Types.h"

#include "../Memory.h"

namespace t
{
	namespace variant
	{
		namespace value
		{
			class Base
			{
			public:
				Base()          = default;
				virtual ~Base() = default;

				virtual SharedPtr< Base > Clone() const = 0;
			};

			template< class T >
			class Derived : public Base
			{
			public:
				Derived( T val ):
					val( std::move( val ) ) {}

				virtual SharedPtr< Base > Clone() const final override
				{
					if constexpr ( type::is_same< T, UniquePtr< Map > > )
					{
						return SharedPtr< Base >( new Derived( UniquePtr< Map >( val->Clone() ) ) );
					}
					else
					{
						return SharedPtr< Base >( new Derived( val ) );
					}
				}

				static constexpr auto type = templateToVariantType< T >();

				T val = {};
			};
		}
	}
}