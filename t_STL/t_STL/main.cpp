#include <iostream>

#include "t.h"
#include "variant/serialization/Serialize.h"
#include "variant/serialization/Deserialize.h"
#include "Timer.h"
#include "HashSet.h"

#include <random>

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
using t::Buffer;
using t::variant::Value;

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
    vm["u8 vector"] = Buffer< uint8 >{ 1, 2, 3, 4, 5 };
    vm["u16 vector"] = Buffer< uint16 >{ 6, 7, 8, 9, 10 };
    vm["u32 vector"] = Buffer< uint32 >{ 11, 12, 13, 14 };
    vm["u64 vector"] = Buffer< uint64 >{ 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };
    vm["i8 vector"] = Buffer< int8 >{ 1, 2, 3, 4, 5 };
    vm["i16 vector"] = Buffer< int16 >{ 6, 7, 8, 9, 10 };
    vm["i32 vector"] = Buffer< int32 >{ 11, 12, 13, 14 };
    vm["i64 vector"] = Buffer< int64 >{ 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };
    vm["str vector"] = Buffer< String >{ "hey", "blahblah", "broke" };
    vm["float vector"] = Buffer< float >{ 1.f, 2.f, 3.f, 4.5f, 5.f, 7.69420f };
    vm["double vector"] = Buffer< double >{ 25, 26, 27, 28, 29, 30, 31 };
    Map map;
    map.insert( { String( "test" ), Value( "value" ) } );
    map.insert( { String( "test2" ), Value( "value2" ) } );
    vm["vm"] = std::move( map );

    Buffer< int32 > buff( 9000 );
    for ( auto i = -4500; i < 4500; ++i )
        buff[ i + 4500 ] = i;

    vm["blahaosvnawrvnaiurvnaevnarvoanrvaijnviarv"] = std::move( buff );

    vm["empty"] = Value();

    Timer< microseconds > t;

    t.start();

    auto buffer = t::variant::Serialize< t::endianness::native >( vm );

    auto ser = t.stop();

    t.start();

    Map vm_2 = t::variant::Deserialize( buffer );

    auto deser = t.stop();

    if ( vm_2 != vm )
        exit( -69 );

    return { ser, deser };
}

constexpr static uint64 get_seed_constexpr()
{
    auto time = t::hasher< String >::hash( String( __TIME__ ) );
    auto date = t::hasher< String >::hash( String( __DATE__ ) );

    return time ^ date;
}

String generateRandomString()
{
    String x( "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" );
    std::random_device rd;
    std::mt19937 generator( rd() );

    std::shuffle( x.data(), x.data() + x.size(), generator );

    std::random_device dev;
    std::mt19937 rng( dev() );
    std::uniform_int_distribution< std::mt19937::result_type > dist( 4, 32 );

    return x.substr( 0, dist(rng) );
}

template< class hash, uint64 NumKeys >
void ShowHashValues( Buffer< String > const& keys )
{
    t::HashSet< uint64 > hashes;

    uint64 collisions = 0;

    using std::chrono::milliseconds;

    Timer< milliseconds > timer;

    timer.start();

    for ( uint64 i = 0; i < NumKeys; ++i )
    {
        uint64 hash_ = 0;
        if constexpr ( t::type::is_same< hash, t::hasher< String > > )
            hash_ = t::hasher< String >::hash( keys[ i ].c_str() );
        else
            hash_ = std::hash< std::string >{}( keys[ i ].c_str() );

        if ( hashes.contains( hash_ % 16 ) )
            ++collisions;
        else
            hashes.insert( hash_ % 16 );
    }

    auto stop = timer.stop();
    
    if constexpr ( t::type::is_same< hash, t::hasher< String > > )
        std::cout << "t::hasher< String >\n";
    else
        std::cout << "std::hash< std::string >\n";

    std::cout << "Took " << stop << "ms (including set operations)\n";

    std::cout << "Total collisions for " << NumKeys << " strings:\n";
    std::cout << "    " << collisions << '\n';
    std::cout << "    " << static_cast< double >( collisions ) * 100.0 / static_cast< double >( NumKeys ) << "% collision rate\n\n";
}

