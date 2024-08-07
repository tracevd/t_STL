#include "Serialize.h"

#include <iostream>
#include <type_traits>
#include <cassert>

#include "../../endianness.h"
#include "../../Error.h"

namespace t
{
	namespace variant
	{
		template< endianness >
		void AddToBuffer( Array< uint8 >& buffer, uint8 data )
		{
			buffer.pushBack( data );
		}

		template< endianness e >
		void AddToBuffer( Array< uint8 >& buffer, uint16 data )
		{
			if constexpr ( e != endianness::native )
				data = byteswap( data );
			const uint8* const ptr = reinterpret_cast< uint8* >( &data );
			buffer.pushBack( ptr[ 0 ] );
			buffer.pushBack( ptr[ 1 ] );
		}
		template< endianness e >
		void AddToBuffer( Array< uint8 >& buffer, uint32 data )
		{
			if constexpr ( e != endianness::native )
				data = byteswap( data );
			const uint8* const ptr = reinterpret_cast< uint8* >( &data );
			buffer.pushBack( ptr[ 0 ] );
			buffer.pushBack( ptr[ 1 ] );
			buffer.pushBack( ptr[ 2 ] );
			buffer.pushBack( ptr[ 3 ] );
		}
		template< endianness e >
		void AddToBuffer( Array< uint8 >& buffer, uint64 data )
		{
			if constexpr ( e != endianness::native )
				data = byteswap( data );
			const uint8* const ptr = reinterpret_cast<uint8*>(&data);
			buffer.pushBack( ptr[ 0 ] );
			buffer.pushBack( ptr[ 1 ] );
			buffer.pushBack( ptr[ 2 ] );
			buffer.pushBack( ptr[ 3 ] );
			buffer.pushBack( ptr[ 4 ] );
			buffer.pushBack( ptr[ 5 ] );
			buffer.pushBack( ptr[ 6 ] );
			buffer.pushBack( ptr[ 7 ] );
		}

		template< endianness e >
		void AddToBuffer( Array< uint8 >& buffer, int8 data )
		{
			AddToBuffer< e >( buffer, *reinterpret_cast< uint8* >( &data ) );
		}

		template< endianness e >
		void AddToBuffer( Array< uint8 >& buffer, int16 data )
		{
			AddToBuffer< e >( buffer, *reinterpret_cast< uint16* >( &data ) );
		}

		template< endianness e >
		void AddToBuffer( Array< uint8 >& buffer, int32 data )
		{
			AddToBuffer< e >( buffer, *reinterpret_cast< uint32* >( &data ) );
		}

		template< endianness e >
		void AddToBuffer( Array< uint8 >& buffer, int64 data )
		{
			AddToBuffer< e >( buffer, *reinterpret_cast< uint64* >( &data ) );
		}

		template< endianness e >
		void AddToBuffer( Array< uint8 >& buffer, float data )
		{
			AddToBuffer< e >( buffer, *reinterpret_cast< uint32* >( &data ) );
		}

		template< endianness e >
		void AddToBuffer( Array< uint8 >& buffer, double data )
		{
			AddToBuffer< e >( buffer, *reinterpret_cast< uint64* >( &data ) );
		}

		template< endianness e, typename T >
		void AddToBuffer( Array< uint8 >& buffer, Array< T > const& data )
		{
			for ( uint64 i = 0; i < data.size(); ++i )
			{
				AddToBuffer< e >( buffer, data[ i ] );
			}
		}

