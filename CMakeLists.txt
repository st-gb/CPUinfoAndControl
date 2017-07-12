#from https://cmake.org/cmake-tutorial/

cmake_minimum_required (VERSION 2.6)
project (CPUinfoAndControl)

if( NOT DEFINED COMMON_SOURCECODE_ROOT_PATH )
  message("warning: COMMON_SOURCECODE_ROOT_PATH is NOT defined")
  set(COMMON_SOURCECODE_ROOT_PATH ../common_sourcecode)
endif( NOT DEFINED COMMON_SOURCECODE_ROOT_PATH )

if( DEFINED PLATFORM_STL_ROOT_PATH )
  message("PLATFORM_STL_ROOT_PATH defined")
endif( DEFINED PLATFORM_STL_ROOT_PATH )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

set(CMAKE_LIST_FILE_PATHS
  ${CMAKE_CURRENT_LIST_FILE}
)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_DEBUG_POSTFIX "_debug")
#set(<variable> <value>  ... <valueN>)

#from http://stackoverflow.com/questions/12896988/passing-the-argument-to-cmake-via-command-prompt
# on command line: "-DEXE_TYPE:STRING=wxGUI"
# #ET(EXE_TYPE "wxGUI")
message("executable type to build:${EXE_TYPE}")

add_subdirectory(Controller)
add_subdirectory(Xerces)

#see http://accountingplusplus.blogspot.de/2010/12/cmake-wxwidgets.html :
if( ${EXE_TYPE} STREQUAL "wxGUI")
  set(WXWIDGETS ON)
  set(CXX_DEFINITIONS 
    -DCOMPILE_AS_GUI)
  add_subdirectory(wxWidgets)
  set(EXE_NAME ${CPUinfoAndControl}_wxGUI)
endif( ${EXE_TYPE} STREQUAL "wxGUI" )

if( DEFINED WXWIDGETS )
  message("WXWIDGETS is defined")
  #see https://cmake.org/cmake/help/v3.0/command/add_subdirectory.html :
  #additionally use CMakeLists.txt from "wxWidgets" folder
#  add_subdirectory(wxWidgets 
#    #EXCLUDE_FROM_ALL
#  )
endif ( DEFINED WXWIDGETS )

set( INCLUDE_DIRS 
  ${INCLUDE_DIRS} #inlcude the wxWidgets dirs from above
  ${COMMON_SOURCECODE_ROOT_PATH}
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/MSVC_adaption
  ${PROJECT_SOURCE_DIR}
  ${PLATFORM_STL_ROOT_PATH}
  ${APACHE_XERCES_ROOT_PATH}
)
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
  -DCOMPILE_WITH_MSR_EXAMINATION
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

if (UNIX)
  message("UNIX defined")
  add_subdirectory(Linux)
  set( LIBRARIES
    ${LIBRARIES}
    pthread
  )
#  add_subdirectory(./Linux
#    #EXCLUDE_FROM_ALL
#    )
  set( CLIENT_SERVER_BASE_SOURCE_FILES
    ${CLIENT_SERVER_BASE_SOURCE_FILES}
    ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/POSIX/multithread/pthreadBasedI_Thread.cpp
  	${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/Linux/EnglishMessageFromErrorCode/EnglishMessageFromErrorCode.cpp
	  ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/Linux/FileSystem/GetCurrentWorkingDir/GetCurrentWorkingDir.cpp
  )
  #message("CLIENT_SERVER_BASE_SOURCE_FILES: ${CLIENT_SERVER_BASE_SOURCE_FILES}")
endif (UNIX)

#add_directory( ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger)
set(LOGGER_SOURCE_FILES
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/preprocessor_logging_macros.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/Logger.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/LogLevel.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/Appender/FormattedLogEntryProcessor.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/Appender/RollingFileOutput.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/Formatter/I_LogFormatter.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/Formatter/HTMLlogFormatter.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/Formatter/Log4jFormatter.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/OutputHandler/I_LogEntryOutputter.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/OutputHandler/StdCoutLogWriter.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/Logger/OutputHandler/StdOfStreamLogWriter.cpp
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
    ${PROJECT_SOURCE_DIR}/service/SMARTmonitorService.cpp
    ${PROJECT_SOURCE_DIR}/service/service_main.cpp
  )
endif(${EXE_TYPE} STREQUAL "UNIX_service")

if( ${EXE_TYPE} STREQUAL "wxGUI")
  set(SOURCE_FILE_PATHS
    ${SOURCE_FILE_PATHS}
  )
endif(${EXE_TYPE} STREQUAL "wxGUI")

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

if(DEFINED WXWIDGETS)
  #https://wiki.wxwidgets.org/CMake
  set(wxWidgets_CONFIGURATION gtkud)
  SET(wxWidgets_USE_LIBS core base adv)
  find_package(wxWidgets COMPONENTS core base adv REQUIRED)
  include(${wxWidgets_USE_FILE})

  message("wxWidgets_LIBRARIES: ${wxWidgets_LIBRARIES}")
endif (DEFINED WXWIDGETS)

message ("SOURCE_FILE_PATHS: ${SOURCE_FILE_PATHS}")

if( ${EXE_TYPE} STREQUAL "wxGUI")
  set(EXE_NAME wx${PROJECT_NAME}_GUI)
  set(SOURCE_FILE_PATHS 
    ${SOURCE_FILE_PATHS}
    )
  add_executable(${EXE_NAME} ${SOURCE_FILE_PATHS})
endif( ${EXE_TYPE} STREQUAL "wxGUI" )
if( ${EXE_TYPE} STREQUAL "UNIX_service")
  set(EXE_NAME ${PROJECT_NAME}_service)
  add_executable(${EXE_NAME} ${SOURCE_FILE_PATHS})
endif( ${EXE_TYPE} STREQUAL "UNIX_service" )

if(DEFINED WXWIDGETS)
  #https://wiki.wxwidgets.org/CMake
  target_link_libraries(${EXE_NAME} ${wxWidgets_LIBRARIES})
endif (DEFINED WXWIDGETS)

message ("EXE_NAME: ${EXE_NAME} LIBRARIES: ${LIBRARIES}")

target_link_libraries(${EXE_NAME} ${LIBRARIES})
message (STATUS "wxWidgets type: ${wxWidgets_CONFIGURATION}")
