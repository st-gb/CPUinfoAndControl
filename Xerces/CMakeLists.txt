
#see http://stackoverflow.com/questions/8934295/add-source-in-a-subdirectory-to-a-cmake-project

set(APACHE_XERCES_SOURCE_FILE_PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}/LogOutputHandler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/PStateConfig.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SAX2CPUtypeConfiguration.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SAX2MainConfigHandler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SAX2VoltagesForFrequencyHandler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/XercesHelper.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/XMLAccess.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Xerces/ConvertXercesStringToStdWstring.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Xerces/DocumentLocationSAX2Handler.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Xerces/XercesAttributesHelper.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Xerces/XercesString.cpp
)

#    ${CMAKE_CURRENT_SOURCE_DIR}/UserInterface/
if( ${EXE_TYPE} STREQUAL "wxGUI")
  set(APACHE_XERCES_SOURCE_FILE_PATHS
    ${APACHE_XERCES_SOURCE_FILE_PATHS}
    ${CMAKE_CURRENT_SOURCE_DIR}/UserInterface/SAX2IPCcurrentCPUdataHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UserInterface/SAX2UserInterfaceConfigHandler.cpp
    )
endif( ${EXE_TYPE} STREQUAL "wxGUI")

set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${APACHE_XERCES_SOURCE_FILE_PATHS}
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  PARENT_SCOPE
)

set( LIBRARIES
  ${LIBRARIES}
  xerces-c
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  PARENT_SCOPE
)
