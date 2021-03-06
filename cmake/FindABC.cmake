FIND_PATH(ABC_INCLUDE_DIR alanmi-abc/src/)

FIND_LIBRARY(ABC_LIBRARY NAMES abcd)

IF (ABC_INCLUDE_DIR AND ABC_LIBRARY)
  SET(ABC_FOUND TRUE)
ENDIF (ABC_INCLUDE_DIR AND ABC_LIBRARY)


IF (ABC_FOUND)
  IF (NOT ABC_FIND_QUIETLY)
    MESSAGE(STATUS "Found ABC: ${ABC_LIBRARY}")
  ENDIF (NOT ABC_FIND_QUIETLY)
ELSE (ABC_FOUND)
  IF (ABC_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find ABC")
  ENDIF (ABC_FIND_REQUIRED)
ENDIF (ABC_FOUND)

