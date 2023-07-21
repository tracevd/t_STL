cmake_minimum_required( VERSION 3.10 )

project( t_STL )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

file( GLOB_RECURSE SRC "*.h" "*.cpp" )

list( REMOVE_ITEM SRC "*.main.cpp" )

add_library( t_STL ${SRC} )

# testing binary
add_executable( cpp_test "*main.cpp" )
target_link_libraries( cpp_test PRIVATE t_STL )

# enable testing functionality
enable_testing()

add_test( NAME cpp_test COMMAND $<TARGET_FILE:cpp_test> )