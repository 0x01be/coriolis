# - Try to find the IO libraries
# Once done this will define
#
#  IO_FOUND       - system has the IO library
#  IO_INCLUDE_DIR - the IO include directory
#  IO_LIBRARIES   - The libraries needed to use IO

SET(IO_FOUND FALSE)

# Setup the DIR_SEARCH_PATH.
MACRO(SETUP_SEARCH_DIR project)
  IF( NOT("$ENV{${project}_USER_TOP}" STREQUAL "") )
    MESSAGE("-- ${project}_USER_TOP is set to $ENV{${project}_USER_TOP}")
    LIST(FIND ${project}_DIR_SEARCH "${${project}_DIR_SEARCH}" DIR_INDEX)
    IF( DIR_INDEX LESS 0)
      LIST(INSERT ${project}_DIR_SEARCH 0 "$ENV{${project}_USER_TOP}")
    ENDIF( DIR_INDEX LESS 0)
  ENDIF( NOT("$ENV{${project}_USER_TOP}" STREQUAL "") )
  
  IF( NOT("$ENV{${project}_TOP}" STREQUAL "") )
    MESSAGE("-- ${project}_TOP is set to $ENV{${project}_TOP}")
    LIST(FIND ${project}_DIR_SEARCH "${${project}_DIR_SEARCH}" DIR_INDEX)
    IF( DIR_INDEX LESS 0)
      LIST(INSERT ${project}_DIR_SEARCH 0 "$ENV{${project}_TOP}")
    ENDIF( DIR_INDEX LESS 0)
  ENDIF( NOT("$ENV{${project}_TOP}" STREQUAL "") )
ENDMACRO(SETUP_SEARCH_DIR project)

SETUP_SEARCH_DIR(IO)

IF(IO_DIR_SEARCH)
    # AGDS
    FIND_PATH(AGDS_INCLUDE_DIR
              NAMES io/agds/GdsLibrary.h 
              PATHS ${IO_DIR_SEARCH}
              PATH_SUFFIXES include
    )
    FIND_LIBRARY(AGDS_LIBRARY
                 NAMES agds
                 PATHS ${IO_DIR_SEARCH}
                 PATH_SUFFIXES lib
    )
    IF(AGDS_INCLUDE_DIR AND AGDS_LIBRARY)
       SET(AGDS_FOUND TRUE)
      #SET(IO_FOUND   TRUE)
    #   SET(IO_LIBRARIES ${AGDS_LIBRARY})
    ELSE(AGDS_INCLUDE_DIR AND AGDS_LIBRARY)
       SET(AGDS_FOUND FALSE)
    #   SET(IO_LIBRARIES)
    ENDIF(AGDS_INCLUDE_DIR AND AGDS_LIBRARY)
    
    # CIF
    FIND_PATH(CIF_INCLUDE_DIR
              NAMES io/cif/CifCircuit.h 
              PATHS ${IO_DIR_SEARCH}
              PATH_SUFFIXES include
    )
    FIND_LIBRARY(CIF_LIBRARY
                 NAMES cif
                 PATHS ${IO_DIR_SEARCH}
                 PATH_SUFFIXES lib
    )
    IF(CIF_INCLUDE_DIR AND CIF_LIBRARY)
       SET(CIF_FOUND TRUE)
    ELSE(CIF_INCLUDE_DIR AND CIF_LIBRARY)
       SET(CIF_FOUND FALSE)
    ENDIF(CIF_INCLUDE_DIR AND CIF_LIBRARY)
    
    # OPENCHAMS
    FIND_PATH(OPENCHAMS_INCLUDE_DIR
              NAMES io/openChams/Circuit.h 
              PATHS ${IO_DIR_SEARCH}
              PATH_SUFFIXES include
    )
    FIND_LIBRARY(OPENCHAMS_LIBRARY
                 NAMES openChams
                 PATHS ${IO_DIR_SEARCH}
                 PATH_SUFFIXES lib
    )
    IF(OPENCHAMS_INCLUDE_DIR AND OPENCHAMS_LIBRARY)
       SET(OPENCHAMS_FOUND TRUE)
    ELSE(OPENCHAMS_INCLUDE_DIR AND OPENCHAMS_LIBRARY)
       SET(OPENCHAMS_FOUND FALSE)
    ENDIF(OPENCHAMS_INCLUDE_DIR AND OPENCHAMS_LIBRARY)
    
    IF(AGDS_FOUND AND CIF_FOUND AND OPENCHAMS_FOUND)
        SET(IO_FOUND TRUE)
    ELSE(AGDS_FOUND AND CIF_FOUND AND OPENCHAMS_FOUND)
        SET(IO_FOUND FALSE)
    ENDIF(AGDS_FOUND AND CIF_FOUND AND OPENCHAMS_FOUND)
ENDIF(IO_DIR_SEARCH)

IF (NOT IO_FOUND)
    SET(IO_MESSAGE
    "IO libraries were not found. Make sure IO_TOP env variable is set.")
    IF (NOT IO_FIND_QUIETLY)
        MESSAGE(STATUS "${IO_MESSAGE}")
    ELSE(NOT IO_FIND_QUIETLY)
        IF(IO_FIND_REQUIRED)
            MESSAGE(FATAL_ERROR "${IO_MESSAGE}")
        ENDIF(IO_FIND_REQUIRED)
    ENDIF(NOT IO_FIND_QUIETLY)
ELSE (NOT IO_FOUND)
    MESSAGE(STATUS "IO library was found in ${IO_DIR_SEARCH}")
ENDIF (NOT IO_FOUND)

