cmake_minimum_required(VERSION 3.5)
project(2SOM)

find_package(PkgConfig)

pkg_check_modules(XSOM xsom REQUIRED)



add_executable(
    twosom
    1SOM/State.cpp
    1SOM/Display.cpp
    1SOM/main.cpp
)

target_link_libraries (twosom ${XSOM_LIBRARIES})
target_compile_options(twosom PUBLIC ${XSOM_CFLAGS_OTHER})
target_include_directories(twosom PUBLIC ${XSOM_INCLUDE_DIRS})
