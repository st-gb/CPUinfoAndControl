#pragma once //include guard
////because of: fatal error C1189: #error :  Building MFC application with /MD[d] (CRT dll version) requires MFC shared dll version. Please #define _AFXDLL or do not use /MD[d]
//#define _AFXDLL
//#include <afxmt.h> //for CCriticalSection
#include <Windows.h> //for ::SetThreadAffinityMask(...) ::CloseHandle(...)
//#include "../Controller/PumaStateCtrl.h"
//#include <Controller/GriffinController.hpp>
//#include "Controller/I_CPUcontrollerAction.hpp"
#include "../ICalculationThread.hpp"
#include "ModelData/SpecificCPUcoreActionAttributes.hpp"

//pre-declaratiosn (faster than to include header files--include them in the
//source file)
//class PumaStateCtrl ;
class I_CPUcontroller ;
class UserInterface ;

//namespace Windows_API
//{
class CalculationThread
  : public ICalculationThread
  //, public I_CPUcontrollerAction
  , public SpecificCPUcoreActionAttributes
{
private:
  //CWinThread * mp_winthread ;
public:
  HANDLE m_hThread ;
  bool m_bCalcError ;
  BYTE m_byCoreID ;
  volatile bool m_vbContinue ;
  DWORD //WINAPI 
    //WINAPI ("stdcall") is needed for Windows API's "::CreateThread(...)"
    ( WINAPI 
    * mpfn_CalculationThreadProc) (LPVOID ) ;
  //PumaStateCtrl 
  //GriffinController * mp_pumastatectrl ;
  I_CPUcontroller * mp_cpucontroller ;
  //CCriticalSection m_ccritsection ;
//  CRITICAL_SECTION m_criticalsection ;
  UserInterface * mp_userinterface ;

  //void
  BYTE Execute() ;
  void Init();
  void StartCalculationThread(BYTE byCoreID, 
    DWORD (WINAPI*) (LPVOID lpParameter)
    ) ;
  void StartCalculationThread(BYTE byCoreID) ;

  CalculationThread() ;
  CalculationThread(//PumaStateCtrl 
    //GriffinController * p_pumastatectrl 
    I_CPUcontroller * p_cpucontroller
    );
  CalculationThread(
    BYTE byCoreID ,
    DWORD 
#ifdef _MSC_VER
    //WINAPI ("stdcall") is needed for Windows API's "::CreateThread(...)"
    WINAPI 
#endif //#ifdef _MSC_VER
    pCalculationThreadProc(LPVOID lpParameter)
    , UserInterface * p_userinterface
    , I_CPUcontroller * p_cpucontroller
    ) ;

  ~CalculationThread() ;
  void EndCalculationThread() { m_vbContinue = false ;
    LOG("End thread\n");
  }
  bool IsRunning() { return m_vbContinue ; }
  //void SetPumaStateCtrl(//PumaStateCtrl 
  //  GriffinController * p_pumastatectrl);
  void SetCPUcontroller(
    I_CPUcontroller * p_cpucontroller ) ;
};
//}
