/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
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
//from http://stackoverflow.com/questions/2094427/dll-export-as-a-c-c-function:
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  //For "storage class" see
  //  http://msdn.microsoft.com/en-us/library/dabb5z75%28v=vs.80%29.aspx
  #define CPU_CONTROL_BASE_CLASS_FUNCTION_STORAGE_CLASS //__declspec(dllexport)
#else
  #define CPU_CONTROL_BASE_CLASS_FUNCTION_STORAGE_CLASS //__attribute__ ((visibility("default")))
#endif // _WIN32

//Base class for the service and the GUI.
class CPUcontrolBase
{
public:
  int m_argumentCount;
  char ** m_ar_ar_ch_programArgs;
  static TCHAR s_ar_tchInstableCPUcoreVoltageWarning[];
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
  static I_CPUaccess * s_p_hardware_access;
  MainController m_maincontroller ;
  static Model m_model ;
  UserInterface * mp_userinterface ;
  static UserInterface * s_p_userinterface;
public:
//  CPUcontrolBase() ;
  CPUcontrolBase(const UserInterface * const cpc_userinterface ) ;
  //  //dtor must exist, else g++: "undefined reference to `vtable for
  //  //CPUcontrolBase'"?
  //Avoid g++ warning "warning: `class CPUcontrolBase' has virtual functions
  //but non-virtual destructor" .
  virtual ~CPUcontrolBase() ;

  I_CPUcontroller * CreateDynLibCPUcontroller_DynLibName(
    const std::string & CPUcontrollerDynLibName);
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
  //inline
    virtual void CreateHardwareAccessObject() ;
#ifdef _WIN32
    inline void CreateHardwareAccessObject_Windows();
#endif
  inline bool GetUsageAndVoltageAndFreqForAllCores(
    float ar_fCPUcoreLoadInPercent []
    , WORD wNumCPUcores
    )
  {
    LOGN(//<< wNumCPUcores << ")"
      "before GetPercentalUsageForAllCores" )
    //TODO exit thread when getting CPU core load fails?
    if( ! mp_cpucoreusagegetter ||
        //Use brackets to avoid g++ warning
        //  "suggest parentheses around '&&' within '||'"
        ( mp_cpucoreusagegetter &&
          //mp_icpu->//GetPercentalUsageForBothCores
          mp_cpucoreusagegetter->
          GetPercentalUsageForAllCores( //mp_cpucoredata->
          //m_arfCPUcoreLoadInPercent
          ar_fCPUcoreLoadInPercent
          )
        )
      )
    {
      LOGN("after GetPercentalUsageForAllCores")
      //Get the current voltage etc. for ALL cores for sending the data for
      // all cores via IPC, even if not needed for
      //DVFS (if single power plane / all cores always at the same p-state
      // then only the frequency for the core with the highest load is
      //needed).
      for( fastestUnsignedDataType coreID = 0; coreID < wNumCPUcores;
          ++ coreID )
      {
        mp_cpucontroller->GetCurrentVoltageAndFrequencyAndStoreValues(coreID) ;
  //            float fVoltageInVolt ;
  //            float fMultiplier ;
  //            float fReferenceClockInMhz ;
  //            mp_cpucontroller->GetCurrentVoltageAndFrequencyAndStoreValues(
  //              fVoltageInVolt,
  //              fMultiplier,
  //              fReferenceClockInMhz ,
  //              coreID ) ;
      }
//      m_criticalsection_typeCPUcoreData.Leave() ;
      LOGN("return true")
      return true ;
    }
//    m_criticalsection_typeCPUcoreData.Leave() ;
    LOGN("return false")
    return false ;
  }
  inline bool GetUsageAndVoltageAndFreqAndTempForAllCores(
    float ar_fCPUcoreLoadInPercent []
    , WORD wNumCPUcores
    )
  {
    bool b = GetUsageAndVoltageAndFreqForAllCores(ar_fCPUcoreLoadInPercent,
      wNumCPUcores);
    for( BYTE byCoreID = 0 ; byCoreID < wNumCPUcores ; ++ byCoreID )
    {
      mp_cpucontroller->GetCurrentTemperatureInCelsiusAndStoreValues(
        byCoreID) ;
    }
    return b;
  }
  inline bool //GetUsageAndVoltageAndFreqAndTempForAllCoresThreadSafe(
    GetUsageAndVoltageAndFreqForAllCoresThreadSafe(
    float ar_fCPUcoreLoadInPercent []
    , WORD wNumCPUcores )
  {
    LOGN( "begin")
    bool b ;
    m_criticalsection_typeCPUcoreData.Enter() ;
    b = //GetUsageAndVoltageAndFreqAndTempForAllCores(
      GetUsageAndVoltageAndFreqForAllCores(
      ar_fCPUcoreLoadInPercent , wNumCPUcores ) ;
    m_criticalsection_typeCPUcoreData.Leave() ;
    LOGN( "return " << (b ? "true" : "false") )
    return b ;
  }
//  BYTE CreateCPUcontrollerAndUsageGetter(
//    I_CPUcontroller * & r_p_cpucontroller
//    ,ICPUcoreUsageGetter * & r_p_icpucoreusagegetter) ;

