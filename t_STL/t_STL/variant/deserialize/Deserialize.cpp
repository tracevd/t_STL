#include "Deserialize.h"


namespace t
{
	namespace variant
	{
		template< typename T >
		T ReadValueFromBuffer( const uint8* buffer )
		{
			return *reinterpret_cast<const T*>(buffer);
		}

		String DeserializeString( const uint8* buffer, uint64& bufferOffset )
		{
			auto length = ReadValueFromBuffer< uint16 >( &buffer[bufferOffset] );
			bufferOffset += sizeof( length );
			auto str = String( reinterpret_cast<const char*>(&buffer[bufferOffset]), length );
			bufferOffset += length;
			return str;
		}

		template< typename T >
		Value DeserializeList( const uint8* buffer, uint64& bufferOffset, uint64 numel )
		{
			if constexpr ( std::is_same_v< T, List< String > > )
			{
				List< String > vec;
				vec.reserve( numel );

				for ( ; numel > 0; --numel )
				{
					vec.pushBack( DeserializeString( buffer, bufferOffset ) );
				}

				return Value( std::move( vec ) );
			}

			T vec;
			vec.reserve( numel );

			auto reinterp_buffer = reinterpret_cast<const T::ValueType*>(&buffer[bufferOffset]);

			for ( auto numel_ = numel; numel_ > 0; --numel_ )
			{
				vec.pushBack( *reinterp_buffer );
				++reinterp_buffer;
			}

			bufferOffset += sizeof( typename T::ValueType ) * numel;

			return Value( std::move( vec ) );
		}

		template< typename T >
		Value DeserializeValue( const uint8* buffer, uint64& bufferOffset )
		{
			if constexpr ( std::is_same_v< T, String > )
			{
				auto length = ReadValueFromBuffer< uint16 >( &buffer[bufferOffset] );
				bufferOffset += sizeof( length );
				auto str = String( reinterpret_cast<const char*>(&buffer[bufferOffset]), length );
				bufferOffset += length;
				return Value( std::move( str ) );
			}

			auto val = ReadValueFromBuffer< T >( &buffer[bufferOffset] );
			bufferOffset += sizeof( val );
			return Value( val );
		}

		namespace bitstream_v1
		{
			Map Deserialize( const uint8* buffer, uint64 bufferSize, uint64& bufferOffset );
		}

		void DeserializeAndInsertMap( Map& map, String&& key, const uint8* buffer, uint64 bufferSize, uint64& bufferOffset )
		{
			//                "tvm<n>"     endianness         numel
			if ( bufferSize < 4 + sizeof( uint16 ) + sizeof( uint64 ) )
			{
				throw std::runtime_error( "Invalid buffer length! Must be long enough for Header!" );
			}
			const auto t = buffer[bufferOffset];
			const auto v = buffer[bufferOffset + 1];
			const auto m = buffer[bufferOffset + 2];

			if ( t != 't' || v != 'v' || m != 'm' )
			{
				throw std::runtime_error( "Expected valid Header!" );
			}

			const auto version = buffer[bufferOffset + 3];

			if ( version != 1 )
				throw std::runtime_error( "Invalid Map version!" );

			bufferOffset += 4;

			map.insert( { std::move( key ), Value( bitstream_v1::Deserialize( buffer, bufferSize, bufferOffset ) ) } );
		}

		namespace bitstream_v1
		{
			template< typename T >
			void DeserializeAndInsertList( Map& map, String&& key, const uint8* buffer, uint64& bufferOffset )
			{
				auto numel = ReadValueFromBuffer< uint64 >( &buffer[bufferOffset] );
				bufferOffset += sizeof( uint64 );
				map.insert( { std::move( key ), DeserializeList< T >( buffer, bufferOffset, numel ) } );
			}

			template< typename T >
			void DeserializeAndInsertValue( Map& map, String&& key, const uint8* buffer, uint64& bufferOffset )
			{
				map.insert( { std::move( key ), DeserializeValue< T >( buffer, bufferOffset ) } );
			}

			Map Deserialize( const uint8* buffer, uint64 bufferSize, uint64& bufferOffset )
			{
				auto const endianness = *reinterpret_cast<const uint16*>(&buffer[bufferOffset]);
				bufferOffset += sizeof( uint16 );

				const bool endianness_is_same = endianness == 1;

				if ( !endianness_is_same )
					throw std::runtime_error( "Conversion from different endianness is not supported yet" );

				const auto numel = *reinterpret_cast<const uint64*>(&buffer[bufferOffset]);

				bufferOffset += sizeof( uint64 );

				Map out_vm;

				out_vm.reserve( numel );

				for ( uint64 numel_found = 0; numel_found < numel; ++numel_found )
				{
					String key = DeserializeString( buffer, bufferOffset );

					Type type = (Type) buffer[bufferOffset];
					bufferOffset += sizeof( Type );

					//std::cout << key << '\n';

					switch ( type )
					{
					case INT8:
						DeserializeAndInsertValue< int8 >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case INT16:
						DeserializeAndInsertValue< int16 >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case INT32:
						DeserializeAndInsertValue< int32 >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case INT64:
						DeserializeAndInsertValue< int64 >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case UINT8:
						DeserializeAndInsertValue< uint8 >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case UINT16:
						DeserializeAndInsertValue< uint16 >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case UINT32:
						DeserializeAndInsertValue< uint32 >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case UINT64:
						DeserializeAndInsertValue< uint64 >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case FLOAT:
						DeserializeAndInsertValue< float >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case DOUBLE:
						DeserializeAndInsertValue< double >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case STRING:
						DeserializeAndInsertValue< String >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case MAP:
						DeserializeAndInsertMap( out_vm, std::move( key ), buffer, bufferSize, bufferOffset );
						continue;
					case INT8_LIST:
						DeserializeAndInsertList< List< int8 > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case INT16_LIST:
						DeserializeAndInsertList< List< int16 > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case INT32_LIST:
						DeserializeAndInsertList< List< int32 > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case INT64_LIST:
						DeserializeAndInsertList< List< int64 > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case UINT8_LIST:
						DeserializeAndInsertList< List< uint8 > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case UINT16_LIST:
						DeserializeAndInsertList< List< uint16 > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case UINT32_LIST:
						DeserializeAndInsertList< List< uint32 > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case UINT64_LIST:
						DeserializeAndInsertList< List< uint64 > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case FLOAT_LIST:
						DeserializeAndInsertList< List< float > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case DOUBLE_LIST:
						DeserializeAndInsertList< List< double > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case STRING_LIST:
						DeserializeAndInsertList< List< String > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					default:
						throw std::runtime_error( "De-Serialization for this type is unsupported" );
					}
				}

				return out_vm;
			}
		}

		Map Deserialize( const uint8* buffer, uint64 bufferSize )
		{
			//                "tvm<n>"     endianness         numel
			if ( bufferSize < 4 + sizeof( uint16 ) + sizeof( uint64 ) )
			{
				throw std::runtime_error( "Invalid buffer length! Must be long enough for Header!" );
			}
			const auto t = buffer[0];
			const auto v = buffer[1];
			const auto m = buffer[2];

			if ( t != 't' || v != 'v' || m != 'm' )
			{
				throw std::runtime_error( "Expected valid Header!" );
			}

			const auto version = buffer[3];

			if ( version != 1 )
				throw std::runtime_error( "Invalid Map version!" );

			uint64 offset = 4;

			return bitstream_v1::Deserialize( buffer, bufferSize, offset );
		}

		Map Deserialize( BufferType const& buffer )
		{
			return Deserialize( buffer.data(), buffer.size() );
		}
	}
}