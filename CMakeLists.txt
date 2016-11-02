cmake_minimum_required( VERSION 2.8 )

#Project identification
set( LIB_NAME "cpplogo" )
project( ${LIB_NAME} CXX )

#Project options
option( CPPLOGO_BUILD_EXAMPLES "Build example executable" FALSE )
option( CPPLOGO_ENABLE_LOGGING "Enable logging library" FALSE )

#Find sources
file( GLOB_RECURSE LIB_SOURCES "src/*.cc" )

#Includes
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

#Build library
add_library( ${LIB_NAME} ${LIB_SOURCES} )

#Find other libraries to use
#Boost - for logging
if( CPPLOGO_ENABLE_LOGGING )
  set( Boost_USE_STATIC_LIBS ON )
  set( Boost_USE_MULTITHREADED ON )
  set( Boost_USE_STATIC_RUNTIME OFF )
  add_definitions( -D_CPPLOGO_ENABLE_LOGGING )
  find_package( Boost 1.61.0 COMPONENTS log REQUIRED )

  include_directories( SYSTEM ${Boost_INCLUDE_DIR} )
  target_link_libraries( ${LIB_NAME} ${Boost_LIBRARIES} pthread )
endif()

#Compilation settings
set( CMAKE_CXX_FLAGS "-std=c++14 -Wall -Weffc++" )
set( CMAKE_CXX_FLAGS_RELEASE "-O3" )
set( CMAKE_CXX_FLAGS_DEBUG "-g" )

#Examples
if( CPPLOGO_BUILD_EXAMPLES )
#Set output location - otherwise the test executable will
#show up in build/driver
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
  add_subdirectory( examples )
endif()
