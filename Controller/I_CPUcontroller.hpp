#pragma once //include guard

//#ifndef __CYGWIN__
//  #include <tchar.h> //for _T(), TCHAR
//#else
//  typedef char _TCHAR ;
//#endif
//For non-MSVC add include dir "Controller/MSVC_adaption" in order to find
//tchar.h .
#ifdef __CYGWIN__ //does not have a "tchar.h" shipped.
    #include <Controller/MSVC_adaption/tchar.h> //_tcsrchr()
#else //MSC, MINGW (,...)
    #include <tchar.h> ////for _T(), TCHAR
#endif
#if __GNUC__ == 4 //cygwin 1.7 has w32api/basetsd.h
  #include <w32api/basetsd.h> //for DWORD_PTR
#endif
//Microft and MinGW have basetsd.h
#if defined(_MSC_VER) || (__MINGW32__==1)
  #include <basetsd.h> //for DWORD_PTR
#endif
#include <Windows_compatible_typedefs.h> //for BOOL,BYTE,DWORD_PTR,...
#include <set> //for std::set
#include <string> // std::string
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

class ICalculationThread ;
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
{
//protected or public to inherit the attributes (member vars)
protected:
//public:
  //The CPU access should be protected against direct access by 
  //not inherited classes because writing to an MSR may damage the CPU.
  //A good place for direct write to MSR is the concrete CPU controller
  //(NOT the CPU access class because there may be many of them->
  //against Dont Repeat Yourself principle) class.
  I_CPUaccess * mp_cpuaccess ;
public:
  int m_byNumberOfCmdLineArgs;
  TCHAR ** m_arartcharCmdLineArg ;
  //std::vector<std::string> m_stdvecstdstringCmdLineArg ;

  UserInterface * mp_userinterface ;
  Model * mp_model ;
  ICalculationThread * mp_calculationthread ;
  IDynFreqScalingAccess * mp_dynfreqscalingaccess ;
  ICPUcoreUsageGetter * mp_icpucoreusagegetter ;
public:
  I_CPUcontroller() ;
  UserInterface * GetUserInterface() { return mp_userinterface ; }

  //This CPU instruction (in contrast to wrmsr) is not dangerous.
  //So it can be implemented for every CPU controller in this base class.
  virtual BOOL CpuidEx(
    DWORD dwIndex,
    PDWORD p_dwEAX,
    PDWORD p_dwEBX,
    PDWORD p_dwECX,
    PDWORD p_dwEDX,
    DWORD_PTR affinityMask
  ) ;
  bool CmdLineParamsContain( TCHAR * ptcharOption, std::string & strValue);
  virtual void DecreaseVoltageBy1Step(float & r_fVoltage) {}
  BYTE DisableFrequencyScalingByOS() ;
  BYTE EnableOwnDVFS() ;
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
  virtual void GetAllPossibleFrequencies(std::set<VoltageAndFreq> & 
    r_stdsetvoltageandfreq) {} ;
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

  BYTE GetInterpolatedVoltageFromFreq(
    WORD wFreqInMHzToGetVoltageFrom,
    float & r_fVoltageInVolt 
    , const std::set<VoltageAndFreq> & r_stdsetvoltageandfreq
    ) ;
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
  //As far as I know there is no standard way to determine the number of CPU
  //cores. So AMD Griffins store the value at CPUID function 8...8 but not
  //Intel. As the number is spectic the CPU models the controller should know
  //return the value. E.g for Intel Pentium Ms it is always? "1".
  virtual WORD GetNumberOfCPUcores() ;
  //Must be declared pure virtual ("virtual ... = 0 ;") else 
  //GetMaximumFrequencyInMHz() of the derived class is NOT called.
  virtual WORD GetMaximumFrequencyInMHz() //{ return 0; } ;
    = 0 ;
  //Must be declared pure virtual ("virtual ... = 0 ;") else 
  //GetMinimumFrequencyInMHz() of the derived class is NOT called.
  virtual WORD GetMinimumFrequencyInMHz() = 0 ;
  virtual WORD GetMaximumVoltageID() = 0 ;
  virtual WORD GetMinimumVoltageID() = 0 ;
  virtual WORD GetNearestHigherPossibleFreqInMHz(WORD wFreqInMhzOld) ;
  virtual WORD GetNearestLowerPossibleFreqInMHz(WORD wFreqInMhzOld) ;
  virtual BYTE GetPstate(WORD wPstateID, VoltageAndFreq & r_voltageandfreq) ;//= 0 ;
  virtual //bool 
    BYTE GetPstateSafefy(
    WORD wFreqInMHz
    , float fVoltage 
    ) ;
  virtual float GetVoltageInVolt(WORD wVoltageID ) = 0 ;
  virtual WORD GetVoltageID(float fVoltageInVolt ) = 0 ;
  BYTE HandleCmdLineArgs() ;
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
  //virtual bool IsLowerVoltageThan( float fValueToProof, float fValueToCompare) ;
  virtual bool VIDisLowerVoltageThan( WORD wVIDisLessThan, WORD wVIDvalueToCompare) ;
  BYTE OtherPerfCtrlMSRwriteIsActive() ;

  virtual void PrepareForNextPerformanceCounting(
    DWORD dwAffinityBitMask 
    , BYTE byPerformanceEventSelectRegisterNumber
    ) { }
  //Advantage for a RdmsrEx() inside CPU controller: one does not
  //need to MANUALLY check if cpuaccess is assigned. (not NULL)
  //because this is done by this class' RdmsrEx() method.
  //inline 
  bool // TRUE: success, FALSE: failure
   RdmsrEx(
	  DWORD dwIndex,		// MSR index
	  DWORD & dwLowmostBits,			// bit  0-31 (register "EAX")
	  DWORD & dwHighmostBits,			// bit 32-63 (register "EDX")
	  DWORD_PTR dwAffinityMask	// Thread Affinity Mask
    ) ;
  //Possible handling for resume from standby (ACPI S3) or 
  //hibernate  (S4). E.g. with Griffin CPUs undervolting
  //is possible without an own DynVoltFreqScal. But  then 
  //the performance state registers muste be rewritten after
  //resume.
  virtual void ResumeFromS3orS4() {} ;
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
  void SetUserInterface( 
    //By using a pointer instead of a reference one can pass NULL 
    //to show that there is no object (e.g. if running as a service).
    UserInterface * ) ;
  void SetModelData( 
    //By using a pointer instead of a reference one can pass NULL 
    //to show that there is no object.
    Model * p_model ) ;
  void SetCPUaccess(
    //By using a pointer instead of a reference one can pass NULL 
    //to show that there is no object.
    I_CPUaccess * ) ;
  void SetCalculationThread( 
    //By using a pointer instead of a reference one can pass NULL 
    //to show that there is no object.
    ICalculationThread * calculationthread) ;
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
  virtual BYTE TooHot() { return 0 ; }
  void SetFrequencyInMHz(WORD wFre) {} ;
  //MUST be declared virtual ("virtual ...") else 
  //GetCurrentPstate() of the derived class is NOT called.
  virtual BYTE 
    //Let voltage be the first element from name because the same in
    //"Dyn Voltage And Freq. Sclaing"
    SetVoltageAndFrequency(
      float fVolt    
      , WORD wFreqInMHz 
      , BYTE byCoreID 
      ) //{return 0 ; }
      = 0 ;
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
