#include "Serialize.h"

#include <iostream>
#include <type_traits>

#include "../../endianness.h"

namespace t
{
	namespace variant
	{
		template< endianness >
		void AddToBuffer( BufferType& buffer, uint8 data )
		{
			buffer.pushBack( data );
		}

		template< endianness e >
		void AddToBuffer( BufferType& buffer, uint16 data )
		{
			if constexpr ( e != endianness::native )
				data = byteswap( data );
			const uint8* const ptr = reinterpret_cast< uint8* >( &data );
			buffer.pushBack( ptr[ 0 ] );
			buffer.pushBack( ptr[ 1 ] );
		}
		template< endianness e >
		void AddToBuffer( BufferType& buffer, uint32 data )
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
		void AddToBuffer( BufferType& buffer, uint64 data )
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
		void AddToBuffer( BufferType& buffer, int8 data )
		{
			AddToBuffer< e >( buffer, *reinterpret_cast< uint8* >( &data ) );
		}

		template< endianness e >
		void AddToBuffer( BufferType& buffer, int16 data )
		{
			AddToBuffer< e >( buffer, *reinterpret_cast< uint16* >( &data ) );
		}

		template< endianness e >
		void AddToBuffer( BufferType& buffer, int32 data )
		{
			AddToBuffer< e >( buffer, *reinterpret_cast< uint32* >( &data ) );
		}

		template< endianness e >
		void AddToBuffer( BufferType& buffer, int64 data )
		{
			AddToBuffer< e >( buffer, *reinterpret_cast< uint64* >( &data ) );
		}

		template< endianness e >
		void AddToBuffer( BufferType& buffer, float data )
		{
			AddToBuffer< e >( buffer, *reinterpret_cast< uint32* >( &data ) );
		}

		template< endianness e >
		void AddToBuffer( BufferType& buffer, double data )
		{
			AddToBuffer< e >( buffer, *reinterpret_cast< uint64* >( &data ) );
		}

		template< endianness e, typename T >
		void AddToBuffer( BufferType& buffer, Buffer< T > const& data )
		{
			for ( uint64 i = 0; i < data.size(); ++i )
			{
				AddToBuffer< e >( buffer, data[ i ] );
			}
		}

		inline namespace bitstream_v1
		{
			template< endianness >
			void Serialize( BufferType& buffer, Map const& map );

			template< endianness e >
			void Serialize( BufferType& buffer, String const& str )
			{
				AddToBuffer< e >( buffer, static_cast< uint16 >( str.size() ) );
				for ( uint64 i = 0; i < str.size(); ++i )
				{
					AddToBuffer< e >( buffer, *reinterpret_cast< uint8 const* >( &str[ i ] ) );
				}
			}

			template< endianness, typename T >
			void SerializeComplexValue( BufferType& buffer, T const& data );

			template< endianness e, typename T >
			void SerializePrimitiveValue( BufferType& buffer, T data )
			{
				auto constexpr type = templateToVariantType< T >();
				static_assert( type != VOID, "Invalid Type attempting to be serialized" );
				AddToBuffer< e >( buffer, static_cast< uint8 >( type ) );
				AddToBuffer< e >( buffer, data );
			}

			template< endianness e, typename T >
			void SerializeListValue( BufferType& buffer, Buffer< T > const& data )
			{
				AddToBuffer< e >( buffer, static_cast< uint8 >( templateToVariantType< Buffer< T > >() ) );
				AddToBuffer< e >( buffer, uint64( data.size() ) );

				for ( uint64 i = 0; i < data.size(); ++i )
				{
					AddToBuffer< e >( buffer, data[ i ] );
				}
			}

			template< endianness e >
			void SerializeComplexValue( BufferType& buffer, String const& data )
			{
				AddToBuffer< e >( buffer, static_cast< uint8 >( templateToVariantType< String >() ) );
				Serialize< e >( buffer, data );
			}

			template< endianness e >
			void SerializeComplexValue( BufferType& buffer, Map const& data )
			{
				AddToBuffer< e >( buffer, static_cast< uint8 >( templateToVariantType< Map >() ) );
				Serialize< e >( buffer, data );
			}

			template< endianness e >
			void SerializeComplexValue( BufferType& buffer, Buffer< String > const& data )
			{
				AddToBuffer< e >( buffer, static_cast< uint8 >( templateToVariantType< Buffer< String > >() ) );
				AddToBuffer< e >( buffer, uint64( data.size() ) );
				for ( uint64 i = 0; i < data.size(); ++i )
				{
					Serialize< e >( buffer, data[ i ] );
				}
			}

			template< endianness e >
			void SerializeValue( BufferType& buffer, Value const& val )
			{
				const auto type = val.getType();

				switch ( type )
				{
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
				case Type::INT8_LIST:
					return SerializeListValue< e >( buffer, val.As< Buffer< int8 > const& >() );
				case Type::INT16_LIST:
					return SerializeListValue< e >( buffer, val.As< Buffer< int16 > const& >() );
				case Type::INT32_LIST:
					return SerializeListValue< e >( buffer, val.As< Buffer< int32 > const& >() );
				case Type::INT64_LIST:
					return SerializeListValue< e >( buffer, val.As< Buffer< int64 > const& >() );
				case Type::UINT8_LIST:
					return SerializeListValue< e >( buffer, val.As< Buffer< uint8 > const& >() );
				case Type::UINT16_LIST:
					return SerializeListValue< e >( buffer, val.As< Buffer< uint16 > const& >() );
				case Type::UINT32_LIST:
					return SerializeListValue< e >( buffer, val.As< Buffer< uint32 > const& >() );
				case Type::UINT64_LIST:
					return SerializeListValue< e >( buffer, val.As< Buffer< uint64 > const& >() );
				case Type::FLOAT_LIST:
					return SerializeListValue< e >( buffer, val.As< Buffer< float > const& >() );
				case Type::DOUBLE_LIST:
					return SerializeListValue< e >( buffer, val.As< Buffer< double > const& >() );
				case Type::STRING_LIST:
					return SerializeComplexValue< e >( buffer, val.As< Buffer< String > const& >() );
				default:
					throw std::runtime_error( "Type not supported" );
				}
			}

			template< endianness e >
			void Serialize( BufferType& buffer, Map const& map )
			{
				buffer.pushBack( 't' );
				buffer.pushBack( 'v' );
				buffer.pushBack( 'm' );
				buffer.pushBack( 1 );

				AddToBuffer< e >( buffer, uint16( 1 ) );
				AddToBuffer< e >( buffer, uint64( map.size() ) );
				
				for ( const auto& [ key, value ] : map )
				{
					Serialize< e >( buffer, key );
					SerializeValue< e >( buffer, value );
				}
			}

			template< endianness e >
			BufferType Serialize( Map const& map )
			{
				BufferType buffer;

				Serialize< e >( buffer, map );

				return buffer;
			}

			template BufferType Serialize< endianness::little >( Map const& );
			template BufferType Serialize< endianness::big >( Map const& );
		}
	}
}
