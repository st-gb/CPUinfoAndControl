#pragma once //include guard

#include <string>
#include <vector> //for std::vector
#include <map> //for std::map
#include <set> //for std::set
#include "CPUcoreData.hpp"
#include "PStates.h" // for class PStates
//because vector needs to know class MSRdata (else: build error)
#include "RegisterData.hpp" 
#include "ValueTables.hpp"
#include "VoltageAndFreq.hpp"
#include <Controller/stdtstr.hpp>

//class MSRdata ;
//class ValueTables ;

class PStates ;
//class GriffinController ;
class I_CPUcontroller ;
class CPUcoreData ;

class Model
{
  BYTE m_byNumberOfCPUCores ;
public:
  volatile bool m_bCollectPstatesAsDefault ;
  bool m_bEnableOvervoltageProtection ;
  bool m_bSkipCPUtypeCheck ;
  bool m_bTruncateLogFileForEveryStartup ;
  bool m_bUseDefaultFormularForOvervoltageProtection ;
  bool m_bUsePstate0AsMaxFreq ;
  double m_dPreviousPERF_CTRvalue ;
  //GriffinController * mp_griffincontroller ;
  I_CPUcontroller * mp_cpucontroller ;
  PStates m_pstates ;
  std::string m_strProcessorName ;
  std::string m_stdstrLogFilePath ;
  std::tstring m_stdtstrProgramName ;
  std::string m_stdstrCPUcontrollerDynLibPath ;
  std::string m_stdstrCPUcoreUsageGetterDynLibPath ;
  std::vector<MSRdata> m_stdvector_msrdata ;
  std::vector<CPUIDdata> m_stdvector_cpuiddata ;
  WORD m_wMaxFrequency ;
  //std::vector<MaxVoltageForFreq> m_vecmaxvoltageforfreq ;
  //Use a std::set because so the elements are sorted when inserting them.
  //Also advantage: if the core frequencies are not sorted within the config
  //file there is no error (because they are sorted by the set).
  std::set<MaxVoltageForFreq> m_setmaxvoltageforfreq ;
  CPUcoreData m_cpucoredata ;
  ValueTables m_valuetables ;
  Model( ) ;//GriffinController * p_griffincontroller ) ;

  void AddMaxVoltageForFreq(WORD wFreqInMHz,float fValue) ;
  void AddValueTableRow(std::vector<std::string [2]>
    stdvecstdstrAttributeNameAndValue) ;
  //void Model::AddValueTableRow(
  //  std::vector<MSRdata>::reverse_iterator rev_iter_msrdata ,
  //  std::vector<std::string [2]> & stdvecstdstrAttributeNameAndValue) ;
  BYTE GetNumberOfCPUCores() ;
  //void SetGriffinController ( GriffinController * p_griffincontroller )
  //{
  //  mp_griffincontroller = p_griffincontroller ;
  //  m_cpucoredata.SetGriffinController( p_griffincontroller ) ;
  //}
  void SetCPUcontroller( I_CPUcontroller * p_cpucontroller )
  {
    mp_cpucontroller = p_cpucontroller ;
    m_cpucoredata.SetCPUcontroller( p_cpucontroller ) ;
  }
  void SetNumberOfCPUCores(BYTE byNumberOfCPUCores) ;
  void SetSkipCPUTypeCheck(bool bSkipCPUtypeCheck) ;
};
