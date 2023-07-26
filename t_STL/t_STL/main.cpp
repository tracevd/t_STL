#include <iostream>
#include <chrono>

#include "t.h"
#include "variant/serialize/Serialize.h"
#include "Timer.h"
#include "HashSet.h"

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

void testDepth( t::variant::Map const& map, uint8 depth )
{
    if ( depth == MAX_DEPTH )
        throw std::runtime_error("Bad things happened!");
    for ( const auto& [ key, value ] : map )
    {
        testDepth( value, depth + 1 );
    }
}

void printTree( t::variant::Map const& map, uint64 spaces )
{
    std::string spacestr( spaces, ' ' );
    for ( auto const& [ key, value ] : map )
    {
        if ( value.Is< t::variant::Map >() )
        {
            /*std::cout << spacestr << key << '\n';
            std::cout << spacestr << std::hex << "0x" << &value.As< t::variant::Map const& >() << '\n';*/
            printTree( value.As< t::variant::Map const& >(), spaces + 2 );
            continue;
        }
        if ( value.Is< t::String >() )
        {
            std::cout << spacestr << key << "(" << t::variant::typeToString( value.getType() ) << ")\n";
            std::cout << spacestr << std::hex << "0x" << &value.As< t::String const& >() << '\n';
            std::cout << std::dec;
        }
    }
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
    vm["vm vector"] = Vector< Map >{ map, map, map };
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

int main()
{
    testTvm();

    Map m;

    auto val = Value( uint8(32) );

    m["blah"] = "hello";

    if ( !m.at("blah").Is< String >() )
        return -1;

    m["mememe"] = (uint8) 69;

    if ( !m.at("mememe").Is< uint8 >() )
        return -2;

    m["woah"] = Vector< uint16 >{ 1, 2, 69, 420 };

    if ( !m.at("woah").Is< Vector< uint16 > >() )
        return -3;

    Map meh;
    meh["bob"] = "blah";
    Map double_nested;
    double_nested["hehe"] = int64(69);
    double_nested["uhoh"] = m;

    if ( !double_nested.at("uhoh").Is< Map >() )
        return -4;

    meh["yoyo"] = std::move( double_nested );
    m["map"] = std::move( meh );

    auto m2 = m;

    m["haha"] = m2;

    testDepth( m, 0 );

    if ( &m2.at("map").As< Map const& >() == &m.at("map").As< Map const& >() )
        return -5;

    if ( &m2.at("blah").As< String const& >() != &m.at("blah").As< String const& >() )
        return -6;

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

    /*t::HashMap< fString, int > test_map;

    test_map["one"]       = 1;
    test_map["sixtynine"] = 69;
    test_map["one"]       = 11;
    test_map["two"]       = 2;
    test_map["thr"]       = 3;
    test_map["fou"]       = 4;*/

    std::cout.clear();

    Timer< microseconds > timer;

    timer.start();

    std::unordered_map< fString, fString > umap;

    umap["hello"] = "goodbye";
    umap["goodbye"] = "hello";
    umap["whatup"] = "bob";
    umap["billy"] = "bob";
    umap["como estas"] = "how are you doing";
    umap["how are you doing"] = "bibarel";
    umap["barber shop quartet"] = "haha";

    for ( auto it = umap.begin(); it != umap.end(); ++it )
    {
        it->second += " jimmy";
    }

    auto time = timer.stop();

    std::cout << "unordered_map took " << time << "us\n";

    timer.start();

    t::HashMap< fString, fString > map;

    map["hello"] = "goodbye";
    map["goodbye"] = "hello";
    map["whatup"] = "bob";
    map["billy"] = "bob";
    map["como estas"] = "how are you doing";
    map["how are you doing"] = "bibarel";
    map["barber shop quartet"] = "haha";

    for ( auto kv = map.begin(); kv != map.end(); ++kv )
    {
        kv->second += " jimmy";
    }

    auto time_ = timer.stop();

    std::cout << "HashMap took " << time_ << "us\n";

    std::cout << "HashMap size: " << map.size() << '\n';

    constexpr int blah = getThing();

    t.start();

    t::HashSet< fString > set;

    set.insert( "blah" );
    set.insert( "hello" );
    set.insert( "whatup" );
    set.insert( "blah_" );
    set.insert( "blah__" );
    set.insert( "_blah" );
    set.insert( "meme" );
    set.insert( " " );
    set.insert( "\\ ^-^ /" );
    set.insert( "bobber" );
    set.insert( "oppenheimer" );
    set.insert( "as;fkgjqenrv" );
    set.insert( "agn4tgt " );
    set.insert( "leign" );
    set.insert( "qeothn4etg" );
    set.insert( "etgw4egeglwentg" );
    set.insert( "q3giq3n" );
    set.remove( "hello" );
    set.remove( "leign" );
    set.insert( "argw" );
    set.insert( "ergsev" );

    fString total;

    t::forEach( set.cbegin(), set.cend(),
        [ &total]( auto const& str ){ ( total += str ) += '\n'; });

    auto end_of_set_insert = t.stop();

    std::cout << total;

    std::cout << "Took " << end_of_set_insert << "us to insert elements\n";

    t.start();

    auto size = set.size();

    auto stop_ = t.stop();

    std::cout << "Took " << stop_ << "us to get size of set\n";

    std::cout << "HashSet size: " << set.size() << '\n';

    return blah;
}
