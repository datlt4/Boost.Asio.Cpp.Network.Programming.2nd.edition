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
set(APP nonstrand_app)
add_executable(${APP} nonstrand.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP strand_app)
add_executable(${APP} strand.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP strandwrap_app)
add_executable(${APP} strandwrap.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP exception_app)
add_executable(${APP} exception.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP exception2_app)
add_executable(${APP} exception2.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP errorcode_app)
add_executable(${APP} errorcode.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP errorcode2_app)
add_executable(${APP} errorcode2.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP timer_app)
add_executable(${APP} timer.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP timer2_app)
add_executable(${APP} timer2.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)

set(APP timer3_app)
add_executable(${APP} timer3.cpp)
target_include_directories(${APP} PRIVATE ${INC})
target_link_libraries(${APP} PRIVATE ${LIB})
unset(APP)