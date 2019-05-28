cmake_minimum_required (VERSION 3.10)

project(FastBelt)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
# this is where `brew install boost-mpi` installs things, withoit this line, there will be something likes `fatal error: 'boost/mpi.hpp' file not found`
include_directories("/usr/local/include")

file(GLOB SRC 
  "*cpp*"
)
add_executable(${PROJECT_NAME} ${SRC})

# I don't know what these means
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# Autamatically detece boost install location after you run `brew install boost-mpi`, if failed, just `brew uninstall boost-mpi && install boost && brew install boost-mpi` then it will find it again.
find_package(Boost REQUIRED COMPONENTS mpi serialization)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

# Find the OpenMPI location, `brew install boost-mpi` will automatically install OpenMPI
find_package(MPI REQUIRED)
include_directories(${MPI_CXX_INCLUDE_PATH})
target_link_libraries(${PROJECT_NAME} ${MPI_CXX_LIBRARIES})
