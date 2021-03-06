/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //include guard

//#ifndef __CYGWIN__
//  #include <tchar.h> //for _T(), TCHAR
//#else
//  typedef char _TCHAR ;
//#endif
//For non-MSVC add include dir "Controller/MSVC_adaption" in order to find
//tchar.h .
#ifndef _WIN32 //__CYGWIN__ //does not have a "tchar.h" shipped.
    #include <Controller/MSVC_adaption/tchar.h> //_tcsrchr()
#else //MSC, MinGW (,...)
    #include <tchar.h> ////for _T(), TCHAR
#endif
#include <Controller/CPU-related/UsageGetterAndControllerBase.hpp>
//#if __GNUC__ == 4 //cygwin 1.7 has w32api/basetsd.h
//  #include <w32api/basetsd.h> //for DWORD_PTR
//#endif
//Microft and MinGW have basetsd.h
#if defined(_MSC_VER) || (__MINGW32__==1)
  #include <basetsd.h> //for DWORD_PTR
#endif
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
//for BOOL,BYTE,DWORD_PTR,...
#include <preprocessor_macros/Windows_compatible_typedefs.h>
#include <set> //for std::set
#include <string> // std::string
#ifdef _MSC_VER
  #define __FLT_MIN__ FLT_MIN
#endif
//typedef unsigned char BYTE ;
//#include "global.h" //for BYTE

//#include <Controller/I_CPUaccess.hpp>

#define SETTING_VOLTAGE_IS_UNSAFE 0
#define VOLTAGE_IS_TOO_HIGH_FOR_FREQUENCY 0
#define SETTING_VOLTAGE_IS_SAFE 1
#define NO_APPROPRIATE_VOLTAGE_FREQUENCY_PAIR_FOUND 2
#define NO_VOLTAGE_FREQUENCY_PAIR_EXISTANT 3
#define NOT_USING_ANY_OVERVOLTING_PROTECTION 4
#define NO_APPROPRIATE_2_VOLTAGE_FREQUENCY_PAIRS_FOUND 5
#define APPROPRIATE_2_VOLTAGE_FREQUENCY_PAIRS_FOUND 6
#define ONE_APPROPRIATE_VOLTAGE_FREQUENCY_PAIR_FOUND 7

//from Winring0's "OlsDef.h"
#define PciBusDevFunc(Bus, Dev, Func)	((Bus&0xFF)<<8) | ((Dev&0x1F)<<3) | (Func&7)

//Forward declaration (because _this_ header file may be included very often /
//more than once) is faster than to #include the while declaration file.
#ifdef COMPILE_WITH_CALC_THREAD
class ICalculationThread ;
#endif
class I_CPUaccess ;
class UserInterface ;
class Model ;
class IDynFreqScalingAccess ;
class VoltageAndFreq ;
class ICPUcoreUsageGetter ;

//#define FORCE_WCHAR_T

