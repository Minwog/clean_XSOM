cmake_minimum_required(VERSION 3.5)
project(2SOM)

find_package(PkgConfig)

pkg_check_modules(XSOM xsom REQUIRED)



add_executable(
    twosomcolors
    2SOM_colors/2D_state.cpp
    2SOM_colors/Display2d.cpp
    2SOM_colors/main_colors2d.cpp
)

target_link_libraries (twosomcolors ${XSOM_LIBRARIES})
target_compile_options(twosomcolors PUBLIC ${XSOM_CFLAGS_OTHER})
target_include_directories(twosomcolors PUBLIC ${XSOM_INCLUDE_DIRS})
