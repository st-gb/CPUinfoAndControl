/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //include guard

#include "UserInterface.hpp"

class UserInterfaceBase
    : public UserInterface
{
public:
  void outputAllPstates(unsigned char byCurrentP_state, int & vid) {}
  bool Confirm(const std::string & str) { return true; }
//  virtual bool Confirm(std::ostrstream & r_ostrstream ) { return true; }
};
