cmake_minimum_required (VERSION 2.6)

project(proc.statCPUcoreUsageGetterDynLib)

set(SOURCE_FILE_PATHS
  main.cpp)

if(COMMON_SOURCECODE_ROOT_PATH STREQUAL "" OR NOT DEFINED COMMON_SOURCECODE_ROOT_PATH)
  message("warning: COMMON_SOURCECODE_ROOT_PATH is EMPTY or UNDEFINED")
  set(COMMON_SOURCECODE_ROOT_PATH ${PROJECT_SOURCE_DIR}/../../../common_sourcecode)
endif()
message("COMMON_SOURCECODE_ROOT_PATH: ${COMMON_SOURCECODE_ROOT_PATH}")

set(CPU_I_AND_C_SOURCE_DIR
  ${PROJECT_SOURCE_DIR}/../.. )

#add_definitions(-fPIC)

if(CMAKE_COMPILER_IS_GNUCC)
 #https://stackoverflow.com/questions/11783932/how-to-add-linker-or-compile-flag-in-cmake-file
 #"--no-undefined" reports errors while compiling
 #see https://stackoverflow.com/questions/2356168/force-gcc-to-notify-about-undefined-references-in-shared-libraries
 SET(CMAKE_CXX_FLAGS "-Wl,--no-undefined")
 
 #if(debugIncludeFiles)
  #SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-H")
endif()

set( INCLUDE_DIRS 
  ${COMMON_SOURCECODE_ROOT_PATH}
  ${CPU_I_AND_C_SOURCE_DIR}
  )

if(NOT WIN32)
 set(INCLUDE_DIRS 
  ${INCLUDE_DIRS}
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/MSVC_adaption
  )
endif()

include_directories( ${INCLUDE_DIRS} )
message("include directories: ${INCLUDE_DIRS}")

#from https://stackoverflow.com/questions/6499364/generating-dll-using-cmake
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILE_PATHS})

#The following needs to be done with "--no-undefined" as compiler flag
#see https://linux.die.net/man/3/dlopen: "Link with -ldl."
#TODO Is this really needed? : is "dlopen" used in the source code of dyn lib?
target_link_libraries(${PROJECT_NAME} dl)

