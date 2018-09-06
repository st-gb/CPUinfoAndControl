cmake_minimum_required (VERSION 2.6)
project (AMD_family_15h_CPU_IandC)

set( SOURCE_FILE_PATHS
  dynlib_main.cpp)
set( COMMON_SOURCECODE_ROOT_PATH 
  ${PROJECT_SOURCE_DIR}/../../../../common_sourcecode )

set(CPU_I_AND_C_SOURC_DIR
  ${PROJECT_SOURCE_DIR}/../../.. )

set( INCLUDE_DIRS 
  ${COMMON_SOURCECODE_ROOT_PATH}
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/MSVC_adaption
  ${CPU_I_AND_C_SOURC_DIR}
  )

include_directories( ${INCLUDE_DIRS} )
message("include directories: ${INCLUDE_DIRS}")

#from https://stackoverflow.com/questions/6499364/generating-dll-using-cmake
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILE_PATHS})
