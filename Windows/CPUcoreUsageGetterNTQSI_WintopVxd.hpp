#pragma once //incl. guard

#include <Controller/ICPUcoreUsageGetter.hpp>
#include "ModelData/CPUcoreData.hpp"
//#include "CpuUsage.h" //Crystal CPU usage getter
//#include "Windows/CPUcoreUsageNTQSI_OO.hpp"
#include "Windows/CPUcoreUsageNTQSI_OO2.hpp"

class CPUcoreUsageGetterNTQSI_WintopVxd
  : public ICPUcoreUsageGetter
{
  CPU_USAGE m_cpu_usage ;
  CPUcoreData * mp_cpucoredata ;
  CPUcoreUsageNTQSI m_cpucoreusagentqsi ;
public:
  CPUcoreUsageGetterNTQSI_WintopVxd(
    CPUcoreData & cpucoredata)
  {
    mp_cpucoredata = & cpucoredata ;
    InitCpuUsage() ;
  }
  ~CPUcoreUsageGetterNTQSI_WintopVxd()
  {
    CleanupCpuUsage() ;
  }
  BYTE Init() { return 0 ; }

  BYTE GetPercentalUsageForAllCores( float arf [] )
  {
      CPU_USAGE cpu_usage ;
    for(BYTE byCPUcoreNumber = 0 ; byCPUcoreNumber < 
      mp_cpucoredata->GetNumberOfCPUcores() ; ++ byCPUcoreNumber 
      )
#ifdef _DEBUG
    {
      //float f = ::GetCpuUsage( byCPUcoreNumber , & //m_cpu_usage 
      //  cpu_usage ) ;
      float f = m_cpucoreusagentqsi.GetCpuUsageNT(0 , & cpu_usage ) ;
      arf[byCPUcoreNumber] = f ;
    }
#else
      arf[byCPUcoreNumber] = (*pfnGetCPUcoreUsage)(byCPUcoreNumber);
#endif
    return 0 ;
  }
};
