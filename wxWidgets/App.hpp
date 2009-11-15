#pragma once

#define NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS 2

//#include "../Windows/DynFreqScalingAccess.hpp"
//#include <Windows/PowerProfDynLinked.hpp>
#include <Controller/IDynFreqScalingAccess.hpp>
#include <Controller/stdtstr.hpp> //std::tstring
#include <UserInterface.hpp>
#include <ModelData/ModelData.hpp>
#include <Controller/MainController.hpp>
#include <Windows/CalculationThread.hpp>
#include <Windows/NamedPipeClient.hpp>

//class Windows_API::DynFreqScalingAccess ;
class Model ;
class MainFrame ;
class UserInterface ;
class WinRing0dynLinked ;
class CalculationThread ;
class DynFreqScalingThread ;
class ICPUcoreUsageGetter ;
class I_CPUcontroller ;

class wxPumaStateCtrlApp
  : public wxApp
  , public UserInterface
{
private:
  _TCHAR ** m_arartchCmdLineArgument ;

//#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
  //Windows_API::DynFreqScalingAccess m_dynfreqscalingaccess;
  //PowerProfDynLinked m_powerprofdynlinked ;
//#endif //#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
  I_CPUcontroller * mp_cpucontroller ;
  //e.g. point to console or GUI.
  MainFrame * mp_frame ;
  UserInterface * mp_userinterface ;
  WinRing0dynLinked * mp_winring0dynlinked ;
  std::tstring m_stdtstrProgramName ;
  Model * mp_modelData ;
#ifdef COMPILE_WITH_SHARED_MEMORY
  HANDLE m_handleMapFile;
  std::string m_stdstrSharedMemoryName ;//= "CPUcontrolService" ;
  LPVOID mp_voidMappedViewStartingAddress ;
#else
  //Model m_modelData ;
#endif //#ifdef COMPILE_WITH_SHARED_MEMORY

  //WinRing0dynLinked m_winring0dynlinked ;

  //http://docs.wxwidgets.org/stable/wx_wxappoverview.html:
  //If there are member objects they are destroyed from MyApp destructor. 
  //But if the MyApp object is deleted after wxWidgets structures that 
  //this member object depends on have been uninitialized: 
  //The solution is to destroy the member object in OnExit.
  virtual int OnExit();
  virtual bool OnInit();
  void outputAllPstates(unsigned char byCurrentP_state, int & vid) ;
  //GriffinController * mp_pstatectrl ;
  //Windows_API::
    CalculationThread m_calculationthread ;
public:
  NamedPipeClient m_ipcclient ;
  IDynFreqScalingAccess * mp_dynfreqscalingaccess ;
  ICPUcoreUsageGetter * mp_cpucoreusagegetter ;
  MainController m_maincontroller ;
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
  wxPumaStateCtrlApp()
    //C++ style inits:
//#ifdef COMPILE_WITH_CPU_SCALING
//    : mp_wxdynfreqscalingtimer(NULL)
//#endif //#ifdef COMPILE_WITH_CPU_SCALING
    : mp_cpucontroller(NULL)
    , mp_dynfreqscalingaccess(NULL)
  {
#ifdef COMPILE_WITH_DEBUG
  //fileDebug = fopen("PumaStateCtrl_debug.txt","w");
  //if( ! fileDebug )
  //  ::wxMessageBox("Error opening debug output file for writing.\n"
  //    "Modifying access rights to file / dir containing it could help ");

  //For g++ the std::string object passed to Logger::OpenFile(std::string & )
  //has to be declared before. the call
  //    ( error if  "logger.OpenFile( std::string("bla");"  )
  std::string stdstr("GriffinControl_log.txt") ;
  g_logger.OpenFile( stdstr ) ;
#endif
    //m_cpucoreusagegetteriwbemservices.Init() ;
  }
  //http://docs.wxwidgets.org/stable/wx_wxappoverview.html:
  //"You should delete all wxWidgets object that you created by 
    //the time OnExit finishes. 
    //In particular, do not destroy them from application class' destructor!"
  ~wxPumaStateCtrlApp()
  {
  }
  bool Confirm(const std::string & str) ;
  bool Confirm(std::ostrstream & r_ostrstream ) ;
  void CurrenCPUfreqAndVoltageUpdated() ;
  I_CPUcontroller * GetCPUcontroller()
  {
    return mp_cpucontroller ;
  }
};

DECLARE_APP(wxPumaStateCtrlApp)
