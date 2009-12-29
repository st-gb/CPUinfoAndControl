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
CC=gcc
CCC=g++-4.4
CXX=g++-4.4
FC=
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM/PentiumM_ClocksNotHaltedCPUcoreUsageGetter.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/daemon.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/FloatValidator.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/time_secure.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/CPUcoreData.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/SpecificCPUcoreActionAttributes.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/XMLAccess.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSRaccessBase.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MainController.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2_CPUspecificHandler.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2DefaultVoltageForFrequency.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/wxDynamicDialog.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/MainFrame.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/DynFreqScalingThreadBase.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/stdtstr.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller/wxStringHelper.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/MSRdeviceFile.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/DynFreqScalingDlg.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM/PentiumM_Controller.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/FreqAndVoltageSettingDlg.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/XercesHelper.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Logger.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/App.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2MainConfigHandler.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/tchar_conversion.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUcontroller.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUcontrollerAction.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/DynVoltAndFreqScaling.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/RegisterData.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUaccess.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/DynFreqScalingThread.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/stdafx.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/EnglishMessageFromErrorCode.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/CPUindependentHelper.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/stdio_secure.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/PStates.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/ModelData.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/VoltageAndFreq.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/string_secure.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/ModelData/wxCPUcoreID.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/stdlib_mscv_extras.o

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
LDLIBSOPTIONS=-L/usr/lib -lxerces-c -lwx_gtk2ud_core-2.8 -lwx_baseud-2.8 -lwx_gtk2u_adv-2.8

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-Linux-x86/x86infoandcontrolgui

