/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * x86IandCservice.hpp
 *
 *  Created on: Dec 30, 2010
 *      Author: sgebauer
 */

#ifndef X86IANDCSERVICE_HPP_
#define X86IANDCSERVICE_HPP_

#ifndef WX_PRECOMP
  #include "wx/app.h"
#endif

#include <Controller/CPUcontrolServiceBase.hpp>
#include <UserInterface/DummyUserInterface.hpp> //class DummyUserInterface

namespace wxWidgets
{

  class x86IandCservice
    :
    //For receiving events for non-blocking sockets.
    public wxApp ,
    public CPUcontrolServiceBase
  {
  public:
    DummyUserInterface m_dummyuserinterface ;
    x86IandCservice();
    virtual
    ~x86IandCservice();
    //Overwrites wxApp::OnInit() ;
//    bool OnInit()
//    {
//      return true ;
//    }
    //http://docs.wxwidgets.org/stable/wx_wxappoverview.html:
    //If there are member objects they are destroyed from MyApp destructor.
    //But if the MyApp object is deleted after wxWidgets structures that
    //this member object depends on have been uninitialized:
    //The solution is to destroy the member object in OnExit.
    virtual int OnExit();
    virtual bool OnInit();
  };

  DECLARE_APP(x86IandCservice)

}

#endif /* X86IANDCSERVICE_HPP_ */
