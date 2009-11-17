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
    //, GriffinController * p_pumastatectrl
    , I_CPUcontroller * p_cpucontroller
    , CPUcoreData & r_cpucoredata
    ) ;
//  DynVoltAndFreqScaling(const DynVoltAndFreqScaling& orig);
  virtual ~DynVoltAndFreqScaling();
  BYTE Start() { return 1 ; }
private:

};

#endif	/* _DYNVOLTANDFREQSCALING_HPP */

