
#see http://stackoverflow.com/questions/8934295/add-source-in-a-subdirectory-to-a-cmake-project

set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${CMAKE_CURRENT_SOURCE_DIR}/BuildTimeString.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CPUcontrolBase.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DynFreqScalingThreadBase.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/export_functions.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/I_CPUaccess.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MainController.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CPU-related/I_CPUcontroller.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CPU-related/GetInterpolatedVoltageFromFreq.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CPU-related/UsageGetterAndControllerBase.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/InputOutput/ReadFileContent/ReadFileContent.cpp
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  #PARENT_SCOPE
)

if( ${EXE_TYPE} STREQUAL "UNIX_service")
  set(SOURCE_FILE_PATHS
    ${SOURCE_FILE_PATHS}
    ${CMAKE_CURRENT_SOURCE_DIR}/CPUcoreLoadBasedDynVoltnFreqScaling.cpp
    #PARENT_SCOPE makes changes made in this script visible to the caller of this file
    #PARENT_SCOPE
  )
endif()

#PARENT_SCOPE can be done only once within a CMake file?! so do it here for all
# builds (GUI, service, ...)
set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  PARENT_SCOPE
)
