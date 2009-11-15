/* 
 * File:   I_CPUcontrollerAction.hpp
 * Author: Stefan
 *
 * Created on 22. September 2009, 13:57
 */

#ifndef _I_CPUCONTROLLERACTION_HPP
#define	_I_CPUCONTROLLERACTION_HPP
#include "global.h" //for BYTE

class I_CPUcontrollerAction
{
public:
  I_CPUcontrollerAction();
  I_CPUcontrollerAction(const I_CPUcontrollerAction& orig);
  virtual ~I_CPUcontrollerAction();
  virtual //void 
    BYTE Execute() = 0;
private:

};

#endif	/* _I_CPUCONTROLLERACTION_HPP */

