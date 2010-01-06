#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc-3.exe
CCC=g++.exe
CXX=g++.exe
FC=
AS=as.exe

# Macros
CND_PLATFORM=Cygwin_gcc3.x-Windows
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CurrentDir.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CPUcoreUsageGetterIWbemServices.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/GetWindowsVersion.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/HighLoadThreadAttributes.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MainController.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/MSRaccess.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CPUcoreUsageGetterNtQuerySystemInformation.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2DefaultVoltageForFrequency.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/DynFreqScalingThreadBase.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM/PentiumM_Controller.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/FreqAndVoltageSettingDlg.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf/PowerProfDynLinked.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/main.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Logger.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/App.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/LocalLanguageMessageFromErrorCode.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/tchar_conversion.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUcontroller.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUcontrollerAction.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/RegisterData.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/DynFreqScalingThread.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/stdio_secure.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/PStates.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/Pstate.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/ModelData.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/ErrorCodeFromGetLastErrorToString.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/CalculationThreadProc.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/AMD/Griffin/SAX2MainConfigHandler.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/WinRing0dynLinked.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/stdlib_mscv_extras.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf/PowerProfFromWin6DynLinked.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM/PentiumM_ClocksNotHaltedCPUcoreUsageGetter.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/FloatValidator.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/time_secure.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/wxDynFreqScalingTimer.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/NamedPipeServer.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/CPUcoreData.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/DynFreqScalingAccess.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/SpecificCPUcoreActionAttributes.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/XMLAccess.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSRaccessBase.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2_CPUspecificHandler.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/wxDynamicDialog.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/MainFrame.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/stdtstr.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller/wxStringHelper.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/SelectPstateAction.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/AMD/Griffin/GriffinController.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/DiscretionaryAccessControlList.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/DynFreqScalingDlg.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/ServiceBase.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/XercesHelper.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/Nehalem/NehalemController.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2MainConfigHandler.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/NamedPipeClient.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/CPUIDaccess.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/DynVoltAndFreqScaling.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUaccess.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CPUcontrolService.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/stdafx.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf/PowerProfUntilWin6DynLinked.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/CPUindependentHelper.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/Nehalem/NehalemClocksNotHaltedCPUcoreUsageGetter.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CalculationThread.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/VoltageAndFreq.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/string_secure.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/LocalLanguageMessageFromErrorCode.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/DynFreqScalingThread.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/ModelData/wxCPUcoreID.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller/wxServiceSocketClient.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/AMD/Griffin/ClocksNotHaltedCPUcoreUsageGetter.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Debug.mk dist/Debug/Cygwin_gcc3.x-Windows/x86infoandcontrolgui_windows.exe

