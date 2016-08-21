cmake_minimum_required(VERSION 3.1)

FILE(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/include/lars/*.h")

get_directory_property(has_parent PARENT_DIRECTORY)

IF(has_parent)
  SET(lars_headers ${lars_headers} ${headers} PARENT_SCOPE)
  SET(lars_include_dirs ${lars_include_dirs} "${CMAKE_CURRENT_SOURCE_DIR}/include" PARENT_SCOPE)
ELSE()
  MESSAGE(WARNING "building outside of parent context. Required headers might be missing.")
ENDIF()

find_package (Threads)

IF(LARS_BUILD_EXAMPLES)
  SET(lars_headers ${lars_headers} ${headers})
  SET(lars_include_dirs ${lars_include_dirs} "${CMAKE_CURRENT_SOURCE_DIR}/include")

  include_directories(${lars_include_dirs})
  set(CMAKE_CXX_STANDARD 11)

  file( GLOB example_sources examples/*.cpp )
  foreach( examplesourcefile ${example_sources} )
    get_filename_component(filename ${examplesourcefile} NAME)
    string( REPLACE ".cpp" "" examplename ${filename} )
    add_executable( ${examplename} ${examplesourcefile} ${headers})
    target_link_libraries(${examplename}  ${CMAKE_THREAD_LIBS_INIT})
  endforeach( examplesourcefile ${test_sources} )
ENDIF()
