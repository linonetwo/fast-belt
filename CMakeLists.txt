cmake_minimum_required (VERSION 3.10)

project(FastBelt)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
include_directories("/usr/local/include")

file(GLOB SRC 
  "*cpp*"
)
add_executable(${PROJECT_NAME} ${SRC})

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost COMPONENTS mpi serialization REQUIRED)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

find_package(MPI REQUIRED)
include_directories(${MPI_CXX_INCLUDE_PATH})
target_link_libraries(${PROJECT_NAME} ${MPI_CXX_LIBRARIES})
