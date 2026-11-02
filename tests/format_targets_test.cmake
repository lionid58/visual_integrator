# Exercise the real build targets in a small project without application dependencies.
file(MAKE_DIRECTORY "${TEST_DIRECTORY}/src")
configure_file("${PROJECT_SOURCE}/.clang-format" "${TEST_DIRECTORY}/.clang-format" COPYONLY)
file(WRITE "${TEST_DIRECTORY}/CMakeLists.txt"
  "cmake_minimum_required(VERSION 3.25)\n"
  "project(format_targets_test NONE)\n"
  "include(\"${PROJECT_SOURCE}/cmake/ClangFormat.cmake\")\n")

function(run_successfully)
  execute_process(COMMAND ${ARGV} RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
  if(NOT result STREQUAL "0")
    message(FATAL_ERROR "Command failed: ${ARGV}\n${output}\n${error}")
  endif()
endfunction()

run_successfully("${CMAKE_COMMAND}" -S "${TEST_DIRECTORY}" -B "${TEST_DIRECTORY}/build"
  -G "${TEST_GENERATOR}" "-DCLANG_FORMAT_EXECUTABLE=${CLANG_FORMAT_EXECUTABLE}")

# Add files after configuration to also check CONFIGURE_DEPENDS discovery.
foreach(directory IN ITEMS src include tests)
  foreach(extension IN ITEMS cpp cc h hh)
    file(MAKE_DIRECTORY "${TEST_DIRECTORY}/${directory}")
    set(fixture "${TEST_DIRECTORY}/${directory}/fixture.${extension}")
    file(WRITE "${fixture}" "int   value( ){return 1;}\n")
    execute_process(COMMAND "${CMAKE_COMMAND}" --build "${TEST_DIRECTORY}/build"
      --target format-check RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
    if(result STREQUAL "0")
      message(FATAL_ERROR "format-check accepted unformatted ${directory}/fixture.${extension}")
    endif()
    run_successfully("${CMAKE_COMMAND}" --build "${TEST_DIRECTORY}/build" --target format)
    file(READ "${fixture}" formatted)
    if(NOT formatted STREQUAL "int value() { return 1; }\n")
      message(FATAL_ERROR "format did not format ${directory}/fixture.${extension}: ${formatted}")
    endif()
    run_successfully("${CMAKE_COMMAND}" --build "${TEST_DIRECTORY}/build" --target format-check)
  endforeach()
endforeach()
