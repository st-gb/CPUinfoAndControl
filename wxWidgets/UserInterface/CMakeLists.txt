
#if COMPILE_WITH_MSR_EXAMINATION

set(USER_INTERFACE_SOURCE_FILE_PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}/AboutDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/App_ShowTaskBarIcon.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CPUregisterReadAndWriteDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DynFreqScalingDlg.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/FloatValidator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/FreqAndVoltageSettingDlg.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MainFrame.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TaskBarIcon.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/wxExamineCPUregistersDialog.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/character_string/format_as_string.cpp
)

message("USER_INTERFACE_SOURCE_FILE_PATHS: ${USER_INTERFACE_SOURCE_FILE_PATHS}" )

set(WXWIDGETS_SOURCE_FILE_PATHS
  ${WXWIDGETS_SOURCE_FILE_PATHS}
  ${USER_INTERFACE_SOURCE_FILE_PATHS}
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  PARENT_SCOPE
  )

