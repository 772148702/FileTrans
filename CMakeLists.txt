cmake_minimum_required(VERSION 3.2)
project(handy)

set(CMAKE_CXX_STANDARD 11)
set(BUILD_HANDY_EXAMPLES TURE)
include(GNUInstallDirs)

find_package(Threads REQUIRED)

list(APPEND HANDY_SRCS
        ${PROJECT_SOURCE_DIR}/handy/conn.cpp
        ${PROJECT_SOURCE_DIR}/handy/net.cpp
        ${PROJECT_SOURCE_DIR}/handy/buffer.cpp
        ${PROJECT_SOURCE_DIR}/handy/logging.cpp
        ${PROJECT_SOURCE_DIR}/handy/port_posix.cpp
        ${PROJECT_SOURCE_DIR}/handy/util.cpp
        )

if(CMAKE_HOST_APPLE)
    add_definitions(-DOS_MACOSX)
elseif(CMAKE_HOST_UNIX)
    add_definitions(-DOS_LINUX)
    #else(CMAKE_HOST_APPLE)
    #    message(FATAL_ERROR "Platform not supported")
endif(CMAKE_HOST_APPLE)

option(BUILD_HANDY_SHARED_LIBRARY "Build Handy Shared Library" OFF)
option(BUILD_HANDY_STATIC_LIBRARY "Build Handy Shared Library" ON)
option(BUILD_HANDY_EXAMPLES "Build Handy Examples" OFF)
option(BUILD_HANDY_TEST "Build Handy TEST" ON)
##Handy Shared Library
if(BUILD_HANDY_SHARED_LIBRARY)
    add_library(handy SHARED ${HANDY_SRCS})
    target_include_directories(handy PUBLIC ${PROJECT_SOURCE_DIR}/handy)
    target_link_libraries(handy Threads::Threads)
    install(TARGETS handy DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif(BUILD_HANDY_SHARED_LIBRARY)

#Handy Static library
if(BUILD_HANDY_STATIC_LIBRARY)
    add_library(handy_s STATIC ${HANDY_SRCS})
    target_include_directories(handy_s PUBLIC ${PROJECT_SOURCE_DIR}/handy/)
    target_link_libraries(handy_s Threads::Threads)
    install(TARGETS handy_s DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif(BUILD_HANDY_STATIC_LIBRARY)

if(BUILD_HANDY_SHARED_LIBRARY OR BUILD_HANDY_STATIC_LIBRARY)
    install(FILES
            ${PROJECT_SOURCE_DIR}/handy/conn.cpp
            ${PROJECT_SOURCE_DIR}/handy/net.cpp
            ${PROJECT_SOURCE_DIR}/handy/buffer.cpp
            ${PROJECT_SOURCE_DIR}/handy/logging.cpp
            ${PROJECT_SOURCE_DIR}/handy/port_posix.cpp
            ${PROJECT_SOURCE_DIR}/handy/util.cpp
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/handy)
endif(BUILD_HANDY_SHARED_LIBRARY OR BUILD_HANDY_STATIC_LIBRARY)


function(add_handy_executable EXECUTABLE_NAME EXECUTABLE_SOURCES)
    add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_SOURCES})
    target_link_libraries(${EXECUTABLE_NAME} handy_s)
    target_include_directories(${EXECUTABLE_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
    install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
endfunction(add_handy_executable)

function(add_hand_test_executable EXECUTABLE_NAME EXECUTABLE_SOURCES)
    add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_SOURCES} test/ut.cc  test/test_harness.h test/test_harness.cc)
    target_link_libraries(${EXECUTABLE_NAME} handy_s)
    target_include_directories(${EXECUTABLE_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
    install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
endfunction()


if(BUILD_HANDY_TEST)

    add_hand_test_executable(test_port test/test_port.cc)
    add_hand_test_executable(test_buffer test/test_buffer.cc)
endif(BUILD_HANDY_TEST)

add_handy_executable(epoll raw-examples/epoll.cpp)

if(BUILD_HANDY_EXAMPLES)
endif(BUILD_HANDY_EXAMPLES)


