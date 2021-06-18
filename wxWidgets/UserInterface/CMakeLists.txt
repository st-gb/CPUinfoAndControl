#If this file was processed via include(...)
set(DirOfThisScript ${CMAKE_CURRENT_LIST_DIR})

#if COMPILE_WITH_MSR_EXAMINATION

set(USER_INTERFACE_SOURCE_FILE_PATHS
  ${DirOfThisScript}/AboutDialog.cpp
  ${DirOfThisScript}/App_ShowTaskBarIcon.cpp
  ${DirOfThisScript}/CPUregisterReadAndWriteDialog.cpp
  ${DirOfThisScript}/DynFreqScalingDlg.cpp
  ${DirOfThisScript}/FloatValidator.cpp
  ${DirOfThisScript}/FreqAndVoltageSettingDlg.cpp
  ${DirOfThisScript}/MainFrame.cpp
  ${DirOfThisScript}/TaskBarIcon.cpp
  ${DirOfThisScript}/wxExamineCPUregistersDialog.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/Controller/character_string/format_as_string.cpp
)

showMsg(3, "USER_INTERFACE_SOURCE_FILE_PATHS: ${USER_INTERFACE_SOURCE_FILE_PATHS}" )

set(WXWIDGETS_SOURCE_FILE_PATHS
  ${WXWIDGETS_SOURCE_FILE_PATHS}
  ${USER_INTERFACE_SOURCE_FILE_PATHS}
  #PARENT_SCOPE makes changes made in this script visible to the caller of this file
  #PARENT_SCOPE
  )

