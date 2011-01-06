/*
 * CPUcontrolBase.hpp
 *
 *  Created on: Mar 23, 2010
 *      Author: Stefan
 */

#ifndef CPUCONTROLBASE_HPP_
#define CPUCONTROLBASE_HPP_

//#include <Controller/UserInterface.hpp>
#include <Controller/CPU-related/I_CPUcontroller.hpp>
#include <Controller/CPU-related/ICPUcoreUsageGetter.hpp>
#include <Controller/DynLibHandler_type.hpp> //dynlibhandler_type
#include <Controller/MainController.hpp> //class MainController
//for criticalsection_type
#include <Controller/multithread/criticalsection_type.hpp>
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
#include <ModelData/ModelData.hpp>

//#include <global.h> //LOGN()

//Forward declaration (because _this_ header file may be included very often /
//more than once) is faster than to #include the while declaration file.
class CPUcoreData ;
class DynFreqScalingThreadBase ;
class IDynFreqScalingAccess ;
class I_CPUaccess ;
class I_CPUcontroller ;
class ICPUcoreUsageGetter ;
class UserInterface ;

//#define NULL 0

//Base class for the service and the GUI.
class CPUcontrolBase
{
public:
  enum
  {
    success = 0,
    no_default_voltages_specified,
    no_preferred_voltages_specified
  };
  enum DynVoltnFreqScalingType
  {
    none,
    LoadBased,
    TemperatureBased
  };
  BYTE m_byVoltageAndFrequencyScalingType;
  //Additional controller Dyn libs to reflect whether there are dyn lib
  // controller dyn libs available. An alternative would be to use RunTime
  // Type Information (RTTI); i.e. "if ( typeid(mp_cpucontroller) ==
  // typeid(wxDynLibCPUcontroller)"
#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
  IDynFreqScalingAccess * mp_dynfreqscalingaccess ;
#endif //#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
  DynFreqScalingThreadBase * mp_dynfreqscalingthreadbase ;
  I_CPUcontroller * m_p_cpucontrollerDynLib ;
  ICPUcoreUsageGetter * m_p_cpucoreusagegetterDynLib ;
  criticalsection_type m_criticalsection_typeCPUcoreData ;
  I_CPUcontroller * mp_cpucontroller ;
  ICPUcoreUsageGetter * mp_cpucoreusagegetter ;
protected:
  bool m_bXercesSuccessfullyInitialized ;
public:
  dynlibhandler_type m_dynlibhandler ;
  I_CPUaccess * mp_i_cpuaccess ;
  MainController m_maincontroller ;
  Model m_model ;
  UserInterface * mp_userinterface ;
public:
//  CPUcontrolBase() ;
  CPUcontrolBase(const UserInterface * const cpc_userinterface ) ;
  //  //dtor must exist, else g++: "undefined reference to `vtable for
  //  //CPUcontrolBase'"?
  //Avoid g++ warning "warning: `class CPUcontrolBase' has virtual functions
  //but non-virtual destructor" .
  virtual ~CPUcontrolBase() ;
  void CreateDynLibCPUcontroller(
    const std::string & stdstrCPUtypeRelativeDirPath
//    , I_CPUcontroller * & r_p_cpucontroller
    ) ;
  void CreateDynLibCPUcoreUsageGetter(
    const std::string & stdstrCPUtypeRelativeDirPath
//    , ICPUcoreUsageGetter * & r_p_icpucoreusagegetter
    ) ;
  //Created an object of subclass of I_CPUacces for CPU and PCIconfig etc.
  //access .
  inline virtual void CreateHardwareAccessObject() ;
  inline bool GetUsageAndVoltageAndFrequencyForAllCores(
    float ar_fCPUcoreLoadInPercent []
    , WORD wNumCPUcores
    )
  {
    LOGN("CPUcontrolBase::GetUsageAndVoltageAndFrequencyForAllCores(...,"
      << wNumCPUcores << ")--before GetPercentalUsageForAllCores" )
    //TODO exit thread when getting CPU core load fails?
    if( ! mp_cpucoreusagegetter || mp_cpucoreusagegetter &&
        //mp_icpu->//GetPercentalUsageForBothCores
        mp_cpucoreusagegetter->
        GetPercentalUsageForAllCores( //mp_cpucoredata->
        //m_arfCPUcoreLoadInPercent
        ar_fCPUcoreLoadInPercent
        )
      )
    {
      LOGN("CPUcontrolBase::GetUsageAndVoltageAndFrequencyForAllCores--"
        "after GetPercentalUsageForAllCores")
      //Get the current voltage etc. for ALL cores for sending the data for
      // all cores via IPC, even if not needed for
      //DVFS (if single power plane / all cores always at the same p-state
      // then only the frequency for the core with the highest load is
      //needed).
      for( BYTE byCoreID = 0 ; byCoreID < wNumCPUcores ; ++ byCoreID )
      {
        mp_cpucontroller->GetCurrentVoltageAndFrequencyAndStoreValues(byCoreID) ;
  //            float fVoltageInVolt ;
  //            float fMultiplier ;
  //            float fReferenceClockInMhz ;
  //            mp_cpucontroller->GetCurrentVoltageAndFrequencyAndStoreValues(
  //              fVoltageInVolt,
  //              fMultiplier,
  //              fReferenceClockInMhz ,
  //              byCoreID ) ;
        mp_cpucontroller->GetCurrentTemperatureInCelsiusAndStoreValues(
          byCoreID) ;
      }
//      m_criticalsection_typeCPUcoreData.Leave() ;
      LOGN("CPUcontrolBase::GetUsageAndVoltageAndFrequencyForAllCores--"
        "return true")
      return true ;
    }
//    m_criticalsection_typeCPUcoreData.Leave() ;
    LOGN("CPUcontrolBase::GetUsageAndVoltageAndFrequencyForAllCores--"
      "return false")
    return false ;
  }
  inline bool GetUsageAndVoltageAndFrequencyForAllCoresThreadSafe(
    float ar_fCPUcoreLoadInPercent []
    , WORD wNumCPUcores )
  {
    bool b ;
    m_criticalsection_typeCPUcoreData.Enter() ;
    b = GetUsageAndVoltageAndFrequencyForAllCores(
      ar_fCPUcoreLoadInPercent , wNumCPUcores ) ;
    m_criticalsection_typeCPUcoreData.Leave() ;
    return b ;
  }
//  BYTE CreateCPUcontrollerAndUsageGetter(
//    I_CPUcontroller * & r_p_cpucontroller
//    ,ICPUcoreUsageGetter * & r_p_icpucoreusagegetter) ;

