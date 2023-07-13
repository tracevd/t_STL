#include <iostream>
#include <chrono>

#define ASSERT

#include "t.h"
#include "variant/serialize/Serialize.h"
#include "Timer.h"

#include <random>

template< typename T >
void printSizeOf()
{
    std::cout << typeid( T ).name() << ": ";
    std::cout << sizeof( T ) << " bytes\n";
}

constexpr uint8_t MAX_DEPTH = 8;

void testDepth( t::variant::Map const& map, uint8_t depth );

void testDepth( t::variant::Value const& val, uint8_t depth )
{
    if ( depth == MAX_DEPTH )
        throw std::runtime_error("Bad things happened!");
    if ( val.Is< t::variant::Map >() )
    {
        testDepth( val.As< t::variant::Map const& >(), depth + 1 );
    }
    else if ( val.Is< t::Vector< t::variant::Map > >() )
    {

    }
}

void testDepth( t::variant::Map const& map, uint8_t depth )
{
    if ( depth == MAX_DEPTH )
        throw std::runtime_error("Bad things happened!");
    for ( const auto& [ key, value ] : map )
    {
        testDepth( value, depth + 1 );
    }
}

void printHexStr( uint64_t val )
{
    std::cout << "0x " << std::hex << val << '\n';
}

void hashAndPrint( t::fast::String const& str )
{
    std::cout << str << ":\n";
    printHexStr( std::hash< t::fast::String >{}( str ) );
}

using std::chrono::microseconds;
using t::variant::Map;
using t::String;
using fString = t::fast::String;
using t::Vector;
using t::variant::Value;

void testTvm()
{
    Map vm;

    vm["string"] = "hello";
    vm["uint8"] = uint8_t( 1 );
    vm["uint16"] = uint16_t( 2 );
    vm["uint32"] = uint32_t( 3 );
    vm["uint64"] = uint64_t( 4 );
    vm["int8"] = int8_t( 5 );
    vm["int16"] = int16_t( 6 );
    vm["int32"] = int32_t( 7 );
    vm["int64"] = int64_t( 8 );
    vm["float"] = float( 9 );
    vm["double"] = double( 10 );
    vm["u8 vector"] = Vector< uint8_t >{ 1, 2, 3, 4, 5 };
    vm["u16 vector"] = Vector< uint16_t >{ 6, 7, 8, 9, 10 };
    vm["u32 vector"] = Vector< uint32_t >{ 11, 12, 13, 14 };
    vm["u64 vector"] = Vector< uint64_t >{ 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };
    vm["i8 vector"] = Vector< int8_t >{ 1, 2, 3, 4, 5 };
    vm["i16 vector"] = Vector< int16_t >{ 6, 7, 8, 9, 10 };
    vm["i32 vector"] = Vector< int32_t >{ 11, 12, 13, 14 };
    vm["i64 vector"] = Vector< int64_t >{ 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };
    vm["str vector"] = Vector< String >{ "hey", "blahblah", "broke" };
    vm["float vector"] = Vector< float >{ 1.f, 2.f, 3.f, 4.5f, 5.f, 7.69420f };
    vm["double vector"] = Vector< double >{ 25, 26, 27, 28, 29, 30, 31 };
    Map map;
    map.insert( { String( "test" ), Value( "value" ) } );
    map.insert( { String( "test2" ), Value( "value2" ) } );
    vm["vm vector"] = Vector< Map >{ map, map, map };
    vm["vm"] = std::move( map );

    Timer< microseconds > t;

    t.start();

    auto buffer = t::variant::Serialize( vm );

    auto ser = t.stop();

    t.start();

    Map vm_2 = t::variant::Deserialize( buffer );

    auto deser = t.stop();

    std::cout << "Serialize:   " << ser << "us\n";
    std::cout << "Deserialize: " << deser << "us\n";

    std::cout << "Maps are equal: " << std::boolalpha << (vm_2 == vm) << '\n';
}

struct Blah
{
    String x;
};

#include <array>

int main()
{
    auto str = fString( 12345678 );

    auto str2 = fString( "blah" );

    std::cout << "Before: " << str << '\n';

    Timer< microseconds > t;

    t.start();

    t::transform( str.data(), str.data() + str.size(), str.data(),
        []( char c ){ return c + 1; } );

    auto stop = t.stop();

    auto constexpr it = t::find( "1234567", "1234567"+7, '4' );

    std::cout << "Found: " << *it << '\n';

    std::cout << "Took: " << stop << "us" << '\n';

    std::cout << "After: " << str << '\n';

    t::Array< fString, 4 > arr { "hey", "blah", "wahwah", "whoa" };

    fString cmp = "whoa";

    t.start();
    
    auto strit = t::findIf( arr.data(), arr.data() + arr.size(),
        [ &cmp ]( fString const& str ){ return str == cmp; } );

    auto findif_stop = t.stop();

    std::cout << "Found: " << *strit << '\n';

    std::cout << "Took: " << findif_stop << "us \n";

    t::forEach( arr.data(), arr.data() + arr.size(),
        []( auto const& str ){ std::cout << str << '\n'; } );

    std::unordered_map< int, int > x;

    std::array< int, 7 > _g = { 0, 1, 2, 3, 4, 5, 6 };
    std::array< int, 7 > _h = { 7, 8, 9, 10, 11, 12, 13 };

    t::transform( _g.cbegin(), _g.cend(), _h.cbegin(), std::inserter( x, x.begin() ),
        []( int a, int b ){ return std::pair< int, int >{ a, b }; } );

    t::forEach( x.cbegin(), x.cend(),
        []( auto const& pair ){ std::cout << pair.first << ": " << pair.second << '\n'; } );

    return 0;
}