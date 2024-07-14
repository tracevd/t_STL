cmake_minimum_required( VERSION 3.10 )

project( t_STL )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

file( GLOB_RECURSE SRC "*.h" "*.cpp" )

file( GLOB_RECURSE TEST "*/tests/*.cpp" )

list( ADD TEST "t_STL/t_STL/main.cpp" )

list( REMOVE SRC TEST )

add_library( t_STL ${SRC} )

# testing binary
add_executable( cpp_test ${TEST} )
target_link_libraries( cpp_test PRIVATE t_STL )

if ( MSVC )
    target_compile_options( cpp_test PRIVATE /W4 /WX )
else()
    target_compile_options( cpp_test PRIVATE -Wall -Wextra -Wpedantic -Werror )
endif()

# enable testing functionality
enable_testing()

add_test( NAME cpp_test COMMAND "$<TARGET_FILE:cpp_test>" )