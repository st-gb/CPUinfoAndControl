
set( ATTRIBUTE_DATA_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/CPUcoreData.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CPUcoreData_AddDefaultVoltageForFreq.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/LogFileAttributes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ModelData.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/PerCPUcoreAttributes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SpecificCPUcoreActionAttributes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/UserInterfaceAttributes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/VoltageAndFreq.cpp
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  PARENT_SCOPE
)
