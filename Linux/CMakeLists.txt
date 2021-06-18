#sudo apt-get install libxerces-c-dev
#apt install libwxgtk3.0-0v5-dbg for debugging

#If this file was processed via include(...)
set(DirOfThisScript ${CMAKE_CURRENT_LIST_DIR})

set(LIBRARIES
  ${LIBRARIES}
  pthread
)

set(CmnSrcOS ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem)
set( CLIENT_SERVER_BASE_SOURCE_FILES
  ${CLIENT_SERVER_BASE_SOURCE_FILES}
  ${CmnSrcOS}/POSIX/multithread/pthreadBasedI_Thread.cpp
  ${CmnSrcOS}/Linux/EnglishMessageFromErrorCode/EnglishMessageFromErrorCode.cpp
  ${CmnSrcOS}/Linux/FileSystem/GetCurrentWorkingDir/GetCurrentWorkingDir.cpp
)

set( LINUX_SOURCE_FILE_PATHS
  ${DirOfThisScript}/MSRdeviceFile.cpp
  ${CmnSrcOS}/Linux/GetNumberOfLogicalCPUcores/GetNumberOfLogicalCPUcores.cpp
  )
#message("LINUX_SOURCE_FILE_PATHS: ${LINUX_SOURCE_FILE_PATHS}")

set( CPU_ACCESS_SOURCE_FILES
  ${LINUX_SOURCE_FILE_PATHS}
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  #PARENT_SCOPE
  )

if( ${EXE_TYPE} STREQUAL "UNIX_service")
  set(SOURCE_FILE_PATHS
    ${SOURCE_FILE_PATHS}
    ${DirOfThisScript}/daemon/CPUcontrolDaemon.cpp
    ${DirOfThisScript}/daemon/daemon_functions.cpp
    ${DirOfThisScript}/daemon/daemon_main.cpp
    #PARENT_SCOPE makes changes made in this script visible to the caller of this file
    #PARENT_SCOPE
  )
endif()