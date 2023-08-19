#include <iostream>
#include <chrono>

#include "t.h"
#include "variant/serialize/Serialize.h"
#include "Timer.h"
#include "HashSet.h"

#include "variant/ValueImpl.h"

#include <random>

template< typename T >
void printSizeOf()
{
    std::cout << typeid( T ).name() << ": ";
    std::cout << sizeof( T ) << " bytes\n";
}

constexpr uint8 MAX_DEPTH = 16;

void testDepth( t::variant::Map const& map, uint8 depth );

void testDepth( t::variant::Value const& val, uint8 depth )
{
    /*if ( depth == MAX_DEPTH )
        throw std::runtime_error("Bad things happened!");
    if ( val.Is< t::variant::Map >() )
    {
        testDepth( val.As< t::variant::Map const& >(), depth + 1 );
    }
    else if ( val.Is< t::Vector< t::variant::Map > >() )
    {

    }*/
}

void testDepth( t::variant::Map const& map, uint8 depth )
{
    /*if ( depth == MAX_DEPTH )
        throw std::runtime_error("Bad things happened!");
    for ( const auto& [ key, value ] : map )
    {
        testDepth( value, depth + 1 );
    }*/
}

void printTree( t::variant::Map const& map, uint64 spaces )
{
    //std::string spacestr( spaces, ' ' );
    //for ( auto const& [ key, value ] : map )
    //{
    //    if ( value.Is< t::variant::Map >() )
    //    {
    //        /*std::cout << spacestr << key << '\n';
    //        std::cout << spacestr << std::hex << "0x" << &value.As< t::variant::Map const& >() << '\n';*/
    //        printTree( value.As< t::variant::Map const& >(), spaces + 2 );
    //        continue;
    //    }
    //    if ( value.Is< t::String >() )
    //    {
    //        std::cout << spacestr << key << "(" << t::variant::typeToString( value.getType() ) << ")\n";
    //        std::cout << spacestr << std::hex << "0x" << &value.As< t::String const& >() << '\n';
    //        std::cout << std::dec;
    //    }
    //}
}

void printHexStr( uint64 val )
{
    std::cout << "0x " << std::hex << val << '\n';
    std::cout << std::dec;
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
    vm["uint8"] = uint8( 1 );
    vm["uint16"] = uint16( 2 );
    vm["uint32"] = uint32( 3 );
    vm["uint64"] = uint64( 4 );
    vm["int8"] = int8( 5 );
    vm["int16"] = int16( 6 );
    vm["int32"] = int32( 7 );
    vm["int64"] = int64( 8 );
    vm["float"] = float( 9 );
    vm["double"] = double( 10 );
    vm["u8 vector"] = Vector< uint8 >{ 1, 2, 3, 4, 5 };
    vm["u16 vector"] = Vector< uint16 >{ 6, 7, 8, 9, 10 };
    vm["u32 vector"] = Vector< uint32 >{ 11, 12, 13, 14 };
    vm["u64 vector"] = Vector< uint64 >{ 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };
    vm["i8 vector"] = Vector< int8 >{ 1, 2, 3, 4, 5 };
    vm["i16 vector"] = Vector< int16 >{ 6, 7, 8, 9, 10 };
    vm["i32 vector"] = Vector< int32 >{ 11, 12, 13, 14 };
    vm["i64 vector"] = Vector< int64 >{ 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };
    vm["str vector"] = Vector< String >{ "hey", "blahblah", "broke" };
    vm["float vector"] = Vector< float >{ 1.f, 2.f, 3.f, 4.5f, 5.f, 7.69420f };
    vm["double vector"] = Vector< double >{ 25, 26, 27, 28, 29, 30, 31 };
    Map map;
    map.insert( { String( "test" ), Value( "value" ) } );
    map.insert( { String( "test2" ), Value( "value2" ) } );
    vm["vm"] = std::move( map );

    Timer< microseconds > t;

    t.start();

    auto buffer = t::variant::Serialize( vm );

    auto ser = t.stop();

    t.start();

    Map vm_2 = t::variant::Deserialize( buffer );

    auto deser = t.stop();

    if ( vm_2 != vm )
        exit( -69 );
}

