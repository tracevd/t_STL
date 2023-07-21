#include "Serialize.h"

#include <iostream>
#include <type_traits>


namespace t
{
	namespace variant
	{
		template< typename T >
		T byteswap( T data );

		template<>
		uint64 byteswap( uint64 data )
		{
			return (data >> 56 & 0xff) | (data >> 48 & 0xff) << 8 | (data >> 40 & 0xff) << 16 | (data >> 32 & 0xff) << 24 |
				   (data >> 24 & 0xff) << 32 | (data >> 16 & 0xff) << 40 | (data >>  8 & 0xff) << 48 | (data & 0xff) << 56;
		}

		template<>
		uint32 byteswap( uint32 data )
		{
			return (data >> 24 & 0xff) | (data >> 16 & 0xff) << 8 | (data >> 8 & 0xff) << 16 | (data & 0xff) << 24;
		}

		template<>
		uint16 byteswap( uint16 data )
		{
			return (data >> 8 & 0xff) | (data & 0xff) << 8;
		}

		template< typename T >
		void AddToBuffer( BufferType& buffer, T data );

		template<>
		void AddToBuffer< uint8 >( BufferType& buffer, uint8 data )
		{
			buffer.pushBack( data );
		}

		template<>
		void AddToBuffer< uint16 >( BufferType& buffer, uint16 data )
		{
			const uint8* const ptr = reinterpret_cast<uint8*>(&data);
			buffer.pushBack( ptr[ 0 ] );
			buffer.pushBack( ptr[ 1 ] );
		}
		template<>
		void AddToBuffer< uint32 >( BufferType& buffer, uint32 data )
		{
			const uint8* const ptr = reinterpret_cast< uint8* >( &data );
			buffer.pushBack( ptr[ 0 ] );
			buffer.pushBack( ptr[ 1 ] );
			buffer.pushBack( ptr[ 2 ] );
			buffer.pushBack( ptr[ 3 ] );
		}
		template<>
		void AddToBuffer< uint64 >( BufferType& buffer, uint64 data )
		{
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

		template<>
		void AddToBuffer< int8 >( BufferType& buffer, int8 data )
		{
			AddToBuffer< uint8 >( buffer, *reinterpret_cast< uint8* >( &data ) );
		}

		template<>
		void AddToBuffer< int16 >( BufferType& buffer, int16 data )
		{
			AddToBuffer< uint16 >( buffer, *reinterpret_cast< uint16* >( &data ) );
		}

		template<>
		void AddToBuffer< int32 >( BufferType& buffer, int32 data )
		{
			AddToBuffer< uint32 >( buffer, *reinterpret_cast< uint32* >( &data ) );
		}

		template<>
		void AddToBuffer< int64 >( BufferType& buffer, int64 data )
		{
			AddToBuffer< uint64 >( buffer, *reinterpret_cast< uint64* >( &data ) );
		}

		template<>
		void AddToBuffer< float >( BufferType& buffer, float data )
		{
			AddToBuffer< uint32 >( buffer, *reinterpret_cast< uint32* >( &data ) );
		}

		template<>
		void AddToBuffer< double >( BufferType& buffer, double data )
		{
			AddToBuffer< uint64 >( buffer, *reinterpret_cast< uint64* >( &data ) );
		}

		template< typename T >
		void AddToBuffer( BufferType& buffer, Vector< T > const& data )
		{
			for ( uint64 i = 0; i < data.size(); ++i )
			{
				AddToBuffer( buffer, data[ i ] );
			}
		}

		inline namespace bitstream_v1
		{
			void Serialize( BufferType& buffer, Map const& map );

			void Serialize( BufferType& buffer, String const& str )
			{
				AddToBuffer( buffer, (uint16) str.size() );
				for ( uint64 i = 0; i < str.size(); ++i )
				{
					AddToBuffer( buffer, *(uint8*) &str[i] );
				}
			}

			template< typename T >
			void SerializeComplexValue( BufferType& buffer, T const& data );

			template< typename T >
			void SerializePrimitiveValue( BufferType& buffer, T data )
			{
				auto constexpr type = templateToVariantType< T >();
				static_assert( type != VOID, "Invalid Type attempting to be serialized" );
				AddToBuffer< uint8 >( buffer, type );
				AddToBuffer< T >( buffer, data );
			}

			template< typename T >
			void SerializeVectorValue( BufferType& buffer, Vector< T > const& data )
			{
				AddToBuffer< uint8 >( buffer, templateToVariantType< Vector< T > >() );
				AddToBuffer< uint64 >( buffer, data.size() );

				for ( uint64 i = 0; i < data.size(); ++i )
				{
					AddToBuffer< T >( buffer, data[ i ] );
				}
			}

			template<>
			void SerializeComplexValue< String >( BufferType& buffer, String const& data )
			{
				AddToBuffer( buffer, (uint8) templateToVariantType< String >() );
				Serialize( buffer, data );
			}

			template<>
			void SerializeComplexValue< Map >( BufferType& buffer, Map const& data )
			{
				AddToBuffer( buffer, (uint8) templateToVariantType< Map >() );
				Serialize( buffer, data );
			}

			template<>
			void SerializeComplexValue< Vector< Map > >( BufferType& buffer, Vector< Map > const& data )
			{
				AddToBuffer( buffer, (uint8) templateToVariantType< Vector< Map > >() );
				AddToBuffer< uint64 >( buffer, data.size() );

				for ( uint64 i = 0; i < data.size(); ++i )
				{
					Serialize( buffer, data[ i ] );
				}
			}

			template<>
			void SerializeComplexValue< Vector< String > >( BufferType& buffer, Vector< String > const& data )
			{
				AddToBuffer< uint8 >( buffer, (uint8) templateToVariantType< Vector< String > >() );
				AddToBuffer< uint64 >( buffer, data.size() );
				for ( uint64 i = 0; i < data.size(); ++i )
				{
					Serialize( buffer, data[ i ] );
				}
			}

			void SerializeValue( BufferType& buffer, Value const& val )
			{
				const auto type = val.getType();

				switch ( type )
				{
				case Type::INT8:
					return SerializePrimitiveValue( buffer, val.As< int8 >() );
				case Type::INT16:
					return SerializePrimitiveValue( buffer, val.As< int16 >() );
				case Type::INT32:
					return SerializePrimitiveValue( buffer, val.As< int32 >() );
				case Type::INT64:
					return SerializePrimitiveValue( buffer, val.As< int64 >() );
				case Type::UINT8:
					return SerializePrimitiveValue( buffer, val.As< uint8 >() );
				case Type::UINT16:
					return SerializePrimitiveValue( buffer, val.As< uint16 >() );
				case Type::UINT32:
					return SerializePrimitiveValue( buffer, val.As< uint32 >() );
				case Type::UINT64:
					return SerializePrimitiveValue( buffer, val.As< uint64 >() );
				case Type::FLOAT:
					return SerializePrimitiveValue( buffer, val.As< float >() );
				case Type::DOUBLE:
					return SerializePrimitiveValue( buffer, val.As< double >() );
				case Type::STRING:
					return SerializeComplexValue( buffer, val.As< String const& >() );
				case Type::MAP:
					return SerializeComplexValue( buffer, val.As< Map const& >() );
				case Type::INT8_VECTOR:
					return SerializeVectorValue( buffer, val.As< Vector< int8 > const& >() );
				case Type::INT16_VECTOR:
					return SerializeVectorValue( buffer, val.As< Vector< int16 > const& >() );
				case Type::INT32_VECTOR:
					return SerializeVectorValue( buffer, val.As< Vector< int32 > const& >() );
				case Type::INT64_VECTOR:
					return SerializeVectorValue( buffer, val.As< Vector< int64 > const& >() );
				case Type::UINT8_VECTOR:
					return SerializeVectorValue( buffer, val.As< Vector< uint8 > const& >() );
				case Type::UINT16_VECTOR:
					return SerializeVectorValue( buffer, val.As< Vector< uint16 > const& >() );
				case Type::UINT32_VECTOR:
					return SerializeVectorValue( buffer, val.As< Vector< uint32 > const& >() );
				case Type::UINT64_VECTOR:
					return SerializeVectorValue( buffer, val.As< Vector< uint64 > const& >() );
				case Type::FLOAT_VECTOR:
					return SerializeVectorValue( buffer, val.As< Vector< float > const& >() );
				case Type::DOUBLE_VECTOR:
					return SerializeVectorValue( buffer, val.As< Vector< double > const& >() );
				case Type::STRING_VECTOR:
					return SerializeComplexValue( buffer, val.As< Vector< String > const& >() );
				case Type::MAP_VECTOR:
					return SerializeComplexValue( buffer, val.As< Vector< Map > const& >() );
				default:
					throw std::runtime_error( "Type not supported" );
				}
			}

			void Serialize( BufferType& buffer, Map const& map )
			{
				buffer.pushBack( 't' );
				buffer.pushBack( 'v' );
				buffer.pushBack( 'm' );
				buffer.pushBack( 1 );

				AddToBuffer< uint16 >( buffer, 1 );
				AddToBuffer< uint64 >( buffer, map.size() );
				
				for ( const auto& [ key, value ] : map )
				{
					Serialize( buffer, key );
					SerializeValue( buffer, value );
				}
			}

			BufferType Serialize( Map const& map )
			{
				BufferType buffer;

				Serialize( buffer, map );

				return buffer;
			}
		}

		template< typename T >
		T ReadValueFromBuffer( const uint8* buffer )
		{
			return *reinterpret_cast< const T* >( buffer );
		}

		String DeserializeString( const uint8* buffer, uint64& bufferOffset )
		{
			auto length = ReadValueFromBuffer< uint16 >( &buffer[ bufferOffset ] );
			bufferOffset += sizeof( length );
			auto str = String( reinterpret_cast< const char* >( &buffer[ bufferOffset ] ), length );
			bufferOffset += length;
			return str;
		}

		template< typename T >
		Value DeserializeVector( const uint8* buffer, uint64& bufferOffset, uint64 numel )
		{
			if constexpr ( std::is_same_v< T, Vector< String > > )
			{
				Vector< String > vec;
				vec.reserve( numel );

				for ( ; numel > 0; --numel )
				{
					vec.pushBack( DeserializeString( buffer, bufferOffset ) );
				}

				return Value( std::move( vec ) );
			}

			T vec;
			vec.reserve( numel );

			auto reinterp_buffer = reinterpret_cast< const T::ValueType* >( &buffer[ bufferOffset ] );

			for ( auto numel_ = numel ; numel_ > 0; --numel_ )
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
				auto length = ReadValueFromBuffer< uint16 >( &buffer[ bufferOffset ] );
				bufferOffset += sizeof( length );
				auto str = String( reinterpret_cast< const char* >( &buffer[ bufferOffset ] ), length );
				bufferOffset += length;
				return Value( std::move( str ) );
			}
			
			auto val = ReadValueFromBuffer< T >( &buffer[ bufferOffset ] );
			bufferOffset += sizeof( val );
			return Value( val );
		}

