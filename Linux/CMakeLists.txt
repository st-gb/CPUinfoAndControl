
set( LINUX_SOURCE_FILE_PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}/MSRdeviceFile.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/Linux/GetNumberOfLogicalCPUcores/GetNumberOfLogicalCPUcores.cpp
  )

set( CPU_ACCESS_SOURCE_FILES
  ${LINUX_SOURCE_FILE_PATHS}
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  PARENT_SCOPE
  )

if( ${EXE_TYPE} STREQUAL "UNIX_service")
  set(SOURCE_FILE_PATHS
    ${SOURCE_FILE_PATHS}
    ${CMAKE_CURRENT_SOURCE_DIR}/daemon/CPUcontrolDaemon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/daemon/daemon_functions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/daemon/daemon_main.cpp
    #PARENT_SCOPE makes changes made in this script visible to the caller of this file
    PARENT_SCOPE
  )
endif()