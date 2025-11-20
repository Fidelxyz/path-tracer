cmake_minimum_required(VERSION 3.20)
project(showcase LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

add_subdirectory(3rdparty/eigen)
add_subdirectory(3rdparty/libigl)

# Executable target
add_executable(
  showcase
  src/bvh/AABB.cpp
  src/bvh/AABBTree.cpp
  src/lights/DirectionalLight.cpp
  src/lights/Light.cpp
  src/lights/PointLight.cpp
  src/objects/Object.cpp
  src/objects/Sphere.cpp
  src/objects/Triangle.cpp
  src/utils/ProgressBar.cpp
  src/utils/Timer.cpp
  src/utils/write_ppm.cpp
  src/blinn_phong_shading.cpp
  src/main.cpp
  src/ray_color.cpp
  src/read_json.cpp
  src/viewing_ray.cpp)

# Make third-party as SYSTEM to suppress warnings from them
target_include_directories(showcase SYSTEM PRIVATE 3rdparty/include)

target_link_libraries(showcase PRIVATE OpenMP::OpenMP_CXX Eigen3::Eigen
                                       igl::core)
target_compile_options(showcase PRIVATE ${OPENMP_CXX_FLAGS})

if(MSVC)
  target_compile_options(showcase PRIVATE /W4 /permissive-)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(showcase PRIVATE /Od /Zi)
  else()
    target_compile_options(showcase PRIVATE /O2 /GL)
    target_link_options(showcase PRIVATE /LTCG)
  endif()
else()
  target_compile_options(showcase PRIVATE -Wall -Wextra -Wpedantic)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(showcase PRIVATE -O0 -g)
  else()
    target_compile_options(showcase PRIVATE -O3 -flto)
    target_link_options(showcase PRIVATE -flto)
  endif()
endif()
