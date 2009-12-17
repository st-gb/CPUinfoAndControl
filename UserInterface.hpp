#pragma once
//#ifndef 
#include <strstream> //for std::ostrstream
#include <string>
//#include "PumaStateCtrl.h"
#include <Controller/AMD/Griffin/GriffinController.hpp>

class GriffinController ;

//#define WRITE_TO_OUTPUTSTREAM(ostrstream1,var,str) /*std::ostrstream ostrstream1 ;*/ \
//  ostrstream1 << var; \
//  char *pch = ostrstream1.str() ; \
//  /*pch[ostrstream.pcount()] = '\0' ;: Must set this, else text may follow after the string we want.*/
//  pch[ostrstream1.pcount()] = '\0' ; \
//    str = std::string(pch) ;
//  /*::AfxMessageBox(pch ) ;*/

class UserInterface
{
//private:
public:
//  GriffinController * mp_pumastatectrl ;
  bool m_bConfirmedYet ;
  UserInterface()
    //Set initially for ALL subclasses to "true".
    : m_bConfirmedYet ( true )
  {

  }
  void showHelp(){};
  virtual void outputAllPstates(unsigned char byCurrentP_state, int & vid) = 0;//{};
  void outputOtherRelevantVoltageInfo(){};
  //Make it pure ("= 0") virtual so that it needs to be implemented by subclasses.
  virtual bool Confirm(const std::string & str) = 0//{return true;}
    ;
  virtual bool Confirm(std::ostrstream & r_ostrstream
    //std::ostream & r_ostream
    ) = 0//{return true;}
    ;
  virtual void RedrawEverything() {} ;
  virtual void CurrenCPUfreqAndVoltageUpdated() {} ;
};
