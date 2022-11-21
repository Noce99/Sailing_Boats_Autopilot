cmake_minimum_required(VERSION 3.7.1)

PROJECT(Sailing_Boats_Controller)

SET(MY_CURRENT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE(STATUS "Current Dir: " ${MY_CURRENT_DIR})

MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child} AND EXISTS ${curdir}/${child}/CMakeLists.txt)
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

SUBDIRLIST(SUBDIRS ${MY_CURRENT_DIR})

FOREACH(subdir ${SUBDIRS})
  ADD_SUBDIRECTORY(${subdir})
ENDFOREACH()

MESSAGE(STATUS "All the SubDirs: ")
FOREACH(subdir ${SUBDIRS})
  MESSAGE(STATUS "\t" ${subdir})
ENDFOREACH()
