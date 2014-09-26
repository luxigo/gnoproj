cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(gnoproj C CXX)

set(CMAKE_CXX_COMPILER g++)

set(CMAKE_BUILD_TYPE Release)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(warnings "-Wall -Wextra -Werror")
    set(cxx_flags "-std=c++11")
endif()

if (NOT CONFIGURED_ONCE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${warnings} ${cxx_flags}"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${warnings}"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif()

set(GNOPROJ_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

find_package(PkgConfig)

# elphelphg
pkg_check_modules(PC_LIBELPHELPHG elphelphg)
set(LIBELPHELPHG_DEFINITIONS ${PC_LIBELPHELPHG_CFLAGS_OTHER})

find_path(LIBELPHELPHG_INCLUDE_DIR elphelphg/camera.hpp
  HINTS ${PC_LIBELPHELPHG_INCLUDEDIR} ${PC_LIBELPHELPHG_INCLUDE_DIRS}
  PATH_SUFFIXES elphelphg )

find_library(LIBELPHELPHG_LIBRARY NAMES elphelphg libelphelphg
  HINTS ${PC_LIBELPHELPHG_LIBDIR} ${PC_LIBELPHELPHG_LIBRARY_DIRS} )

# opencv
find_package(OpenCV REQUIRED)

# libxml2
pkg_check_modules(PC_LIBXML QUIET libxml-2.0)
set(LIBXML2_DEFINITIONS ${PC_LIBXML_CFLAGS_OTHER})

find_path(LIBXML2_INCLUDE_DIR libxml/xpath.h
          HINTS ${PC_LIBXML_INCLUDEDIR} ${PC_LIBXML_INCLUDE_DIRS}
          PATH_SUFFIXES libxml2 )

find_library(LIBXML2_LIBRARY NAMES xml2 libxml2
             HINTS ${PC_LIBXML_LIBDIR} ${PC_LIBXML_LIBRARY_DIRS} )

# libboost
pkg_check_modules(PC_LIBBOOST QUIET libboost-filesystem)
set(LIBBOOST_DEFINITIONS ${PC_LIBBOOST_CFLAGS_OTHER})

find_path(LIBBOOST_INCLUDE_DIR boost/filesystem.hpp
  HINTS ${PC_LIBBOOST_INCLUDEDIR} ${PC_LIBBOOST_INCLUDE_DIRS}
          PATH_SUFFIXES boost )

find_library(LIBBOOST_FILESYSTEM_LIBRARY NAMES boost_filesystem libboost_filesystem
   HINTS ${PC_LIBBOOST_LIBDIR} ${PC_LIBBOOST_LIBRARY_DIRS} )
find_library(LIBBOOST_SYSTEM_LIBRARY NAMES boost_system libboost_system
   HINTS ${PC_LIBBOOST_LIBDIR} ${PC_LIBBOOST_LIBRARY_DIRS} )
find_library(LIBBOOST_REGEX_LIBRARY NAMES boost_regex libboost_regex
   HINTS ${PC_LIBBOOST_LIBDIR} ${PC_LIBBOOST_LIBRARY_DIRS} )

# libgnomonic
add_custom_target(
  libgnomonic
  COMMAND make
  WORKING_DIRECTORY ${GNOPROJ_SOURCE_DIR}/../lib/libgnomonic
)
set(LIBGNOMONIC_INCLUDE_DIR ${GNOPROJ_SOURCE_DIR}/../lib/libgnomonic/src)
set(LIBGNOMONIC_LIBS ${GNOPROJ_SOURCE_DIR}/../lib/libgnomonic/bin/libgnomonic.a)

# libinter
set(LIBINTER_INCLUDE_DIR ${GNOPROJ_SOURCE_DIR}/../lib/libgnomonic/lib/libinter/src)
set(LIBINTER_LIBS ${GNOPROJ_SOURCE_DIR}/../lib/libgnomonic/lib/libinter/bin/libinter.a)

# lib3dreconstruct
add_custom_target(
  lib3dreconstruct
  COMMAND make
  WORKING_DIRECTORY ${GNOPROJ_SOURCE_DIR}/../lib/lib3dreconstruct
)
set(LIB3DRECONSTRUCT_INCLUDE_DIR ${GNOPROJ_SOURCE_DIR}/../lib/lib3dreconstruct/src)
set(LIB3DRECONSTRUCT_LIBS ${GNOPROJ_SOURCE_DIR}/../lib/lib3dreconstruct/bin/lib3dreconstruct.a)

# includes
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${LIBELPHELPHG_INCLUDE_DIR}
  ${OpenCV_INCLUDE_DIRS}
  ${LIBXML2_INCLUDE_DIR} 
  ${LIB3DRECONSTRUCT_INCLUDE_DIR}  
  ${LIBGNOMONIC_INCLUDE_DIR}  
  ${LIBINTER_INCLUDE_DIR}  
  ${LIBBOOST_INCLUDE_DIR}
  #  ${LIBTIFF_INCLUDE_DIR}
)

# libraries
set(GNOPROJ_LIBRARY_LIST
  ${LIBELPHELPHG_LIBRARY}
  ${OpenCV_LIBS}
  ${LIBXML2_LIBRARY}
  ${LIB3DRECONSTRUCT_LIBS}
  ${LIBGNOMONIC_LIBS}
  ${LIBELPHELPHG_LIBS}
  ${LIBBOOST_SYSTEM_LIBRARY}
  ${LIBBOOST_FILESYSTEM_LIBRARY}
  ${LIBBOOST_REGEX_LIBRARY}
  ${LIBINTER_LIBS}
 #  ${LIBTIFF_LIBRARY}
 #  ${CMAKE_THREAD_LIBS_INIT}
)
# build gnoproj
add_executable(gnoproj gnoproj.cpp)
target_link_libraries(gnoproj
  ${GNOPROJ_LIBRARY_LIST}
)

add_dependencies(gnoproj libgnomonic lib3dreconstruct)

install(TARGETS gnoproj DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

set(CONFIGURED_ONCE TRUE CACHE INTERNAL
    "A flag showing that CMake has configured at least once.")
