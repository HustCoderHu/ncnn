

SET(OpenCV_INCLUDE_SEARCH_PATHS
  $ENV{OpenCV_HOME}
  $ENV{OpenCV_HOME}/include
)

SET(OpenCV_LIB_SEARCH_PATHS
        $ENV{OpenCV}
        $ENV{OpenCV}/lib
        $ENV{OpenCV_HOME}
        $ENV{OpenCV_HOME}/lib
 )

FIND_PATH(OpenCV_INCLUDE_DIR NAMES cblas.h PATHS ${OpenCV_INCLUDE_SEARCH_PATHS})
FIND_LIBRARY(OpenCV_LIB NAMES openblas PATHS ${OpenCV_LIB_SEARCH_PATHS})

SET(OpenCV_FOUND ON)

#    Check include files
IF(NOT OpenCV_INCLUDE_DIR)
    SET(OpenCV_FOUND OFF)
    MESSAGE(STATUS "Could not find OpenCV include. Turning OpenCV_FOUND off")
ENDIF()

#    Check libraries
IF(NOT OpenCV_LIB)
    SET(OpenCV_FOUND OFF)
    MESSAGE(STATUS "Could not find OpenCV lib. Turning OpenCV_FOUND off")
ENDIF()

IF (OpenCV_FOUND)
  IF (NOT OpenCV_FIND_QUIETLY)
    MESSAGE(STATUS "Found OpenCV libraries: ${OpenCV_LIB}")
    MESSAGE(STATUS "Found OpenCV include: ${OpenCV_INCLUDE_DIR}")
  ENDIF (NOT OpenCV_FIND_QUIETLY)
ELSE (OpenCV_FOUND)
  IF (OpenCV_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find OpenCV")
  ENDIF (OpenCV_FIND_REQUIRED)
ENDIF (OpenCV_FOUND)

MARK_AS_ADVANCED(
    OpenCV_INCLUDE_DIR
    OpenCV_LIB
    OpenCV
)

