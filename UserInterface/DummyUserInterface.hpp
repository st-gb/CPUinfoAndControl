/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //include guard

#include "UserInterfaceBase.hpp"
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
#include <Controller/character_string/getUTF8string.hpp> //getUTF8string(...)
#include <conio.h>

//This class is not a real userinterface: instead, it logs the messages.
class DummyUserInterface
    : public UserInterfaceBase
{
public:
  void outputAllPstates(unsigned char byCurrentP_state, int & vid) {}
  bool Confirm(const std::string & str) 
  {
      //DEBUG("message: %s\n",str.c_str());
      LOG( "message: " << str.c_str() //<< "\n"
        );
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
//      LOG( "message: " << pch //
//        << "\n" );
//      return true;
//  }
  bool Confirm( std::ostringstream & r_stdostringstream )
  {
    LOG( r_stdostringstream.str() )
    return true ;
  }
  /** The following function are for ease to not need to create the time stamp
  * string on our own in order to display it. */
  void MessageWithTimeStamp(const std::wstring & c_r_std_wstrMessage
    //Flag like "OK_BUTTON"
    , unsigned flags = 0) const
  {
//    std::string c_r_std_strMessage;
//    getUTF8string_inline(c_r_std_wstrMessage, c_r_std_strMessage);
//    _cprintf("%s", c_r_std_strMessage.c_str() );
  }
};