  virtual void DeleteCPUcontroller() ;

  virtual void EndDVFS() ; //{}
  virtual void FreeRessources() ;
  static std::wstring GetExecutableFileName( const wchar_t * const
    executableFilePath );
  static void GetLogFileExtension(std::string & std_strFileExt);
  //static
    bool GetLogFilePropertiesAndOpenLogFile(
    std::wstring & std_wstrLogFilePath,
    std::wstring & std_wstrLogFileName);

  static void GetLogLevel(std::string & std_strLogLevels);
  static void GetLogTimeFormatString(std::string & std_strLogTimeFormatString);
  //Used for choosing the CPU controller, for saving as CPU controller for the
  //current CPU.
  //inline
  std::string GetCPUcontrollerConfigFilePath(
    const std::string & cr_stdstrCPUtypeRelativeDirPath ) ;
  void InitMemberVariables() ;
  static void OuputCredits() ;
  static void OutputLinkageWarning();

  //Should be implemented here (and not in UserInterface) because the service
  // usally can mot get feedback from a user (so this method shouldn't be
  // implemented there)
  virtual void PossiblyAskForOSdynFreqScalingDisabling() {}
  //Declare here because e.g. either a service or a GUI may delete a CPU
  //controller.
  //"Possibly" because: if the controller is NULL it is not being deleted.
  void PossiblyDeleteCPUcontrollerDynLib() ;
  void RemoveDefaultVoltagesInsertedByCPUcontroller();
  //Declare here because e.g. either a service or a GUI may delete a CPU core
  //usage getter.
  void PossiblyDeleteCPUcoreUsageGetter() ;

  I_CPUaccess * GetCPUaccess() { return mp_i_cpuaccess ; }

  void ReadLogConfig(//std::tstring & r_std_tstrLogFilePath
    std::wstring & r_std_wstrLogFilePath);
  static CPU_CONTROL_BASE_CLASS_FUNCTION_STORAGE_CLASS
  BOOL ReadPCIconfigSpace(
    BYTE byPCIbus , //"8-bit PCI bus",
    BYTE byDeviceAndFunction ,//"5-bit device, and 3-bit function"
    DWORD dwRegisterAddress ,
    PDWORD p_dwValue
    );
  static CPU_CONTROL_BASE_CLASS_FUNCTION_STORAGE_CLASS
    BOOL ReadMSR(
    DWORD dwIndex,    // MSR index
    PDWORD p_dweax,     // bit  0-31
    PDWORD p_dwedx,     // bit 32-63
    DWORD_PTR affinityMask  // Thread Affinity Mask
    );
  static CPU_CONTROL_BASE_CLASS_FUNCTION_STORAGE_CLASS
    BOOL WriteMSR(
    DWORD dwIndex,    // MSR index
    DWORD dwEAX,     // bit  0-31
    DWORD dwEDX,     // bit 32-63
    DWORD_PTR affinityMask  // Thread Affinity Mask
    );

  virtual void SetCPUcontroller( I_CPUcontroller * p_cpucontrollerNew ) {}
  //inline
    void SetDynVoltnFreqScalingType_Inline();
  virtual void StartDynamicVoltageAndFrequencyScaling() ;
} ;

#endif /* CPUCONTROLBASE_HPP_ */
