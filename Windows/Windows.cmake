add_definitions(
  -DCOMPILE_WITH_NAMED_WINDOWS_PIPE
  -DCOMPILE_WITH_INTER_PROCESS_COMMUNICATION
  -DCOMPILE_WITH_CPU_SCALING
  -DCOMPILE_WITH_OTHER_DVFS_ACCESS)

set(CmnSrcMSW ${COMMON_SOURCECODE_ROOT_PATH}/OperatingSystem/Windows)
set(MSWsrcFilePaths
  ModelData/InstableCPUcoreOperationDetection.cpp
  Windows/CalculationThread.cpp
  Windows/InstableCPUcoreOperationDetection.cpp
  Windows/NamedPipe/NamedPipeClient.cpp
  Windows/WinRing0/WinRing0_1_3RunTimeDynLinked.cpp
  ${CmnSrcMSW}/ErrorCode/ErrorCodeFromGetLastErrorToString.cpp
  ${CmnSrcMSW}/ErrorCode/GetErrorMessageFromErrorCode.cpp
  ${CmnSrcMSW}/ErrorCode/GetErrorMessageFromLastErrorCode.cpp
  ${CmnSrcMSW}/ErrorCode/LocalLanguageMessageFromErrorCode.cpp
  ${CmnSrcMSW}/GetWindowsVersion/GetWindowsVersion.cpp
  ${CmnSrcMSW}/HardwareAccess/HardwareAccessError/GetHardwareAccessErrorDescription.cpp
  ${CmnSrcMSW}/Logger/LogEntryOutputter.cpp
  ${CmnSrcMSW}/multithread/Thread.cpp
  ${CmnSrcMSW}/NumberOfLogicalCPUcores/GetNumberOfLogicalCPUs.cpp

  ${CmnSrcMSW}/PowerProfAccess/I_PowerProfDynLinked.cpp
  ${CmnSrcMSW}/PowerProfAccess/PowerProfDynLinked.cpp
  ${CmnSrcMSW}/PowerProfAccess/PowerProfAccessUntilWin6/PowerProfUntilWin6DynLinked.cpp
  ${CmnSrcMSW}/PowerProfAccess/PowerProfAccessFromWin6/PowerProfFromWin6DynLinked.cpp
  ${CmnSrcMSW}/Service/ServiceBase.cpp
  )
#if(DEFINED $ENV{WINRING0_VERSIONS_TOP_DIR})
  message($ENV{WINRING0_VERSIONS_TOP_DIR})
  set( INCLUDE_DIRS 
    ${INCLUDE_DIRS}
    # path top of "xercesc", e.g. "xerces-c-3.2.2\src".
    #Not needed if e.g. in installed via "make install" in MinGW.
    ${ApacheXercesInclPath}
    ${WinRing0InclPath}
    )

set(SOURCE_FILE_PATHS ${SOURCE_FILE_PATHS} ${MSWsrcFilePaths} )
link_directories(${ApacheXercesLibPath})
#else()
#  message(--warning " WinRing0Path is NOT set")
#endif()

