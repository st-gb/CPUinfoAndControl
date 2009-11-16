#pragma once //include guard

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

#ifndef __CYGWIN__
  #include <tchar.h> //for _T(), TCHAR
#else
  typedef char _TCHAR ;
#endif
//#include <basetsd.h> //for DWORD_PTR
#include <Windows_compatible_typedefs.h> //for DWORD_PTR
#include <set> //for std::set
//typedef unsigned char BYTE ;
#include "global.h" //for BYTE

class VoltageAndFreq ;
class ICPUcoreUsageGetter ;

//base class for specific CPU controllers like e.g. 
//GriffinController and PentiumMcontroller
class I_CPUcontroller
{
//protected or public to inherit the attributes (member vars)
protected:
  //The CPU access should be protected against direct access by 
  //not inherited classes because writing to an MSR may damage the CPU.
  //A good place for direct write to MSR is the concrete CPU controller
  //(NOT the CPU access class because there may be many of them->
  //against Dont Repeat Yourself principle) class.
  I_CPUaccess * mp_cpuaccess ;
public:
  int m_byNumberOfCmdLineArgs;
  _TCHAR ** m_arartcharCmdLineArg ;
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
  bool CmdLineParamsContain(_TCHAR * ptcharOption, std::string & strValue);
  BYTE DisableFrequencyScalingByOS() ;
  BYTE EnableOwnDVFS() ;
  virtual BYTE Init() { return 1 ; } ;
  BYTE GetInterpolatedVoltageFromFreq(
    WORD wFreqInMHzToGetVoltageFrom,
    float & r_fVoltageInVolt 
    , std::set<VoltageAndFreq> & r_stdsetvoltageandfreq
    ) ;
  //BYTE GetInterpolatedVoltageFromFreq(
  //  WORD wFreqInMHzToGetVoltageFrom,
  //  float & r_fVoltageInVolt 
  //  , std::map<VoltageAndFreq> & r_stdsetvoltageandfreq
  //  ) ;
  virtual WORD GetNumberOfPstates() { return 0 ; }
  WORD GetNumberOfCPUcores() ; 
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
  virtual BYTE GetPstate(WORD wPstateID, VoltageAndFreq & r_voltageandfreq) = 0 ;
  virtual //bool 
    BYTE GetPstateSafefy(
    WORD wFreqInMHz
    , float fVoltage 
    ) ;
  virtual float GetVoltageInVolt(WORD wVoltageID ) = 0 ;
  virtual WORD GetVoltageID(float fVoltageInVolt ) = 0 ;
  BYTE HandleCmdLineArgs() ;
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
      _TCHAR * argv[]
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
  virtual void SetFreqAndVoltageFromFreq(
    WORD wFreqInMHz 
    , BYTE byCoreID) {//return 0 ; 
    } 
    //= 0 ;
  BYTE TooHot() { return 0 ; }
  void SetFrequencyInMHz(WORD wFre) {} ;
  //MUST be declared virtual ("virtual ...") else 
  //GetCurrentPstate() of the derived class is NOT called.
  virtual BYTE 
    //Let voltage be the first element from name because the same in
    //"Dyn Voltage And Freq. Sclaing"
    SetVoltageAndFrequency(
      float fVolt    
      , WORD wFreqInMHz 
      ) {return 0 ; }
      //;
};
