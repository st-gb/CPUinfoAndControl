/*
 * CPUcoreLoadBasedDynVoltnFreqScaling.hpp
 *
 *  Created on: Dec 30, 2010
 *      Author: sgebauer
 */

#ifndef CPUCORELOADBASEDDYNVOLTNFREQSCALING_HPP_
#define CPUCORELOADBASEDDYNVOLTNFREQSCALING_HPP_

#include "DynFreqScalingThreadBase.hpp"

class CPUcoreLoadBasedDynVoltnFreqScaling
  :
  public DynFreqScalingThreadBase
{
public:
//  CPUcoreLoadBasedDynVoltnFreqScaling();
  CPUcoreLoadBasedDynVoltnFreqScaling(
    CPUcontrolBase & r_cpucontrolbase
    , CPUcoreData & r_cpucoredata
    ) ;
  virtual
  ~CPUcoreLoadBasedDynVoltnFreqScaling();
  //Must be virtual in order to be called in subclasses if this class.
  //virtual
  void HandleCPUnotTooHot() ;
};

#endif /* CPUCORELOADBASEDDYNVOLTNFREQSCALING_HPP_ */