dist/Debug/Cygwin_gcc3.x-Windows/x86infoandcontrolgui_windows.exe: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/Cygwin_gcc3.x-Windows
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/x86infoandcontrolgui_windows ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CurrentDir.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/CurrentDir.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CurrentDir.o ../../Windows/CurrentDir.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CPUcoreUsageGetterIWbemServices.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/CPUcoreUsageGetterIWbemServices.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CPUcoreUsageGetterIWbemServices.o ../../Windows/CPUcoreUsageGetterIWbemServices.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/GetWindowsVersion.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/GetWindowsVersion.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/GetWindowsVersion.o ../../Windows/GetWindowsVersion.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/HighLoadThreadAttributes.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/HighLoadThreadAttributes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/HighLoadThreadAttributes.o ../../ModelData/HighLoadThreadAttributes.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MainController.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MainController.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MainController.o ../../Controller/MainController.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/MSRaccess.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/MSRaccess.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/MSRaccess.o ../../wxWidgets/MSRaccess.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CPUcoreUsageGetterNtQuerySystemInformation.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/CPUcoreUsageGetterNtQuerySystemInformation.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CPUcoreUsageGetterNtQuerySystemInformation.o ../../Windows/CPUcoreUsageGetterNtQuerySystemInformation.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2DefaultVoltageForFrequency.o: nbproject/Makefile-${CND_CONF}.mk ../../Xerces/SAX2DefaultVoltageForFrequency.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2DefaultVoltageForFrequency.o ../../Xerces/SAX2DefaultVoltageForFrequency.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/DynFreqScalingThreadBase.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/DynFreqScalingThreadBase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/DynFreqScalingThreadBase.o ../../Controller/DynFreqScalingThreadBase.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM/PentiumM_Controller.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/Intel/PentiumM/PentiumM_Controller.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM/PentiumM_Controller.o ../../Controller/Intel/PentiumM/PentiumM_Controller.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/FreqAndVoltageSettingDlg.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/UserInterface/FreqAndVoltageSettingDlg.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/FreqAndVoltageSettingDlg.o ../../wxWidgets/UserInterface/FreqAndVoltageSettingDlg.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf/PowerProfDynLinked.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/PowerProf/PowerProfDynLinked.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf/PowerProfDynLinked.o ../../Windows/PowerProf/PowerProfDynLinked.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/main.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/main.o ../../Windows/main.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Logger.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/Logger.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Logger.o ../../Controller/Logger.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/App.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/App.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/App.o ../../wxWidgets/App.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/LocalLanguageMessageFromErrorCode.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/LocalLanguageMessageFromErrorCode.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/LocalLanguageMessageFromErrorCode.o ../../Windows/LocalLanguageMessageFromErrorCode.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/tchar_conversion.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/tchar_conversion.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/tchar_conversion.o ../../Controller/tchar_conversion.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUcontroller.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/I_CPUcontroller.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUcontroller.o ../../Controller/I_CPUcontroller.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUcontrollerAction.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/I_CPUcontrollerAction.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUcontrollerAction.o ../../Controller/I_CPUcontrollerAction.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/RegisterData.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/RegisterData.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/RegisterData.o ../../ModelData/RegisterData.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/DynFreqScalingThread.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/DynFreqScalingThread.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/DynFreqScalingThread.o ../../wxWidgets/DynFreqScalingThread.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/stdio_secure.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MSVC_adaption/stdio_secure.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/stdio_secure.o ../../Controller/MSVC_adaption/stdio_secure.c

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/PStates.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/PStates.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/PStates.o ../../ModelData/PStates.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/Pstate.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/Pstate.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/Pstate.o ../../ModelData/Pstate.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/ModelData.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/ModelData.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/ModelData.o ../../ModelData/ModelData.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/ErrorCodeFromGetLastErrorToString.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/ErrorCodeFromGetLastErrorToString.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/ErrorCodeFromGetLastErrorToString.o ../../Windows/ErrorCodeFromGetLastErrorToString.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/CalculationThreadProc.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/CalculationThreadProc.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/CalculationThreadProc.o ../../Controller/CalculationThreadProc.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/AMD/Griffin/SAX2MainConfigHandler.o: nbproject/Makefile-${CND_CONF}.mk ../../Xerces/AMD/Griffin/SAX2MainConfigHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/AMD/Griffin
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/AMD/Griffin/SAX2MainConfigHandler.o ../../Xerces/AMD/Griffin/SAX2MainConfigHandler.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/WinRing0dynLinked.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/WinRing0dynLinked.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/WinRing0dynLinked.o ../../Windows/WinRing0dynLinked.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/stdlib_mscv_extras.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MSVC_adaption/stdlib_mscv_extras.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/stdlib_mscv_extras.o ../../Controller/MSVC_adaption/stdlib_mscv_extras.c

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf/PowerProfFromWin6DynLinked.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/PowerProf/PowerProfFromWin6DynLinked.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf/PowerProfFromWin6DynLinked.o ../../Windows/PowerProf/PowerProfFromWin6DynLinked.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM/PentiumM_ClocksNotHaltedCPUcoreUsageGetter.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/Intel/PentiumM/PentiumM_ClocksNotHaltedCPUcoreUsageGetter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM/PentiumM_ClocksNotHaltedCPUcoreUsageGetter.o ../../Controller/Intel/PentiumM/PentiumM_ClocksNotHaltedCPUcoreUsageGetter.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/FloatValidator.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/UserInterface/FloatValidator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/FloatValidator.o ../../wxWidgets/UserInterface/FloatValidator.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/time_secure.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MSVC_adaption/time_secure.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/time_secure.o ../../Controller/MSVC_adaption/time_secure.c

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/wxDynFreqScalingTimer.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/wxDynFreqScalingTimer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/wxDynFreqScalingTimer.o ../../wxWidgets/wxDynFreqScalingTimer.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/NamedPipeServer.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/NamedPipeServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/NamedPipeServer.o ../../Windows/NamedPipeServer.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/CPUcoreData.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/CPUcoreData.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/CPUcoreData.o ../../ModelData/CPUcoreData.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/DynFreqScalingAccess.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/DynFreqScalingAccess.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/DynFreqScalingAccess.o ../../Windows/DynFreqScalingAccess.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/SpecificCPUcoreActionAttributes.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/SpecificCPUcoreActionAttributes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/SpecificCPUcoreActionAttributes.o ../../ModelData/SpecificCPUcoreActionAttributes.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/XMLAccess.o: nbproject/Makefile-${CND_CONF}.mk ../../Xerces/XMLAccess.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/XMLAccess.o ../../Xerces/XMLAccess.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSRaccessBase.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MSRaccessBase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSRaccessBase.o ../../Controller/MSRaccessBase.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2_CPUspecificHandler.o: nbproject/Makefile-${CND_CONF}.mk ../../Xerces/SAX2_CPUspecificHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2_CPUspecificHandler.o ../../Xerces/SAX2_CPUspecificHandler.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/wxDynamicDialog.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/UserInterface/wxDynamicDialog.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/wxDynamicDialog.o ../../wxWidgets/UserInterface/wxDynamicDialog.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/MainFrame.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/UserInterface/MainFrame.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/MainFrame.o ../../wxWidgets/UserInterface/MainFrame.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/stdtstr.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/stdtstr.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/stdtstr.o ../../Controller/stdtstr.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller/wxStringHelper.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/Controller/wxStringHelper.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller/wxStringHelper.o ../../wxWidgets/Controller/wxStringHelper.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/SelectPstateAction.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/SelectPstateAction.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/SelectPstateAction.o ../../Controller/SelectPstateAction.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/AMD/Griffin/GriffinController.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/AMD/Griffin/GriffinController.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/AMD/Griffin
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/AMD/Griffin/GriffinController.o ../../Controller/AMD/Griffin/GriffinController.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/DiscretionaryAccessControlList.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/DiscretionaryAccessControlList.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/DiscretionaryAccessControlList.o ../../Windows/DiscretionaryAccessControlList.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/DynFreqScalingDlg.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/UserInterface/DynFreqScalingDlg.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/DynFreqScalingDlg.o ../../wxWidgets/UserInterface/DynFreqScalingDlg.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/ServiceBase.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/ServiceBase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/ServiceBase.o ../../Windows/ServiceBase.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/XercesHelper.o: nbproject/Makefile-${CND_CONF}.mk ../../Xerces/XercesHelper.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/XercesHelper.o ../../Xerces/XercesHelper.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/Nehalem/NehalemController.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/Intel/Nehalem/NehalemController.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/Nehalem
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/Nehalem/NehalemController.o ../../Controller/Intel/Nehalem/NehalemController.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2MainConfigHandler.o: nbproject/Makefile-${CND_CONF}.mk ../../Xerces/SAX2MainConfigHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2MainConfigHandler.o ../../Xerces/SAX2MainConfigHandler.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/NamedPipeClient.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/NamedPipeClient.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/NamedPipeClient.o ../../Windows/NamedPipeClient.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/CPUIDaccess.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/CPUIDaccess.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/CPUIDaccess.o ../../wxWidgets/CPUIDaccess.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/DynVoltAndFreqScaling.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/DynVoltAndFreqScaling.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/DynVoltAndFreqScaling.o ../../Controller/DynVoltAndFreqScaling.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUaccess.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/I_CPUaccess.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUaccess.o ../../Controller/I_CPUaccess.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CPUcontrolService.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/CPUcontrolService.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CPUcontrolService.o ../../Windows/CPUcontrolService.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/stdafx.o: nbproject/Makefile-${CND_CONF}.mk ../../stdafx.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/stdafx.o ../../stdafx.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf/PowerProfUntilWin6DynLinked.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/PowerProf/PowerProfUntilWin6DynLinked.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf/PowerProfUntilWin6DynLinked.o ../../Windows/PowerProf/PowerProfUntilWin6DynLinked.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/CPUindependentHelper.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/CPUindependentHelper.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/CPUindependentHelper.o ../../Controller/CPUindependentHelper.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/Nehalem/NehalemClocksNotHaltedCPUcoreUsageGetter.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/Intel/Nehalem/NehalemClocksNotHaltedCPUcoreUsageGetter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/Nehalem
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/Nehalem/NehalemClocksNotHaltedCPUcoreUsageGetter.o ../../Controller/Intel/Nehalem/NehalemClocksNotHaltedCPUcoreUsageGetter.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CalculationThread.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/CalculationThread.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CalculationThread.o ../../Windows/CalculationThread.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/VoltageAndFreq.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/VoltageAndFreq.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/VoltageAndFreq.o ../../ModelData/VoltageAndFreq.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/string_secure.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MSVC_adaption/string_secure.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/string_secure.o ../../Controller/MSVC_adaption/string_secure.c

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/LocalLanguageMessageFromErrorCode.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MSVC_adaption/LocalLanguageMessageFromErrorCode.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/LocalLanguageMessageFromErrorCode.o ../../Controller/MSVC_adaption/LocalLanguageMessageFromErrorCode.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/DynFreqScalingThread.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/DynFreqScalingThread.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/DynFreqScalingThread.o ../../Windows/DynFreqScalingThread.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/ModelData/wxCPUcoreID.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/ModelData/wxCPUcoreID.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/ModelData
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/ModelData/wxCPUcoreID.o ../../wxWidgets/ModelData/wxCPUcoreID.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller/wxServiceSocketClient.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/Controller/wxServiceSocketClient.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller/wxServiceSocketClient.o ../../wxWidgets/Controller/wxServiceSocketClient.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/AMD/Griffin/ClocksNotHaltedCPUcoreUsageGetter.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/AMD/Griffin/ClocksNotHaltedCPUcoreUsageGetter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/AMD/Griffin
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXMSW__ -DNOMINMAX -DCOMPILE_WITH_XERCES -DINCLUDE_POWERPROF_FOR_WINDOWS_ABOVE_VERSION6 -D_GLIBCXX_USE_WCHAR_T_ -D_GLIBCXX_OSTREAM_ -DDEFINE_POWER_DATA_ACCESSOR -D_DEBUG -D_GLIBCXX_DEBUG -I../.. -I../../Controller/MSVC_adaption -I/cygdrive/C/wxWidgets-2.8.10/include -I/cygdrive/C/wxWidgets-2.8.10/msw-cygwin-static-debug/lib/wx/include/msw-ansi-debug-static-2.8 -I/cygdrive/C/Libs/xerces-c-3.0.1-x86-windows-vc-9.0/xerces-c-3.0.1-x86-windows-vc-9.0/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/AMD/Griffin/ClocksNotHaltedCPUcoreUsageGetter.o ../../Controller/AMD/Griffin/ClocksNotHaltedCPUcoreUsageGetter.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/Cygwin_gcc3.x-Windows/x86infoandcontrolgui_windows.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
