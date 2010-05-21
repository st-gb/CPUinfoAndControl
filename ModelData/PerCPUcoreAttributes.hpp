/*
 * PerCPUcoreAttributes.hpp
 *
 *  Created on: May 19, 2010
 *      Author: Stefan
 */

#ifndef PERCPUCOREATTRIBUTES_HPP_
#define PERCPUCOREATTRIBUTES_HPP_

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

//creating a special class for data for a single CPU core has the advantage:
// if an attribute is added that exists for all CPU cores then we do not need
// to allocate a dynamically created array (because the array size is unknown
// at compile time) for _each_ attribute.
// Instead, an array of the size >>number of logical CPU cores<< must exist
//only once for _this_ class.
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
  //Keep away the dependance on this class for dyn libs.
  wxWidgets::DynFreqScalingThread * mp_dynfreqscalingthread ;
#endif
  float m_fPreviousCPUusage ;
  float m_fVoltageInVoltCalculatedFromCPUload ;
private:
  I_CPUcontroller * mp_cpucontroller ;
  ICPUcoreUsageGetter * mp_icpucoreusagegetter ;
  WORD m_wCurrentFreqInMHz ;
public:
  WORD m_wFreqInMHzCalculatedFromCPUload ;

#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  //Keep away the dependance on this class for dyn libs.
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
    //, GriffinController * p_griffincontroller
    , I_CPUcontroller * p_cpucontroller
    , CPUcoreData & r_cpucoredata
    ) ;
  void SetCPUcontroller( I_CPUcontroller * p_cpucontroller )
  {
    mp_cpucontroller = p_cpucontroller ;
  }
} ;

#endif /* PERCPUCOREATTRIBUTES_HPP_ */