dist/Debug/GNU-Linux-x86/x86infoandcontrolgui: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/x86infoandcontrolgui ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM/PentiumM_ClocksNotHaltedCPUcoreUsageGetter.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/Intel/PentiumM/PentiumM_ClocksNotHaltedCPUcoreUsageGetter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM/PentiumM_ClocksNotHaltedCPUcoreUsageGetter.o ../../Controller/Intel/PentiumM/PentiumM_ClocksNotHaltedCPUcoreUsageGetter.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/daemon.o: nbproject/Makefile-${CND_CONF}.mk ../../Linux/daemon.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/daemon.o ../../Linux/daemon.c

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/FloatValidator.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/UserInterface/FloatValidator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/FloatValidator.o ../../wxWidgets/UserInterface/FloatValidator.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/time_secure.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MSVC_adaption/time_secure.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/time_secure.o ../../Controller/MSVC_adaption/time_secure.c

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/CPUcoreData.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/CPUcoreData.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/CPUcoreData.o ../../ModelData/CPUcoreData.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/SpecificCPUcoreActionAttributes.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/SpecificCPUcoreActionAttributes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/SpecificCPUcoreActionAttributes.o ../../ModelData/SpecificCPUcoreActionAttributes.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/XMLAccess.o: nbproject/Makefile-${CND_CONF}.mk ../../Xerces/XMLAccess.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/XMLAccess.o ../../Xerces/XMLAccess.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSRaccessBase.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MSRaccessBase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSRaccessBase.o ../../Controller/MSRaccessBase.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MainController.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MainController.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MainController.o ../../Controller/MainController.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2_CPUspecificHandler.o: nbproject/Makefile-${CND_CONF}.mk ../../Xerces/SAX2_CPUspecificHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2_CPUspecificHandler.o ../../Xerces/SAX2_CPUspecificHandler.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2DefaultVoltageForFrequency.o: nbproject/Makefile-${CND_CONF}.mk ../../Xerces/SAX2DefaultVoltageForFrequency.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2DefaultVoltageForFrequency.o ../../Xerces/SAX2DefaultVoltageForFrequency.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/wxDynamicDialog.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/UserInterface/wxDynamicDialog.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/wxDynamicDialog.o ../../wxWidgets/UserInterface/wxDynamicDialog.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/MainFrame.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/UserInterface/MainFrame.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/MainFrame.o ../../wxWidgets/UserInterface/MainFrame.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/DynFreqScalingThreadBase.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/DynFreqScalingThreadBase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/DynFreqScalingThreadBase.o ../../Controller/DynFreqScalingThreadBase.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/stdtstr.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/stdtstr.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/stdtstr.o ../../Controller/stdtstr.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller/wxStringHelper.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/Controller/wxStringHelper.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/Controller/wxStringHelper.o ../../wxWidgets/Controller/wxStringHelper.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/MSRdeviceFile.o: nbproject/Makefile-${CND_CONF}.mk ../../Linux/MSRdeviceFile.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/MSRdeviceFile.o ../../Linux/MSRdeviceFile.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/DynFreqScalingDlg.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/UserInterface/DynFreqScalingDlg.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/DynFreqScalingDlg.o ../../wxWidgets/UserInterface/DynFreqScalingDlg.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM/PentiumM_Controller.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/Intel/PentiumM/PentiumM_Controller.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Intel/PentiumM/PentiumM_Controller.o ../../Controller/Intel/PentiumM/PentiumM_Controller.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/FreqAndVoltageSettingDlg.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/UserInterface/FreqAndVoltageSettingDlg.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/UserInterface/FreqAndVoltageSettingDlg.o ../../wxWidgets/UserInterface/FreqAndVoltageSettingDlg.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/XercesHelper.o: nbproject/Makefile-${CND_CONF}.mk ../../Xerces/XercesHelper.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/XercesHelper.o ../../Xerces/XercesHelper.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Logger.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/Logger.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/Logger.o ../../Controller/Logger.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/App.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/App.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/App.o ../../wxWidgets/App.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2MainConfigHandler.o: nbproject/Makefile-${CND_CONF}.mk ../../Xerces/SAX2MainConfigHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Xerces/SAX2MainConfigHandler.o ../../Xerces/SAX2MainConfigHandler.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/tchar_conversion.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/tchar_conversion.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/tchar_conversion.o ../../Controller/tchar_conversion.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUcontroller.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/I_CPUcontroller.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUcontroller.o ../../Controller/I_CPUcontroller.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUcontrollerAction.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/I_CPUcontrollerAction.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUcontrollerAction.o ../../Controller/I_CPUcontrollerAction.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/DynVoltAndFreqScaling.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/DynVoltAndFreqScaling.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/DynVoltAndFreqScaling.o ../../Controller/DynVoltAndFreqScaling.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/RegisterData.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/RegisterData.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/RegisterData.o ../../ModelData/RegisterData.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUaccess.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/I_CPUaccess.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/I_CPUaccess.o ../../Controller/I_CPUaccess.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/DynFreqScalingThread.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/DynFreqScalingThread.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/DynFreqScalingThread.o ../../wxWidgets/DynFreqScalingThread.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/stdafx.o: nbproject/Makefile-${CND_CONF}.mk ../../stdafx.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/stdafx.o ../../stdafx.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/EnglishMessageFromErrorCode.o: nbproject/Makefile-${CND_CONF}.mk ../../Linux/EnglishMessageFromErrorCode.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Linux/EnglishMessageFromErrorCode.o ../../Linux/EnglishMessageFromErrorCode.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/CPUindependentHelper.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/CPUindependentHelper.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/CPUindependentHelper.o ../../Controller/CPUindependentHelper.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/stdio_secure.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MSVC_adaption/stdio_secure.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/stdio_secure.o ../../Controller/MSVC_adaption/stdio_secure.c

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/PStates.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/PStates.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/PStates.o ../../ModelData/PStates.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/ModelData.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/ModelData.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/ModelData.o ../../ModelData/ModelData.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/VoltageAndFreq.o: nbproject/Makefile-${CND_CONF}.mk ../../ModelData/VoltageAndFreq.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/ModelData/VoltageAndFreq.o ../../ModelData/VoltageAndFreq.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/string_secure.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MSVC_adaption/string_secure.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/string_secure.o ../../Controller/MSVC_adaption/string_secure.c

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/ModelData/wxCPUcoreID.o: nbproject/Makefile-${CND_CONF}.mk ../../wxWidgets/ModelData/wxCPUcoreID.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/ModelData
	${RM} $@.d
	$(COMPILE.cc) -g -D__WXGTK__ -D_DEBUG -D_UNICODE -DCOMPILE_WITH_XERCES -I../.. -I../../Controller/MSVC_adaption -I/usr/include/wx-2.8 -I/usr/lib/wx/include/gtk2-unicode-debug-2.8 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/wxWidgets/ModelData/wxCPUcoreID.o ../../wxWidgets/ModelData/wxCPUcoreID.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/stdlib_mscv_extras.o: nbproject/Makefile-${CND_CONF}.mk ../../Controller/MSVC_adaption/stdlib_mscv_extras.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Controller/MSVC_adaption/stdlib_mscv_extras.o ../../Controller/MSVC_adaption/stdlib_mscv_extras.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/x86infoandcontrolgui

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
