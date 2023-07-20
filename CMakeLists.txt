cmake_minimum_required( VERSION 3.10 )

project( t_STL )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

file( GLOB_RECURSE "*.h" "*.cpp" )

add_executable( t_STL "t_STL/t_STL/main.cpp" )