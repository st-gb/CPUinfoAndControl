/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * PerCPUcoreAttributes.hpp
 *
 *  Created on: May 19, 2010
 *      Author: Stefan
 */

#ifndef PERCPUCOREATTRIBUTES_HPP_
#define PERCPUCOREATTRIBUTES_HPP_
#include <windef.h> //for BYTE etc.

//pre-declarations (faster than to include a file)
class CPUcoreData ;

class I_CPUcontroller ;
//class wxWidgets::DynFreqScalingThread ;
class ICPUcoreUsageGetter ;

#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  //Keep away the dependency on this class for dynamic libraries.
namespace wxWidgets
{
  class DynFreqScalingThread ;
}
#endif

/** creating a special class for data for a single CPU core has the advantage:
* if an attribute is added that exists for all CPU cores then we do not need
* to allocate a dynamically created array (because the array size is unknown
* at compile time) for _each_ attribute.
* Instead, an array of the size >>number of logical CPU cores<< must exist
* only once for _this_ class. */
class PerCPUcoreAttributes
{
//Sort the attributes  lexicographically for simplification of:
//Initialize in the same order as textual in the declaration?
//(to avoid g++ warnings)
private :
  BYTE m_byCoreID ;
  CPUcoreData * mp_cpucoredata ;
public:
#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  //Keep away the dependence on this class for dyn libs.
  wxWidgets::DynFreqScalingThread * mp_dynfreqscalingthread ;
#endif
  float m_fPreviousCPUusage ;
  float m_fMultiplier ;
  float m_fReferenceClockInMhz ;
  float m_fTempInDegCelsius ;
  float m_fVoltageInVolt ;
  float m_fVoltageInVoltCalculatedFromCPUload ;
  /** [0.0 ... 1.0] 1.0: not throttled; 0.25: throttled by 75% */
  float m_fThrottleLevel;
private:
  I_CPUcontroller * mp_cpucontroller ;
  ICPUcoreUsageGetter * mp_icpucoreusagegetter ;
  WORD m_wCurrentFreqInMHz ;
public:
  WORD m_wFreqInMHzCalculatedFromCPUload ;

#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  //Keep away the dependence on this class for dyn libs.
  void CreateDynFreqScalingThread(
    ICPUcoreUsageGetter * p_icpucoreusagegetter
    ) ;
#endif

  PerCPUcoreAttributes() ;
  ~PerCPUcoreAttributes() ;

  //when this class is an element of an array, the paramless ctor is
  //called?! So do the init with params here.
  void Create(
    BYTE byCoreID
    //ICPUcoreUsageGetter * p_icpucoreusagegetter
    , I_CPUcontroller * p_cpucontroller
    , CPUcoreData & r_cpucoredata
    ) ;
  //inline
  float GetFreqInMHz() ;
  void SetCPUcontroller( I_CPUcontroller * p_cpucontroller )
  {
    mp_cpucontroller = p_cpucontroller ;
  }
} ;

#endif /* PERCPUCOREATTRIBUTES_HPP_ */
