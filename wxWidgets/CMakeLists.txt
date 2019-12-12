
#see http://stackoverflow.com/questions/8934295/add-source-in-a-subdirectory-to-a-cmake-project

#  ${CMAKE_CURRENT_SOURCE_DIR}/wxDynFreqScalingTimer.cpp
set( WXWIDGETS_SOURCE_FILE_PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}/App.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/App_FindLowestStableCPUcoreVoltage.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/App_service_actions.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DynFreqScalingThread.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Controller/DynLibHandler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Controller/wxDynLibCPUcontroller.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Controller/wxDynLibCPUcoreUsageGetter.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/FileSystem/GetFilenameWithoutExtension/GetFilenameWithoutExtension.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/InputOutput/WriteFileContent/WriteFileContent.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/wxWidgets/Controller/character_string/wxStringHelper.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/wxWidgets/icon/IconDrawer.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/wxWidgets/multithread/wxThreadBasedI_Thread.cpp
)

if( ${EXE_TYPE} STREQUAL "wxGUI")
  add_subdirectory(UserInterface)
endif(${EXE_TYPE} STREQUAL "wxGUI")

showMsg(3, "WXWIDGETS_SOURCE_FILE_PATHS: ${WXWIDGETS_SOURCE_FILE_PATHS}" )

set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${WXWIDGETS_SOURCE_FILE_PATHS}
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  PARENT_SCOPE  
  )

set(CMAKE_LIST_FILE_PATHS
  ${CMAKE_LIST_FILE_PATHS}
  ${CMAKE_CURRENT_LIST_FILE}
  PARENT_SCOPE
)

#see http://accountingplusplus.blogspot.de/2010/12/cmake-wxwidgets.html :
#Set wxWidgets_USE_DEBUG to "ON" to link against WxWidgets debug libraries.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
set( wxWidgets_USE_DEBUG ON
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  PARENT_SCOPE )
endif()
message("wxWidgets_USE_DEBUG: ${wxWidgets_USE_DEBUG}")
#Set to appropriate version--releae, debug, 32/64 bit
#Use wxWidgets >= v.2.9 as it provides an appropriate task bar support
if( NOT DEFINED WXWIDGETS_SETUP_H_PATH )
  message("warning: WXWIDGETS_SETUP_H_PATH is NOT defined->compiling will fail"
   "on Linux it may be like: /usr/lib/i386-linux-gnu/wx/include/gtk2-unicode-debug-2.8")
endif( NOT DEFINED WXWIDGETS_SETUP_H_PATH )
if( NOT DEFINED WXWIDGETS_INCLUDE_DIR )
  message("warning: WXWIDGETS_INCLUDE_DIR is NOT defined->compiling will fail"
   "on Linux it may be like: /usr/include/wx-2.8")
endif( NOT DEFINED WXWIDGETS_INCLUDE_DIR )
  
set(INCLUDE_DIRS
  ${INCLUDE_DIRS}
  ${WXWIDGETS_SETUP_H_PATH}
  ${WXWIDGETS_INCLUDE_DIR}
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  PARENT_SCOPE)

#include_directories( ${INCLUDE_DIRS} )

#https://wiki.wxwidgets.org/CMake
#set(wxWidgets_CONFIGURATION mswu)
#find_package(wxWidgets COMPONENTS core base REQUIRED)
#include(${wxWidgets_USE_FILE})

set( LIBRARIES
  ${LIBRARIES}
  ${wxWidgets_USE_FILE}
  PARENT_SCOPE )

if (WIN32)
  add_definitions(-D__WXMSW__)
  set(CXX_DEFINITIONS
    ${CXX_DEFINITIONS}
    -D__WXMSW__
    PARENT_SCOPE
  )
endif (WIN32)

if (UNIX)
  #add_definitions(-D__WXGTK__)
  set(CXX_DEFINITIONS
    ${CXX_DEFINITIONS}
    -D__WXGTK__
    PARENT_SCOPE
  )
  set( LIBRARIES
    ${LIBRARIES}
    #libwx_gtk2ud_core-2.8.a
    #wx_baseud-2.8
    #wx_gtk2ud_adv-2.8
    PARENT_SCOPE
  )
endif (UNIX)

#add_executable(VTrans ${SOURCE_FILE_PATHS})
