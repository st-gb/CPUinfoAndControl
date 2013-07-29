/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/* 
 * File:   DynVoltAndFreqScaling.hpp
 * Author: sgebauer
 *
 * Created on 17. November 2009, 19:47
 */

#ifndef _DYNVOLTANDFREQSCALING_HPP
#define	_DYNVOLTANDFREQSCALING_HPP

#include "DynFreqScalingThreadBase.hpp"

class DynVoltAndFreqScaling 
:public DynFreqScalingThreadBase
{
public:
  //DynVoltAndFreqScaling();
  DynVoltAndFreqScaling(
    ICPUcoreUsageGetter * p_icpu
    , I_CPUcontroller * p_cpucontroller
    , CPUcoreData & r_cpucoredata
    ) ;
//  DynVoltAndFreqScaling(const DynVoltAndFreqScaling& orig);
  virtual ~DynVoltAndFreqScaling();
  //BYTE Start() { return 1 ; }
  DWORD Start() { return 1 ; }
private:

};

#endif	/* _DYNVOLTANDFREQSCALING_HPP */

