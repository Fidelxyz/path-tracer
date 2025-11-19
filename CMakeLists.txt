cmake_minimum_required(VERSION 3.20)
project(showcase LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

# Executable target
add_executable(
  showcase
  src/utils/ProgressBar.cpp
  src/utils/Timer.cpp
  src/main.cpp
  src/blinn_phong_shading.cpp
  src/DirectionalLight.cpp
  src/first_hit.cpp
  src/main.cpp
  src/Plane.cpp
  src/PointLight.cpp
  src/raycolor.cpp
  src/read_json.cpp
  src/Sphere.cpp
  src/Triangle.cpp
  src/TriangleSoup.cpp
  src/viewing_ray.cpp
  src/write_ppm.cpp)

# Make third-party as SYSTEM to suppress warnings from them
target_include_directories(showcase SYSTEM PRIVATE 3rdparty/include eigen)

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