#include <assert.h>
//#include "HashTable.h"

constexpr int getThing()
{
    /*t::HashTable< fString, int > map;

    t::UniquePtr< fString > strptr = fString("blah");

    *strptr = "blah";

    *strptr += "heyy brother";

    map.insert( { "hey", 0 } );
    map.insert( { "bye", 2 } );
    map.insert( { "blah", 4 } );
    map.insert( { "bahbah", 1 } );
    map.insert( { "haha", 5 } );

    uint64 i = 0;

    Vector< int > vec;
    vec.resize( map.size() );

    t::transform( map.cbegin(), map.cend(), vec.begin(),
        []( auto const& pair ){ return pair.val; } );*/

    return 0;
}

fString generateRandomString()
{
    fString x( "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" );
    std::random_device rd;
    std::mt19937 generator( rd() );

    std::shuffle( x.data(), x.data() + x.size(), generator );

    std::random_device dev;
    std::mt19937 rng( dev() );
    std::uniform_int_distribution< std::mt19937::result_type > dist( 4, 32 );

    return x.substr( 0, dist(rng) );
}

void testAndPrintMatch( fString const& pattern, fString const& str )
{
    std::cout << pattern << " matches " << str << ":\n";
    std::cout << std::boolalpha << t::string::match( pattern, str ) << '\n';
}

template< class hash, uint64 NumKeys >
int64 InsertAndTimeStuff( Vector< fString > const& keys )
{
    Timer< microseconds > timer;

    timer.start();

    hash map;

    for ( uint64 i = 0; i < NumKeys; ++i )
    {
        map.insert( { fString( keys[ i ] ), uint64( i ) } );
    }

    for ( auto it = map.begin(); it != map.end(); ++it )
    {
        it->second += 69;
    }

    return timer.stop();
}

template< uint64 NumKeys >
void HashMapVsUnorderedMap()
{
    constexpr uint64 numLoops = 5000;

    Vector< int64 > times( numLoops );

    for ( uint64 i = 0; i < numLoops; ++i )
    {
        Vector< fString > keys( numLoops );

        for ( uint64 i = 0; i < NumKeys; ++i )
            keys[ i ] = generateRandomString();

        int64 time = 0, time_ = 0;

        if ( i % 2 )
        {
            time = InsertAndTimeStuff< std::unordered_map< fString, uint64 >, NumKeys >( keys );

            time_ = InsertAndTimeStuff< t::HashMap< fString, uint64 >, NumKeys >( keys );
        }
        else
        {
            time_ = InsertAndTimeStuff< t::HashMap< fString, uint64 >, NumKeys >( keys );

            time = InsertAndTimeStuff< std::unordered_map< fString, uint64 >, NumKeys >( keys );
        }

        times[ i ] = time - time_;
    }

    int64 totalTime = 0;

    t::forEach( times.data(), times.data() + times.size(),
        [ &totalTime ]( auto time ) { totalTime += time; } );

    std::cout << "For " << NumKeys << " keys inserted over " << numLoops << " iterations:\n";

    std::cout << "    Total time difference (us): " << totalTime << '\n';

    if ( totalTime >= 0 )
        std::cout << "    average: " << static_cast< double >( totalTime ) / static_cast< double >( numLoops ) << "us faster for HashMap\n\n";
    else
        std::cout << "    average: " << static_cast< double >( totalTime ) / static_cast< double >( numLoops ) * -1 << "us faster for unordered_map\n\n";
}

int main()
{
    HashMapVsUnorderedMap<  10 >();
    HashMapVsUnorderedMap<  20 >();
    HashMapVsUnorderedMap<  40 >();
    HashMapVsUnorderedMap<  60 >();
    HashMapVsUnorderedMap<  80 >();
    HashMapVsUnorderedMap< 100 >();
    HashMapVsUnorderedMap< 120 >();
    return 0;
}