template< class hash, uint64 NumKeys >
int64 InsertAndTimeStuff( Buffer< String > const& keys )
{
    Timer< microseconds > timer;

    timer.start();

    hash map {};

    for ( uint64 i = 0; i < NumKeys; ++i )
    {
        map.insert( { String( keys[ i ] ), uint64( i ) } );
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

    Buffer< int64 > times( numLoops );

    for ( uint64 i = 0; i < numLoops; ++i )
    {
        Buffer< String > keys( numLoops );

        for ( uint64 i = 0; i < NumKeys; ++i )
            keys[ i ] = generateRandomString();

        int64 time = 0, time_ = 0;

        if ( i % 2 )
        {
            time = InsertAndTimeStuff< std::unordered_map< String, uint64 >, NumKeys >( keys );

            time_ = InsertAndTimeStuff< t::HashMap< String, uint64 >, NumKeys >( keys );
        }
        else
        {
            time_ = InsertAndTimeStuff< t::HashMap< String, uint64 >, NumKeys >( keys );

            time = InsertAndTimeStuff< std::unordered_map< String, uint64 >, NumKeys >( keys );
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
    Buffer< String > keys( 150 );
    for ( uint64 i = 0; i < keys.size(); ++i )
        keys[ i ] = generateRandomString();

    ShowHashValues< t::hasher< String >, NumKeys >( keys );
    ShowHashValues< std::hash< std::string >, NumKeys >( keys );
}

constexpr int TestUniquePtr()
{
    auto ptr = t::make_unique< int >( 123 );
    auto ptr2 = std::move( ptr );
    ptr2 = new int( 456 );

    auto _arr = t::UniquePtr< int[] >( new int[ 5 ] );
    auto _arr2 = std::move( _arr );
    _arr2 = new int[ 3 ];

    if ( ptr == ptr2 )
        throw std::runtime_error("aorgnwrfv");

    if ( ptr != nullptr )
        throw std::runtime_error("nonononono");

    auto arr = t::make_unique< int[] >( { 4, 5, 6 } );
    auto arr2 = std::move( arr );

    if ( arr.get() != nullptr )
        throw std::runtime_error("nonono");

    return arr2.get()[ 0 ];
}

constexpr int TestSharedPtr()
{
    auto ptr = t::make_shared< int >( 123 );
    auto ptr2 = t::SharedPtr< int >( ptr );

    if ( !ptr2.isShared() || ptr2.isUnique() )
        throw std::runtime_error("nonooo");

    auto ptr3 = std::move( ptr2 );

    if ( ptr2.get() != nullptr )
        throw std::runtime_error("glkwnfv");

    if ( ptr2.isShared() || !ptr2.isUnique() )
        throw std::runtime_error("qorgwnf");

    auto arr = t::make_shared< int[] >( { 1, 2, 3, 4, 5 } );

    return *ptr;
}

constexpr int TestImmSharedPtr()
{
    auto ims = t::make_immutable_shared< int >( 10 );

    auto ims2 = t::ImmutableSharedPtr< int >( ims );

    if ( ims.isUnique() )
        throw std::runtime_error("afjnaefn");

    auto val = *ims;

    if ( ims.isShared() )
        throw std::runtime_error("why are you so stupid");

    return val;
}

constexpr int TestFastString()
{
    String str = "blah";

    str += ", lots of words.";

    str += "\nThis is a really cool sentence.";

    str.replace('.', '!');

    str.replace(',', ':');

    //auto strings = str.split(' ');

    return static_cast< int >( str.size() );
}

constexpr t::Array< int, 2 > TestLinkedList()
{
    t::LinkedList< int > list;

    list.pushBack( 3 );
    list.pushFront( 2 );
    list.pushFront( 1 );
    list.pushBack( 4 );
    list.pushBack( 5 );

    list.remove( 5 );

    auto it = list.find( 4 );

    *it = 45;

    list.remove( it );

    it = list.find( 2 );

    auto blah = list.back();

    list.pushBack( 69 );

    auto blah2 = list.back();

    return { blah, blah2 };
}

#include "Tree.h"

constexpr int testTree()
{
    t::Tree< int > tree;

    tree.insert( 5 );
    tree.insert( 15 );
    tree.insert( 0 );
    tree.insert( -1 );
    tree.insert( 25 );

    if ( tree.size() != 5 )
        throw std::runtime_error("asgl ");

    auto ptr = tree.find( 25 );

    tree.remove( 5 );

    if ( tree.size() != 4 )
        throw std::runtime_error("sljfnsf");

    tree.insert( 105 );

    if ( tree.size() != 5 )
        throw std::runtime_error( "aslnv" );

    tree.insert( 45 );
    tree.insert( 69 );
    tree.insert( 43 );

    tree.remove( 45 );
    tree.remove( 105 );

    tree.insert( 26 );
    tree.insert( 27 );
    tree.insert( -69 );

    if ( ptr == nullptr )
        throw std::runtime_error("kfmgsfv");

    constexpr t::Array< int, 9 > arr = { -69, -1, 0, 15, 25, 26, 27, 43, 69 };

    auto data = arr.data();

    for ( auto it = tree.begin(); it != tree.end(); ++it, ++data )
    {
        if ( data >= arr.data() + arr.size() )
            throw std::runtime_error("ofgnsf");
        if ( *data != *it )
            throw std::runtime_error("expected to be in order!");
    }

    return *ptr;
}

int main()
{
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

    printSizeOf< String >();
    printSizeOf< t::GenericString< char, uint32 > >();

    constexpr auto a = TestSharedPtr();    (void) a;
    constexpr auto b = TestUniquePtr();    (void) b;
    constexpr auto c = TestImmSharedPtr(); (void) c;
    constexpr auto d = TestFastString();   (void) d;
    constexpr auto e = TestLinkedList();   (void) e;

    std::cout << "---------------\n";

    constexpr int aoasnf = testTree(); (void) aoasnf;

    auto ser = int64( 0 );
    auto deser = int64( 0 );

    constexpr auto NumLoops = 10000;

    for ( auto i = 0; i < NumLoops; ++i )
    {
        auto [ _ser, _deser ] = testTvm();
        ser += _ser;
        deser += _deser;
    }

    std::cout << "Serialization took an average of " << ser / NumLoops << "uS\n";
    std::cout << "Deserialization took an average of " << deser / NumLoops << "uS\n";

    {
        constexpr int16 val = int16(0xf00d);
        constexpr auto val2 = t::byteswap( val );
        static_assert( val2 == 0x0df0 );
        static_assert( t::byteswap( val2 ) == val );

        constexpr int64 _val = int64(0xf00ddeadf00ddead);
        constexpr auto _val2 = t::byteswap( _val );
        static_assert( _val2 == int64(0xadde0df0adde0df0) );
        static_assert( t::byteswap( _val2 ) == _val );
    }

    std::cout << "---------------------------\n";
    
    std::cout << (int) std::endian::native << '\n';
    std::cout << "Little: " << (int) std::endian::little << '\n';
    std::cout << "Big:    " << (int) std::endian::big << '\n';

    return 0;
}
