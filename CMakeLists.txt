project(boxplasmazoomscroll)
cmake_minimum_required(VERSION 3.0.0)

include(FindPkgConfig)

if(WITH_DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3 -O0")
endif()

find_package(X11)
pkg_check_modules(dbus REQUIRED dbus-1)

add_executable(
    boxplasmazoomscroll
        src/boxplasmazoomscroll.c
)

target_include_directories(
    boxplasmazoomscroll
    PUBLIC
        ${dbus_INCLUDE_DIRS}
)

target_link_libraries(
    boxplasmazoomscroll
        dbus-1
        X11
)

install(
    TARGETS
        boxplasmazoomscroll
    DESTINATION
        bin
)
