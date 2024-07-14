#include <cassert>
#include "Deserialize.h"

#include "../../endianness.h"
#include "../../Error.h"

namespace t
{
	namespace variant
	{
		template< typename T >
		T ReadValueFromBuffer( const uint8* buffer, bool swapbytes )
		{
			T val{};
			uint8* valBuffer = static_cast< uint8* >( static_cast< void* >( &val ) );

			for ( auto i = 0; i < sizeof( T ); ++i )
			{
				valBuffer[ i ] = buffer[ i ];
			}

			if ( swapbytes )
				return byteswap( val );
			return val;
		}

		String DeserializeString( const uint8* buffer, uint64& bufferOffset, bool swapbytes )
		{
			auto length = ReadValueFromBuffer< uint32 >( &buffer[ bufferOffset ], swapbytes );
			bufferOffset += sizeof( length );
			auto str = String( reinterpret_cast< const char* >( &buffer[ bufferOffset ] ), length );
			bufferOffset += length;
			return str;
		}

		template< typename T >
		Value DeserializeArray( const uint8* buffer, uint64& bufferOffset, uint64 numel, const bool swapbytes )
		{
			if constexpr ( std::is_same_v< T, Array< String > > )
			{
				Array< String > vec( numel );

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
				auto length = ReadValueFromBuffer< uint32 >( &buffer[ bufferOffset ], swapbytes );
				bufferOffset += sizeof( length );
				auto str = String( reinterpret_cast<const char*>( &buffer[ bufferOffset ] ), length );
				bufferOffset += length;
				return Value( std::move( str ) );
			}
			else
			{
				auto val = ReadValueFromBuffer< T >( &buffer[bufferOffset], swapbytes );
				bufferOffset += sizeof( val );
				return Value( val );
			}
		}

		Value DeserializeEmptyValue()
		{
			return Value();
		}

		namespace bitstream_v1
		{
			Map Deserialize( const uint8* buffer, uint64 bufferSize, uint64& bufferOffset );
		}

		void DeserializeAndInsertMap( Map& map, String&& key, const uint8* buffer, uint64 bufferSize, uint64& bufferOffset, const bool /*swapbytes*/)
		{
			//                "tvm<n>"     endianness         numel
			if ( bufferSize < 4 + sizeof( uint16 ) + sizeof( uint64 ) )
			{
				throw Error( "Invalid buffer length! Must be long enough for Header!", 1 );
			}
			const auto t = buffer[ bufferOffset ];
			const auto v = buffer[ bufferOffset + 1 ];
			const auto m = buffer[ bufferOffset + 2 ];

			if ( t != 't' || v != 'v' || m != 'm' )
			{
				throw Error( "Expected valid Header!", 1 );
			}

			const auto version = buffer[ bufferOffset + 3 ];

			if ( version != 1 )
				throw Error( "Invalid Map version!", 1 );

			bufferOffset += 4;

			map.insert( { std::move( key ), Value( bitstream_v1::Deserialize( buffer, bufferSize, bufferOffset ) ) } );
		}

		namespace bitstream_v1
		{
			template< typename T >
			void DeserializeAndInsertArray( Map& map, String&& key, const uint8* buffer, uint64& bufferOffset, bool swapbytes )
			{
				auto numel = ReadValueFromBuffer< uint64 >( &buffer[ bufferOffset ], swapbytes );
				bufferOffset += sizeof( uint64 );
				map.insert( { std::move( key ), DeserializeArray< T >( buffer, bufferOffset, numel, swapbytes ) } );
			}

			template< typename T >
			void DeserializeAndInsertValue( Map& map, String&& key, const uint8* buffer, uint64& bufferOffset, bool swapbytes )
			{
				map.insert( { std::move( key ), DeserializeValue< T >( buffer, bufferOffset, swapbytes ) } );
			}

			void DeserializeAndInsertEmptyValue( Map& map, String&& key )
			{
				map.insert( { std::move( key ), DeserializeEmptyValue() } );
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
					case Type::VOID:
						DeserializeAndInsertEmptyValue( out_vm, std::move( key ) );
						continue;
					case Type::INT8:
						DeserializeAndInsertValue< int8 >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::INT16:
						DeserializeAndInsertValue< int16 >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::INT32:
						DeserializeAndInsertValue< int32 >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::INT64:
						DeserializeAndInsertValue< int64 >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::UINT8:
						DeserializeAndInsertValue< uint8 >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::UINT16:
						DeserializeAndInsertValue< uint16 >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::UINT32:
						DeserializeAndInsertValue< uint32 >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::UINT64:
						DeserializeAndInsertValue< uint64 >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::FLOAT:
						DeserializeAndInsertValue< float >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::DOUBLE:
						DeserializeAndInsertValue< double >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::STRING:
						DeserializeAndInsertValue< String >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::MAP:
						DeserializeAndInsertMap( out_vm, std::move( key ), buffer, bufferSize, bufferOffset, swapbytes );
						continue;
					case Type::INT8_ARRAY:
						DeserializeAndInsertArray< Array< int8 > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::INT16_ARRAY:
						DeserializeAndInsertArray< Array< int16 > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::INT32_ARRAY:
						DeserializeAndInsertArray< Array< int32 > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::INT64_ARRAY:
						DeserializeAndInsertArray< Array< int64 > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::UINT8_ARRAY:
						DeserializeAndInsertArray< Array< uint8 > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::UINT16_ARRAY:
						DeserializeAndInsertArray< Array< uint16 > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::UINT32_ARRAY:
						DeserializeAndInsertArray< Array< uint32 > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::UINT64_ARRAY:
						DeserializeAndInsertArray< Array< uint64 > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::FLOAT_ARRAY:
						DeserializeAndInsertArray< Array< float > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::DOUBLE_ARRAY:
						DeserializeAndInsertArray< Array< double > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
						continue;
					case Type::STRING_ARRAY:
						DeserializeAndInsertArray< Array< String > >( out_vm, std::move( key ), buffer, bufferOffset, swapbytes );
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
				throw Error( "Invalid buffer length! Must be long enough for Header!", 1 );
			}
			const auto t = buffer[ 0 ];
			const auto v = buffer[ 1 ];
			const auto m = buffer[ 2 ];

			if ( t != 't' || v != 'v' || m != 'm' )
				throw Error( "Expected valid Header!", 1 );

			const auto version = buffer[ 3 ];

			if ( version != 1 )
				throw Error( "Invalid Map version!", 1 );

			uint64 offset = 4;

			return bitstream_v1::Deserialize( buffer, bufferSize, offset );
		}

		Map Deserialize( Array< uint8 > const& buffer )
		{
			return Deserialize( buffer.data(), buffer.size() );
		}
	}
}