		namespace bitstream_v1
		{
			Map Deserialize( const uint8* buffer, uint64 bufferSize, uint64& bufferOffset );
		}

		void DeserializeAndInsertMapVector( Map& map, String&& key, const uint8* buffer, uint64 bufferSize, uint64& bufferOffset )
		{
			auto numel = ReadValueFromBuffer< uint64 >( &buffer[ bufferOffset ] );
			bufferOffset += sizeof( uint64 );

			Vector< Map > vec;
			vec.reserve( numel );

			for ( auto numel_ = numel; numel_ > 0; --numel_ )
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
				vec.pushBack( Deserialize( buffer, bufferSize, bufferOffset ) );
			}

			map.insert( { std::move( key ), Value( std::move( vec ) ) } );
		}

		void DeserializeAndInsertMap( Map& map, String&& key, const uint8* buffer, uint64 bufferSize, uint64& bufferOffset )
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
			void DeserializeAndInsertVector( Map& map, String&& key, const uint8* buffer, uint64& bufferOffset )
			{
				auto numel = ReadValueFromBuffer< uint64 >( &buffer[ bufferOffset ] );
				bufferOffset += sizeof( uint64 );
				map.insert( { std::move( key ), DeserializeVector< T >( buffer, bufferOffset, numel ) } );
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

				const auto numel = *reinterpret_cast< const uint64* >( &buffer[ bufferOffset ] );

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
					case INT8_VECTOR:
						DeserializeAndInsertVector< Vector< int8 > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case INT16_VECTOR:
						DeserializeAndInsertVector< Vector< int16 > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case INT32_VECTOR:
						DeserializeAndInsertVector< Vector< int32 > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case INT64_VECTOR:
						DeserializeAndInsertVector< Vector< int64 > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case UINT8_VECTOR:
						DeserializeAndInsertVector< Vector< uint8 > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case UINT16_VECTOR:
						DeserializeAndInsertVector< Vector< uint16 > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case UINT32_VECTOR:
						DeserializeAndInsertVector< Vector< uint32 > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case UINT64_VECTOR:
						DeserializeAndInsertVector< Vector< uint64 > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case FLOAT_VECTOR:
						DeserializeAndInsertVector< Vector< float > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case DOUBLE_VECTOR:
						DeserializeAndInsertVector< Vector< double > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case STRING_VECTOR:
						DeserializeAndInsertVector< Vector< String > >( out_vm, std::move( key ), buffer, bufferOffset );
						continue;
					case MAP_VECTOR:
						DeserializeAndInsertMapVector( out_vm, std::move( key ), buffer, bufferSize, bufferOffset );
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
				throw std::runtime_error("Invalid buffer length! Must be long enough for Header!");
			}
			const auto t = buffer[ 0 ];
			const auto v = buffer[ 1 ];
			const auto m = buffer[ 2 ];

			if ( t != 't' || v != 'v' || m != 'm' )
			{
				throw std::runtime_error("Expected valid Header!");
			}

			const auto version = buffer[ 3 ];

			if ( version != 1 )
				throw std::runtime_error("Invalid Map version!");

			uint64 offset = 4;

			return bitstream_v1::Deserialize( buffer, bufferSize, offset );
		}

		Map Deserialize( BufferType const& buffer )
		{
			return Deserialize( buffer.data(), buffer.size() );
		}
	}
}
