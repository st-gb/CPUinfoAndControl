#pragma once

//for use with "/MD[d]" else: fatal error C1189: #error :  Building MFC application with /MD[d] (CRT dll version) requires MFC shared dll version. Please #define _AFXDLL or do not use /MD[d]
#define _AFXDLL

#include <ModelData/PStates.h>
//#include "../UserInterface.hpp"
#include "stdio.h" //for printf() in DEBUG() etc.
#include <string> //for type "std::string"
#include <string.h> //for memcpy(...)
#include <stdlib.h> //for atoi()
#include <Controller/IDynFreqScalingAccess.hpp> //for IDynFreqScalingAccess
//#ifdef WIN32 
//#ifdef _WINDOWS
#ifdef _MSC_VER //MicroSoft C compiler (macro in Visual Studio )
//  #include <conio.h> //for getche(...), only MS compiler?!
#ifndef __CYGWIN__
  #include "tchar.h" //for _T(), TCHAR
#else
  typedef char _TCHAR ;
#endif
  typedef unsigned long ULONG; //for PowrProf.h
  #include <Windows_specific.h>
#else
  //If included under MSCV: error messages like "C:\Programme\cygwin\usr\include\sys/unistd.h(17) : error C2733: Zweite C-Bindung fr berladene Funktion '_exit' nicht zulssig"
  #include <unistd.h> //for usleep(...)
  //#include "Linux.h"
#endif //#ifdef WIN32
#include <iostream> //for "std::cin"
#include <global.h> //for DEBUG(...) etc.
#include <sstream> //for ostringstream
#include <strstream> //for std::ostrstream
//#include "../ModelData/ModelData.hpp"
#include <preprocessor_helper_macros.h>
#include <Controller/AMD/Griffin/AMD_family17.h>
//#include "Windows/WinRing0dynLinked.hpp"
//#include "../ISpecificController.hpp"
#include <Controller/I_CPUaccess.hpp>
#include <ModelData/PStates.h>
#include <ModelData/VoltageAndFreq.hpp>
#include <Controller/I_CPUcontroller.hpp>
#include <tchar.h>
#ifndef _WINDOWS
  #include <Windows_compatible_typedefs.h>
#endif


#define WRITE_MSR_FAILED 2
#define CPUID_PROCESSOR_NAME_CHAR_NUMBER 4*4*3

#ifndef _CONSOLE
  #ifdef WIN32
    //#include <afxwin.h>
  #endif //#ifdef WIN32
#endif

//#ifdef WIN32
//#ifdef _WINDOWS
#ifdef _MSC_VER //MicroSoft C compiler (macro in Visual Studio )
  //#include <OlsDef.h> //for "#define PciBusDevFunc(Bus, Dev, Func) " etc.
  #ifndef _MFC_VER //"MFC apps must not #include <windows.h>"
  //#include <Windows.h> //for ::WaitForSingleObject(), HANDLE CreateEvent(...)
  //#include <windows.h> //for type "HINSTANCE"
  //pdh.h Must be AFTER #include "../Model.hpp" else: 
  //"fatal error C1189: #error :  WINDOWS.H already included.  MFC apps must not #include <windows.h>"
  //# include <pdh.h>
  #endif
#else
  //#include "Linux.h"
#endif
//#include <afxmt.h> //for class CEvent
//#include <Winbase.h> //for HANDLE CreateEvent(...)

#ifdef WIN32 //Function signatures for the WinRing0-DLL that let us access the MSR and PCI config.
  //unsigned long msr_register_number ;
//#define LINK_TO_WINRING0_STATICALLY
//#ifdef LINK_TO_WINRING0_STATICALLY
//  #include "OlsApi.h"
//#else
//  #include "WinRing0.h" //or: include <OlsApiInitDef.h> and use "_RdmsrEx" etc.
//
//  extern pfnReadIoPortByteEx g_pfnreadioportbyteex ;
//  extern pfnReadIoPortDWordEx g_pfnreadioportdwordex ;
//#endif //#ifdef LINK_TO_WINRING0_STATICALLY

  //extern HANDLE g_handleEvent ;
