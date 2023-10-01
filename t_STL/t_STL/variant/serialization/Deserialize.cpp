#include <cassert>
#include "Deserialize.h"

#include "../../endianness.h"

namespace t
{
	namespace variant
	{
		template< typename T >
		T ReadValueFromDynamicArray( const uint8* buffer, bool swapbytes )
		{
			auto const val = *reinterpret_cast< const T* >( buffer );
			if ( swapbytes )
				return byteswap( val );
			return val;
		}

		String DeserializeString( const uint8* buffer, uint64& bufferOffset, bool swapbytes )
		{
			auto length = ReadValueFromDynamicArray< uint16 >( &buffer[ bufferOffset ], swapbytes );
			bufferOffset += sizeof( length );
			auto str = String( reinterpret_cast< const char* >( &buffer[ bufferOffset ] ), length );
			bufferOffset += length;
			return str;
		}

		template< typename T >
		Value DeserializeList( const uint8* buffer, uint64& bufferOffset, uint64 numel, const bool swapbytes )
		{
			if constexpr ( std::is_same_v< T, DynamicArray< String > > )
			{
				DynamicArray< String > vec( numel );

				for ( uint64 i = 0; i < numel; ++i )
				{
					vec[ i ] = DeserializeString( buffer, bufferOffset, swapbytes );
				}

				return Value( std::move( vec ) );
			}
			else
			{
				T vec( numel );

				auto reinterp_buffer = reinterpret_cast< const T::ValueType* >( &buffer[ bufferOffset ] );

				for ( uint64 i = 0; i < numel; ++i )
				{
					auto val = reinterp_buffer[ i ];

					if ( swapbytes )
						val = byteswap( val );

					vec[ i ] = val;
				}

				bufferOffset += sizeof( typename T::ValueType ) * numel;

				return Value( std::move( vec ) );
			}
		}

		template< typename T >
		Value DeserializeValue( const uint8* buffer, uint64& bufferOffset, const bool swapbytes )
		{
			if constexpr ( std::is_same_v< T, String > )
			{
				auto length = ReadValueFromDynamicArray< uint16 >( &buffer[ bufferOffset ], swapbytes );
				bufferOffset += sizeof( length );
				auto str = String( reinterpret_cast<const char*>( &buffer[ bufferOffset ] ), length );
				bufferOffset += length;
				return Value( std::move( str ) );
			}
			else
			{
				auto val = ReadValueFromDynamicArray< T >( &buffer[bufferOffset], swapbytes );
				bufferOffset += sizeof( val );
				return Value( val );
			}
		}

		Value DeserializeEmptyValue( const uint8* buffer, uint64& bufferOffset, const bool swapbytes )
		{
			return Value();
		}

		namespace bitstream_v1
		{
			Map Deserialize( const uint8* buffer, uint64 bufferSize, uint64& bufferOffset );
		}

		void DeserializeAndInsertMap( Map& map, String&& key, const uint8* buffer, uint64 bufferSize, uint64& bufferOffset, const bool swapbytes )
		{
			//                "tvm<n>"     endianness         numel
			if ( bufferSize < 4 + sizeof( uint16 ) + sizeof( uint64 ) )
			{
				throw std::runtime_error( "Invalid buffer length! Must be long enough for Header!" );
			}
			const auto t = buffer[ bufferOffset ];
			const auto v = buffer[ bufferOffset + 1 ];
			const auto m = buffer[ bufferOffset + 2 ];

			if ( t != 't' || v != 'v' || m != 'm' )
			{
				throw std::runtime_error( "Expected valid Header!" );
			}

			const auto version = buffer[ bufferOffset + 3 ];

			if ( version != 1 )
				throw std::runtime_error( "Invalid Map version!" );

			bufferOffset += 4;

			map.insert( { std::move( key ), Value( bitstream_v1::Deserialize( buffer, bufferSize, bufferOffset ) ) } );
		}

		namespace bitstream_v1
		{
			template< typename T >
			void DeserializeAndInsertList( Map& map, String&& key, const uint8* buffer, uint64& bufferOffset, bool swapbytes )
			{
				auto numel = ReadValueFromDynamicArray< uint64 >( &buffer[ bufferOffset ], swapbytes );
				bufferOffset += sizeof( uint64 );
				map.insert( { std::move( key ), DeserializeList< T >( buffer, bufferOffset, numel, swapbytes ) } );
			}

