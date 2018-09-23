cmake_minimum_required (VERSION 2.6)
project (AMD_NPT_family_0Fh_CPU_IandC)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set( SOURCE_FILE_PATHS
  dllmain.cpp
  )

set( COMMON_SOURCECODE_ROOT_PATH 
  ${PROJECT_SOURCE_DIR}/../../../../common_sourcecode )

set(CPU_I_AND_C_SOURCE_DIR
  ${PROJECT_SOURCE_DIR}/../../.. )

#  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/MSVC_adaption
set( INCLUDE_DIRS 
  ${COMMON_SOURCECODE_ROOT_PATH}
  ${CPU_I_AND_C_SOURCE_DIR}
  )

#TODO :create a flag that is passed to cmake whether to build a logging dynamic 
#  library or not
#if(COMPILE WITH LOG
set( SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/Appender/FormattedLogEntryProcessor.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/Appender/RollingFileOutput.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/OutputHandler/I_LogEntryOutputter.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/OutputHandler/StdCoutLogWriter.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/Formatter/I_LogFormatter.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/Formatter/Log4jFormatter.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/Formatter/HTMLlogFormatter.cpp
  #getBinaryRepresentation[abi:cxx11](unsigned long)'
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/character_string/format_as_string.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/Logger.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/preprocessor_logging_macros.cpp
  )
#endif()

#see https://cmake.org/cmake/help/v3.0/variable/WIN32.html
if(WIN32)
message("building for Microsoft Windows")
set( SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${COMMON_SOURCECODE_ROOT_PATH}/Windows/Logger/LogEntryOutputter.cpp
  )
endif()

#Necessary preprocessor macros for 
# ${COMMON_SOURCECODE_ROOT_PATH}/preprocessor_macros/logging_preprocessor_macros.h
add_definitions("-DCOMPILE_WITH_LOG -DUSE_OWN_LOGGER -D_DEBUG")
set( CXX_DEFINITIONS
  -DUSE_OWN_LOGGER
  -DCOMPILE_WITH_LOG
  -DCOMPILE_AS_EXECUTABLE
  -DUNICODE #both "UNICODE" and "_UNICODE" are needed for Microsoft Visual C++
  -D_UNICODE #in order to define TCHAR types and functions using these types
  -D_DEBUG
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  #from https://stackoverflow.com/questions/4479049/show-include-equivalent-option-in-g
  #"-H" for g++ causes to show all the include files
  #add_definitions("-H")

  #TODO: verify: If not added this may cause Windows error code 126:("The specified procedure 
  #  could not be found."? 
  # "-Wno-undef" forces unresolved definitions to be reported at compile time
  #  instead of at dynamic link/load time.
  #-fPIC: Position-Independent Code (necessary at all?)
  add_definitions("-Wno-undef -fPIC")
  #"-s": "Remove all symbol table and relocation information from the executable." 
  #   (explanation from https://gcc.gnu.org/onlinedocs/gcc-5.5.0/gcc.pdf)
  # This saves some space in the binary.
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -s")

  set(CMAKE_CXX_FLAGS "-Wall -Wextra")
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
else()
  #add_definitions("-DCOMPILE_WITH_LOG")
endif()

include_directories( ${INCLUDE_DIRS} )
message("include directories: ${INCLUDE_DIRS}")

#from https://stackoverflow.com/questions/6499364/generating-dll-using-cmake
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILE_PATHS})
#Statically link necessary libraries in order to be independent on the ones from
# GUI or service
#target_link_libraries(${PROJECT_NAME} stdc++ gcc_s msvcrt)
