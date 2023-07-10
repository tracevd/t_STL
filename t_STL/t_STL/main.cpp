#include <iostream>
#include <chrono>

#include "t.h"
#include "variant/serialize/Serialize.h"
#include "Timer.h"

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

int main()
{
#if 0
    printSizeOf< t::HashTable< t::String, t::v::Value > >();
    printSizeOf< t::HashMap< t::String, t::v::Value > >();

    auto x = t::HashTable< t::String, t::v::Value >();

    x["hey"] = "woah";

    x["wah"] = uint8_t( 123 );

    x["double_vector"] = t::Vector< double >{ 1, 2.3, 3 };

    std::cout << std::boolalpha << x.containsKey( "hey" ) << '\n';

    std::cout << std::boolalpha << x.containsKey( "wah" ) << '\n';

    std::cout << std::boolalpha << x.containsKey( "double_vector" ) << '\n';

    x.remove("double_vector");
    std::cout << "Removed key\n";
    std::cout << std::boolalpha << x.containsKey( "double_vector" ) << '\n';

    std::cout << x.at("hey").As< t::String const& >() << '\n';

    int64_t time_v1 = 0;
    int64_t time_v2 = 0;

    size_t constexpr NumOfLoops = 100;

    for ( size_t i = 0; i < NumOfLoops; ++i )
    {
        Map map;

        Timer< microseconds > t;

        t.start();

        map["hello__"] = int8_t( 1 );

        map["hello"] = "hello";
        map["hi"]    = "hi";
        map["uint8_1"] = uint8_t( 1 );
        map["uint8_2"] = uint8_t( 2 );
        map["heyyo"] = 0.77f;
        map["vector"] = t::Vector< int8_t >{ 1, 2, 3, 4, 5, 6, 7 };

        map["hello_"] = "blahblah";

        auto float_value = map["heyyo"].As< float >();
        auto& string_value = map["hello"].As< t::String const& >();
        auto& vec_v2 = map["vector"].As< t::Vector< int8_t >& >();
        vec_v2[ 0 ] += 1;

        auto const temp_v2 = t.stop();

        time_v2 += temp_v2;
    }

    for ( size_t i = 0; i < NumOfLoops; ++i )
    {
        t::HashTable< t::String, t::v::Value > map;

        Timer< microseconds > t;

        t.start();

        map["hello__"] = int8_t( 1 );

        map["hello"] = "hello";
        map["hi"] = "hi";
        map["uint8_1"] = uint8_t( 1 );
        map["uint8_2"] = uint8_t( 2 );
        map["heyyo"] = 0.77f;
        map["vector"] = t::Vector< int8_t >{ 1, 2, 3, 4, 5, 6, 7 };

        map["hello_"] = "blahblah";

        auto float_value = map["heyyo"].As< float >();
        auto& string_value = map["hello"].As< t::String const& >();
        auto& vec_v2 = map["vector"].As< t::Vector< int8_t >& >();
        vec_v2[0] += 1;

        auto const temp_v2 = t.stop();

        time_v1 += temp_v2;
    }
    
    std::cout << "V2: ";
    std::cout << time_v2 / NumOfLoops << '\n';

    std::cout << "V1: ";
    std::cout << time_v1 / NumOfLoops << '\n';

    printSizeOf< t::v::Value >();

    std::cout << t::v::templateToString< t::Vector< t::String > >() << '\n';
#endif

    printSizeOf< std::string >();
    printSizeOf< String >();

    Map vm;

    vm["string"]    = "hello";
    vm["uint8"]     = uint8_t( 1 );
    vm["uint16"]    = uint16_t( 2 );
    vm["uint32"]    = uint32_t( 3 );
    vm["uint64"]    = uint64_t( 4 );
    vm["int8"]      = int8_t( 5 );
    vm["int16"]     = int16_t( 6 );
    vm["int32"]     = int32_t( 7 );
    vm["int64"]     = int64_t( 8 );
    vm["float"]     = float( 9 );
    vm["double"]    = double( 10 );
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
    map.insert( { String("test"), Value("value") } );
    map.insert( { String("test2"), Value("value2") } );
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

    std::cout << "Maps are equal: " << std::boolalpha << ( vm_2 == vm ) << '\n';

    printSizeOf< fString >();

    fString x;

    std::string_view xy = "hello";

    x.reserve( 70 );

    x += "This is a really cool sentence.";

    x += '\n';

    x += "This is another cool sentence.";

    constexpr t::StringView s ("hello");

    t::StringView str = x;

    std::cout << str << '\n';

    fString str1 = "hello";

    hashAndPrint( str1 );

    fString str2 = "blah blah";

    hashAndPrint( str2 );

    fString str_ = "bleh bleh";

    hashAndPrint( str_ );

    fString str3 = "ayyo whatup broski";

    hashAndPrint( str3 );

    fString str4 = "Image_Data";

    hashAndPrint( str4 );

    fString str5 = "Image_data";

    hashAndPrint( str5 );

    fString str6 = "hi";

    hashAndPrint( str6 );

    t::HashTable< fString, Value > m;

    t.start();

    auto teststr = fString( std::numeric_limits< uint64_t >::max() );

    std::cout << t.stop() << '\n';

    std::cout << "ToString: " << teststr << '\n';
    
    t.start();

    auto teststr2 = fString( std::numeric_limits< uint64_t >::min() );

    std::cout << t.stop() << '\n';

    std::cout << "ToString: " << teststr2 << '\n';

    return 0;
}