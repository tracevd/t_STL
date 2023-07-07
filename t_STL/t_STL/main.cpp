#include <iostream>
#include <chrono>

#include "t.h"
//#include "variant/serialize/Serialize.h"
#include "Timer.h"

template< typename T >
void printSizeOf()
{
    std::cout << typeid( T ).name() << ": ";
    std::cout << sizeof( T ) << " bytes\n";
}

constexpr uint8_t MAX_DEPTH = 8;

void testDepth( t::v::Map const& map, uint8_t depth );

void testDepth( t::v::Value const& val, uint8_t depth )
{
    if ( depth == MAX_DEPTH )
        throw std::runtime_error("Bad things happened!");
    if ( val.Is< t::v::Map >() )
    {
        testDepth( val.As< t::v::Map const& >(), depth + 1 );
    }
    else if ( val.Is< t::Vector< t::v::Map > >() )
    {

    }
}

void testDepth( t::v::Map const& map, uint8_t depth )
{
    if ( depth == MAX_DEPTH )
        throw std::runtime_error("Bad things happened!");
    for ( const auto& [ key, value ] : map )
    {
        testDepth( value, depth + 1 );
    }
}

int main()
{
    using t::v::Map;

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

        using std::chrono::microseconds;

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

        using std::chrono::microseconds;

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

    return 0;
}