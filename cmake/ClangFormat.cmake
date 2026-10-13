find_program(CLANG_FORMAT_EXECUTABLE NAMES clang-format REQUIRED)

file(GLOB_RECURSE FORMAT_SOURCES CONFIGURE_DEPENDS
     "${PROJECT_SOURCE_DIR}/src/*.cpp"
     "${PROJECT_SOURCE_DIR}/src/*.h"
     "${PROJECT_SOURCE_DIR}/include/*.h"
     "${PROJECT_SOURCE_DIR}/tests/*.cpp")

add_custom_target(format
  COMMAND "${CLANG_FORMAT_EXECUTABLE}" -i ${FORMAT_SOURCES}
  COMMENT "Formatting C++ sources with clang-format (Google style)"
  VERBATIM)

add_custom_target(format-check
  COMMAND "${CLANG_FORMAT_EXECUTABLE}" --dry-run --Werror ${FORMAT_SOURCES}
  COMMENT "Checking C++ formatting"
  VERBATIM)
