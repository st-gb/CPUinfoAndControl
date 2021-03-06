#from https://cmake.org/cmake-tutorial/

#https://stackoverflow.com/questions/48509911/cmake-add-subdirectory-vs-include
# -> use include() instead of "add_subdirectory()" for the same target
# Also a "PARENT_SCOPE" is neccessary when doing add_subdirectory(...) and the
# variables should be updated to the calling script?!

#TODO Enable a minimal build (MinSizeRel CMAKE_BUILD_TYPE)
# with omitted symbols (GCC: "-s" option)
#TODO Really the minimal CMake version for this project?
cmake_minimum_required(VERSION 2.6)

#Also <> "" if not passed to CMake but it is an environment variable?
if(COMMON_SOURCECODE_ROOT_PATH STREQUAL "" #works under Linux
  OR NOT DEFINED COMMON_SOURCECODE_ROOT_PATH)
  message("warning: COMMON_SOURCECODE_ROOT_PATH is NOT defined->set to default")
  set(COMMON_SOURCECODE_ROOT_PATH ../common_sourcecode)
endif()

if(UNIX)
  if(NOT DEFINED CMAKE_C_COMPILER OR CMAKE_C_COMPILER STREQUAL "")
    #If using a non-absolute path when setting the C Compiler causes this:
	# http://public.kitware.com/Bug/view.php?id=13756
    #So use an absolute path, as http://public.kitware.com/Bug/view.php?id=14294
    # proposes.
    message("compiler not defined->setting to default")
    set(CMAKE_C_COMPILER "/usr/bin/gcc")
    # "-pg" option for "gprof" profiling
  endif()
  if(NOT DEFINED CMAKE_CXX_COMPILER OR CMAKE_CXX_COMPILER STREQUAL "")
    #If using a non-absolute path when setting the C Compiler causes this:
	# http://public.kitware.com/Bug/view.php?id=13756
    #So use an absolute path, as http://public.kitware.com/Bug/view.php?id=14294
    # proposes.
    message("compiler not defined->setting to default")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
  endif()
endif()
message("C compiler: ${CMAKE_C_COMPILER}")
message("C++ compiler: ${CMAKE_CXX_COMPILER}")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3 -gdwarf-2")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -gdwarf-2")
endif()

message("COMMON_SOURCECODE_ROOT_PATH: ${COMMON_SOURCECODE_ROOT_PATH}")

include(${COMMON_SOURCECODE_ROOT_PATH}/CMake/MinGW_PATH_sh.exe_message.cmake)

project(CPUinfoAndControl)

#from http://stackoverflow.com/questions/12896988/passing-the-argument-to-cmake-via-command-prompt
# on command line: "-DEXE_TYPE:STRING=wxGUI"
message("executable type to build (\"EXE_TYPE\"):${EXE_TYPE}")
message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

message("Possibly use \"--log-level=\" for output")
#https://stackoverflow.com/questions/22803607/how-do-i-debug-cmakelists-txt-files
#set(CMAKE_VERBOSE_MAKEFILE ON)

#show1ItmPerLne(...), showMsg(...)
include(${COMMON_SOURCECODE_ROOT_PATH}/CMake/ShowMsg_CompLvl.cmake)

#Include Operating System-specific CMake file here to keep _this_ file small.
if(WIN32)
  include(Windows/Windows.cmake)
endif()
# https://cmake.org/cmake/help/latest/variable/UNIX.html : 
# "target system is UNIX or UNIX-like (e.g. APPLE and CYGWIN)"
if(UNIX)# "UNIX" must be written in capital letters else it does not work.
  message("Unix defined")
#  add_subdirectory(./Linux
#    #EXCLUDE_FROM_ALL
#    )
  include(Linux/CMakeLists.txt)
  #message("CLIENT_SERVER_BASE_SOURCE_FILES: ${CLIENT_SERVER_BASE_SOURCE_FILES}")
endif()

if( DEFINED PLATFORM_STL_ROOT_PATH )
  message("PLATFORM_STL_ROOT_PATH defined")
endif( DEFINED PLATFORM_STL_ROOT_PATH )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

set(CMAKE_LIST_FILE_PATHS
  ${CMAKE_CURRENT_LIST_FILE}
)

