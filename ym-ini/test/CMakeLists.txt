cmake_minimum_required (VERSION 3.8)

project ("ym-ini-wrapper-test")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ym-ini-test "ym_ini_test.cpp")

target_link_libraries(ym-ini-test PRIVATE ym-ini)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Data.ini ${CMAKE_CURRENT_BINARY_DIR}/Data.ini COPYONLY)