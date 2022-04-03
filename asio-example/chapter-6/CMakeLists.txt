cmake_minimum_required(VERSION 3.10.2)

project(network LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# Boost
find_package(Boost 1.77.0 REQUIRED filesystem program_options regex timer chrono)
set(INC ${INC} ${Boost_INCLUDE_DIR})
set(LIB ${LIB} ${Boost_LIBRARIES})
set(LIB ${LIB} -lboost_system -lboost_thread -lboost_filesystem -lpthread)

# Message
message(STATUS "Include directories: ${INC}")
message(STATUS "Source directories:  ${SRC}")
message(STATUS "Libray directories:  ${LIB}")

# Add include directory
set(INC ${INC} ${CMAKE_CURRENT_SOURCE_DIR})

# Add sources
set(APP connectsync_app)
add_executable(${APP} connectsync.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP connectasync_app)
add_executable(${APP} connectasync.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP serverasync_app)
add_executable(${APP} serverasync.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP readwritesocket_app)
add_executable(${APP} readwritesocket.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP echoserver_app)
add_executable(${APP} echoserver.cpp wrapper.cpp)
target_include_directories(${APP} PRIVATE ${INC} )
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP clienthttpget_app)
add_executable(${APP} clienthttpget.cpp wrapper.cpp)
target_include_directories(${APP} PRIVATE ${INC} )
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)
