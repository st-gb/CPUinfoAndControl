/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //Include guard
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
  virtual bool Confirm(const std::string & cr_stdstr) = 0//{return true;}
    ;
  //wstring version e.g. for chinese filenames.
  virtual bool Confirm(const std::wstring & cr_stdwstr) = 0//{return true;}
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
  virtual void DynVoltnFreqScalingEnabled() {}
  virtual void EndAllAccessToCPUcontroller() {};
  /** The following function are for ease to not need to create the time stamp
  * string on our own in order to display it. */
  virtual void MessageWithTimeStamp(const std::wstring & cr_stdwstr
    //Flag like "OK_BUTTON"
    , unsigned flags = 0) const {}
  virtual void MessageWithTimeStamp(const wchar_t * cp_wch) const {}
  virtual void outputAllPstates(unsigned char byCurrentP_state, int & vid) = 0;//{};
  void outputOtherRelevantVoltageInfo(){};
  virtual void RedrawEverything() {} ;
  void showHelp(){};
};
