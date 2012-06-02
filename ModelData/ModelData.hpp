/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //include guard

#include <map> //for class std::map
#include <set> //for class std::set
#include <string> //class std::string
#include <vector> //for class std::vector

#include "CPUcoreData.hpp"
#include "VoltageAndFreq.hpp"
#include <Controller/character_string/stdtstr.hpp>

#if defined COMPILE_AS_GUI
  #include <ModelData/UserInterfaceAttributes.hpp>
  //because vector needs to know class MSRdata (else: build error)
  #include "RegisterData.hpp"
  #include "ValueTables.hpp"

  #define COMPILE_AS_EXECUTABLE
#endif
#if defined COMPILE_AS_SERVICE
  #include "ServiceAttributes.hpp"
  #define COMPILE_AS_EXECUTABLE
#endif

#ifdef COMPILE_AS_EXECUTABLE
  //class InstableCPUcoreVoltageDetection
  #include "InstableCPUcoreVoltageDetection.hpp"
#endif

//class MSRdata ;
//class ValueTables ;

class I_CPUcontroller ;
class CPUcoreData ;

//The member variables/ functions that differ between GUI and service _must_
//be situated at the _end_ of _this_ class because all members that are used
//in the attached (CPU controller) dynamic library must be aligned as in the
//(CPU controller) executable (Graphical User Interface/ service!)
class Model
{
public:
  //This is the last member that is also accessed by the CPU dyn. libs.
  CPUcoreData m_cpucoredata ;

#ifdef COMPILE_AS_EXECUTABLE
  //TODO all member that are needed by both the dyn libs and the executables
  //must be on top.
  BYTE m_byNumberOfCPUCores ;
  bool m_bAppendProcessID ;
  volatile bool m_bCollectPstatesAsDefault ;
  bool m_bEnableOvervoltageProtection ;
  bool m_bSkipCPUtypeCheck ;
  bool m_bSyncGUIshowDataWithService ;
  bool m_bTruncateLogFileForEveryStartup ;
//  bool m_bUseDefaultFormulaForOvervoltageProtection ;
//  bool m_bUsePstate0AsMaxFreq ;
//  double m_dPreviousPERF_CTRvalue ;
  I_CPUcontroller * mp_cpucontroller ;

  std::string m_strProcessorName ;
  std::string m_stdstrLogFilePath ;
  std::tstring m_stdtstrProgramName ;
  std::string m_stdstrCPUcontrollerDynLibPath ;
  std::string m_stdstrCPUcoreUsageGetterDynLibPath ;
  std::string m_stdstrExecutableStartupPath ;
//  WORD m_wMaxFrequency ;

//  //std::vector<MaxVoltageForFreq> m_vecmaxvoltageforfreq ;
//  //Use a std::set because so the elements are sorted when inserting them.
//  //Also advantage: if the core frequencies are not sorted within the config
//  //file there is no error (because they are sorted by the set).
//  std::set<MaxVoltageForFreq> m_setmaxvoltageforfreq ;

  //->Insert/ change additional data here.

  std::string m_std_strConfigFilePath;
  std::string m_std_strLogFilePath;
  InstableCPUcoreVoltageDetection m_instablecpucorevoltagedetection;
  uint16_t m_StableCPUcoreVoltageWaitTimeInMillis;

//  #include "GUI_member_variables.hpp"

  //The attributes that differ between GUI and service _must_ be at the _end_
  //of _this_ class because all members that are used in the dynamic
  //library must be aligned as in the executable(!)
#ifdef COMPILE_AS_GUI
  std::wstring m_stdwstrPipeName ;
//  ValueTables m_valuetables ;
  std::vector<MSRdata> m_stdvector_msrdata ;
  std::vector<CPUIDdata> m_stdvector_cpuiddata ;
//#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  Attributes::UserInterfaceAttributes m_userinterfaceattributes ;
//#endif //#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
#endif
#ifdef COMPILE_AS_SERVICE
  ServiceAttributes m_serviceattributes ;
#endif //#ifdef COMPILE_AS_SERVICE

  Model( ) ;

//  void AddMaxVoltageForFreq(WORD wFreqInMHz,float fValue) ;
  void AddValueTableRow(std::vector<std::string [2]>
    stdvecstdstrAttributeNameAndValue) ;
  //void Model::AddValueTableRow(
  //  std::vector<MSRdata>::reverse_iterator rev_iter_msrdata ,
  //  std::vector<std::string [2]> & stdvecstdstrAttributeNameAndValue) ;
  BYTE GetNumberOfCPUCores() ;
  void SetCPUcontroller( I_CPUcontroller * p_cpucontroller )
  {
    mp_cpucontroller = p_cpucontroller ;
    m_cpucoredata.SetCPUcontroller( p_cpucontroller ) ;
  }
  void SetNumberOfCPUCores(BYTE byNumberOfCPUCores) ;
  void SetSkipCPUTypeCheck(bool bSkipCPUtypeCheck) ;
#endif //COMPILE_AS_EXECUTABLE
};
