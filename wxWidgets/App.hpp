#pragma once

#define NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS 2

#ifdef _WINDOWS
  #define _COMPILE_WITH_CALC_THREAD
#endif

#include <Controller/CPUcontrolBase.hpp> //base class CPUcontrolBase
//#include <Controller/CPU-related/ICPUcoreUsageGetter.hpp>
//#include <Controller/IDynFreqScalingAccess.hpp>
//#include <Controller/MSVC_adaption/tchar.h> //for TCHAR
#include <Controller/stdtstr.hpp> //std::tstring
#include <Controller/MainController.hpp> //class MainController
#include <ModelData/ModelData.hpp> //class Model
#include <UserInterface/UserInterface.hpp>//base class UserInterface
//#include <Windows/DynFreqScalingAccess.hpp>
//#include <Windows/PowerProfDynLinked.hpp>
#include <Windows/NamedPipe/NamedPipeClient.hpp> //class NamedPipeClient
//class SAX2IPCcurrentCPUdataHandler
#include <Xerces/IPC/SAX2IPCcurrentCPUdataHandler.hpp>
//for x86IandC::thread_type
#include <wxWidgets/multithread/wxThreadBasedI_Thread.hpp>

//TODO program Did not exit when a taskbar icon is included.
#define COMPILE_WITH_SYSTEM_TRAY_ICON

#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
  #include <wxWidgets/UserInterface/TaskBarIcon.hpp>
#endif

#ifdef _WINDOWS
//  #include "SystemTrayAccess.hpp"
#endif
//#include <wxWidgets/UserInterface/Mainframe.hpp>
#ifdef _MSC_VER //MicroSoft compiler
  #pragma  message( "including tchar.h" )
#endif
//Add "Controller/MSVC_adaption/" to the include dir under non-MSVC
#include <tchar.h> //for TCHAR
#ifdef _COMPILE_WITH_CALC_THREAD
  #include <Windows/CalculationThread.hpp>
#endif //#ifdef _COMPILE_WITH_CALC_THREAD
//#include <Windows/NamedPipeClient.hpp>

//Forward declaration (because _this_ header file may be included very often /
//more than once) is faster than to #include the while declaration file.
//class Windows_API::DynFreqScalingAccess ;
//class Model ;
class MainFrame ;
class MyTaskBarIcon ;
class UserInterface ;
#ifdef _WINDOWS
class WinRing0_1_3RunTimeDynLinked ;
#else
  #include <Linux/MSRdeviceFile.h>
#endif
#ifdef COMPILE_WITH_CALC_THREAD
class CalculationThread ;
#endif //#ifdef COMPILE_WITH_CALC_THREAD
class DynFreqScalingThread ;
class ICPUcoreUsageGetter ;
class I_CPUcontroller ;

