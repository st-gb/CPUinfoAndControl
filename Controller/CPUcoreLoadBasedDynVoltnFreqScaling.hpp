/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
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
