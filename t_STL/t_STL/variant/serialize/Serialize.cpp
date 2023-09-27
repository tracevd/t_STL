#include "Serialize.h"

#include <iostream>
#include <type_traits>

#include "../../byteswap.h"

namespace t
{
	namespace variant
	{
		template< std::endian >
		void AddToBuffer( BufferType& buffer, uint8 data )
		{
			buffer.pushBack( data );
		}

		template< std::endian endianness >
		void AddToBuffer( BufferType& buffer, uint16 data )
		{
			if constexpr ( endianness != std::endian::native )
				data = byteswap( data );
			const uint8* const ptr = reinterpret_cast< uint8* >( &data );
			buffer.pushBack( ptr[ 0 ] );
			buffer.pushBack( ptr[ 1 ] );
		}
		template< std::endian endianness >
		void AddToBuffer( BufferType& buffer, uint32 data )
		{
			if constexpr ( endianness != std::endian::native )
				data = byteswap( data );
			const uint8* const ptr = reinterpret_cast< uint8* >( &data );
			buffer.pushBack( ptr[ 0 ] );
			buffer.pushBack( ptr[ 1 ] );
			buffer.pushBack( ptr[ 2 ] );
			buffer.pushBack( ptr[ 3 ] );
		}
		template< std::endian endianness >
		void AddToBuffer( BufferType& buffer, uint64 data )
		{
			if constexpr ( endianness != std::endian::native )
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

		template< std::endian endianness >
		void AddToBuffer( BufferType& buffer, int8 data )
		{
			AddToBuffer< endianness >( buffer, *reinterpret_cast< uint8* >( &data ) );
		}

		template< std::endian endianness >
		void AddToBuffer( BufferType& buffer, int16 data )
		{
			AddToBuffer< endianness >( buffer, *reinterpret_cast< uint16* >( &data ) );
		}

		template< std::endian endianness >
		void AddToBuffer( BufferType& buffer, int32 data )
		{
			AddToBuffer< endianness >( buffer, *reinterpret_cast< uint32* >( &data ) );
		}

		template< std::endian endianness >
		void AddToBuffer( BufferType& buffer, int64 data )
		{
			AddToBuffer< endianness >( buffer, *reinterpret_cast< uint64* >( &data ) );
		}

		template< std::endian endianness >
		void AddToBuffer( BufferType& buffer, float data )
		{
			AddToBuffer< endianness >( buffer, *reinterpret_cast< uint32* >( &data ) );
		}

		template< std::endian endianness >
		void AddToBuffer( BufferType& buffer, double data )
		{
			AddToBuffer< endianness >( buffer, *reinterpret_cast< uint64* >( &data ) );
		}

		template< std::endian endianness, typename T >
		void AddToBuffer( BufferType& buffer, List< T > const& data )
		{
			for ( uint64 i = 0; i < data.size(); ++i )
			{
				AddToBuffer< endianness >( buffer, data[ i ] );
			}
		}

		inline namespace bitstream_v1
		{
			template< std::endian >
			void Serialize( BufferType& buffer, Map const& map );

			template< std::endian endianness >
			void Serialize( BufferType& buffer, String const& str )
			{
				AddToBuffer< endianness >( buffer, static_cast< uint16 >( str.size() ) );
				for ( uint64 i = 0; i < str.size(); ++i )
				{
					AddToBuffer< endianness >( buffer, *reinterpret_cast< uint8 const* >( &str[ i ] ) );
				}
			}

			template< std::endian, typename T >
			void SerializeComplexValue( BufferType& buffer, T const& data );

			template< std::endian endianness, typename T >
			void SerializePrimitiveValue( BufferType& buffer, T data )
			{
				auto constexpr type = templateToVariantType< T >();
				static_assert( type != VOID, "Invalid Type attempting to be serialized" );
				AddToBuffer< endianness >( buffer, static_cast< uint8 >( type ) );
				AddToBuffer< endianness >( buffer, data );
			}

			template< std::endian endianness, typename T >
			void SerializeListValue( BufferType& buffer, List< T > const& data )
			{
				AddToBuffer< endianness >( buffer, static_cast< uint8 >( templateToVariantType< List< T > >() ) );
				AddToBuffer< endianness >( buffer, uint64( data.size() ) );

				for ( uint64 i = 0; i < data.size(); ++i )
				{
					AddToBuffer< endianness >( buffer, data[ i ] );
				}
			}

			template< std::endian endianness >
			void SerializeComplexValue( BufferType& buffer, String const& data )
			{
				AddToBuffer< endianness >( buffer, static_cast< uint8 >( templateToVariantType< String >() ) );
				Serialize< endianness >( buffer, data );
			}

			template< std::endian endianness >
			void SerializeComplexValue( BufferType& buffer, Map const& data )
			{
				AddToBuffer< endianness >( buffer, static_cast< uint8 >( templateToVariantType< Map >() ) );
				Serialize< endianness >( buffer, data );
			}

			template< std::endian endianness >
			void SerializeComplexValue( BufferType& buffer, List< String > const& data )
			{
				AddToBuffer< endianness >( buffer, static_cast< uint8 >( templateToVariantType< List< String > >() ) );
				AddToBuffer< endianness >( buffer, uint64( data.size() ) );
				for ( uint64 i = 0; i < data.size(); ++i )
				{
					Serialize< endianness >( buffer, data[ i ] );
				}
			}

			template< std::endian endianness >
			void SerializeValue( BufferType& buffer, Value const& val )
			{
				const auto type = val.getType();

				constexpr auto end = endianness;

				switch ( type )
				{
				case Type::INT8:
					return SerializePrimitiveValue< end >( buffer, val.As< int8 >() );
				case Type::INT16:
					return SerializePrimitiveValue< end >( buffer, val.As< int16 >() );
				case Type::INT32:
					return SerializePrimitiveValue< end >( buffer, val.As< int32 >() );
				case Type::INT64:
					return SerializePrimitiveValue< end >( buffer, val.As< int64 >() );
				case Type::UINT8:
					return SerializePrimitiveValue< end >( buffer, val.As< uint8 >() );
				case Type::UINT16:
					return SerializePrimitiveValue< end >( buffer, val.As< uint16 >() );
				case Type::UINT32:
					return SerializePrimitiveValue< end >( buffer, val.As< uint32 >() );
				case Type::UINT64:
					return SerializePrimitiveValue< end >( buffer, val.As< uint64 >() );
				case Type::FLOAT:
					return SerializePrimitiveValue< end >( buffer, val.As< float >() );
				case Type::DOUBLE:
					return SerializePrimitiveValue< end >( buffer, val.As< double >() );
				case Type::STRING:
					return SerializeComplexValue< end >( buffer, val.As< String const& >() );
				case Type::MAP:
					return SerializeComplexValue< end >( buffer, val.As< Map const& >() );
				case Type::INT8_LIST:
					return SerializeListValue< end >( buffer, val.As< List< int8 > const& >() );
				case Type::INT16_LIST:
					return SerializeListValue< end >( buffer, val.As< List< int16 > const& >() );
				case Type::INT32_LIST:
					return SerializeListValue< end >( buffer, val.As< List< int32 > const& >() );
				case Type::INT64_LIST:
					return SerializeListValue< end >( buffer, val.As< List< int64 > const& >() );
				case Type::UINT8_LIST:
					return SerializeListValue< end >( buffer, val.As< List< uint8 > const& >() );
				case Type::UINT16_LIST:
					return SerializeListValue< end >( buffer, val.As< List< uint16 > const& >() );
				case Type::UINT32_LIST:
					return SerializeListValue< end >( buffer, val.As< List< uint32 > const& >() );
				case Type::UINT64_LIST:
					return SerializeListValue< end >( buffer, val.As< List< uint64 > const& >() );
				case Type::FLOAT_LIST:
					return SerializeListValue< end >( buffer, val.As< List< float > const& >() );
				case Type::DOUBLE_LIST:
					return SerializeListValue< end >( buffer, val.As< List< double > const& >() );
				case Type::STRING_LIST:
					return SerializeComplexValue< end >( buffer, val.As< List< String > const& >() );
				default:
					throw std::runtime_error( "Type not supported" );
				}
			}

			template< std::endian endianness >
			void Serialize( BufferType& buffer, Map const& map )
			{
				buffer.pushBack( 't' );
				buffer.pushBack( 'v' );
				buffer.pushBack( 'm' );
				buffer.pushBack( 1 );

				AddToBuffer< endianness >( buffer, uint16( 1 ) );
				AddToBuffer< endianness >( buffer, uint64( map.size() ) );
				
				for ( const auto& [ key, value ] : map )
				{
					Serialize< endianness >( buffer, key );
					SerializeValue< endianness >( buffer, value );
				}
			}

			template< std::endian endianness >
			BufferType Serialize( Map const& map )
			{
				BufferType buffer;

				Serialize< endianness >( buffer, map );

				return buffer;
			}

			template BufferType Serialize< std::endian::little >( Map const& );
			template BufferType Serialize< std::endian::big >( Map const& );
		}
	}
}
