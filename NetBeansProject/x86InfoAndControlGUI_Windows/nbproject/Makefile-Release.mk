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
CC=gcc-4.exe
CCC=g++-4.exe
CXX=g++-4.exe
FC=
AS=

# Macros
CND_PLATFORM=Cygwin_4.x-Windows
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/console.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/WinRing0/WinRing0dynLinked.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CurrentDir.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/GetWindowsVersion.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxGUI.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/WinRing0/WinRing0_1_3RunTimeDynLinked.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/HighLoadThreadAttributes.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MainController.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/MSRaccess.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2DefaultVoltageForFrequency.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/DynFreqScalingThreadBase.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Windows.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/MSRdeviceFile.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller/wxServiceSocketServer.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM/PentiumM_Controller.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/FreqAndVoltageSettingDlg.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf/PowerProfDynLinked.o \
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
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/WinRing0/WinRing0_1_3LoadTimeDynLinked.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/stdlib_mscv_extras.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf/PowerProfFromWin6DynLinked.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM/PentiumM_ClocksNotHaltedCPUcoreUsageGetter.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/daemon.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/FloatValidator.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/time_secure.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/wxDynFreqScalingTimer.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/NamedPipeServer.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/CPUcoreData.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/DynFreqScalingAccess.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/SpecificCPUcoreActionAttributes.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSRaccessBase.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/XMLAccess.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2_CPUspecificHandler.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/wxDynamicDialog.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/MainFrame.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/stdtstr.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller/wxStringHelper.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/SelectPstateAction.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/AMD/Griffin/GriffinController.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/DiscretionaryAccessControlList.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/DynFreqScalingDlg.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/main.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/XercesHelper.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/Nehalem/NehalemController.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2MainConfigHandler.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/NamedPipeClient.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/CPUIDaccess.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/DynVoltAndFreqScaling.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUaccess.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/stdafx.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf/PowerProfUntilWin6DynLinked.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/EnglishMessageFromErrorCode.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/CPUindependentHelper.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/Nehalem/NehalemClocksNotHaltedCPUcoreUsageGetter.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Linux.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CalculationThread.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/VoltageAndFreq.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/string_secure.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/CalculationThread.o \
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
	${MAKE}  -f nbproject/Makefile-Release.mk dist/Release/Cygwin_4.x-Windows/x86infoandcontrolgui_windows.exe

