/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //include guard

//#include "Controller/I_CPUcontrollerAction.hpp"
#include <Controller/ICalculationThread.hpp>
#include "ModelData/SpecificCPUcoreActionAttributes.hpp"

//Forward declaration (faster than to include header files--include them in the
//source file)
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
public:
  bool m_bCalcError ;
  BYTE m_byCoreID ;
  volatile bool m_vbContinue ;
  DWORD //WINAPI 
    ( WINAPI * mpfn_CalculationThreadProc) (LPVOID ) ;
  I_CPUcontroller * mp_cpucontroller ;
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
  CalculationThread(
    I_CPUcontroller * p_cpucontroller
    );
  CalculationThread(
    BYTE byCoreID ,
    DWORD WINAPI pCalculationThreadProc(LPVOID lpParameter)
    , UserInterface * p_userinterface
    , I_CPUcontroller * p_cpucontroller
    ) ;

  ~CalculationThread() ;
  void EndCalculationThread() { m_vbContinue = false ;
    LOG("End thread\n");
  }
  bool IsRunning() { return m_vbContinue ; }
  void SetCPUcontroller(
    I_CPUcontroller * p_cpucontroller ) ;
};
//}
