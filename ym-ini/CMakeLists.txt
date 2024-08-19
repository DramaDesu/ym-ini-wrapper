cmake_minimum_required (VERSION 3.8)

project ("ym-ini-wrapper")

option(BUILD_TESTS "Build tests" ON)

add_subdirectory(lib)

if (BUILD_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()