  virtual void DeleteCPUcontroller() ;

  virtual void EndDVFS() ; //{}
  virtual void FreeRessources() ;
  //Used for choosing the CPU controller, for saving as CPU controller for the
  //current CPU.
  //inline
  std::string GetCPUcontrollerConfigFilePath(
    const std::string & cr_stdstrCPUtypeRelativeDirPath ) ;
  void InitMemberVariables() ;
  static void OuputCredits() ;
  //Should be implemented here (and not in UserInterface) because the service
  // usally can mot get feedback from a user (so this method shouldn't be
  // implemented there)
  virtual void PossiblyAskForOSdynFreqScalingDisabling() {}
  //Declare here because e.g. either a service or a GUI may delete a CPU
  //controller.
  //"Possibly" because: if the controller is NULL it is not being deleted.
  void PossiblyDeleteCPUcontrollerDynLib() ;

  //Declare here because e.g. either a service or a GUI may delete a CPU core
  //usage getter.
  void PossiblyDeleteCPUcoreUsageGetter() ;

  I_CPUaccess * GetCPUaccess() { return mp_i_cpuaccess ; }
  virtual void SetCPUcontroller( I_CPUcontroller * p_cpucontrollerNew ) {}
  //inline
    void SetDynVoltnFreqScalingType_Inline();
  virtual void StartDynamicVoltageAndFrequencyScaling() ;
} ;

#endif /* CPUCONTROLBASE_HPP_ */
