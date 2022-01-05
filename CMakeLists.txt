cmake_minimum_required(VERSION 3.0.0)
set(TARGET_NAME Algos)

project(${TARGET_NAME} VERSION 0.0.1)
set(CMAKE_CXX_STANDARD 20)

FILE(GLOB src src/*.cpp src/*.cc )
FILE(GLOB includes include/*.h)

add_executable(${TARGET_NAME} ${src} ${includes})
target_include_directories(${TARGET_NAME} PUBLIC ${includes} )

set(ZeroMQ_DIR ${CMAKE_SOURCE_DIR}/lib/libzmq/cmake)
set(cppzmq_DIR ${CMAKE_SOURCE_DIR}/lib/cppzmq/share/cmake/cppzmq)
find_package(cppzmq)
target_link_libraries(${TARGET_NAME} PUBLIC cppzmq-static)
target_include_directories(${TARGET_NAME} PUBLIC ${cppzmq_INCLUDE_DIRS} )

set(eigen ${CMAKE_SOURCE_DIR}/../../Libraries/eigen)
target_include_directories(${TARGET_NAME} PUBLIC ${eigen} )

set(BOOST_ROOT $ENV{BOOST_ROOT})
find_package(BOOST 1.75.0 REQUIRED COMPONENTS container json)
target_include_directories(${TARGET_NAME} PUBLIC ${Boost_INCLUDE_DIRS} )
target_link_libraries(${TARGET_NAME} PUBLIC ${Boost_LIBRARIES})


