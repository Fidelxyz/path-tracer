cmake_minimum_required(VERSION 3.20)
project(path_tracer LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

add_subdirectory(3rdparty/eigen)
add_subdirectory(3rdparty/libigl)
add_subdirectory(3rdparty/tinyobjloader)

# Executable target
add_executable(
  path_tracer
  src/bvh/AABB.cpp
  src/bvh/AABBTree.cpp
  src/bvh/build_bvh.cpp
  src/light/DirectionalLight.cpp
  src/light/PointLight.cpp
  src/reader/read_json.cpp
  src/reader/read_obj.cpp
  src/geometry/Sphere.cpp
  src/geometry/Triangle.cpp
  src/util/ProgressBar.cpp
  src/util/random.cpp
  src/util/Timer.cpp
  src/brdf.cpp
  src/Camera.cpp
  src/main.cpp
  src/path_tracing.cpp
  src/render.cpp
  src/Scene.cpp)

# Make third-party as SYSTEM to suppress warnings from them
target_include_directories(path_tracer SYSTEM PRIVATE 3rdparty/include)

target_link_libraries(path_tracer PRIVATE OpenMP::OpenMP_CXX Eigen3::Eigen
                                          igl::core tinyobjloader)
target_compile_options(path_tracer PRIVATE ${OPENMP_CXX_FLAGS})

if(MSVC)
  target_compile_options(path_tracer PRIVATE /W4 /permissive-)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(path_tracer PRIVATE /Od /Zi)
  else()
    target_compile_options(path_tracer PRIVATE /O2 /GL)
    target_link_options(path_tracer PRIVATE /LTCG)
  endif()
else()
  target_compile_options(path_tracer PRIVATE -Wall -Wextra -Wpedantic
                                             -Wno-psabi)
  target_link_options(path_tracer PRIVATE -Wno-psabi)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(path_tracer PRIVATE -O0 -g)
  else()
    target_compile_options(path_tracer PRIVATE -O3 -flto -ffast-math
                                               -fno-finite-math-only)
    target_link_options(path_tracer PRIVATE -flto)
  endif()
endif()
