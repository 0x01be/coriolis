# - Find the OpenAccess includes and libraries.
# The following variables are set if OpenAccess is found.  If OPENACCESS is not
# found, OPENACCESS_FOUND is set to false.
#  OPENACCESS_FOUND        - True when the OpenAccess include directory is found.
#  OPENACCESS_INCLUDE_DIR - the path to where the OpenAccess include files are.
#  OPENACCESS_LIBRARIES - The path to where the OpenAccess library files are.


SET(OPENACCESS_INCLUDE_PATH_DESCRIPTION "directory containing the OpenAccess include files. E.g /usr/local/include")

SET(OPENACCESS_DIR_MESSAGE "Set the OPENACCESS_INCLUDE_DIR cmake cache entry to the ${OPENACCESS_INCLUDE_PATH_DESCRIPTION}")

# don't even bother under WIN32
IF(UNIX)

  SET(OPENACCESS_DIR_SEARCH $ENV{OPENACCESS_TOP})
  IF(OPENACCESS_DIR_SEARCH)
    FILE(TO_CMAKE_PATH ${OPENACCESS_DIR_SEARCH} OPENACCESS_DIR_SEARCH)
  ENDIF(OPENACCESS_DIR_SEARCH)

  #
  # Look for an installation.
  #
  FIND_PATH(OPENACCESS_INCLUDE_PATH NAMES oa/oaDesign.h PATHS
    # Look in other places.
    ${OPENACCESS_DIR_SEARCH}
    PATH_SUFFIXES include
    # Help the user find it if we cannot.
    DOC "The ${OPENACCESS_INCLUDE_PATH_DESCRIPTION}"
  )

  FIND_LIBRARY(OPENACCESS_LIBRARY_PATH
    NAMES oaDesignD
    PATHS ${OPENACCESS_DIR_SEARCH}
    PATH_SUFFIXES lib/linux_rhel30_64/dbg #for the moment...
    # Help the user find it if we cannot.
    DOC "The ${OPENACCESS_INCLUDE_PATH_DESCRIPTION}"
  )

  # Assume we didn't find it.
  SET(OPENACCESS_FOUND 0)

  IF(OPENACCESS_INCLUDE_PATH)
  IF(OPENACCESS_LIBRARY_PATH)
      SET(OPENACCESS_FOUND "YES")
      SET(OPENACCESS_INCLUDE_DIR
        ${OPENACCESS_INCLUDE_PATH}
      )
      SET(OPENACCESS_LIBRARIES
        ${OPENACCESS_LIBRARY_PATH}
      )
  ENDIF(OPENACCESS_LIBRARY_PATH)
  ENDIF(OPENACCESS_INCLUDE_PATH)

  IF(OPENACCESS_FOUND)
    IF(NOT OPENACCESS_FIND_QUIETLY)
      MESSAGE(STATUS "Found OPENACCESS : ${OPENACCESS_LIBRARIES}")
    ENDIF(NOT OPENACCESS_FIND_QUIETLY)
  ELSE(OPENACCESS_FOUND)
    IF(OPENACCESS_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "OPENACCESS was not found. ${OPENACCESS_DIR_MESSAGE}")
    ENDIF(OPENACCESS_FIND_REQUIRED)
  ENDIF(OPENACCESS_FOUND)

  MARK_AS_ADVANCED(
    OPENACCESS_INCLUDE_PATH
    OPENACCESS_LIBRARY_PATH
  )

ENDIF(UNIX)
