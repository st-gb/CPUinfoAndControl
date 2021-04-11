cmake_minimum_required(VERSION 2.6)
project(IntelCore2_IandC)

set( SOURCE_FILE_PATHS
  dllmain.cpp 
  ../../../Controller/CPU-related/Intel/Core/Core.cpp)

message("SOURCE_FILE_PATHS: ${SOURCE_FILE_PATHS}")

set( COMMON_SOURCECODE_ROOT_PATH 
  ${PROJECT_SOURCE_DIR}/../../../../common_sourcecode )

add_definitions("-DCOMPILE_FOR_INTEL_CORE2")

set(CPU_I_AND_C_SOURCE_DIR
  ${PROJECT_SOURCE_DIR}/../../.. )

set( INCLUDE_DIRS 
  ${COMMON_SOURCECODE_ROOT_PATH}
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/MSVC_adaption
  ${CPU_I_AND_C_SOURCE_DIR}
  )

#SET( CMAKE_CXX_FLAGS "-Wl,--no-undefined")

include_directories( ${INCLUDE_DIRS} )
message("include directories: ${INCLUDE_DIRS}")

#from https://stackoverflow.com/questions/6499364/generating-dll-using-cmake
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILE_PATHS})
