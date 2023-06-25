#include <iostream>
#include <chrono>

#include "t.h"
//#include "variant/serialize/Serialize.h"
#include "Timer.h"

template< typename T >
void printSizeOf()
{
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

    int64_t time_v1 = 0;
    int64_t time_v2 = 0;

    size_t constexpr NumOfLoops = 1;

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
        map["map"] = map;
        /*map.insert( {"map_copy", map["map"] } );
        map["nested"] = Map();
        map["nested"].As< Map& >()["test"] = map;*/

        //auto val_ = t::v::Value( std::forward< t::Vector< int > >( t::Vector< int >{ 1, 2, 3, 4 } ) );

        //testDepth( map, 0 );

        auto float_value = map["heyyo"].As< float >();
        auto& string_value = map["hello"].As< t::String const& >();
        auto& vec_v2 = map["vector"].As< t::Vector< int8_t >& >();
        vec_v2[ 0 ] += 1;

        //auto copy_v2 { map };

        //bool equal_v2 = copy_v2 == map;

        auto const temp_v2 = t.stop();

        //std::cout << std::boolalpha << equal_v2 << '\n';

        time_v2 += temp_v2;
    }
    
    std::cout << "V2: ";
    std::cout << time_v2 / NumOfLoops << '\n';

    printSizeOf< t::v::Value >();

    std::cout << t::v::templateToString< t::Vector< t::String > >() << '\n';

    return 0;
}