		inline namespace bitstream_v1
		{
			template< endianness >
			void Serialize( Array< uint8 >& buffer, Map const& map );

			template< endianness e >
			void Serialize( Array< uint8 >& buffer, String const& str )
			{
				auto const size = str.size();
				if ( size > limit< uint32 >::max )
				{
					throw Error( "Strings lengths must fit into a 32-bit number", 1 );
				}
				AddToBuffer< e >( buffer, static_cast< uint32 >( size ) );
				for ( uint64 i = 0; i < str.size(); ++i )
				{
					AddToBuffer< e >( buffer, *reinterpret_cast< uint8 const* >( &str[ i ] ) );
				}
			}

			template< endianness, typename T >
			void SerializeComplexValue( Array< uint8 >& buffer, T const& data );

			template< endianness e, typename T >
			void SerializePrimitiveValue( Array< uint8 >& buffer, T data )
			{
				auto constexpr type = details::templateToVariantType< T >();
				AddToBuffer< e >( buffer, static_cast< uint8 >( type ) );
				AddToBuffer< e >( buffer, data );
			}

			template< endianness e, typename T >
			void SerializeArrayValue( Array< uint8 >& buffer, Array< T > const& data )
			{
				AddToBuffer< e >( buffer, static_cast< uint8 >( details::templateToVariantType< Array< T > >() ) );
				AddToBuffer< e >( buffer, uint64( data.size() ) );

				for ( uint64 i = 0; i < data.size(); ++i )
				{
					AddToBuffer< e >( buffer, data[ i ] );
				}
			}

			template< endianness e >
			void SerializeComplexValue( Array< uint8 >& buffer, String const& data )
			{
				AddToBuffer< e >( buffer, static_cast< uint8 >( details::templateToVariantType< String >() ) );
				Serialize< e >( buffer, data );
			}

			template< endianness e >
			void SerializeComplexValue( Array< uint8 >& buffer, Map const& data )
			{
				AddToBuffer< e >( buffer, static_cast< uint8 >( details::templateToVariantType< Map >() ) );
				Serialize< e >( buffer, data );
			}

			template< endianness e >
			void SerializeComplexValue( Array< uint8 >& buffer, Array< String > const& data )
			{
				AddToBuffer< e >( buffer, static_cast< uint8 >( details::templateToVariantType< Array< String > >() ) );
				AddToBuffer< e >( buffer, uint64( data.size() ) );
				for ( uint64 i = 0; i < data.size(); ++i )
				{
					Serialize< e >( buffer, data[ i ] );
				}
			}

			template< endianness e >
			void SerializeEmptyValue( Array< uint8 >& buffer )
			{
				AddToBuffer< e >( buffer, static_cast< uint8 >( Type::VOID ) );
			}

			template< endianness e >
			void SerializeValue( Array< uint8 >& buffer, Value const& val )
			{
				const auto type = val.getType();

				switch ( type )
				{
				case Type::VOID:
					return SerializeEmptyValue< e >( buffer );
				case Type::INT8:
					return SerializePrimitiveValue< e >( buffer, val.As< int8 >() );
				case Type::INT16:
					return SerializePrimitiveValue< e >( buffer, val.As< int16 >() );
				case Type::INT32:
					return SerializePrimitiveValue< e >( buffer, val.As< int32 >() );
				case Type::INT64:
					return SerializePrimitiveValue< e >( buffer, val.As< int64 >() );
				case Type::UINT8:
					return SerializePrimitiveValue< e >( buffer, val.As< uint8 >() );
				case Type::UINT16:
					return SerializePrimitiveValue< e >( buffer, val.As< uint16 >() );
				case Type::UINT32:
					return SerializePrimitiveValue< e >( buffer, val.As< uint32 >() );
				case Type::UINT64:
					return SerializePrimitiveValue< e >( buffer, val.As< uint64 >() );
				case Type::FLOAT:
					return SerializePrimitiveValue< e >( buffer, val.As< float >() );
				case Type::DOUBLE:
					return SerializePrimitiveValue< e >( buffer, val.As< double >() );
				case Type::STRING:
					return SerializeComplexValue< e >( buffer, val.As< String const& >() );
				case Type::MAP:
					return SerializeComplexValue< e >( buffer, val.As< Map const& >() );
				case Type::INT8_ARRAY:
					return SerializeArrayValue< e >( buffer, val.As< Array< int8 > const& >() );
				case Type::INT16_ARRAY:
					return SerializeArrayValue< e >( buffer, val.As< Array< int16 > const& >() );
				case Type::INT32_ARRAY:
					return SerializeArrayValue< e >( buffer, val.As< Array< int32 > const& >() );
				case Type::INT64_ARRAY:
					return SerializeArrayValue< e >( buffer, val.As< Array< int64 > const& >() );
				case Type::UINT8_ARRAY:
					return SerializeArrayValue< e >( buffer, val.As< Array< uint8 > const& >() );
				case Type::UINT16_ARRAY:
					return SerializeArrayValue< e >( buffer, val.As< Array< uint16 > const& >() );
				case Type::UINT32_ARRAY:
					return SerializeArrayValue< e >( buffer, val.As< Array< uint32 > const& >() );
				case Type::UINT64_ARRAY:
					return SerializeArrayValue< e >( buffer, val.As< Array< uint64 > const& >() );
				case Type::FLOAT_ARRAY:
					return SerializeArrayValue< e >( buffer, val.As< Array< float > const& >() );
				case Type::DOUBLE_ARRAY:
					return SerializeArrayValue< e >( buffer, val.As< Array< double > const& >() );
				case Type::STRING_ARRAY:
					return SerializeComplexValue< e >( buffer, val.As< Array< String > const& >() );
				}
				assert( false );
			}

			template< endianness e >
			void Serialize( Array< uint8 >& buffer, Map const& map )
			{
				buffer.pushBack( 't' );
				buffer.pushBack( 'v' );
				buffer.pushBack( 'm' );
				buffer.pushBack( 1 );

				AddToBuffer< e >( buffer, uint16( 1 ) );
				AddToBuffer< e >( buffer, uint64( map.size() ) );
				
				for ( auto const& [ key, value ] : map )
				{
					Serialize< e >( buffer, key );
					SerializeValue< e >( buffer, value );
				}
			}

			template< endianness e >
			Array< uint8 > Serialize( Map const& map )
			{
				Array< uint8 > buffer;

				constexpr auto DefaultArrayLength = 160;

				buffer.reserve( DefaultArrayLength );

				Serialize< e >( buffer, map );

				return buffer;
			}

			template Array< uint8 > Serialize< endianness::little >( Map const& );
			template Array< uint8 > Serialize< endianness::big >( Map const& );
		}
	}
}
