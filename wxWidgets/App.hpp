#pragma once

#define NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS 2

#ifdef _WINDOWS
  #define _COMPILE_WITH_CALC_THREAD
#endif

//#include "../Windows/DynFreqScalingAccess.hpp"
//#include <Windows/PowerProfDynLinked.hpp>
#include <Controller/CPUcontrolBase.hpp>
#include <Controller/ICPUcoreUsageGetter.hpp>
#include <Controller/IDynFreqScalingAccess.hpp>
#include <Controller/stdtstr.hpp> //std::tstring
#include <Controller/MainController.hpp>
//#include <Controller/MSVC_adaption/tchar.h> //for TCHAR
#include <UserInterface.hpp>
#include <ModelData/ModelData.hpp>

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

//class Windows_API::DynFreqScalingAccess ;
class Model ;
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
  ////an exception (that should be catched, else runtime error) when it is created.
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
  //GriffinController * mp_pstatectrl ;
  #ifdef _COMPILE_WITH_CALC_THREAD
  //Windows_API::
    CalculationThread m_calculationthread ;
  #endif //#ifdef _COMPILE_WITH_CALC_THREAD
public:
  //Must be created on heap, else left mouse clicks were not processed?
  MyTaskBarIcon * mp_taskbaricon ;
//  MyTaskBarIcon m_taskbaricon ;
  #ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
    NamedPipeClient m_ipcclient ;
  #endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  IDynFreqScalingAccess * mp_dynfreqscalingaccess ;
//  ICPUcoreUsageGetter * mp_cpucoreusagegetter ;
  MainController m_maincontroller ;
  std::tstring m_stdtstrProgramName ;
#ifdef COMPILE_WITH_CPU_SCALING
  //wxDynFreqScalingTimer * mp_wxdynfreqscalingtimer ;
  //#include "wxDynLinkedCPUcoreUsageGetter.hpp"
  //wxDynLinkedCPUcoreUsageGetter * mp_wxDynLinkedCPUcoreUsageGetter ;
  //ICPUcoreUsageGetter * mp_cpucoreusagegetter ;
  //http://docs.wxwidgets.org/stable/wx_wxthread.html#wxthreadctor:
  //"Because the detached threads delete themselves, they can only be
  //allocated on the heap."
  DynFreqScalingThread * mp_dynfreqscalingthread ;
 #ifdef COMPILE_WITH_IWBEMSERVICES
  //CPUcoreUsageGetterIWbemServices m_cpucoreusagegetteriwbemservices ;
  //Windows_API::DynFreqScalingThread m_dynfreqscalingthread ;
  //DynFreqScalingThread m_dynfreqscalingthread ;
 #endif //#ifndef COMPILE_WITHOUT_IWBEMSERVICES

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
  bool Confirm(std::ostrstream & r_ostrstream ) ;
  void CPUcontrollerChanged() ;
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
  Model * GetModel()
  {
    return mp_modelData ;
  }
  void RedrawEverything() ;
  void SetCPUcontroller( I_CPUcontroller * p_cpucontroller ) ;
  void ShowTaskBarIcon(MainFrame * p_mf) ;
};

DECLARE_APP(wxX86InfoAndControlApp)
