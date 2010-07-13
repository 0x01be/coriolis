

#
# Adds -Wall to the C/C++ flags.
#
 set(CMAKE_C_FLAGS_DEBUG     "-g -Wall"           CACHE STRING "Debug options."   FORCE)
 set(CMAKE_CXX_FLAGS_DEBUG   "-g -Wall"           CACHE STRING "Debug options."   FORCE)
 set(CMAKE_C_FLAGS_RELEASE   "-O3 -DNDEBUG -Wall" CACHE STRING "Release options." FORCE)
 set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -Wall" CACHE STRING "Release options." FORCE)


#
# Build <PROJECT>_INCLUDE_DIR & <PROJECT>_LIBRARIES and sets up <PROJECT>_FOUND
# Usage:  set_library_path(<PROJECT> <library>)
#
# May be used any number of time on the same <PROJECT> to create a list of
# <library>.
#
 macro(set_libraries_path configname library)
   set(${configname}_FOUND "NOTFOUND")
 
   if(${library}_LIBRARY_PATH)
     set(${configname}_FOUND "YES")
     set(${configname}_INCLUDE_DIR ${${library}_INCLUDE_PATH})
     set(${configname}_LIBRARIES ${${library}_LIBRARY_PATH} ${${configname}_LIBRARIES})
     mark_as_advanced(${configname}_INCLUDE_DIR ${configname}_LIBRARIES)
   endif(${library}_LIBRARY_PATH)
 
   if(NOT ${library}_INCLUDE_PATH)
     set(${configname}_FOUND "NOTFOUND")
   endif(NOT ${library}_INCLUDE_PATH)
 endmacro ( set_libraries_path )


#
# Checks if a set of libraries has been found, could be blocking or not.
# Usage: hurricane_check_libraries(<PROJECT> <REQUIRED>)
#
# If <REQUIRED> is ommitted, it is guessed from <PROJECT>_FIND_REQUIRED.
#
 macro(hurricane_check_libraries)
   if(ARGC LESS 2)
     set(REQUIRED ${ARGV0}_FIND_REQUIRED)
   else(ARGC LESS 2)
     set(REQUIRED ${ARGV1})
   endif(ARGC LESS 2)
   if(${ARGV0}_FOUND)
     if(NOT ${ARGV0}_FIND_QUIETLY)
       if(${ARGV0}_FOUND)
         message(STATUS "Found ${ARGV0} : ${${ARGV0}_LIBRARIES}")
       endif(${ARGV0}_FOUND)
     endif(NOT ${ARGV0}_FIND_QUIETLY)
   else(${ARGV0}_FOUND)
     if(REQUIRED)
       message(FATAL_ERROR "${ARGV0} was not found. ${${ARGV0}_DIR_MESSAGE}")
     endif(REQUIRED)
   endif(${ARGV0}_FOUND)
 endmacro(hurricane_check_libraries)


#
# Perform some tweaks on shared/static linking.
#
 macro(set_lib_link_mode)
   if(NOT BUILD_SHARED_LIBS)
   # check for qmake
     find_program(QT_QMAKE_EXECUTABLE NAMES qmake-qt4 qmake PATHS
       /opt/qt4-static-4.3.2/bin
       NO_DEFAULT_PATH
       )
     message(STATUS "Building static libraries.")
   else(NOT BUILD_SHARED_LIBS)
     message(STATUS "Building dynamic libraries.")
   endif(NOT BUILD_SHARED_LIBS)
 endmacro(set_lib_link_mode)


#
# Find Boost, checking different versions.
#
 macro(SetupBoost)
  #set(Boost_USE_STATIC_LIBS ON)
  #message(STATUS "Always uses Boost static libraries.")
   if(ARGC LESS 1)
     find_package(Boost 1.33.1 REQUIRED)
   else(ARGC LESS 1)
     find_package(Boost 1.35.0 COMPONENTS ${ARGV} system)
     if(NOT Boost_FOUND)
       find_package(Boost 1.33.1 COMPONENTS ${ARGV} REQUIRED)
     endif(NOT Boost_FOUND)
   endif(ARGC LESS 1)
   message(STATUS "Found Boost libraries ${Boost_LIB_VERSION} in ${Boost_INCLUDE_DIR}")
   message(STATUS "  ${Boost_LIBRARIES}")
 endmacro(SetupBoost)
 

#
# Setup the <PROJECT>_SEARCH_PATH.
#   Where to find includes & libraries.
#
 MACRO(SETUP_SEARCH_DIR project)
   IF( NOT("$ENV{${project}_TOP}" STREQUAL "") )
     MESSAGE("-- ${project}_TOP is set to $ENV{${project}_TOP}")
     LIST(INSERT ${project}_DIR_SEARCH 0 "${DESTDIR}$ENV{${project}_TOP}")
   ENDIF( NOT("$ENV{${project}_TOP}" STREQUAL "") )

   IF( NOT("$ENV{${project}_USER_TOP}" STREQUAL "") )
     MESSAGE("-- ${project}_USER_TOP is set to $ENV{${project}_USER_TOP}")
     LIST(INSERT ${project}_DIR_SEARCH 0 "${DESTDIR}$ENV{${project}_USER_TOP}")
   ENDIF( NOT("$ENV{${project}_USER_TOP}" STREQUAL "") )

   LIST(REMOVE_DUPLICATES ${project}_DIR_SEARCH)
  
   MESSAGE("-- Components of ${project}_DIR_SEARCH:")
   FOREACH(PATH IN LISTS ${project}_DIR_SEARCH)
     MESSAGE("--   ${PATH}")
   ENDFOREACH(PATH)
 ENDMACRO(SETUP_SEARCH_DIR project)
