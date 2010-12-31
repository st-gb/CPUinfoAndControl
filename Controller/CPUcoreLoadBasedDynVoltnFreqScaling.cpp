/*
 * CPUcoreLoadBasedDynVoltnFreqScaling.cpp
 *
 *  Created on: Dec 30, 2010
 *      Author: sgebauer
 */

#include "CPUcoreLoadBasedDynVoltnFreqScaling.hpp"

CPUcoreLoadBasedDynVoltnFreqScaling::CPUcoreLoadBasedDynVoltnFreqScaling(
  CPUcontrolBase & r_cpucontrolbase
  , CPUcoreData & r_cpucoredata
  )
  :
  DynFreqScalingThreadBase(r_cpucontrolbase, r_cpucoredata)
{

}

//CPUcoreLoadBasedDynVoltnFreqScaling::CPUcoreLoadBasedDynVoltnFreqScaling()
//{
//  // TODO Auto-generated constructor stub
//}

CPUcoreLoadBasedDynVoltnFreqScaling::~CPUcoreLoadBasedDynVoltnFreqScaling()
{
//  ~DynFreqScalingThreadBase() ;
}

void CPUcoreLoadBasedDynVoltnFreqScaling::HandleCPUnotTooHot()
{
  //Assign for the next time it gets too hot.
  m_b1stTimeInRowTooHot = true ;
  LOGN("too hot:no")
  //        if( mp_icpu->//GetPercentalUsageForBothCores
  //            GetPercentalUsageForAllCores(mp_cpucoredata->
  //            m_arfCPUcoreLoadInPercent)
  //          )
  for( byCoreID = 0 ; byCoreID < m_wNumCPUcores ; ++ byCoreID )
  {
    ChangeOperatingPointByLoad( byCoreID ,
      //mp_cpucoredata->m_arp_percpucoreattributes[byCoreID]
  //                mp_cpucoredata->m_arfCPUcoreLoadInPercent [byCoreID]
      m_ar_fCPUcoreLoadInPercent [byCoreID]
    ) ;
  }
}