dist/Release/Cygwin_4.x-Windows/x86infoandcontrolgui_windows.exe: ${OBJECTFILES}
	${MKDIR} -p dist/Release/Cygwin_4.x-Windows
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/x86infoandcontrolgui_windows ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/console.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/console.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/console.o ../../Controller/console.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/WinRing0/WinRing0dynLinked.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/WinRing0/WinRing0dynLinked.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/WinRing0
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/WinRing0/WinRing0dynLinked.o ../../Windows/WinRing0/WinRing0dynLinked.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CurrentDir.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/CurrentDir.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CurrentDir.o ../../Windows/CurrentDir.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/GetWindowsVersion.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/GetWindowsVersion.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/GetWindowsVersion.o ../../Windows/GetWindowsVersion.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxGUI.o: nbproject/Makefile-${CND_CONF}.mk ../../wxGUI.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxGUI.o ../../wxGUI.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/WinRing0/WinRing0_1_3RunTimeDynLinked.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/WinRing0/WinRing0_1_3RunTimeDynLinked.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/WinRing0
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/WinRing0/WinRing0_1_3RunTimeDynLinked.o ../../Windows/WinRing0/WinRing0_1_3RunTimeDynLinked.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/HighLoadThreadAttributes.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/HighLoadThreadAttributes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/HighLoadThreadAttributes.o ../../ModelData/HighLoadThreadAttributes.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MainController.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MainController.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MainController.o ../../Controller/MainController.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/MSRaccess.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/MSRaccess.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/MSRaccess.o ../../wxWidgets/MSRaccess.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2DefaultVoltageForFrequency.o: nbproject/Makefile-${CND_CONF}.mk ../../Xerces/SAX2DefaultVoltageForFrequency.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2DefaultVoltageForFrequency.o ../../Xerces/SAX2DefaultVoltageForFrequency.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/DynFreqScalingThreadBase.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/DynFreqScalingThreadBase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/DynFreqScalingThreadBase.o ../../Controller/DynFreqScalingThreadBase.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Windows.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/Windows.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Windows.o ../../Controller/Windows.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/MSRdeviceFile.o: nbproject/Makefile-${CND_CONF}.mk ../../Linux/MSRdeviceFile.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/MSRdeviceFile.o ../../Linux/MSRdeviceFile.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller/wxServiceSocketServer.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/Controller/wxServiceSocketServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller/wxServiceSocketServer.o ../../wxWidgets/Controller/wxServiceSocketServer.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM/PentiumM_Controller.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/Intel/PentiumM/PentiumM_Controller.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM/PentiumM_Controller.o ../../Controller/Intel/PentiumM/PentiumM_Controller.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/Controller.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller.o ../../wxWidgets/Controller.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/FreqAndVoltageSettingDlg.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/UserInterface/FreqAndVoltageSettingDlg.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/FreqAndVoltageSettingDlg.o ../../wxWidgets/UserInterface/FreqAndVoltageSettingDlg.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf/PowerProfDynLinked.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/PowerProf/PowerProfDynLinked.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf/PowerProfDynLinked.o ../../Windows/PowerProf/PowerProfDynLinked.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Logger.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/Logger.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Logger.o ../../Controller/Logger.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/App.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/App.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/App.o ../../wxWidgets/App.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/LocalLanguageMessageFromErrorCode.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/LocalLanguageMessageFromErrorCode.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/LocalLanguageMessageFromErrorCode.o ../../Windows/LocalLanguageMessageFromErrorCode.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/tchar_conversion.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/tchar_conversion.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/tchar_conversion.o ../../Controller/tchar_conversion.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUcontroller.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/I_CPUcontroller.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUcontroller.o ../../Controller/I_CPUcontroller.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUcontrollerAction.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/I_CPUcontrollerAction.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUcontrollerAction.o ../../Controller/I_CPUcontrollerAction.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/RegisterData.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/RegisterData.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/RegisterData.o ../../ModelData/RegisterData.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/DynFreqScalingThread.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/DynFreqScalingThread.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/DynFreqScalingThread.o ../../wxWidgets/DynFreqScalingThread.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/stdio_secure.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MSVC_adaption/stdio_secure.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/stdio_secure.o ../../Controller/MSVC_adaption/stdio_secure.c

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/PStates.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/PStates.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/PStates.o ../../ModelData/PStates.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/Pstate.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/Pstate.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/Pstate.o ../../ModelData/Pstate.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/ModelData.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/ModelData.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/ModelData.o ../../ModelData/ModelData.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/ErrorCodeFromGetLastErrorToString.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/ErrorCodeFromGetLastErrorToString.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/ErrorCodeFromGetLastErrorToString.o ../../Windows/ErrorCodeFromGetLastErrorToString.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/CalculationThreadProc.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/CalculationThreadProc.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/CalculationThreadProc.o ../../Controller/CalculationThreadProc.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/AMD/Griffin/SAX2MainConfigHandler.o: nbproject/Makefile-${CND_CONF}.mk ../../Xerces/AMD/Griffin/SAX2MainConfigHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/AMD/Griffin
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/AMD/Griffin/SAX2MainConfigHandler.o ../../Xerces/AMD/Griffin/SAX2MainConfigHandler.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/WinRing0/WinRing0_1_3LoadTimeDynLinked.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/WinRing0/WinRing0_1_3LoadTimeDynLinked.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/WinRing0
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/WinRing0/WinRing0_1_3LoadTimeDynLinked.o ../../Windows/WinRing0/WinRing0_1_3LoadTimeDynLinked.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/stdlib_mscv_extras.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MSVC_adaption/stdlib_mscv_extras.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/stdlib_mscv_extras.o ../../Controller/MSVC_adaption/stdlib_mscv_extras.c

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf/PowerProfFromWin6DynLinked.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/PowerProf/PowerProfFromWin6DynLinked.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf/PowerProfFromWin6DynLinked.o ../../Windows/PowerProf/PowerProfFromWin6DynLinked.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM/PentiumM_ClocksNotHaltedCPUcoreUsageGetter.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/Intel/PentiumM/PentiumM_ClocksNotHaltedCPUcoreUsageGetter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM/PentiumM_ClocksNotHaltedCPUcoreUsageGetter.o ../../Controller/Intel/PentiumM/PentiumM_ClocksNotHaltedCPUcoreUsageGetter.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/daemon.o: nbproject/Makefile-${CND_CONF}.mk ../../Linux/daemon.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/daemon.o ../../Linux/daemon.c

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/FloatValidator.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/UserInterface/FloatValidator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/FloatValidator.o ../../wxWidgets/UserInterface/FloatValidator.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/time_secure.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MSVC_adaption/time_secure.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/time_secure.o ../../Controller/MSVC_adaption/time_secure.c

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/wxDynFreqScalingTimer.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/wxDynFreqScalingTimer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/wxDynFreqScalingTimer.o ../../wxWidgets/wxDynFreqScalingTimer.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/NamedPipeServer.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/NamedPipeServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/NamedPipeServer.o ../../Windows/NamedPipeServer.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/CPUcoreData.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/CPUcoreData.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/CPUcoreData.o ../../ModelData/CPUcoreData.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/DynFreqScalingAccess.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/DynFreqScalingAccess.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/DynFreqScalingAccess.o ../../Windows/DynFreqScalingAccess.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/SpecificCPUcoreActionAttributes.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/SpecificCPUcoreActionAttributes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/SpecificCPUcoreActionAttributes.o ../../ModelData/SpecificCPUcoreActionAttributes.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSRaccessBase.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MSRaccessBase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSRaccessBase.o ../../Controller/MSRaccessBase.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/XMLAccess.o: nbproject/Makefile-${CND_CONF}.mk ../../Xerces/XMLAccess.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/XMLAccess.o ../../Xerces/XMLAccess.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2_CPUspecificHandler.o: nbproject/Makefile-${CND_CONF}.mk ../../Xerces/SAX2_CPUspecificHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2_CPUspecificHandler.o ../../Xerces/SAX2_CPUspecificHandler.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/wxDynamicDialog.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/UserInterface/wxDynamicDialog.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/wxDynamicDialog.o ../../wxWidgets/UserInterface/wxDynamicDialog.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/MainFrame.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/UserInterface/MainFrame.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/MainFrame.o ../../wxWidgets/UserInterface/MainFrame.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/stdtstr.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/stdtstr.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/stdtstr.o ../../Controller/stdtstr.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller/wxStringHelper.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/Controller/wxStringHelper.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller/wxStringHelper.o ../../wxWidgets/Controller/wxStringHelper.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/SelectPstateAction.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/SelectPstateAction.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/SelectPstateAction.o ../../Controller/SelectPstateAction.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/AMD/Griffin/GriffinController.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/AMD/Griffin/GriffinController.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/AMD/Griffin
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/AMD/Griffin/GriffinController.o ../../Controller/AMD/Griffin/GriffinController.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/DiscretionaryAccessControlList.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/DiscretionaryAccessControlList.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/DiscretionaryAccessControlList.o ../../Windows/DiscretionaryAccessControlList.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/DynFreqScalingDlg.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/UserInterface/DynFreqScalingDlg.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/DynFreqScalingDlg.o ../../wxWidgets/UserInterface/DynFreqScalingDlg.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/main.o: nbproject/Makefile-${CND_CONF}.mk ../../main.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/main.o ../../main.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/XercesHelper.o: nbproject/Makefile-${CND_CONF}.mk ../../Xerces/XercesHelper.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/XercesHelper.o ../../Xerces/XercesHelper.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/Nehalem/NehalemController.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/Intel/Nehalem/NehalemController.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/Nehalem
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/Nehalem/NehalemController.o ../../Controller/Intel/Nehalem/NehalemController.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2MainConfigHandler.o: nbproject/Makefile-${CND_CONF}.mk ../../Xerces/SAX2MainConfigHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2MainConfigHandler.o ../../Xerces/SAX2MainConfigHandler.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/NamedPipeClient.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/NamedPipeClient.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/NamedPipeClient.o ../../Windows/NamedPipeClient.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/CPUIDaccess.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/CPUIDaccess.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/CPUIDaccess.o ../../wxWidgets/CPUIDaccess.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/DynVoltAndFreqScaling.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/DynVoltAndFreqScaling.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/DynVoltAndFreqScaling.o ../../Controller/DynVoltAndFreqScaling.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUaccess.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/I_CPUaccess.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUaccess.o ../../Controller/I_CPUaccess.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/stdafx.o: nbproject/Makefile-${CND_CONF}.mk ../../stdafx.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/stdafx.o ../../stdafx.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf/PowerProfUntilWin6DynLinked.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/PowerProf/PowerProfUntilWin6DynLinked.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/PowerProf/PowerProfUntilWin6DynLinked.o ../../Windows/PowerProf/PowerProfUntilWin6DynLinked.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/EnglishMessageFromErrorCode.o: nbproject/Makefile-${CND_CONF}.mk ../../Linux/EnglishMessageFromErrorCode.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/EnglishMessageFromErrorCode.o ../../Linux/EnglishMessageFromErrorCode.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/CPUindependentHelper.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/CPUindependentHelper.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/CPUindependentHelper.o ../../Controller/CPUindependentHelper.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/Nehalem/NehalemClocksNotHaltedCPUcoreUsageGetter.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/Intel/Nehalem/NehalemClocksNotHaltedCPUcoreUsageGetter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/Nehalem
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/Nehalem/NehalemClocksNotHaltedCPUcoreUsageGetter.o ../../Controller/Intel/Nehalem/NehalemClocksNotHaltedCPUcoreUsageGetter.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Linux.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/Linux.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Linux.o ../../Controller/Linux.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CalculationThread.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/CalculationThread.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/CalculationThread.o ../../Windows/CalculationThread.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/VoltageAndFreq.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/VoltageAndFreq.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/VoltageAndFreq.o ../../ModelData/VoltageAndFreq.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/string_secure.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MSVC_adaption/string_secure.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/string_secure.o ../../Controller/MSVC_adaption/string_secure.c

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/CalculationThread.o: nbproject/Makefile-${CND_CONF}.mk ../../Linux/CalculationThread.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/CalculationThread.o ../../Linux/CalculationThread.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/LocalLanguageMessageFromErrorCode.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MSVC_adaption/LocalLanguageMessageFromErrorCode.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/LocalLanguageMessageFromErrorCode.o ../../Controller/MSVC_adaption/LocalLanguageMessageFromErrorCode.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/DynFreqScalingThread.o: nbproject/Makefile-${CND_CONF}.mk ../../Windows/DynFreqScalingThread.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Windows/DynFreqScalingThread.o ../../Windows/DynFreqScalingThread.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/ModelData/wxCPUcoreID.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/ModelData/wxCPUcoreID.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/ModelData
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/ModelData/wxCPUcoreID.o ../../wxWidgets/ModelData/wxCPUcoreID.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller/wxServiceSocketClient.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/Controller/wxServiceSocketClient.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller/wxServiceSocketClient.o ../../wxWidgets/Controller/wxServiceSocketClient.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/AMD/Griffin/ClocksNotHaltedCPUcoreUsageGetter.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/AMD/Griffin/ClocksNotHaltedCPUcoreUsageGetter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/AMD/Griffin
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/AMD/Griffin/ClocksNotHaltedCPUcoreUsageGetter.o ../../Controller/AMD/Griffin/ClocksNotHaltedCPUcoreUsageGetter.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} dist/Release/Cygwin_4.x-Windows/x86infoandcontrolgui_windows.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
