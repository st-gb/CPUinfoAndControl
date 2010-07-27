#pragma once
//#ifndef 
//TODO file "strstream" is deprecated.
//#include <strstream> //for std::ostrstream
#include <sstream> //for std::ostringstream
#include <string>

//Use C comment, else compiler warning: multi-line comment because of "\" at
// line end. Or append another char like "_" to "\".  _
//#define WRITE_TO_OUTPUTSTREAM(ostrstream1,var,str) \ _
//  /*std::ostrstream ostrstream1 ;*/ \_
//  ostrstream1 << var; \_
//  char *pch = ostrstream1.str() ; \_
//  /*pch[ostrstream.pcount()] = '\0' ;: Must set this, else text may follow
//    after the string we want.*/
//Use C comment, else compiler warning: multi-line comment because of "\" at
// line end.
/*  pch[ostrstream1.pcount()] = '\0' ; \_
//    str = std::string(pch) ; */
//  /*::AfxMessageBox(pch ) ;*/

class UserInterface
{
//private:
public:
  bool m_bConfirmedYet ;
  UserInterface()
    //Set initially for ALL subclasses to "true".
    : m_bConfirmedYet ( true )
  {

  }
  //In order for destructorts of subclasses to be called
  virtual ~UserInterface() {}
  //Make it pure ("= 0") virtual so that it needs to be implemented by subclasses.
  virtual bool Confirm(const std::string & str) = 0//{return true;}
    ;
  //wstring version e.g. for chinese filenames.
  virtual bool Confirm(const std::wstring & str) = 0//{return true;}
    ;
//  virtual bool Confirm(
//    //http://fara.cs.uni-potsdam.de/~kaufmann/?page=GenCppFaqs&faq=IntToString#Answ:
//    //"schnell" , "deprecated",
//    //"Ein fehlendes ends führt dazu, dass der Puffer nicht nullterminiert wird."
//    std::ostrstream & r_ostrstream
//    //std::ostream & r_ostream
//    ) = 0//{return true;}
//    ;
  virtual bool Confirm(
    //http://fara.cs.uni-potsdam.de/~kaufmann/?page=GenCppFaqs&faq=IntToString#Answ:
    //"schnell" , "deprecated",
    //"Ein fehlendes ends führt dazu, dass der Puffer nicht nullterminiert wird."
    std::ostringstream & r_stdostringstream
    //std::ostream & r_ostream
    ) = 0//{return true;}
    ;
  virtual void CPUcontrollerDeleted() {} ;
  virtual void CPUcoreUsageGetterDeleted() {} ;
  virtual void CurrenCPUfreqAndVoltageUpdated() {} ;
  virtual void outputAllPstates(unsigned char byCurrentP_state, int & vid) = 0;//{};
  void outputOtherRelevantVoltageInfo(){};
  virtual void RedrawEverything() {} ;
  void showHelp(){};
};