#endif//#ifdef WIN32
#include <ICalculationThread.hpp>

////Declare as "extern": else linker error "LNK2005" with MSVC.
//extern pfnRdMsrExDef g_pfnRdMsrEx ;

//class Model ;
class UserInterface ;
class Model ;
class MaxVoltageForFreq ;

std::string getBinaryRepresentation(unsigned long ulValue);

//This class should only contain standard C++ and no plattform/OS
//dependant/ specific code.
class GriffinController
  : public I_CPUcontroller
{
private:
  ////This should be private so that other classes can not access this
  ////member directly in order to avoid writes to dangerous MSR registers.
  //I_CPUaccess * mp_controller ;
#ifdef _WINDOWS //win32, 64 Bit (etc.)
  //HINSTANCE is available under MS Vis Studio/ C++
  //HINSTANCE m_hinstanceThisModule;
//#endif //#ifdef _WINDOWS
//#ifdef WIN32
  #ifndef LINK_TO_WINRING0_STATICALLY

  #endif // #ifdef LINK_TO_WINRING0_STATICALLY
//#endif //#ifdef WIN32
#endif //#ifdef _WINDOWS
  UserInterface * mp_userinterface ;

  //Use the method with std::string parameter rather than this 
  //version with "char * &" where the
  //memory is allocated inside the function!
  bool GetProcessorNameByCPUID(
    //Use a pointer to an array in order to allocate the array within 
    //this method.
    char * & parchCPUID ) ;
  //CEvent m_event ;
  //HANDLE m_handleEvent;
public :
  BYTE m_byPstateForFindingLowVoltage ;
  bool m_bFrequencyScalingByOSDisabled ;
  bool m_bPstateSet ;
  BYTE m_byPstateID ;
  IDynFreqScalingAccess * mp_dynfreqscalingaccess ;
  int m_byNumberOfCmdLineArgs;
  //_TCHAR ** m_arartcharCmdLineArg ;
  TCHAR ** m_arartcharCmdLineArg ;
  //PStates m_pstates;
  //Model m_model ;
  Model * mp_model ;
  //ISpecificController 
  ICalculationThread * mp_calculationthread ;
  ICalculationThread ** marp_calculationthread ;
  //accessed by "IncreaseVoltage(...)" and "FindLowestOperatingVoltage(...)"
  PState m_pstate ;
  //CRITICAL_SECTION m_criticalsectionMSRaccess ;

  void AccuratelyStartPerformanceCounting( 
    DWORD dwAffinityBitMask ,
    BYTE byPerformanceCounterNumber ,
    WORD wEventSelect
    //BYTE byCounterMask 
    , bool bInvertCounterMask
    );
  BYTE ApplyAllPStates() ;
  bool ApplyAllPStates(const PStates & pstates);

  BYTE changePstate(BYTE byNewPstate,DWORD dwCoreBitmask);

  bool cmdLineParamsContain(_TCHAR * ptcharOption, std::string & strValue);
  bool cmdLineParamsContain(_TCHAR * ptcharOption, BYTE & rbyOption) ;

  void DecreaseVoltageBy1Step(float & r_fVoltage)
  { 
    WORD wVoltageID = GetVoltageID( r_fVoltage ) ;
    r_fVoltage = GetVoltageInVolt( 
      //Higher voltage ID means lower voltage 
      ++ wVoltageID ) ; 
  }

  void GetAllPossibleFrequencies(
    std::set<VoltageAndFreq> & r_stdsetvoltageandfreq ) ;

  //float
  ULONGLONG GetCurrentCPUload(BYTE byCPUcoreID) ;

  BYTE GetCurrentPstate(
    WORD & r_wFreqInMHz 
    , float & Volt
    , BYTE byCoreID 
    ) ;
  BYTE GetCurrentPstate(DWORD dwAffinityMask) ;

  //BYTE GetInterpolatedVoltageFromFreq(
  //  WORD wFreqInMHzToGetVoltageFrom,
  //  float & r_fVoltageInVolt ) ;
  WORD GetMaximumFrequencyInMHz() ;

  DIDandFID GetNearestHigherPossibleFreqInMHzAsDIDnFID(
      WORD wFreqInMHz ) ;
  DIDandFID GetNearestLowerPossibleFreqInMHzAsDIDnFID(
      WORD wFreqInMHz ) ;
  DIDandFID GetNearestPossibleFreqInMHzAsDIDnFID(
      WORD wFreqInMHz ) ;

  //Useful for reads from p-state and COFVID status registers.
  void GetVIDmFIDnDID(DWORD dwLowmostMSRvalue, PState & r_pstate) ;

  bool GetProcessorNameByCPUID(std::string & r_stdstr) ;
  BYTE GetVoltageIDFromVoltageInVolt(float fVoltageInVolt) ;

  //void 
    BYTE Init() ;
  void ResumeFromS3orS4() ;

//#ifdef WIN32
//  void DeInitWinRing0();
//#endif//#ifdef WIN32
  BYTE GetValuesOfClockPower_TimingControl2Register(
    BYTE & byNbVid, BYTE & byPstateMaxVal, BYTE & byAltVid);

//#ifdef WIN32
#ifdef _WINDOWS
  #ifndef LINK_TO_WINRING0_STATICALLY
  BYTE GetValuesOfPowerControlMiscellaneousRegister(
    //BYTE & byNbVid, BYTE & byPstateMaxVal, BYTE & byAltVid
    std::string & r_str
    );
  BYTE GetValuesOfReportedTemperatureControlRegister(
    //BYTE & byNbVid, BYTE & byPstateMaxVal, BYTE & byAltVid
    );
  BYTE GetValuesOfSBIAddressRegister();
  BYTE GetValuesOfSBIControlRegister();
  BYTE GetValuesOfSBIDataRegister();
  #endif //#ifndef LINK_TO_WINRING0_STATICALLY
//#endif//#ifdef WIN32
#endif//#ifdef _WINDOWS

  void IncreaseVoltageBy1Step(float & r_fVoltage)
  { 
    WORD wVoltageID = GetVoltageID( r_fVoltage ) ;
    r_fVoltage = GetVoltageInVolt( 
      //Lower voltage ID means higher voltage.
      -- wVoltageID ) ; 
  }

  void IncreaseVoltageForCurrentPstate(BYTE byCoreID );

  bool isVIDOptionSpecified(BYTE & rbyVID) ;

  bool isDivIDOptionSpecified(BYTE & rbyDivID);

  bool isFreqIDOptionSpecified(BYTE & rbyFreqID);

  bool isModifierOptionSpecified(
    BYTE & rbyVID,
    BYTE & rbyDivID,
    BYTE & rbyFreqID
    );

  bool IsSupportedCPUModel() ;

  BYTE handleCmdLineArgs(//int argc
    );

  GriffinController() {};
  GriffinController(
    Model * p_modelData, 
    I_CPUaccess * p_cpuaccessmethod 
    ) 
  { 
    mp_model = p_modelData ; 
    mp_cpuaccess = p_cpuaccessmethod ;
    Init() ;
  }
  
  GriffinController(
    int argc
    //, _TCHAR * argv[]
    , TCHAR * argv[]
    , Model & m_modelData
    , //ISpecificController 
      I_CPUaccess * p_controller
    , ICalculationThread * p_calculationthread
    , IDynFreqScalingAccess & p_dynfreqscalingaccess
    ) ;

  GriffinController(
    int argc,
    _TCHAR * argv[] ,
    //This param must be a pointer because we want to pass concrete classes
    //inherited from the abstract class "UserInterface".
    UserInterface * p_userinterface ,
    Model & m_modelData ,
    //ISpecificController 
      I_CPUaccess * p_controller ,
    ICalculationThread * p_calculationthread ,
    IDynFreqScalingAccess & p_dynfreqscalingaccess
    );

  ~GriffinController() ;

  bool handleNBVIDOption() ;

  void SetValuesOfClockPower_TimingControl2Register(
    BYTE byNbVid, BYTE byPstateMaxVal, BYTE byAltVid);

//#ifdef WIN32 
#ifdef _WINDOWS
  #ifndef LINK_TO_WINRING0_STATICALLY

  //bool isNBVIDOptionSpecified(

  #endif //#ifndef LINK_TO_WINRING0_STATICALLY
#endif//#ifdef WIN32

   void DisableFrequencyScalingByOS() ;
   void EnableFrequencyScalingByOS() ;

   bool isValidPstateOption(BYTE & rbyP_State) ;

   static WORD getFrequencyInMHz(BYTE byFrequID,BYTE byDivID) ;

//#ifdef WIN32
//  bool InitWinRing0();
//#endif//#ifdef WIN32

  float GetDefaultVoltageForFrequency(WORD wFrequInMHz);
  //float 
  BYTE GetMaxVoltageForFreq(
      WORD wFreqToGetMaxVoltageFor
      , float & r_fMaxVoltageInVolt
      ) ;
  WORD GetMinimumFrequencyInMHz() { return 150 ; }
  WORD GetMaximumVoltageID()  { return 64 ; }
  WORD GetMinimumVoltageID() { return 36 ; }
  //BYTE GetPstate(WORD wPstateID, VoltageAndFreq & r_voltageandfreq
  //  ) { return 0 ; }
  //bool GetPstateSafefy(
  //  WORD wFreqInMHz
  //  , float fVoltage 
  //  ) ;
  float GetVoltageInVolt(WORD wVoltageID ) ;//{ return 0.0 ; }
  void FindLowestOperatingVoltage(BYTE byPstate, BYTE byCoreID) ;

  BYTE GetPstateSafefy(//PState & r_pstate,
    BYTE byVID,BYTE byFreqID,BYTE byDivID) ;

  WORD GetVoltageID(float fVoltageInVolt ) ;

  bool IsTurionUltra() ;

  bool GetCPUID(
    //last/rightmost/least significant 2 digits of function bumber 
    DWORD dwFunctionIndex,
    DWORD & dwEAX,
    DWORD & dwEBX,
    DWORD & dwECX,
    DWORD & dwEDX
    );

  float GetMinimumVoltageInVolt() ;

  BYTE GetNumberOfCPUCores() ;

  //bool
  BYTE GetSafefy(
    const PState & r_pstate
    )
  {
    return GetPstateSafefy(
      r_pstate.m_byVoltageID,
      r_pstate.m_byFreqID,
      r_pstate.m_byDivisorID
      ) ;
  }

  //Call this method if the cause of the unsafety is not needed.
  bool IsSafe(const PState & cr_pstate)
  {
    //Only value "1" means "safe". Values 0, 2 etc. mean "unsafe"
    return GetSafefy(cr_pstate) == SETTING_VOLTAGE_IS_SAFE ;
  }

  void PerformanceEventSelectRegisterWrite(
    DWORD dwAffinityBitMask ,
    BYTE byPerformanceEventSelectRegisterNumber ,
    WORD wEventSelect ,
    BYTE byCounterMask ,
    bool bInvertCounterMask ,
    bool bEnablePerformanceCounter,
    bool bEnableAPICinterrupt,
    bool bEdgeDetect,
    bool bOSmode,
    bool bUserMode,
    BYTE byEventQualification
  ) ;

  void SetFreqAndVoltage(
    BYTE byCoreID 
    , double d ) ;

  //TODO: maybe this is useful: AMD 11h CPU PDF: PSI_L Bit
  BYTE //setVidAndFrequencyForPState_Puma
    WriteToPstateOrCOFVIDcontrolRegister (
	  DWORD dwRegNr, 
   // unsigned char byVID, //unsigned short wFreqInMHz
	  //unsigned char byFreqID, 
   // unsigned char byDivID,
    //const PState & rpstateFromMSR,

    //Used to check against overvolting.
    const PState & r_pstateMergedFromUserAndMSR,
    //Used to write to MSR.
    PState & r_pstateFromUser,
    //PState & r_pstateMergedFromUserAndMSR,
    unsigned long dwMSRhigh,
    unsigned long dwMSRlow,
    //DWORD_PTR w64ulAffinityMask
    //DWORD dwCoreID
    //CPU core number, beginning from number "0"
    BYTE byCoreID
    ) ;

  //int ReadMSR(DWORD dwRegisterNumber, DWORD & dwLow, DWORD & dwHigh, BYTE byCoreID)
  //{
  //  return (*g_pfnRdMsrEx)(dwRegisterNumber, &dwLow, &dwHigh, byCoreID );
  //}

//#ifdef WIN32
#ifdef COMPILE_WITH_WINRING0
  BYTE SetSBIAddressRegister(BYTE bySBISMBusRegisterData);
#endif//#ifdef WIN32

  BYTE GetCurrentTempInDegCelsius(float & fTempInDegCelsius) ;

  //BYTE GetInterpolatedVoltageFromFreq(
  //  WORD wFreqInMHzToGetVoltageFrom
  //  , const //MaxVoltageForFreq 
  //    VoltageAndFreq & cr_maxfreqandvoltageNearestFreqGreaterEqual
  //  , const //MaxVoltageForFreq 
  //    VoltageAndFreq & cr_maxfreqandvoltageNearestFreqLowerEqual
  //  , float & r_fVoltageInVolt 
  //  ) ;

  WORD GetMinFreqToPreventOvervoltage(
    const MaxVoltageForFreq & maxvoltageforfreq,
    float fWantedVoltageInVolt) ;

  int GetPStateFromMSR(
    BYTE byPStateID, 
    DWORD & dwLow, 
    DWORD & dwHigh, 
    PState & r_pstate, 
    BYTE byCoreID
    //BYTE byThreadAffinityBitmask 
    );

  static unsigned long GetMSRregisterForPstate(
    unsigned char byPstate ) ;
  void GetMSRregisterValue( 
      BYTE byVoltageID, 
      const DIDandFID & didandfid , 
      DWORD & dwMSRhighmost , 
      DWORD & dwMSRlowmost 
      ) ;

  UserInterface * GetUserInterface() ;

  inline bool // TRUE: success, FALSE: failure
   RdmsrEx(
	  DWORD dwIndex,   // MSR index
	  DWORD & dwLowmostBits,	   // bit  0-31 ( register "EAX" )
	  DWORD & dwHighmostBits,   // bit 32-63 (register "EDX")
	  DWORD_PTR dwAffinityMask	// Thread Affinity Mask
    ) ;
  
  //There was linker error 2019 when it was declared as inline under MSVC 2005.
  //inline 
  bool // TRUE: success, FALSE: failure
   ReadPerformanceEventCounterRegister(
	  //DWORD dwIndex,   // MSR index
    BYTE byPerformanceEventCounterNumber ,
	  //DWORD & dwLowmostBits,	   // bit  0-31 ( register "EAX" )
	  //DWORD & dwHighmostBits,   // bit 32-63 (register "EDX")
    ULONGLONG & r_ull ,
	  DWORD_PTR dwAffinityMask	// Thread Affinity Mask
    ) ;

  bool UseDefaultFormulaForOvervoltageProtection(
    BYTE byVID,
    WORD wWantedFrequInMHz) ;

  //void SetFrequencyAndVoltageForAllCoresAccCPULoad();
  void SetFrequencyAndVoltageAccCPULoad(BYTE byCPUcoreID);
  void SetFrequencyAndVoltageForAllCoresAccCPULoad() ;

  BYTE SetNorthBridgeVoltage() ;

  BYTE SetPstate(BYTE byPstateID,BYTE byCoreID) ;
   
  //void SetPstate(BYTE byPstateID,DWORD dwCoreBitmask)
  //{
  //  changePstate(byPstateID,dwCoreBitmask);
  //}

  void SetUserInterface(UserInterface * p_userinterface ) ;
  BYTE SetVoltageAndFrequency( 
    float fVoltageInVolt, 
    WORD wFreqInMHz, 
    BYTE byCoreID 
    ) ;
  BYTE StartOrStopCalculationThread( BYTE byCoreID) ;

  bool GetCPUMiscControlDWORD(DWORD dwRegisterAddress, DWORD & dwValue) ;

//#ifdef WIN32
#ifdef _WINDOWS
//  void messageLoop();
  //ATOM MyRegisterClass();

//#ifndef LINK_TO_WINRING0_STATICALLY
//  void * GetHandleToDLLFunction(const std::string & strFuncName)
//  {
//    void * pfn ;
//    pfn = (void *) GetProcAddress(m_hinstanceWinRingDLL,strFuncName.c_str() );
//    return pfn ;
//  }
//#endif // #ifndef LINK_TO_WINRING0_STATICALLY
  
//  void SetPstateAccCPULoad()
//  {
////In addition to the RDMSR instruction, the PERF_CNT[3:0] registers 
  //can be read using a special read performance-monitoring counter instruction, RDPMC. 
  //The RDPMC instruction loads the contents of the PERF_CTR[3:0] register 
  //specified by the ECX register, into the EDX register and the EAX register.
//
//    //HQUERY hQuery;
//    //HCOUNTER hCounter;
//
//    //PDH_FMT_COUNTERVALUE FmtValue;
//
//    //PdhOpenQuery(NULL, 0, &hQuery);
//
//    //PdhAddCounter(hQuery, "\\Processor(_Total)\\% Processor Time", 0, &hCounter);
//
//    //printf("Starting the process...\n");
//
//    //PdhCollectQueryData(hQuery);
//
//    //PdhGetFormattedCounterValue(hCounter, PDH_FMT_DOUBLE, NULL, &FmtValue);
//    //printf("The cpu usage is : %f%%\n", FmtValue.doubleValue);
//
//    //PdhCloseQuery(hQuery);
//  }
  
  bool SetCPUMiscControlDWORD(
    //DWORD & r_dwPCIAddress,
    DWORD & r_dwRegisterAddress,
    DWORD & r_dwValue) ;
#endif//#ifdef _WINDOWS

  bool GetMaxPState(BYTE & byMaxPstate) ;
  bool SetMaxPState(BYTE byMaxPstate) ;

  //Implement this method our own because of this:
  //the OS freezes when freq > 1/2 max. freq && < max. freq.
  //So, of the freq is in this range, set it to either
  //1/2 max. freq or max. freq.
   BYTE SetFreqAndVoltageFromFreq(
    WORD wFreqInMHz 
    , BYTE byCoreID ) ;

  BYTE TooHot( ) ;

  void WriteToCOFVID(PState & pstate, BYTE byCoreID ) ;
  BOOL // TRUE: success, FALSE: failure
    //WINAPI
    WrmsrEx(
      DWORD index,		// MSR index
      DWORD dwLow ,//eax,			// bit  0-31
      DWORD dwHigh, //edx,			// bit 32-63
      DWORD affinityMask	// Thread Affinity Mask
    ) ;
public:
  BYTE UseMaxVoltageForFreqForOvervoltageProtection(   
    BYTE byVID
    ,
    WORD wWantedFrequInMHz
    );
public:
}; //end class

//#ifdef WIN32
//  LRESULT CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
//#endif