//base class for specific CPU controllers like e.g. 
//GriffinController and PentiumMcontroller
class I_CPUcontroller
  : public CPUcoreUsageGetterAndControllerBase
{
//protected or public to inherit the attributes (member vars)
public:
//  enum CheckWhetherVoltageIsAboveDefaultVoltage
//  {
//    not_above = 0 ,
//    No2PstatesForVoltageInterpolationFound ,
//    VoltageIsAboveDefaultVoltage
//  };
//  enum CheckWhetherVoltageIsBelowLowestStableVoltage
//  {
//    not_below = 0 ,
//    No2PstatesForVoltageInterpolationFound ,
//    VoltageIsBelowLowestStableVoltage
//  };
  enum CheckWhetherVoltageIsInSafeRange
  {
    in_safe_range = 0 ,
    VoltageMatchesInterpolatedVoltage,
    No2PstatesForVoltageInterpolationFound ,
    VoltageIsOutsideSafeRange
  };
  enum exec_state { success, not_implemented};
//  enum register_access_state{ success, failure};
  const std::set<VoltageAndFreq> * m_p_std_set_voltageandfreqUseForDVFS;
  float m_fReferenceClockInMHz ;
  bool m_b1CPUcorePowerPlane ;
  //For notifying via user interface if the CPU is too hot/ being throttled.
  long long m_llLastTimeTooHot;
  bool m_bDVFSfromServiceIsRunning;
  int m_byNumberOfCmdLineArgs;
  TCHAR ** m_arartcharCmdLineArg ;
  //std::vector<std::string> m_stdvecstdstringCmdLineArg ;

  UserInterface * mp_userinterface ;
  Model * mp_model ;
#ifdef COMPILE_WITH_CALC_THREAD
  ICalculationThread * mp_calculationthread ;
#endif //#ifdef COMPILE_WITH_CALC_THREAD
  IDynFreqScalingAccess * mp_dynfreqscalingaccess ;
  ICPUcoreUsageGetter * mp_icpucoreusagegetter ;
public:
  I_CPUcontroller() ;
  //in order for ~wxDynLibController to be called
  virtual ~I_CPUcontroller()
  {
    LOGN(/*"~I_CPUcontroller()"*/ "begin")
  }
  UserInterface * GetUserInterface() { return mp_userinterface ; }

  BYTE CheckWhetherVoltageIsAboveDefaultVoltage(float fVoltageInVolt,
    float fCPUcoreFrequencyinMHz) ;
  BYTE CheckWhetherVoltageIsBelowLowestStableVoltage(float fVoltageInVolt,
    float fCPUcoreFrequencyinMHz) ;
  BYTE CheckWhetherCPUcoreVoltageIsBelowHighestInstableVoltage(
    float fVoltageInVolt, float fCPUcoreFrequencyinMHz) ;
  /** This function exists because the reference clock may fluctuate, so
   * one cannot simply divide the CPU core frequency by the reference clock
   * for getting the multiplier. */
  float GetClosestMultiplier(WORD wFrequencyInMHz//, float fMultiplier
    );

  /** @brief This CPU instruction (in contrast to wrmsr) is not dangerous.
   * So it can be implemented for every CPU controller in this base class.
   * @param p_EAX : Use data type "uint32_t" because this is the exact width  
   *   of the CPU register */
  virtual BOOL CpuidEx(
    DWORD dwIndex,
    uint32_t * p_EAX,
    uint32_t * p_EBX,
    uint32_t * p_ECX,
    uint32_t * p_EDX,
    DWORD_PTR affinityMask
  ) ;
//  virtual bool CmdLineParamsContain( TCHAR * ptcharOption,
//      std::string & strValue );
  virtual void DecreaseVoltageBy1Step(float & r_fVoltage) {}
  /** Called before unloading a dyn lib CPU controller/ calling this class'
   * destructor so that resources (that were allocated in the CPU controller)
   * like threads etc. can be freed.
   * stopping threads in "DllMain(...) on Windows did not work?->program crash?*/
  virtual void DeInit() {};
  BYTE DisableFrequencyScalingByOS() ;
//  BYTE EnableOwnDVFS() ;
  //Some of these frequences may not be applicable.
  //e.g. AMD Griffin crashes with freq > 1/2 max freq & < max freq
  //It probably was more correct it would be called to walk through the possible 
  //>>multipliers<< because the frequency depends on the reference that can be
  //modified e.g. by the BIOS. 
  //So the frequency depends on the reference clock and the multiplier:
  //Clock frequency= multiplier * reference clock.
  //For instance the AMD Griffin and the Pentium M have a typical 
  //reference clock of 100 MHz.
  //The reference clock can be determined by querying the TimeStamp counter twice and
  //calculating a difference from these values.
//  virtual void GetAllPossibleFrequencies(std::set<VoltageAndFreq> &
//    r_stdsetvoltageandfreq) {} ;
  virtual void GetAvailableMultipliers(std::set<float> &
    r_stdset_float) {} ;
  virtual void GetAvailableThrottleLevels(std::set<float> &
    r_stdset_float) {} ;
  //MUST be declared virtual ("virtual ...") else 
  //GetCurrentPstate() of the derived class is NOT called.
  virtual BYTE GetCurrentPstate(
    WORD & r_wFreqInMHz 
    , float & Volt
    , BYTE byCoreID 
    )
  {
    return 0 ; 
  }
  //Stores multiplier, reference clock and voltage into the model data.
  virtual BYTE GetCurrentVoltageAndFrequencyAndStoreValues(
    WORD wCoreID ) ;
  virtual void GetCurrentTemperatureInCelsiusAndStoreValues(
    WORD wCoreID ) ;

  virtual BYTE GetCurrentVoltageAndFrequency(
    float & r_fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float & r_fMultiplier
    , float & r_fReferenceClockInMHz
    , WORD wCoreID
    )
  {
    LOGN(//"StoreCurrentVoltageAndFreqInArray"
      "begin")
//    LOGN("I_CPUcontroller::GetCurrentVoltageAndFrequency("
//      << wCoreID << ") begin" )
    return 0 ;
  }
  virtual float GetCPUcoreFrequencyInMHz( WORD wMultiplierIndex ) ;

//  BYTE GetInterpolatedVoltageFromFreq(
//    WORD wFreqInMHzToGetVoltageFrom,
//    float & r_fVoltageInVolt
//    , const std::set<VoltageAndFreq> & r_stdsetvoltageandfreq
//    ) ;

  //Can not simply do maximum Voltage ID -> voltage in Volt because
  //AMD:   max. voltage ID = MIN. voltage, 
  //Intel: max. voltage ID = MAX. voltage
  virtual float GetMaximumVoltageInVolt() ;
  //Can not simply do minimum Voltage ID -> voltage in Volt because
  //AMD:   min. voltage ID = MAX. voltage, 
  //Intel: min. voltage ID = MIN. voltage
  virtual float GetMinimumVoltageInVolt() { return 0.0 ; }
  //BYTE GetInterpolatedVoltageFromFreq(
  //  WORD wFreqInMHzToGetVoltageFrom,
  //  float & r_fVoltageInVolt 
  //  , std::map<VoltageAndFreq> & r_stdsetvoltageandfreq
  //  ) ;
  virtual WORD GetNumberOfPstates() ;//{ return 0 ; }
  //As far as I know there is no default way to determine the number of CPU
  //cores. 
  //So AMD Griffins store the value at CPUID function 8...8 but Intel
  //does not. 
  //As the number is specific the CPU models the controller should now
  //return the value. E.g for Intel Pentium Ms it is always? "1".
  virtual WORD GetNumberOfCPUcores() ;
  //Must be declared pure virtual ("virtual ... = 0 ;") else 
  //GetMaximumFrequencyInMHz() of the derived class is NOT called.
  virtual WORD GetMaximumFrequencyInMHz()
    { return 0; } ;
    //= 0 ;
  //Must be declared pure virtual ("virtual ... = 0 ;") else 
  //GetMinimumFrequencyInMHz() of the derived class is NOT called.
  virtual WORD GetMinimumFrequencyInMHz() //= 0 ;
    { return 0 ; }
  virtual WORD GetMaximumVoltageID() //= 0 ;
    { return 0 ; }
  virtual WORD GetMinimumVoltageID() //= 0 ;
    { return 0 ; }
  virtual WORD GetNearestHigherPossibleFreqInMHz(WORD wFreqInMhzOld) ;
  virtual WORD GetNearestLowerPossibleFreqInMHz(WORD wFreqInMhzOld) ;
  virtual BYTE GetPstate(
    const std::set<VoltageAndFreq> & c_r_std_set_voltages,
    WORD wPstateID,
    VoltageAndFreq & r_voltageandfreq) ;//= 0 ;
  virtual //bool 
    BYTE GetPstateSafefy(
    WORD wFreqInMHz
    , float fVoltage 
    ) ;
  virtual float GetTemperatureInCelsius( WORD wCoreID )
  {
    return
#ifdef _MSC_VER
    FLT_MIN ;
#else
    __FLT_MIN__ ;
#endif
  }
  virtual float GetThrottleLevel(unsigned coreID) = 0;
  virtual float GetVoltageInVolt(WORD wVoltageID )
    //= 0 ;
    {
      return 0.0f ;
    }
  virtual WORD GetVoltageID(float fVoltageInVolt ) //= 0 ;
    { return 0 ; }
//  BYTE HandleCmdLineArgs() ;
  virtual void IncreaseVoltageBy1Step(float & r_fVoltage) {}
  virtual void IncreaseVoltageForCurrentPstate(BYTE byCoreID)
  {
    //float & fVolt ;
    //WORD wFreqInMHz ;
    ////if( 
    //  GetCurrentPstate(
    //  wFreqInMHz 
    //  , fVolt
    //  , byCoreID 
    //  ) 
    //  //)
    //  ;
  }
  virtual BYTE Init() { return 1 ; } ;
  BYTE OtherPerfCtrlMSRwriteIsActive() ;

  virtual void PrepareForNextPerformanceCounting(
    DWORD dwAffinityBitMask 
    , BYTE byPerformanceEventSelectRegisterNumber
    ) { }
  void SetCmdLineArgs(
    //std::vector<std::string> & r_stdvecstdstringCmdLineArg 
      int argc, 
      //TCHAR * argv[]
      //Array of strings of type TCHAR
  #ifdef FORCE_WCHAR_T
    wchar_t ** argv
  #else
    TCHAR ** argv
  #endif
    ) ;
//  void SetCPUaccess(I_CPUaccess * p_cpu_access)
//  {
//    mp_cpuaccess = p_cpu_access ;
//  }
  void SetUserInterface( 
    //By using a pointer instead of a reference one can pass NULL 
    //to show that there is no object (e.g. if running as a service).
    UserInterface * ) ;
  void SetModelData( 
    //By using a pointer instead of a reference one can pass NULL 
    //to show that there is no object.
    Model * p_model ) ;
  void SetNextLowerThrottleLevel(unsigned coreID, float fCPUcoreMultiplierToUse);
#ifdef COMPILE_WITH_CALC_THREAD
  void SetCalculationThread( 
    //By using a pointer instead of a reference one can pass NULL 
    //to show that there is no object.
    ICalculationThread * calculationthread) ;
#endif //#ifdef COMPILE_WITH_CALC_THREAD
  void SetOtherDVFSaccess(
    //By using a pointer instead of a reference one can pass NULL 
    //to show that there is no object.
    IDynFreqScalingAccess * p_dynfreqscalingaccess ) ;
  //virtual 
    void SetFreqAndVoltage(
    BYTE byCoreID ,
    float fLoad ) {} ;
  //MUST be declared virtual ("virtual ...") else 
  //GetCurrentPstate() of the derived class is NOT called.
  virtual BYTE SetFreqAndVoltageFromFreq(
    WORD wFreqInMHz 
    , BYTE byCoreID) //{//return 0 ; 
    //} 
    //= 0 
    ;
  virtual BYTE SetFreqAndVoltageFromFreq(
    WORD wFreqInMHz 
    , const std::set<VoltageAndFreq> & 
      cr_stdsetvoltageandfreqForInterpolation
    , BYTE byCoreID ) ;//{ return 0 ; };

  //MUST be declared virtual ("virtual ...") else 
  //GetCurrentPstate() of the derived class is NOT called.
  virtual BYTE GetCPUcoreTooHot() { return 0 ; }
  void SetFrequencyInMHz(WORD wFre) {} ;
  //MUST be declared virtual ("virtual ...") else 
  //SetVoltageAndFrequency() of the derived class is NOT called.
  virtual BYTE 
    //Let voltage be the first element from name because the same in
    //"Dyn Voltage And Freq. Sclaing"
    SetVoltageAndFrequency(
      float fVolt    
      , WORD wFreqInMHz 
      , BYTE byCoreID 
      )
      {return 0 ; }
  //MUST be declared virtual ("virtual ...") else
  //SetCurrentVoltageAndMultiplier() of the derived class is NOT called.
  virtual BYTE SetCurrentVoltageAndMultiplier(
    float fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float fMultiplier
    , WORD wCoreID
    )
  {return 0 ; }
  virtual BYTE SetThrottleLevel(float level, unsigned coreID) { return
    not_implemented; }
      //= 0 ;
  //If this method should be implemented in a derived class it should check for
  // MSR indices and value for validity.
  inline virtual BOOL // TRUE: success, FALSE: failure
    //In g++ virtual methods can't be declared as stdcall
    //WINAPI
    WrmsrEx(
      DWORD index,		// MSR index
      DWORD dwLow ,//eax,			// bit  0-31
      DWORD dwHigh, //edx,			// bit 32-63
      DWORD affinityMask	// Thread Affinity Mask
      ) { return FALSE ; }
};