class wxX86InfoAndControlApp
  : public wxApp
  , public UserInterface
  //In order for the functions that are exported by this exe file to have 
  //the CPU access.
  , public CPUcontrolBase
{
private:
  TCHAR ** m_arartchCmdLineArgument ;
//  wxThread m_wxthreadIPC ;
  void * m_systemtray_icon_notification_window ;
#ifdef _WINDOWS
//  SystemTrayAccess m_systemtrayaccess ;
#endif
//#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
  //Windows_API::DynFreqScalingAccess m_dynfreqscalingaccess;
  //PowerProfDynLinked m_powerprofdynlinked ;
//#endif //#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
  //I_CPUcontroller * mp_cpucontroller ;
  //e.g. point to console or GUI.
  MainFrame * mp_frame ;
//  UserInterface * mp_userinterface ;
  #ifdef _WINDOWS
  WinRing0_1_3RunTimeDynLinked * mp_winring0dynlinked ;
  #else
    //MSRdeviceFile m_MSRdeviceFile ;
  #endif
  ////This member needs to be created on runtime because it may throw
  ////an exception (that should be caught, else runtime error) when it is created.
  //I_CPUaccess * mp_i_cpuaccess ;
  Model * mp_modelData ;
#ifdef COMPILE_WITH_SHARED_MEMORY
  HANDLE m_handleMapFile;
  std::string m_stdstrSharedMemoryName ;//= "CPUcontrolService" ;
  LPVOID mp_voidMappedViewStartingAddress ;
#else
  //Model m_modelData ;
#endif //#ifdef COMPILE_WITH_SHARED_MEMORY

  void InitSharedMemory() ;
  //WinRing0_1_3RunTimeDynLinked m_winring0dynlinked ;

  //http://docs.wxwidgets.org/stable/wx_wxappoverview.html:
  //If there are member objects they are destroyed from MyApp destructor. 
  //But if the MyApp object is deleted after wxWidgets structures that 
  //this member object depends on have been uninitialized: 
  //The solution is to destroy the member object in OnExit.
  virtual int OnExit();
  virtual bool OnInit();
  void outputAllPstates(unsigned char byCurrentP_state, int & vid) ;
  #ifdef _COMPILE_WITH_CALC_THREAD
  //Windows_API::
    CalculationThread m_calculationthread ;
  #endif //#ifdef _COMPILE_WITH_CALC_THREAD
public:
  Model m_model ;
  SAX2IPCcurrentCPUdataHandler m_sax2_ipc_current_cpu_data_handler ;
  //Must be created on heap, else left mouse clicks were not processed?
  MyTaskBarIcon * mp_taskbaricon ;
//  MyTaskBarIcon m_taskbaricon ;
  #ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
    NamedPipeClient m_ipcclient ;
  #endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  IDynFreqScalingAccess * mp_dynfreqscalingaccess ;
//  ICPUcoreUsageGetter * mp_cpucoreusagegetter ;
  MainController m_maincontroller ;
  //"volatile" because it is accessed from more than 1 thread.
  volatile bool m_vbRetrieveCPUcoreData ;
  volatile bool m_vbGotCPUcoreData ;
  std::tstring m_stdtstrProgramName ;
  wxCriticalSection m_wxcriticalsectionIPCthread ;
  wxMutex m_wxmutexIPCthread ;
  wxCondition m_wxconditionIPCthread ;
  x86IandC::thread_type m_x86iandc_threadIPC ;
#ifdef COMPILE_WITH_CPU_SCALING
  //wxDynFreqScalingTimer * mp_wxdynfreqscalingtimer ;
  //#include "wxDynLinkedCPUcoreUsageGetter.hpp"
  //wxDynLinkedCPUcoreUsageGetter * mp_wxDynLinkedCPUcoreUsageGetter ;
  //ICPUcoreUsageGetter * mp_cpucoreusagegetter ;
  //http://docs.wxwidgets.org/stable/wx_wxthread.html#wxthreadctor:
  //"Because the detached threads delete themselves, they can only be
  //allocated on the heap."
  DynFreqScalingThread * mp_dynfreqscalingthread ;
  //Windows_API::DynFreqScalingThread m_dynfreqscalingthread ;
  //DynFreqScalingThread m_dynfreqscalingthread ;
#endif //#ifdef COMPILE_WITH_CPU_SCALING
  wxX86InfoAndControlApp() ;
  //http://docs.wxwidgets.org/stable/wx_wxappoverview.html:
  //"You should delete all wxWidgets object that you created by 
    //the time OnExit finishes. 
    //In particular, do not destroy them from application class' destructor!"
  ~wxX86InfoAndControlApp()
  {
  }
  bool Confirm(const std::string & str) ;
  bool Confirm(const std::wstring & str) ;
//  bool Confirm(std::ostrstream & r_ostrstream ) ;
  bool Confirm( std::ostringstream & r_stdostringstream ) ;
  void CPUcontrollerChanged() ;
  void CPUcontrollerDeleted() ;
  void CPUcoreUsageGetterDeleted() ;
  void CurrenCPUfreqAndVoltageUpdated() ;
  void DeleteCPUcontroller() ;
  void EndDVFS() ;
  I_CPUcontroller * GetCPUcontroller()
  {
    return mp_cpucontroller ;
  }
  I_CPUaccess * GetCPUaccess()
  {
    return mp_i_cpuaccess ;
  }
  void GetCurrentCPUcoreDataViaIPCNonBlocking() ;
  void GetCurrentCPUcoreDataViaIPCNonBlockingCreateThread() ;
  Model * GetModel()
  {
    return mp_modelData ;
  }
  void PossiblyAskForOSdynFreqScalingDisabling() ;
  void RedrawEverything() ;
  void SetCPUcontroller( I_CPUcontroller * p_cpucontroller ) ;
  bool ShowTaskBarIcon(MainFrame * p_mf) ;
};

DECLARE_APP(wxX86InfoAndControlApp)
