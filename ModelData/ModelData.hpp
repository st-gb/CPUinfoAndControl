#pragma once //include guard

#include <map> //for class std::map
#include <set> //for class std::set
#include <string> //class std::string
#include <vector> //for class std::vector

#include "CPUcoreData.hpp"
//because vector needs to know class MSRdata (else: build error)
#include "RegisterData.hpp" 
#include "ServiceAttributes.hpp"
#include "ValueTables.hpp"
#include "VoltageAndFreq.hpp"
#include <Controller/character_string/stdtstr.hpp>
#include <ModelData/UserInterfaceAttributes.hpp>

//class MSRdata ;
//class ValueTables ;

class I_CPUcontroller ;
class CPUcoreData ;

class Model
{
  BYTE m_byNumberOfCPUCores ;
public:
  bool m_bAppendProcessID ;
  volatile bool m_bCollectPstatesAsDefault ;
  bool m_bEnableOvervoltageProtection ;
  bool m_bSkipCPUtypeCheck ;
  bool m_bSyncGUIshowDataWithService ;
  bool m_bTruncateLogFileForEveryStartup ;
  bool m_bUseDefaultFormularForOvervoltageProtection ;
  bool m_bUsePstate0AsMaxFreq ;
//  double m_dPreviousPERF_CTRvalue ;
  I_CPUcontroller * mp_cpucontroller ;
  std::string m_strProcessorName ;
  std::string m_stdstrLogFilePath ;
  std::tstring m_stdtstrProgramName ;
  std::string m_stdstrCPUcontrollerDynLibPath ;
  std::string m_stdstrCPUcoreUsageGetterDynLibPath ;
  std::string m_stdstrExecutableStartupPath ;
  std::vector<MSRdata> m_stdvector_msrdata ;
  std::vector<CPUIDdata> m_stdvector_cpuiddata ;
  WORD m_wMaxFrequency ;
//  //std::vector<MaxVoltageForFreq> m_vecmaxvoltageforfreq ;
//  //Use a std::set because so the elements are sorted when inserting them.
//  //Also advantage: if the core frequencies are not sorted within the config
//  //file there is no error (because they are sorted by the set).
//  std::set<MaxVoltageForFreq> m_setmaxvoltageforfreq ;
  CPUcoreData m_cpucoredata ;
  //The attributes that differ between GUI and service _must_ be at the _end_
  //of _this_ class because all members that are used in the dynamic
  //library must be aligned as in the executable(!)
#ifdef COMPILE_AS_GUI
  std::wstring m_stdwstrPipeName ;
#endif
#ifdef COMPILE_AS_SERVICE
  ServiceAttributes m_serviceattributes ;
#endif //#ifdef COMPILE_AS_SERVICE

#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  Attributes::UserInterfaceAttributes m_userinterfaceattributes ;
#endif //#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  ValueTables m_valuetables ;
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
};
