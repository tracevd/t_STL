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

constexpr static uint64 get_seed_constexpr()
{
    auto time = t::hasher< fString >::hash( fString( __TIME__ ) );
    auto date = t::hasher< fString >::hash( fString( __DATE__ ) );

    return time ^ date;
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

template< class hash, uint64 NumKeys >
void ShowHashValues( Vector< fString > const& keys )
{
    t::HashSet< uint64 > hashes;

    uint64 collisions = 0;

    using std::chrono::milliseconds;

    Timer< milliseconds > timer;

    timer.start();

    for ( uint64 i = 0; i < NumKeys; ++i )
    {
        uint64 hash_ = 0;
        if constexpr ( t::type::is_same< hash, t::hasher< fString > > )
            hash_ = t::hasher< fString >::hash( keys[ i ].c_str() );
        else
            hash_ = std::hash< std::string >{}( keys[ i ].c_str() );

        if ( hashes.contains( hash_ % 16 ) )
            ++collisions;
        else
            hashes.insert( hash_ % 16 );
    }

    auto stop = timer.stop();
    
    if constexpr ( t::type::is_same< hash, t::hasher< fString > > )
        std::cout << "t::hasher< fString >\n";
    else
        std::cout << "std::hash< std::string >\n";

    std::cout << "Took " << stop << "ms (including set operations)\n";

    std::cout << "Total collisions for " << NumKeys << " strings:\n";
    std::cout << "    " << collisions << '\n';
    std::cout << "    " << static_cast< double >( collisions ) * 100.0 / static_cast< double >( NumKeys ) << "% collision rate\n\n";
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

constexpr uint64 lce_a = 4096;
constexpr uint64 lce_c = 150889;
constexpr uint64 lce_m = 714025239563;

constexpr static uint64 uniform_distribution( uint64& previous )
{
    previous = ( (lce_a * previous + lce_c ) % lce_m );
    return previous;
}

constexpr static double uniform_distribution_n( uint64& previous )
{
    auto dst = uniform_distribution( previous );
    return static_cast< double >( dst ) / lce_m;
}

#include <array>

template <typename T, std::size_t sz>
constexpr static auto uniform_distribution( T min, T max )
{
    std::array< T, sz > dst{};
    auto previous = get_seed_constexpr();
    for ( auto& el : dst )
        el = static_cast< T >( uniform_distribution_n( previous ) * (max - min) + min );

    return dst;
}

template <typename T, std::size_t sz, std::size_t irwin_numbers = 12>
constexpr static auto normal_distribution( T min, T max )
{
    std::array<T, sz> dst = uniform_distribution< T, sz >( min, max );

    auto previous = get_seed_constexpr();

    for ( auto& el : dst )
    {
        auto val = previous * 350693;

        auto const tmp = previous;

        for ( std::size_t i = 0; i < ( tmp & 0xff ) + 12; ++i )
        {
            previous *= 0xdbfb00b;
            val ^= uniform_distribution( previous );
            val += uniform_distribution( previous );
        }

        el = val;
    }

    return dst;
}

template< uint64 NumKeys >
void StdHashVsTHash()
{
    Vector< fString > keys( 150 );
    for ( uint64 i = 0; i < keys.size(); ++i )
        keys[ i ] = generateRandomString();

    ShowHashValues< t::hasher< fString >, NumKeys >( keys );
    ShowHashValues< std::hash< std::string >, NumKeys >( keys );
}

constexpr int TestUniquePtr()
{
    auto ptr = t::make_unique< int >( 123 );
    auto ptr2 = t::move( ptr );

    if ( ptr.get() != nullptr )
        throw std::runtime_error("nonononono");

    auto arr = t::make_unique< int[ 3 ] >( 4, 5, 6 );
    auto arr2 = t::move( arr );

    if ( arr.get() != nullptr )
        throw std::runtime_error("nonono");

    return arr2->data()[ 0 ];
}

constexpr int TestSharedPtr()
{
    auto ptr = t::make_shared< int >( 123 );
    auto ptr2 = ptr;

    auto arr = t::make_shared< int[ 3 ] >( 1, 2, 3 );
    auto arr2 = arr;
    auto arr3 = arr2;

    return ( *arr )[ 2 ];
}

constexpr int TestImmSharedPtr()
{
    auto ims = t::make_immutable_shared< int >( 10 );

    auto ims2 = ims;

    if ( ims.isUnique() )
        throw std::runtime_error("afjnaefn");

    auto val = *ims;

    if ( ims.isShared() )
        throw std::runtime_error("why are you so stupid");

    return val;
}

int main()
{
    /*constexpr auto init  = get_seed_constexpr();
    constexpr auto rands = normal_distribution< uint64, 20 >( 0, t::limit< uint64 >::max );

    std::cout << init << '\n';*/

    /*for ( auto num : rands )
        std::cout << num << '\n';*/

    /*StdHashVsTHash< 15 >();
    StdHashVsTHash< 20 >();
    StdHashVsTHash< 35 >();
    StdHashVsTHash< 50 >();
    StdHashVsTHash< 65 >();*/

    /*HashMapVsUnorderedMap<  10 >();
    HashMapVsUnorderedMap<  20 >();
    HashMapVsUnorderedMap<  40 >();
    HashMapVsUnorderedMap<  60 >();
    HashMapVsUnorderedMap<  80 >();
    HashMapVsUnorderedMap< 100 >();
    HashMapVsUnorderedMap< 120 >();*/

    constexpr int a = TestSharedPtr();
    constexpr int b = TestUniquePtr();
    constexpr int c = TestImmSharedPtr();

    auto const blah{ t::make_immutable_shared< int >( 123 ) };

    auto const blah2 = blah;

    if ( blah.get() != blah2.get() )
    {
        std::cout << "What the frick bro\n";
    }
    else
    {
        std::cout << "ptrs were the same, good job\n";
    }

    t::ImmutableSharedPtr< int > blah3( 456 );

    auto blah4 = blah3;

    if ( blah3.get() == blah4.get() )
    {
        std::cout << "what the frick bro (2)\n";
    }
    else
    {
        std::cout << "ptrs were different, good job\n";
    }

    return 0;
}
