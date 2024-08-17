cmake_minimum_required (VERSION 3.8)

project ("ym-ini-wrapper-lib")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_path(SIMPLEINI_INCLUDE_DIRS "ConvertUTF.c")

add_library(ym-ini "src/ym_ini.cpp")

target_include_directories(ym-ini PRIVATE .)
target_include_directories(ym-ini PRIVATE ${SIMPLEINI_INCLUDE_DIRS})
target_include_directories(ym-ini INTERFACE ./include)

include(GNUInstallDirs)

install(TARGETS ym-ini
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)