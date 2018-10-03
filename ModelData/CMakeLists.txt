
set( ATTRIBUTE_DATA_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/CPUcoreData.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CPUcoreData_AddDefaultVoltageForFreq.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/LogFileAttributes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ModelData.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/PerCPUcoreAttributes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/VoltageAndFreq.cpp
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  #PARENT_SCOPE
)

#PARENT_SCOPE for the same variable can be done only once within a CMake file?!
# So do it here for all builds (GUI, service, ...)

if( ${EXE_TYPE} STREQUAL "wxGUI")
  set( ATTRIBUTE_DATA_SOURCE_FILES
    ${ATTRIBUTE_DATA_SOURCE_FILES}
  ${CMAKE_CURRENT_SOURCE_DIR}/RegisterData.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SpecificCPUcoreActionAttributes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/UserInterfaceAttributes.cpp
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  PARENT_SCOPE
  )
endif()

if( ${EXE_TYPE} STREQUAL "UNIX_service")
  set( ATTRIBUTE_DATA_SOURCE_FILES
    ${ATTRIBUTE_DATA_SOURCE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/ServiceAttributes.cpp
    #PARENT_SCOPE makes changes made in this script visible to the caller of this file
    PARENT_SCOPE
  )
endif()
