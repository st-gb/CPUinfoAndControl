/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
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

//    //Called by the base class.
//    void CreateHardwareAccessObject() ;
//    inline void Start() ;
    bool Start() ;
  };

}

#endif /* CPUCONTROLDAEMON_HPP_ */