			template< typename T >
			void DeserializeAndInsertValue( Map& map, String&& key, const uint8* buffer, uint64& bufferOffset, bool swapbytes )
			{
				map.insert( { std::move( key ), DeserializeValue< T >( buffer, bufferOffset, swapbytes ) } );
			}

			void DeserializeAndInsertEmptyValue( Map& map, String&& key, const uint8* buffer, uint64& bufferOffset, bool swapbytes )
			{
				map.insert( { std::move( key ), DeserializeEmptyValue( buffer, bufferOffset, swapbytes ) } );
			}

			Map Deserialize( const uint8* buffer, uint64 bufferSize, uint64& bufferOffset )
			{
				auto const endianness = *reinterpret_cast< const uint16* >( &buffer[ bufferOffset ] );
				bufferOffset += sizeof( uint16 );

				const bool swapbytes = endianness != 1;

				auto numel = *reinterpret_cast< const uint64* >( &buffer[ bufferOffset ] );

				if ( swapbytes )
					numel = byteswap( numel );

				bufferOffset += sizeof( uint64 );

				Map out_vm;

				for ( uint64 numel_found = 0; numel_found < numel; ++numel_found )
				{
					String key = DeserializeString( buffer, bufferOffset, swapbytes );

					Type type = static_cast< Type >( buffer[ bufferOffset ] );
					bufferOffset += sizeof( Type );

					switch ( type )
					{
					case VOID:
						DeserializeAndInsertEmptyValue( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case INT8:
						DeserializeAndInsertValue< int8 >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case INT16:
						DeserializeAndInsertValue< int16 >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case INT32:
						DeserializeAndInsertValue< int32 >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case INT64:
						DeserializeAndInsertValue< int64 >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case UINT8:
						DeserializeAndInsertValue< uint8 >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case UINT16:
						DeserializeAndInsertValue< uint16 >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case UINT32:
						DeserializeAndInsertValue< uint32 >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case UINT64:
						DeserializeAndInsertValue< uint64 >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case FLOAT:
						DeserializeAndInsertValue< float >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case DOUBLE:
						DeserializeAndInsertValue< double >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case STRING:
						DeserializeAndInsertValue< String >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case MAP:
						DeserializeAndInsertMap( out_vm, std::move( key ), buffer, bufferSize, bufferOffset, swapbytes );
						continue;
					case INT8_ARRAY:
						DeserializeAndInsertList< DynamicArray< int8 > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case INT16_ARRAY:
						DeserializeAndInsertList< DynamicArray< int16 > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case INT32_ARRAY:
						DeserializeAndInsertList< DynamicArray< int32 > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case INT64_ARRAY:
						DeserializeAndInsertList< DynamicArray< int64 > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case UINT8_ARRAY:
						DeserializeAndInsertList< DynamicArray< uint8 > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case UINT16_ARRAY:
						DeserializeAndInsertList< DynamicArray< uint16 > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case UINT32_ARRAY:
						DeserializeAndInsertList< DynamicArray< uint32 > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case UINT64_ARRAY:
						DeserializeAndInsertList< DynamicArray< uint64 > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case FLOAT_ARRAY:
						DeserializeAndInsertList< DynamicArray< float > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case DOUBLE_ARRAY:
						DeserializeAndInsertList< DynamicArray< double > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case STRING_ARRAY:
						DeserializeAndInsertList< DynamicArray< String > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					}
					assert( false );
				}

				return out_vm;
			}
		}

		Map Deserialize( const uint8* buffer, uint64 bufferSize )
		{
			//            "tvm<n>"    endianness         numel
			if ( bufferSize < 4 + sizeof( uint16 ) + sizeof( uint64 ) )
			{
				throw std::runtime_error( "Invalid buffer length! Must be long enough for Header!" );
			}
			const auto t = buffer[ 0 ];
			const auto v = buffer[ 1 ];
			const auto m = buffer[ 2 ];

			if ( t != 't' || v != 'v' || m != 'm' )
				throw std::runtime_error( "Expected valid Header!" );

			const auto version = buffer[ 3 ];

			if ( version != 1 )
				throw std::runtime_error( "Invalid Map version!" );

			uint64 offset = 4;

			return bitstream_v1::Deserialize( buffer, bufferSize, offset );
		}

		Map Deserialize( DynamicArray< uint8 > const& buffer )
		{
			return Deserialize( buffer.data(), buffer.size() );
		}
	}
}