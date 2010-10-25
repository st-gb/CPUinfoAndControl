/*
 * CPUcontrolDaemon.hpp
 *
 *  Created on: Oct 11, 2010
 *      Author: sgebauer
 */

#ifndef CPUCONTROLDAEMON_HPP_
#define CPUCONTROLDAEMON_HPP_

#include <Controller/CPUcontrolServiceBase.hpp>

namespace Linux
{

  class CPUcontrolDaemon
    : public CPUcontrolServiceBase
  {
  public:
    CPUcontrolDaemon(//user interface is needed for outputting messages.
      UserInterface * p_userinterface);
//    virtual
//    ~CPUcontrolDaemon();
    //Called by the base class.
    void CreateHardwareAccessObject() ;
//    inline void Start() ;
    bool Start() ;
  };

}

#endif /* CPUCONTROLDAEMON_HPP_ */
