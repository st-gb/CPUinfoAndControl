/*
 * CPUcontrolBase.hpp
 *
 *  Created on: Mar 23, 2010
 *      Author: Stefan
 */

#ifndef CPUCONTROLBASE_HPP_
#define CPUCONTROLBASE_HPP_

class I_CPUaccess ;

//Base class for the service and the GUI.
class CPUcontrolBase
{
protected:
  I_CPUaccess * mp_i_cpuaccess ;
public:
  I_CPUaccess * GetCPUaccess() { return mp_i_cpuaccess ; }
} ;

#endif /* CPUCONTROLBASE_HPP_ */
