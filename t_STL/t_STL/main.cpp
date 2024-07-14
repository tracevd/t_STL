#include <iostream>
#include <unordered_map>

#include "t.h"
#include "variant/serialization/Serialize.h"
#include "variant/serialization/Deserialize.h"
#include "Timer.h"
#include "HashSet.h"

#include "BasicHashMap.h"

#include "ArrayView.h"

#include <random>

#include "Optional.h"

#include "Fraction.h"

#include "Any.h"
#include "Variant.h"
#include <variant>

#include "Tree.h"

template< typename T >
void printSizeOf()
{
    std::cout << typeid( T ).name() << ": ";
    std::cout << sizeof( T ) << " bytes\n";
}

void printHexStr( uint64 val )
{
    std::cout << "0x " << std::hex << val << '\n';
    std::cout << std::dec;
}

using std::chrono::microseconds;
using t::variant::Map;
using t::String;
using t::Array;

constexpr int testVm()
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
    vm["float"] = float( 9.0 );
    vm["double"] = double( 10.0 );
    vm["u8 vector"] = Array< uint8 >{ 1, 2, 3, 4, 5 };
    vm["u16 vector"] = Array< uint16 >{ 6, 7, 8, 9, 10 };
    vm["u32 vector"] = Array< uint32 >{ 11, 12, 13, 14 };
    vm["u64 vector"] = Array< uint64 >{ 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };
    vm["i8 vector"] = Array< int8 >{ 1, 2, 3, 4, 5 };
    vm["i16 vector"] = Array< int16 >{ 6, 7, 8, 9, 10 };
    vm["i32 vector"] = Array< int32 >{ 11, 12, 13, 14 };
    vm["i64 vector"] = Array< int64 >{ 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };
    vm["str vector"] = Array< String >{ "hey", "blahblah", "broke" };
    vm["float vector"] = Array< float >{ 1.f, 2.f, 3.f, 4.5f, 5.f, 7.69420f };
    vm["double vector"] = Array< double >{ 25, 26, 27, 28, 29, 30, 31 };
    Map map;
    map.insert( { String( "test" ), t::variant::Value( "value" ) } );
    map.insert( { String( "test2" ), t::variant::Value( "value2" ) } );
    vm["vm"] = std::move( map );

    auto vm2 = vm;

    if ( &vm2.at("int32").As< int32 const& >() != &vm.at("int32").As< int32 const& >() )
        throw std::runtime_error("wornfwr");

    if ( &vm2.at("int32").As< int32& >() == &vm.at("int32").As< int32 const& >() )
        throw std::runtime_error("wofnwv");

    if ( &vm2.at("int32").As< int32& >() != &vm2.at("int32").As< int32& >() )
        throw std::runtime_error("3ougnrw");


    return vm2.at("int32").As< int32 >();
}

t::pair< int64, int64 > testTvm()
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
    vm["float"] = float( 9.0 );
    vm["double"] = double( 10.0 );
    vm["u8 vector"] = Array< uint8 >{ 1, 2, 3, 4, 5 };
    vm["u16 vector"] = Array< uint16 >{ 6, 7, 8, 9, 10 };
    vm["u32 vector"] = Array< uint32 >{ 11, 12, 13, 14 };
    vm["u64 vector"] = Array< uint64 >{ 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };
    vm["i8 vector"] = Array< int8 >{ 1, 2, 3, 4, 5 };
    vm["i16 vector"] = Array< int16 >{ 6, 7, 8, 9, 10 };
    vm["i32 vector"] = Array< int32 >{ 11, 12, 13, 14 };
    vm["i64 vector"] = Array< int64 >{ 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };
    vm["str vector"] = Array< String >{ "hey", "blahblah", "broke" };
    vm["float vector"] = Array< float >{ 1.f, 2.f, 3.f, 4.5f, 5.f, 7.69420f };
    vm["double vector"] = Array< double >{ 25, 26, 27, 28, 29, 30, 31 };
    Map map;
    map.insert( { String( "test" ), t::variant::Value( "value" ) } );
    map.insert( { String( "test2" ), t::variant::Value( "value2" ) } );
    vm["vm"] = std::move( map );

    /*Array< int32 > buff( 9000 );
    for ( auto i = -4500; i < 4500; ++i )
        buff[ i + 4500 ] = i;

    vm["blahaosvnawrvnaiurvnaevnarvoanrvaijnviarv"] = std::move( buff );*/

    //vm["empty"] = Value();

    Timer< microseconds > t;

    t.start();

    auto buffer = t::variant::Serialize( vm );

    auto ser = t.stop();

    t.start();

    Map vm_2 = t::variant::Deserialize( buffer );

    auto deser = t.stop();

    if ( vm_2 != vm )
        throw std::runtime_error("Maps were not the same");

    auto val = t::variant::Value( t::move( vm_2 ) );

    auto val2 = val.Clone();

    auto val3 = t::variant::Value();

    if ( !val3.Is< void >() )
        throw std::runtime_error("slfgf");

    return { ser, deser };
}

