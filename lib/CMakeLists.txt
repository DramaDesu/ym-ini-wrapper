cmake_minimum_required (VERSION 3.8)

project ("ym_ini")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_path(SIMPLEINI_INCLUDE_DIRS "ConvertUTF.c")

if (NOT SIMPLEINI_INCLUDE_DIRS)
    message(FATAL_ERROR "simpleini include directory not found!")
endif()

add_library(ym_ini src/ym_ini.cpp)

target_include_directories(ym_ini PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(ym_ini PRIVATE ${SIMPLEINI_INCLUDE_DIRS})
target_include_directories(ym_ini INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

include(GNUInstallDirs)

install(TARGETS ym_ini
    EXPORT ym_ini_targets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

install(EXPORT ym_ini_targets
    FILE ym_ini-targets.cmake
    NAMESPACE ym_ini::
    DESTINATION share/ym_ini
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(CMakePackageConfigHelpers)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ym_ini-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ym_ini-config.cmake"
    @ONLY
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ym_ini-config.cmake"
    DESTINATION share/ym_ini
)