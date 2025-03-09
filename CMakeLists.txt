cmake_minimum_required(VERSION 3.5.0)
project(boxplasmazoomscroll)

find_package(X11 REQUIRED)
pkg_check_modules(
    dbus
        dbus-1
    REQUIRED
)

if(WITH_DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3 -O0")
endif()

add_executable(
    boxplasmazoomscroll
        src/boxplasmazoomscroll.c
)

target_include_directories(
    boxplasmazoomscroll
    PRIVATE
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