#set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_DEBUG_POSTFIX "_debug")
#set(<variable> <value>  ... <valueN>)

add_subdirectory(Controller)
add_subdirectory(Xerces)

#see http://accountingplusplus.blogspot.de/2010/12/cmake-wxwidgets.html :
if( ${EXE_TYPE} STREQUAL "wxGUI")
  set(WXWIDGETS ON)
  set(CXX_DEFINITIONS
    -DCOMPILE_AS_GUI
    -DCOMPILE_WITH_MSR_EXAMINATION
    )
  set(EXE_NAME ${CPUinfoAndControl}_wxGUI)
endif( ${EXE_TYPE} STREQUAL "wxGUI" )

if( ${EXE_TYPE} STREQUAL "UNIX_service" )
  set(WXWIDGETS ON)
  set(CXX_DEFINITIONS
    ${CXX_DEFINITIONS}
    -DBUILD_AS_UNIX_SERVICE
    -DCOMPILE_AS_SERVICE
    )
  set(EXE_NAME ${CPUinfoAndControl}_daemon)
endif()

if(DEFINED WXWIDGETS)
  message("WXWIDGETS is defined")
  #see https://cmake.org/cmake/help/v3.0/command/add_subdirectory.html :
  #additionally use CMakeLists.txt from "wxWidgets" folder
#  add_subdirectory(wxWidgets 
#    #EXCLUDE_FROM_ALL
#  )
  #add_subdirectory(wxWidgets)
  include(wxWidgets/CMakeLists.txt)
endif()

set( INCLUDE_DIRS 
  ${INCLUDE_DIRS} #include the wxWidgets dirs from above
  ${COMMON_SOURCECODE_ROOT_PATH}
  ${PROJECT_SOURCE_DIR}
  ${PLATFORM_STL_ROOT_PATH}
  ${APACHE_XERCES_ROOT_PATH}
)

if(NOT WIN32) 
 set(INCLUDE_DIRS 
  ${INCLUDE_DIRS}
  #only for Cygwin, Linux, Android?:
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/MSVC_adaption)
endif()

message("include directories: ${INCLUDE_DIRS}")

#set(PROJECT_BINARY_DIR /home/sg/SourceCodeManagement/VTrans3/build)
#http://stackoverflow.com/questions/6594796/how-do-i-make-cmake-output-into-a-bin-dir
#set(CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}/../build)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin/")
#set(CMAKE_CURRENT_BINARY_DIRECTORY /home/sg/SourceCodeManagement/VTrans3/bin)

