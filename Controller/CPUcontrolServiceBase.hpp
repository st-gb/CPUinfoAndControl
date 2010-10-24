/*
 * CPUcontrolServiceBase.hpp
 *
 *  Created on: Oct 11, 2010
 *      Author: Stefan
 */

#ifndef CPUCONTROLSERVICEBASE_HPP_
#define CPUCONTROLSERVICEBASE_HPP_

#include <Controller/character_string/stdtstr.hpp> //std::tstring
#include <Controller/CPUcontrolBase.hpp> //Base class CPUcontrolBase
//Member variable of class MainController
#include <Controller/MainController.hpp>
//#include <Linux/multithread/pthreadBasedI_Thread.hpp>
#include <Controller/multithread/thread_type.hpp> //thread_type
#include <UserInterface/DummyUserInterface.hpp>//class DummyUserInterface
//class wxThreadFuncStarterThread
//#include <wxWidgets/multithread/wxThreadBasedI_Thread.hpp>

#ifdef _WIN32
  typedef LPTSTR CPU_CONTROL_SERVICE_BASE_STRING_POINTER_TYPE  ;
#else
  typedef char * CPU_CONTROL_SERVICE_BASE_STRING_POINTER_TYPE ;
#endif

//Forward declarations (faster than #include)
class DynFreqScalingThreadBase ;

class CPUcontrolServiceBase
  : public CPUcontrolBase
{
public:
  //For initialization of CPU controller and CPU core usage getter.
  MainController m_maincontroller ;
  ////Must be static. Else: runtime error when calling
  //// DummyUserInterface::Confirm(ostream)
//  static
    DummyUserInterface m_dummyuserinterface ;
//  LPTSTR * mar_tch ;
  CPU_CONTROL_SERVICE_BASE_STRING_POINTER_TYPE * mar_tch ;
  std::tstring m_stdtstrProgramArg ;
  std::tstring m_stdtstrProgramName ;
//  wxThreadBasedI_Thread m_wxthreadbasedi_thread ;
//  Linux::pthreadBasedI_Thread m_pthreadbasedi_thread ;
  x86IandC::thread_type m_x86iandc_thread_typeDVFSthread ;
  CPUcontrolServiceBase(//user interface is needed for outputting messages.
    UserInterface * p_userinterface);
  virtual
  ~CPUcontrolServiceBase();
  inline void CreateDVFSthreadObject_Inline() ;
  //"virtual" because it should be overwritten in derivative classes.
  virtual void CreateHardwareAccessObject() {}
  virtual void DisableOtherVoltageOrFrequencyChange() {}
  //inline bool HandleStartDynVoltAndFreqScalingThread() ;
  //Must not be inline (else "undefined reference to ")
  bool HandleStartDynVoltAndFreqScalingThread() ;
//static
  DWORD Initialize(
    DWORD dwArgumentCount,
    //LPTSTR * argv
    CPU_CONTROL_SERVICE_BASE_STRING_POINTER_TYPE * pp_Arguments
    ) ;
  //"virtual" because it should be overwritten in derivative classes.
  inline virtual void ShowMessage( const std::string & cr_stdstrMessage ) {}
  //If not virtual " undefined reference to" in Windows' CPUcontrolService
//  inline bool StartDynVoltnFreqScaling() ;
  bool StartDynVoltnFreqScaling() ;
  inline //bool
    void StartDVFSviaThreadType_Inline(bool &) ;
};

#endif /* CPUCONTROLSERVICEBASE_HPP_ */
