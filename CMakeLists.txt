cmake_minimum_required(VERSION 3.25)
project(visual_integrator VERSION 0.1.0 LANGUAGES CXX)

option(VISUAL_INTEGRATOR_BUILD_TESTS "Build unit tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Eigen3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_executable(visual_integrator src/main.cpp)
target_link_libraries(visual_integrator PRIVATE
  Eigen3::Eigen
  glad::glad
  glfw
  glm::glm
  imgui::imgui)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(visual_integrator PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(cmake/ClangFormat.cmake)

if(VISUAL_INTEGRATOR_BUILD_TESTS)
  enable_testing()
  find_package(Catch2 3 CONFIG REQUIRED)
  add_executable(unit_tests tests/smoke_test.cpp)
  target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)
  include(Catch)
  catch_discover_tests(unit_tests)
endif()
