/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * CPUcoreLoadBasedDynVoltnFreqScaling.cpp
 *
 *  Created on: Dec 30, 2010
 *      Author: sgebauer
 */

#include "CPUcoreLoadBasedDynVoltnFreqScaling.hpp"
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)

CPUcoreLoadBasedDynVoltnFreqScaling::CPUcoreLoadBasedDynVoltnFreqScaling(
  CPUcontrolBase & r_cpucontrolbase
  , CPUcoreData & r_cpucoredata
  )
  :
  DynFreqScalingThreadBase(r_cpucontrolbase, r_cpucoredata)
{
  LOGN( "begin")
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
  LOGN( /*" too hot:no"*/ "begin # CPU cores:"
    << m_wNumCPUcores)
  DynFreqScalingThreadBase::HandleCPUnotTooHot();
  //        if( mp_icpu->//GetPercentalUsageForBothCores
  //            GetPercentalUsageForAllCores(mp_cpucoredata->
  //            m_arfCPUcoreLoadInPercent)
  //          )
  for( coreID = 0 ; coreID < m_wNumCPUcores ; ++ coreID )
  {
    ChangeOperatingPointByLoad( coreID ,
      //mp_cpucoredata->m_arp_percpucoreattributes[byCoreID]
  //                mp_cpucoredata->m_arfCPUcoreLoadInPercent [byCoreID]
      m_ar_fCPUcoreLoadInPercent [coreID]
    ) ;
  }
}
