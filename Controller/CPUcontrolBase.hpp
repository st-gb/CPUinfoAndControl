/*
 * CPUcontrolBase.hpp
 *
 *  Created on: Mar 23, 2010
 *      Author: Stefan
 */

#ifndef CPUCONTROLBASE_HPP_
#define CPUCONTROLBASE_HPP_

class I_CPUaccess ;
class wxDynLibCPUcontroller ;
class wxDynLibCPUcoreUsageGetter ;

#define NULL 0

//Base class for the service and the GUI.
class CPUcontrolBase
{
protected:
  I_CPUaccess * mp_i_cpuaccess ;
public:
  wxDynLibCPUcontroller * mp_wxdynlibcpucontroller ;
  wxDynLibCPUcoreUsageGetter * mp_wxdynlibcpucoreusagegetter ;
  CPUcontrolBase( )
    : 
    mp_wxdynlibcpucontroller (NULL)
    , mp_wxdynlibcpucoreusagegetter ( NULL )
  {
  }
  I_CPUaccess * GetCPUaccess() { return mp_i_cpuaccess ; }
} ;

#endif /* CPUCONTROLBASE_HPP_ */
