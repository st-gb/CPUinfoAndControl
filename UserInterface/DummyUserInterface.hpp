#pragma once //include guard

#include "UserInterfaceBase.hpp"
#include <global.h>

//This class is not a real userinterface: instead, it logs the messages.
class DummyUserInterface
    : public UserInterfaceBase
{
public:
  void outputAllPstates(unsigned char byCurrentP_state, int & vid) {}
  bool Confirm(const std::string & str) 
  {
      //DEBUG("message: %s\n",str.c_str());
      LOG( "message: " << str.c_str() << "\n" );
      return true; 
  }
  bool Confirm(const std::wstring & cr_stdwstr )
  {
      //DEBUG("message: %s\n",str.c_str());
      LOGWN_WSPRINTF( L"message: %ls" , cr_stdwstr.c_str() );
      return true;
  }
//  virtual bool Confirm(std::ostrstream & r_ostrstream )
//  {
//      //Must set this, else text may follow after the string we want.
//      //I had program crashes with the following method:
//      //pch[r_ostrstream.pcount()] = '\0' ;
//      //r_ostrstream.ends();
//      r_ostrstream.put('\0'); //the same as "ends()" does.
//      char *pch = r_ostrstream.str() ;
//      //DEBUG("message: %s\n",pch);
//      LOG( "message: " << pch << "\n" );
//      return true;
//  }
  bool Confirm( std::ostringstream & r_stdostringstream )
  {
    LOG( r_stdostringstream.str() )
    return true ;
  }
};
