
set( LINUX_SOURCE_FILE_PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}/MSRdeviceFile.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/Linux/GetNumberOfLogicalCPUcores/GetNumberOfLogicalCPUcores.cpp
  )

set( CPU_ACCESS_SOURCE_FILES
  ${LINUX_SOURCE_FILE_PATHS}
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  PARENT_SCOPE
  )

