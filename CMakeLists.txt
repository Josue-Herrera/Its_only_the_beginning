cmake_minimum_required(VERSION 3.0.0)
set(TARGET_NAME Algos)

project(${TARGET_NAME} VERSION 0.0.1)
set(CMAKE_CXX_STANDARD 20)


FILE(GLOB src src/*.cpp src/*.cc )
FILE(GLOB includes include/*.h)

set(eigen ${CMAKE_SOURCE_DIR}/../../Libraries/eigen)
set(boost ${CMAKE_SOURCE_DIR}/../../Libraries/boost_1_75_0)

set(ZeroMQ_DIR ${CMAKE_SOURCE_DIR}/lib/zmq/CMake)
set(cppzmq_DIR ${CMAKE_SOURCE_DIR}/lib/cppzmq/share/cmake/cppzmq)

find_package(cppzmq)

add_executable(${TARGET_NAME} ${src} ${includes})


target_link_libraries(${TARGET_NAME} PUBLIC cppzmq-static)

target_include_directories(${TARGET_NAME} PUBLIC ${boost} )
target_include_directories(${TARGET_NAME} PUBLIC ${eigen} )
target_include_directories(${TARGET_NAME} PUBLIC ${includes} )