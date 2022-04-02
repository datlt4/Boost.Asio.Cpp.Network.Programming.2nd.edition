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

# Add sources
set(APP concurrent_app)
add_executable(${APP} concurrent.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP unblocked_app)
add_executable(${APP} unblocked.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP blocked_app)
add_executable(${APP} blocked.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP poll_app)
add_executable(${APP} poll.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP pollwork_app)
add_executable(${APP} pollwork.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP removework_app)
add_executable(${APP} removework.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP multithreads_app)
add_executable(${APP} multithreads.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP calledbind_app)
add_executable(${APP} calledbind.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP ioservicebind_app)
add_executable(${APP} ioservicebind.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP mutexbind_app)
add_executable(${APP} mutexbind.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP post_app)
add_executable(${APP} post.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP dispatch_app)
add_executable(${APP} dispatch.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)