cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# ---- Options ----

option(ENABLE_TEST_COVERAGE "Enable test coverage" OFF)

# ---- Dependencies ----

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  find_package(LarsEvent REQUIRED)
endif()

include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.5.0
)

FetchContent_MakeAvailable(Catch2)

# ---- Create binary ----

file(GLOB tests_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(lars-event-tests ${tests_sources})
target_link_libraries(lars-event-tests Catch2 LarsEvent)
set_target_properties(lars-event-tests PROPERTIES CXX_STANDARD 17 COMPILE_FLAGS "-Wall -pedantic -Wextra")

if (NOT ${ENABLE_TEST_COVERAGE}) # clang warns about unused coverage flag
  set_target_properties(lars-event-tests PROPERTIES COMPILE_FLAGS "-Werror")
endif()


# ---- Add tests ----

ENABLE_TESTING() 
ADD_TEST(lars-event-tests lars-event-tests)
# ---- code coverage ----

if (${ENABLE_TEST_COVERAGE})

  FetchContent_Declare(
    extra-cmake-modules
    GIT_REPOSITORY https://github.com/bilke/cmake-modules
    GIT_TAG        5893e3eb3aaec104f86ba81ee90b7e9279b74c3f
  )

  if(NOT extra-cmake-modules_POPULATED)
    FetchContent_Populate(extra-cmake-modules)
    include(${extra-cmake-modules_SOURCE_DIR}/CodeCoverage.cmake)
  endif()
  
  APPEND_COVERAGE_COMPILER_FLAGS()

  set(COVERAGE_GCOVR_EXCLUDES ${Catch2_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR})

  SETUP_TARGET_FOR_COVERAGE_GCOVR_HTML(
    NAME lars-event-tests-coverage
    EXECUTABLE lars-event-tests
    DEPENDENCIES lars-event-tests
  )
endif()