String generateRandomString()
{
    String x( "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" );
    std::random_device rd;
    std::mt19937 generator( rd() );

    std::shuffle( x.begin(), x.end(), generator );

    std::random_device dev;
    std::mt19937 rng( dev() );
    std::uniform_int_distribution< std::mt19937::result_type > dist( 4, 32 );

    return x.substr( 0, dist( rng ) );
}

template< class T >
T generateRandom()
{
    std::random_device dev;
    std::mt19937 rng( dev() );
    std::uniform_int_distribution< T > dist;

    return dist( rng );
}

template<>
double generateRandom< double >()
{
    auto rand = generateRandom< int64 >();

    return std::bit_cast< double >( rand );
}

template< uint64 CountSize, class T >
void countSort( T* arr, uint64 numel, uint64 shift )
{
    t::HeapArray< T > output( numel );

    uint64 i = 0;
    constinit uint64 count[ CountSize ] = { 0 };

    for ( i = 0; i < numel; ++i )
        ++count[ ( arr[ i ] >> shift ) % CountSize ];

    for ( i = 1; i < CountSize; ++i )
        count[ i ] += count[ i - 1 ];

    for ( i = numel; i > 0; --i ) {
        output[ count [ ( arr[ i - 1 ] >> shift ) % CountSize ] - 1 ] = arr[ i - 1 ];
        --count[ ( arr[ i - 1 ] >> shift ) % CountSize ];
    }

    for ( i = 0; i < numel; ++i )
        arr[ i ] = output[ i ];
}

template< uint64 BitsAtOnce = 4, class T >
void radixSort( T* data, uint64 numel )
{
    constexpr uint64 NumBuckets = 0x1 << BitsAtOnce;
    static_assert( BitsAtOnce != 0 );

    constexpr double Temp = sizeof( T ) * 8.0 / static_cast< double >( BitsAtOnce );
    constexpr uint64 NumLoops = static_cast< uint64 >( Temp ) == Temp ? Temp : static_cast< uint64 >( Temp ) + 1;

    for ( uint64 i = 0; i < NumLoops; ++i )
    {
        countSort< NumBuckets >( data, numel, i * BitsAtOnce );
    }
}

struct Yapper
{
    Yapper() { std::cout << "Yapper created\n"; }
    ~Yapper() { std::cout << "Yapper destroyed\n"; }
};

int main()
{
    testTvm();

    std::cout << "main\n\n";

    std::random_device dev;
    std::mt19937 rng( dev() );
    std::uniform_int_distribution< std::mt19937::result_type > length_dist( 1000, 2000 );
    std::uniform_int_distribution< std::mt19937::result_type > num_dist;

    auto tsWasFaster = 0;
    auto ssWasFaster = 0;

    constexpr auto numTries = 100;

    for ( int i = 0; i < numTries; ++i )
    {
        auto const length = length_dist( rng );

        auto arr = Array< uint32 >( length );

        for ( auto& elem : arr )
        {
            elem = num_dist( rng );
        }

        auto arrCpy = arr;

        auto comparator = []( auto const lhs, auto const rhs ) { return lhs < rhs; };

        Timer< std::chrono::nanoseconds > t;

        t.start();

        t::sort( arr.data(), arr.data() + arr.size(), comparator );

        auto const tsElapsed = t.stop();

        std::cout << "------------------------\n";
        std::cout << "Elements: " << length << '\n';
        std::cout << "t::sort: " << tsElapsed << "nS\n";

        t.start();

        std::sort( arrCpy.data(), arrCpy.data() + arrCpy.size(), comparator );

        auto const ssElapsed = t.stop();

        std::cout << "std::sort: " << ssElapsed << "nS\n";

        if ( !t::isSorted( arr ) )
        {
            std::cout << "Oh no\n";
        }

        if ( ssElapsed > tsElapsed )
            ++tsWasFaster;
        else if ( tsElapsed > ssElapsed )
            ++ssWasFaster;
    }

    auto const percentQs  = static_cast< double >( tsWasFaster ) / static_cast< double >( numTries ) * 100;
    auto const percentSs  = static_cast< double >( ssWasFaster ) / static_cast< double >( numTries ) * 100;
    auto const percentTie = 100.0 - percentQs - percentSs;

    std::cout << "quicksort was faster " << percentQs << "% of the time\n";
    std::cout << "std::sort was faster " << percentSs << "% of the time\n";
    std::cout << "They were the same   " << percentTie << "% of the time\n";

    auto arr = t::Array< int >{ 5, 2, 1, 3, 7, 4, 9 };

    t::sort( arr.begin(), arr.end(), [](auto lhs, auto rhs) { return lhs < rhs; });

    std::cout << "----------------------\n";
    t::forEach( arr, []( auto val ){ std::cout << val << ' '; });
    std::cout << '\n';

    return 0;
}
