//Including <Wbemidl.h and other INDIRECTLY included files makes problems on
//cygwin.
//#ifndef COMPILE_WITHOUT_IWBEMSERVICES
#ifdef COMPILE_WITH_IWBEMSERVICES

#pragma once //incl. guard

//Prevent indirect inclusion of BaseTsd.h
#define _BASETSD_H_
#include "../Controller/ICPUcoreUsageGetter.hpp"
#include <math.h> //fabs()
#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <objbase.h> //CoInitializeEx()
#include <Wbemidl.h> //for IWbemServices, IWbemLocator, ...
//for "VC/inlcude/excpt.h" etc.
#define __in
#define __inout
//end of: the following includes must be here above, else compiler errors

#include "global.h" //for DEBUG(...)
#include "UserInterface.hpp"

# pragma comment(lib, "wbemuuid.lib")
//for CoInitializeEx
# pragma comment(lib, "ole32.lib")

enum
{
  //Start with "1" because "0" means "success".
  CoInitializeExFailed = 1,
  CoInitializeSecurityFailed ,
  CoCreateInstanceFailed ,
  ConnectServerFailed ,
  CoSetProxyBlanketFailed ,
  IEnumWbemClassObject_NextReturnedNoObject
};

class CPUcoreUsageGetterIWbemServices
  : public ICPUcoreUsageGetter
{
private:
  bool m_bSuccesfullyInitialized ;
  DWORD m_dwMilliSecondsToSleepBetweenGetWMIdata ;
  float m_fThreshold ;
  //Make as member variable: so it's faster than allocating it inside a
  //funtion.
  HRESULT m_hresult ;
  IEnumWbemClassObject * mp_ienumwbemclassobject ;
  IWbemClassObject * mp_iwbemclassobject ;
  IWbemServices * mp_iwbemservices ;
  IWbemLocator * mp_iwbemlocator ;
  UserInterface * mp_userinterface ;
  ULONG * m_ar_ulinitVal ;
  ULONG * m_arulong ;
	int m_wNumberOfCPUCores ;//= 2 ;
  //Make it a member variable because it is used often (else if within 
  //function/ methode it has to be created on the stack)
  VARIANT m_variant ;

public:
  bool m_bSuccess ;
  //Initilisation should be extracted because with e.g COM:
  //"CoInitializeEx must be called at least once, and is usually called only once, for each thread that uses the COM library."
  //float Init() ;
  BYTE Init() ;
  CPUcoreUsageGetterIWbemServices()
    //C++ style initialisations:
#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
    : m_wNumberOfCPUCores(1)
#else
    : m_wNumberOfCPUCores(2)
#endif //#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
    , mp_userinterface(NULL)
    , m_bSuccesfullyInitialized(false)
    , m_dwMilliSecondsToSleepBetweenGetWMIdata(//100
      500)
    , m_fThreshold (80.0f )
  {
    //Init() ;
    //if( Init() >= CoInitializeSecurityFailed )
    //  CoUninitialize();
  }

  ~CPUcoreUsageGetterIWbemServices()
  {
    if(m_bSuccesfullyInitialized)
    {
      delete m_ar_ulinitVal;
	    delete m_arulong;

    //Cleanup:
      Cleanup() ;
    }
  }

  void Cleanup()
  {
    // Cleanup
    // ========
    mp_iwbemservices->Release();
    mp_iwbemlocator->Release();
    //"Closes the COM library on the current thread, unloads all DLLs loaded by the thread, frees any other resources that the thread maintains, and forces all RPC connections on the thread to close. A thread must call CoUninitialize once for each successful call it has made to CoInitialize or CoInitializeEx. Only the CoUninitialize call corresponding to the CoInitialize or CoInitializeEx call that initialized the library can close it."
    CoUninitialize();
  }

  //float GetUsageForCore(BYTE byCore)
  //float GetPercentalUsageForBothCores(float r_arf[] ) ;
  BYTE GetPercentalUsageForAllCores( float r_arf[] ) ;
  BYTE GetPercentalUsageForBothCores(float r_arf[] ) ;

  int GetValues(//IWbemServices *pSvc,
    ULONG arulong [],int nNumberOfArrayElements) ;

  void SetUserInterface(UserInterface * p_userinterface) ;

  void ShowMessage(const std::string & str ) 
  { 
    DEBUG("CPU usage--show message\n");
    if ( mp_userinterface )
      mp_userinterface->Confirm(str) ;
  }
};
#endif //#ifndef COMPILE_WITHOUT_IWBEMSERVICES
