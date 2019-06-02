cmake_minimum_required (VERSION 3.14)

project(FastBelt)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
# this is where `brew install boost-mpi` installs things, withoit this line, there will be something likes `fatal error: 'boost/mpi.hpp' file not found`
include_directories("/usr/local/include")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdParty)

# file(GLOB SRC 
#   "*cpp*"
# )
set(SRC "trympi.cpp")

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

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(
  ${PROJECT_NAME}
  ${OPENGL_gl_LIBRARY}
  ${GLEW_LIBRARIES}
  glfw
)

OPTION (USE_OpenMP "Use OpenMP to enamble <omp.h>" ON)

# Find OpenMP
if(APPLE AND USE_OpenMP)
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        set(OpenMP_C "${CMAKE_C_COMPILER}")
        set(OpenMP_C_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument")
        set(OpenMP_C_LIB_NAMES "libomp" "libgomp" "libiomp5")
        set(OpenMP_libomp_LIBRARY ${OpenMP_C_LIB_NAMES})
        set(OpenMP_libgomp_LIBRARY ${OpenMP_C_LIB_NAMES})
        set(OpenMP_libiomp5_LIBRARY ${OpenMP_C_LIB_NAMES})
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
      set(OpenMP_CXX_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument")
      set(OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
      set(OpenMP_libomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
      set(OpenMP_libgomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
      set(OpenMP_libiomp5_LIBRARY ${OpenMP_CXX_LIB_NAMES})
    endif()
endif()

if(USE_OpenMP)
  find_package(OpenMP REQUIRED)
endif(USE_OpenMP)

if (OPENMP_FOUND)
    include_directories("/usr/local/Cellar/libomp/8.0.0/include")
    link_directories(${OPENMP_LIBRARIES})
#     set (CMAKE_C_FLAGS ${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS})
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -openmp -std=c++17")
#     # set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif(OPENMP_FOUND)
