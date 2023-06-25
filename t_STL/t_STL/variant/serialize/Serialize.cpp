#pragma once

#include "Serialize.h"

#include <iostream>


namespace t
{
	namespace v
	{
		struct MapHeader
		{
			uint8_t version;
			uint16_t endianness;
			size_t size;
		};

		template< typename T >
		T byteswap( T data );

		template<>
		uint64_t byteswap( uint64_t data )
		{
			return (data >> 56 & 0xff) | (data >> 48 & 0xff) << 8 | (data >> 40 & 0xff) << 16 | (data >> 32 & 0xff) << 24 |
				   (data >> 24 & 0xff) << 32 | (data >> 16 & 0xff) << 40 | (data >>  8 & 0xff) << 48 | (data & 0xff) << 56;
		}

		template<>
		uint32_t byteswap( uint32_t data )
		{
			return (data >> 24 & 0xff) | (data >> 16 & 0xff) << 8 | (data >> 8 & 0xff) << 16 | (data & 0xff) << 24;
		}

		template<>
		uint16_t byteswap( uint16_t data )
		{
			return (data >> 8 & 0xff) | (data & 0xff) << 8;
		}

		template< typename T, typename = std::enable_if_t< !std::is_integral_v< T > > >
		Vector< uint8_t > SerializeValue( T const& data );

		template< typename T, typename = std::enable_if_t< std::is_integral_v< T > > >
		Vector< uint8_t > SerializeValue( T data );

		template<>
		Vector< uint8_t > SerializeValue< uint8_t, void >( uint8_t data )
		{
			Vector< uint8_t > buffer;
			buffer.resize( sizeof( Type ) + 1 );
			auto const type = templateToVariantType< uint8_t >();
			buffer[ 0 ] = type;
			buffer[ 1 ] = data;
			return buffer;
		}

		template<>
		Vector< uint8_t > SerializeValue< String, void >( String const& data )
		{
			Vector< uint8_t > buffer;
			buffer.resize( data.size() + 1 + sizeof( Type ) );
			buffer[ 0 ] = (uint8_t) templateToVariantType< String >();
			for ( size_t i = 0; i < data.size(); ++i )
			{
				buffer[ i + 1 ] = *(uint8_t*) &data[ i ];
			}
			buffer[ data.size() + 1 ] = 0;
			return buffer;
		}
		template<>
		Vector< uint8_t > SerializeValue( Value const& val )
		{
			const auto type = val.getType();

			switch ( type )
			{
			case Type::UINT8:
				return SerializeValue( val.As< uint8_t >() );
			case Type::STRING:
				return SerializeValue( val.As< String const& >() );
			default:
				throw std::runtime_error( "Type not supported" );
			}
		}

		Vector< uint8_t > Serialize( String const& str )
		{
			Vector< uint8_t > buffer;
			buffer.resize( str.size() + 1 );
			for ( size_t i = 0; i < str.size(); ++i )
			{
				buffer[ i ] = *(uint8_t*) &str[ i ];
			}
			buffer[ str.size() ] = 0;
			return buffer;
		}

		template< typename T >
		void AddToBuffer( Vector< uint8_t >& buffer, T data );

		template<>
		void AddToBuffer< uint16_t >( Vector< uint8_t >& buffer, uint16_t data )
		{
			const uint8_t* const ptr = reinterpret_cast<uint8_t*>(&data);
			buffer.pushBack( ptr[ 0 ] );
			buffer.pushBack( ptr[ 1 ] );
		}
		template<>
		void AddToBuffer< uint32_t >( Vector< uint8_t >& buffer, uint32_t data )
		{
			const uint8_t* const ptr = reinterpret_cast< uint8_t* >( &data );
			buffer.pushBack( ptr[ 0 ] );
			buffer.pushBack( ptr[ 1 ] );
			buffer.pushBack( ptr[ 2 ] );
			buffer.pushBack( ptr[ 3 ] );
		}
		template<>
		void AddToBuffer< uint64_t >( Vector< uint8_t >& buffer, uint64_t data )
		{
			const uint8_t* const ptr = reinterpret_cast<uint8_t*>(&data);
			buffer.pushBack( ptr[ 1 ] );
			buffer.pushBack( ptr[ 2 ] );
			buffer.pushBack( ptr[ 3 ] );
			buffer.pushBack( ptr[ 4 ] );
			buffer.pushBack( ptr[ 5 ] );
			buffer.pushBack( ptr[ 0 ] );
			buffer.pushBack( ptr[ 6 ] );
			buffer.pushBack( ptr[ 7 ] );
		}

		Vector< uint8_t > Serialize( Map const& map )
		{
			Vector< uint8_t > buffer;

			MapHeader header{ 0, 1, map.size() };

			buffer.pushBack( header.version );
			AddToBuffer( buffer, header.endianness );
			AddToBuffer( buffer, static_cast< uint64_t >( header.size ) );
			
			for ( const auto& [ key, value ] : map )
			{
				auto key_buff = Serialize( key );
				auto val_buf = SerializeValue( value );
				for ( size_t i = 0; i < key_buff.size(); ++i )
				{
					buffer.pushBack( key_buff[ i ] );
				}
				for ( size_t i = 0; i < val_buf.size(); ++i )
				{
					buffer.pushBack( val_buf[ i ] );
				}
			}
			buffer.shrinkToFit();
			return buffer;
		}

		template< typename T >
		Value DeserializeValue( const uint8_t* buffer, size_t numel )
		{
			if constexpr ( std::is_same_v< T, String > )
			{
				return Value( reinterpret_cast< const char* >( buffer ) );
			}
			if constexpr ( std::is_same_v< T, uint8_t > )
			{
				return Value( *reinterpret_cast< const uint8_t* >( buffer ) );
			}
			else
				throw std::runtime_error("Deserializing does not support this type");
		}

		Map Deserialize( Vector< uint8_t > const& buffer )
		{
			const auto version = buffer[ 0 ];

			if ( version != 0 )
				throw std::runtime_error("Invalid Map version!");

			auto const endianness = *reinterpret_cast< const uint16_t* >( &buffer[ 1 ] );

			const bool endianness_is_same = endianness == 1;

			if ( !endianness_is_same )
				throw std::runtime_error("Conversion from different endianness is not supported yet");

			const auto numel = *reinterpret_cast< const uint64_t* >( &buffer[ 1 + sizeof( uint16_t ) ] );

			size_t bufferOffset = 3 + sizeof( numel );

			Map out_vm;

			for ( ; bufferOffset < buffer.size(); )
			{
				String key = (const char*) &buffer[ bufferOffset ];
				bufferOffset += key.size() + 1;
				Type type = (Type) buffer[ bufferOffset ];
				bufferOffset += sizeof( Type );
				switch ( type )
				{
				case UINT8:
				{
					auto [ it, inserted ] = out_vm.insert( { std::move( key ), DeserializeValue< uint8_t >( &buffer[ bufferOffset ], 1 ) } );
					bufferOffset += 1;
					break;
				}
				case STRING:
				{
					auto [ it, inserted ] = out_vm.insert( { std::move( key ), DeserializeValue< String >( &buffer[ bufferOffset ], 1 ) } );
					bufferOffset += it->second.As< String const& >().size() + 1;
					break;
				}
				default:
					throw std::runtime_error("De-Serialization for this type is unsupported");
				}
			}

			return out_vm;
		}
	}
}
