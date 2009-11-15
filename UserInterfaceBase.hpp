#pragma once //include guard

#include "UserInterface.hpp"

class UserInterfaceBase
    : public UserInterface
{
public:
  void outputAllPstates(unsigned char byCurrentP_state, int & vid) {}
  bool Confirm(const std::string & str) { return true; }
  virtual bool Confirm(std::ostrstream & r_ostrstream ) { return true; }
};