message("PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message("PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")

#  -DCOMPILE_AS_EXECUTABLE
set( CXX_DEFINITIONS
  ${CXX_DEFINITIONS}
  -DCOMPILE_LOGGER_MULTITHREAD_SAFE
  #-D__WXDEBUG__
  -DUSE_OWN_LOGGER
  -DCOMPILE_WITH_LOG
  -DUNICODE
)

message (STATUS "${CMAKE_CURRENT_LIST_FILE} LIBRARIES: ${LIBRARIES}")

#include_directories("${PROJECT_BINARY_DIR}")

#https://www.johnlamp.net/cmake-tutorial-4-libraries-and-subdirectories.html
include_directories( ${INCLUDE_DIRS} )

#"Add directories to the end of this directory’s include paths."
#https://cmake.org/Wiki/CMake_Useful_Variables for "ENV
#include_directories($ENV{COMMON_SOURCECODE_DIR})

#https://cmake.org/cmake/help/v3.0/command/add_executable.html

#http://stackoverflow.com/questions/17653738/recursive-cmake-search-for-header-and-source-files
#file(GLOB_RECURSE SOURCE_FILE_PATHS *.cpp)

#http://stackoverflow.com/questions/16449676/remove-specific-file-from-cmake-build
#list(REMOVE_ITEM SOURCE_FILE_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
#LIST(FILTER <list> <regex> [<regex> ...] [OUTPUT_VARIABLE <variable>])
#set(SOURCE_FILE_PATHS_TEMP ${SOURCE_FILE_PATHS})
#list(REMOVE_ITEM SOURCE_FILE_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/testing.cpp)

add_subdirectory("ModelData")

#add_directory( ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger)
set(CmnSrcLoggerDir ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger)
include(${CmnSrcLoggerDir}/Logger.cmake)
set(LOGGER_SOURCE_FILES
  ${SG_BasicLoggerSrcFilePaths}
  ${CmnSrcLoggerDir}/Appender/RollingFileOutput.cpp
  ${CmnSrcLoggerDir}/Formatter/HTMLlogFormatter.cpp
  ${CmnSrcLoggerDir}/Formatter/Log4jFormatter.cpp
  ${CmnSrcLoggerDir}/OutputHandler/StdCoutLogWriter.cpp
  ${CmnSrcLoggerDir}/OutputHandler/StdOfStreamLogWriter.cpp
)

#Source files needed for both client/GUI and server builds.
set(CLIENT_SERVER_BASE_SOURCE_FILES
  ${CLIENT_SERVER_BASE_SOURCE_FILES}
  ${LOGGER_SOURCE_FILES}
  ${CPU_ACCESS_SOURCE_FILES}
  ${ATTRIBUTE_DATA_SOURCE_FILES}
  #${COMMON_SOURCECODE_ROOT_PATH}/data_structures/Trie/byteTrie/ByteTrie.cpp
  #${COMMON_SOURCECODE_ROOT_PATH}/data_structures/Trie/byteTrie/Trie.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/multithread/I_Thread.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/character_string/stdtstr.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/FileSystem/PathSeperatorChar.cpp
)

if( ${EXE_TYPE} STREQUAL "UNIX_service")
  set(SOURCE_FILE_PATHS
    ${SOURCE_FILE_PATHS}
    ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/Linux/daemon/daemon.cpp
    ${PROJECT_SOURCE_DIR}/Controller/CPUcontrolServiceBase.cpp
  )
endif(${EXE_TYPE} STREQUAL "UNIX_service")

set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${CLIENT_SERVER_BASE_SOURCE_FILES}
  ${CONFIGURATION_READER_SOURCE_FILES}
  ${ATTRIBUTE_DATA_SOURCE_FILES}
)

message("adding definitions:${CXX_DEFINITIONS}")
add_definitions(${CXX_DEFINITIONS})
#add_library(${LIBRARIES} )
#specify which libraries an executable target links against.
#TARGET_LINK_LIBRARIES( ${LIBRARIES})

#target_link_libraries(${LIBRARIES})

showMsg(3, "SOURCE_FILE_PATHS: ${SOURCE_FILE_PATHS}")
showInclDirs(5)

if( ${EXE_TYPE} STREQUAL "wxGUI")
  set(EXE_NAME ${PROJECT_NAME}_wxGUI)
endif( ${EXE_TYPE} STREQUAL "wxGUI" )
if( ${EXE_TYPE} STREQUAL "UNIX_service")
  set(EXE_NAME ${PROJECT_NAME}_service)
endif( ${EXE_TYPE} STREQUAL "UNIX_service" )

#TODO also wxWidgets (and compiler) version in filename
# CMAKE_CPP_COMPILER_ID  CMAKE_CPP_COMPILER_VERSION
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  SET(EXE_NAME ${EXE_NAME}_dbg)
endif()
add_executable(${EXE_NAME} ${SOURCE_FILE_PATHS})

if(DEFINED WXWIDGETS)
  message("target_link_ibraries with wxWidgets_LIBRARIES")
  show1ItmPerLne(5 "wxWidgets_LIBRARIES:" "${wxWidgets_LIBRARIES}")
  #https://wiki.wxwidgets.org/CMake
  target_link_libraries(${EXE_NAME} ${wxWidgets_LIBRARIES})
endif (DEFINED WXWIDGETS)

message ("EXE_NAME: ${EXE_NAME} LIBRARIES: ${LIBRARIES}")

#https://cmake.org/pipermail/cmake/2017-April/065258.html
get_target_property(_compile_defs ${EXE_NAME} COMPILE_DEFINITIONS)
#https://stackoverflow.com/questions/5403705/cmake-add-definitions-and-compile-definitions-how-to-see-them
get_directory_property( _compile_defs DIRECTORY ${CMAKE_SOURCE_DIR} COMPILE_DEFINITIONS )
message("COMPILE_DEFINITIONS: ${_compile_defs}")

message("CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")

set(varname CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE})
message("${varname}: ${${varname}}")
message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

target_link_libraries(${EXE_NAME} ${LIBRARIES})
