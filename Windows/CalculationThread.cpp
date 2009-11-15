#include "CalculationThread.hpp"
#include <Controller/CalculationThreadProc.h>
#include <UserInterface.hpp>

//class CalculationThread;
//using namespace Windows_API;

//class PumaStateCtrl ;
class CalculationThread ;
//namespace Windows_API ;

CalculationThread::CalculationThread()
//  :
//  m_bCalcError(false),
//  m_vbContinue(true)
  //NULL also indicates failure for ::CreateThread(...).
//  m_hThread (NULL)
{
  Init() ;
}

CalculationThread::CalculationThread(
  BYTE byCoreID ,
  DWORD WINAPI pCalculationThreadProc(LPVOID lpParameter)
  , UserInterface * p_userinterface
  , I_CPUcontroller * p_cpucontroller
  )
  :
  m_byCoreID ( byCoreID )
//  m_bCalcError(false) ,
//  m_vbContinue(true) ,
  //, mpfn_CalculationThreadProc (pCalculationThreadProc)
  , mp_userinterface (p_userinterface)
  , mp_cpucontroller (p_cpucontroller )
  //NULL also indicates failure for ::CreateThread(...).
//  , m_hThread (NULL)
{
  Init() ;
  mpfn_CalculationThreadProc = pCalculationThreadProc ;
}

CalculationThread::CalculationThread(//PumaStateCtrl 
  //GriffinController * p_pumastatectrl 
  I_CPUcontroller * p_cpucontroller
  )
  //: m_bCalcError(false)
  //: CalculationThread()
{
    LOG("Creating high load calc thread\n");
  //CalculationThread();
  Init() ;
  //SetPumaStateCtrl(p_pumastatectrl);
  SetCPUcontroller(p_cpucontroller);
  LOG("Finished creating high load calc thread\n");
}

CalculationThread::~CalculationThread()
{
    //If the handle was successfully assigned.
    if( m_hThread )
      ::CloseHandle(m_hThread);
}

void CalculationThread::Init()
{
  m_bCalcError = false ;
  m_vbContinue = true ;
  mpfn_CalculationThreadProc = NULL ;
  m_hThread = NULL ;
}

//Start or stop thread. This method may be invoked by selecting a menu item.
//void
BYTE CalculationThread::Execute()
{
  BYTE byAction = STARTED;
  DWORD dwExitCode ;

  LOG("start or stop high load calc thread for core " <<
  (WORD) m_byCoreID << "\n");
  if( //marp_calculationthread[ byCoreID ]->m_vbContinue == true
    //IsRunning()
    m_hThread != NULL &&
    GetExitCodeThread(
      m_hThread,
      & dwExitCode )
    && //m_hThread 
      dwExitCode == STILL_ACTIVE
    )
  {
      LOG("Thread is running\n");
    //marp_calculationthread[ byCoreID ]->m_vbContinue = false ;
      EndCalculationThread() ;
      m_hThread = NULL ;
      byAction = ENDED ;
//    mp_userinterface->//SetMenuItemText(
//      ChangeUserInterface(
//      //Pass a pointer to THIS object to let the UserInterface select the
//      //UserInterface element associated woith THIS action (e.g. by using a
//      //std::map<cpucontrolleraction2menuitem> stdmapmenuitem2ui ; and do:
//      //"stdmapmenuitem2ui.get(cpucontrolleraction)->SetMenuItemText(str) ;
//      this,
//      "start thread" ) ;
    //::WaitForSingleObject(marp_calculationthread[ byCoreID ]->m_hThread);
    //delete marp_calculationthread[ byCoreID ] ;
    //marp_calculationthread[ byCoreID ] = NULL ;
  }
  else
  {
    LOG("Thread is NOT running\n");
    StartCalculationThread( m_byCoreID) ;
    //If successfully started.
    if( m_hThread )
//      mp_userinterface->//SetMenuItemText(
//        ChangeUserInterface(
//        //Pass a pointer to THIS object to let the UserInterface select the
//        //UserInterface element associated woith THIS action (e.g. by using a
//        //std::map<cpucontrolleraction2menuitem> stdmapmenuitem2ui ; and do:
//        //"stdmapmenuitem2ui.get(cpucontrolleraction)->SetMenuItemText(str) ;
//        this,
//        "end thread" ) ;
      byAction = STARTED;
  }
  return byAction ;
}

void //Windows_API::
CalculationThread::StartCalculationThread(
  BYTE byCoreID, 
  DWORD WINAPI pCalculationThreadProc(LPVOID lpParameter)
  )
{
    DWORD dwThreadID ;
  m_hThread = 
    //If the function fails, the return value is NULL. 
    ::CreateThread(
    //LPSECURITY_ATTRIBUTES lpThreadAttributes,
    //"If lpThreadAttributes is NULL, the thread gets a default security descriptor."
    NULL,
    //SIZE_T dwStackSize,
    //If this parameter is zero, the new thread uses the default size for the executable.
    0,
    //LPTHREAD_START_ROUTINE lpStartAddress,
    pCalculationThreadProc,
    //LPVOID lpParameter,
    //NULL,
    //this,
    //mp_pumastatectrl,
    this,
    //DWORD dwCreationFlags,
    //If this value is zero, the thread runs immediately after creation. 
    0,
    //LPDWORD lpThreadId
    & dwThreadID
  );
  if(m_hThread)
  {
    LOG("Successfully got thread handle from OS\n");
    ::SetThreadAffinityMask( m_hThread , 1 << byCoreID ) ;
  }
}

void //Windows_API::
CalculationThread::StartCalculationThread(BYTE byCoreID)
{
  //mp_winthread = new CWinThread();
  DWORD dwThreadID ;
  m_hThread = 
    //If the function fails, the return value is NULL. 
    ::CreateThread(
    //LPSECURITY_ATTRIBUTES lpThreadAttributes,
    //"If lpThreadAttributes is NULL, the thread gets a default security descriptor."
    NULL,
    //SIZE_T dwStackSize,
    //If this parameter is zero, the new thread uses the default size for the executable.
    0,
    //LPTHREAD_START_ROUTINE lpStartAddress,
    mpfn_CalculationThreadProc,
    //LPVOID lpParameter,
    //NULL,
    //this,
    //mp_pumastatectrl,
    this,
    //DWORD dwCreationFlags,
    //If this value is zero, the thread runs immediately after creation. 
    0,
    //LPDWORD lpThreadId
    & dwThreadID
  );
  if(m_hThread)
  {
    LOG("Successfully got thread handle from OS\n");
    ::SetThreadAffinityMask( m_hThread , 1 << byCoreID ) ;
  }
    //if( mp_wxthread->Create() == wxTHREAD_NO_ERROR )
    //  m_wxthread->Run() ;
}

//void //Windows_API::
//CalculationThread::SetPumaStateCtrl(//PumaStateCtrl * p_pumastatectrl
//  GriffinController * p_pumastatectrl )
//{
//  mp_pumastatectrl = p_pumastatectrl ;
//}

void CalculationThread::SetCPUcontroller(
  I_CPUcontroller * p_cpucontroller )
{
  mp_cpucontroller = p_cpucontroller ;
}
