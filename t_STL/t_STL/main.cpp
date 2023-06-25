#include <iostream>
#include <chrono>

#include "t.h"
#include "variant/serialize/Serialize.h"
#include "Timer.h"

template< typename T >
void printSizeOf()
{
    std::cout << sizeof( T ) << " bytes\n";
}

int main()
{
    using t::v::Map;

    int64_t time_v1 = 0;
    int64_t time_v2 = 0;

    size_t constexpr NumOfLoops = 10000;

    for ( size_t i = 0; i < NumOfLoops; ++i )
    {
        Map map;

        using std::chrono::microseconds;

        Timer< microseconds > t;

        t::v::v1::Map map_v1;

        // t.start();

        // map_v1["hello"] = "hello";
        // map_v1["hi"] = "hi";
        // map_v1["uint8_1"] = uint8_t( 1 );
        // map_v1["uint8_2"] = uint8_t( 2 );
        // map_v1["heyyo"] = 0.77f;
        // map_v1["vector"] = t::Vector{ 1, 2, 3, 4, 5, 6, 7 };
        // map_v1["map"] = map_v1;

        // auto float_value_v1 = map_v1["heyyo"].As< float >();
        // auto& string_value_v1 = map_v1["hello"].As< t::String& >();
        // auto& vec_v1 = map_v1["vector"].As< t::Vector< int32_t >& >();
        // vec_v1[ 0 ] += 1;

        // auto copy_v1 { map_v1 };

        // bool equal = copy_v1 == map_v1;

        // auto const temp_v1 = t.stop();
        // time_v1 += temp_v1;

        // std::cout << std::boolalpha << equal << '\n';

        t.start();

        map["hello"] = "hello";
        map["hi"] = "hi";
        map["uint8_1"] = uint8_t( 1 );
        map["uint8_2"] = uint8_t( 2 );
        map["heyyo"] = 0.77f;
        map["vector"] = t::Vector< int8_t >{ 1, 2, 3, 4, 5, 6, 7 };
        map["map"] = map;

        auto float_value = map["heyyo"].As< float >();
        auto& string_value = map["hello"].As< t::String& >();
        auto& vec_v2 = map["vector"].As< t::Vector< int32_t >& >();
        vec_v2[ 0 ] += 1;

        auto copy_v2 { map };

        bool equal_v2 = copy_v2 == map;

        auto const temp_v2 = t.stop();

        //std::cout << std::boolalpha << equal_v2 << '\n';

        time_v2 += temp_v2;
    }

    /*std::cout << "V1: ";
    std::cout << time_v1 / NumOfLoops << '\n';*/
    std::cout << "V2: ";
    std::cout << time_v2 / NumOfLoops << '\n';

    return 0;
}