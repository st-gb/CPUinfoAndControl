
#see http://stackoverflow.com/questions/8934295/add-source-in-a-subdirectory-to-a-cmake-project

#If this file was processed via add_subdirectory(...)
#CMAKE_CURRENT_SOURCE_DIR is the path of the 1st processed CMakeLists.txt.
#set(DirOfThisScript ${CMAKE_CURRENT_SOURCE_DIR})

#If this file was processed via include(...)
set(DirOfThisScript ${CMAKE_CURRENT_LIST_DIR})

set(CmnSrcWx ${COMMON_SOURCECODE_ROOT_PATH}/wxWidgets)
#  ${CMAKE_CURRENT_SOURCE_DIR}/wxDynFreqScalingTimer.cpp
set( WXWIDGETS_SOURCE_FILE_PATHS
  ${DirOfThisScript}/DynFreqScalingThread.cpp
  ${DirOfThisScript}/Controller/DynLibHandler.cpp
  ${DirOfThisScript}/Controller/wxDynLibCPUcontroller.cpp
  ${DirOfThisScript}/Controller/wxDynLibCPUcoreUsageGetter.cpp
  ${CmnSrcWx}/Controller/character_string/wxStringHelper.cpp
  ${CmnSrcWx}/multithread/wxThreadBasedI_Thread.cpp
)

if( ${EXE_TYPE} STREQUAL "wxGUI")
  if(CMAKE_COMPILER_IS_GNUCC)#put GCC-specific stuff in here
    #"App.cpp:: error: cast from 'void*' to 'DWORD {aka long unsigned int}' 
    # loses precision [-fpermissive]
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-fpermissive")
  endif()

set( WXWIDGETS_SOURCE_FILE_PATHS
  ${WXWIDGETS_SOURCE_FILE_PATHS}
  ${DirOfThisScript}/App.cpp
  ${DirOfThisScript}/App_FindLowestStableCPUcoreVoltage.cpp
  ${DirOfThisScript}/App_service_actions.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/wxWidgets/icon/IconDrawer.cpp
  #NOT wxWidgets-related but necessary for the wxGUI
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/FileSystem/GetFilenameWithoutExtension/GetFilenameWithoutExtension.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/InputOutput/WriteFileContent/WriteFileContent.cpp
)
  #add_subdirectory(UserInterface)
  #Include it after "DirOfThisScript" has been used in this file because the 
  #included script alters this variable.
  include(${DirOfThisScript}/UserInterface/CMakeLists.txt)

  #TODO set wxWidgets_FIND_COMPONENTS ?
  #SET(wxWidgets_FIND_COMPONENTS core base adv)
  #for callFindwxWidgets.cmake
  SET(wxWidgets_USE_LIBS core base adv)#"adv" for GUI.
else()
  SET(wxWidgets_USE_LIBS core base)#for callFindwxWidgets.cmake
endif(${EXE_TYPE} STREQUAL "wxGUI")

showMsg(3, "WXWIDGETS_SOURCE_FILE_PATHS: ${WXWIDGETS_SOURCE_FILE_PATHS}" )

set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${WXWIDGETS_SOURCE_FILE_PATHS}
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  #PARENT_SCOPE
  )

set(CMAKE_LIST_FILE_PATHS
  ${CMAKE_LIST_FILE_PATHS}
  ${CMAKE_CURRENT_LIST_FILE}
  #PARENT_SCOPE
)

#TODO does not work?!
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
#see http://accountingplusplus.blogspot.de/2010/12/cmake-wxwidgets.html :
#Set wxWidgets_USE_DEBUG to "ON" to link against WxWidgets debug libraries.
#https://cmake.org/cmake/help/v3.0/module/FindwxWidgets.html :
set( wxWidgets_USE_DEBUG ON
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  #PARENT_SCOPE
  )
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
  #PARENT_SCOPE
  )
message("INCLUDE_DIRS set to:${INCLUDE_DIRS}")

#include_directories( ${INCLUDE_DIRS} )

if(setWxManually)
  include(${CmnSrcWx}/setWxManually.cmake)
else()
  set(wxWidgets_ROOT_DIR ${WXWIDGETS_SETUP_H_PATH})
  set(debugCallFindwxWidgets 1)

  include(${CmnSrcWx}/callFindwxWidgets.cmake)
  #callFindwxWidgets("core base adv")#function call
endif()

message(STATUS "wxWidgets_CONFIGURATION: ${wxWidgets_CONFIGURATION}")
