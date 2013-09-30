/*
 * Start_GUI.cpp
 *
 *  Created on: 23.09.2011
 *      Author: Stefan
 */

//Windows_API::CreateProcess(...)
#include <Windows/CreateProcess/CreateProcess.hpp>
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
#include <ModelData/ServiceAttributes.hpp> //class ServiceAttributes
//#include "StartGUI.hpp" //DWORD

struct CreateProcessAttributesAndDelayTime
{
public:
  Windows_API::CreateProcess * m_p_create_gui_process;
  DWORD m_dwDelayTimeInMillis;
  CreateProcessAttributesAndDelayTime(
    Windows_API::CreateProcess * p_create_gui_process,
    DWORD dwDelayTimeInMillis)
  :
    m_p_create_gui_process(p_create_gui_process),
    m_dwDelayTimeInMillis(dwDelayTimeInMillis)
  {

  }
  CreateProcessAttributesAndDelayTime(
    Windows_API::CreateProcess & r_create_gui_process,
      DWORD dwDelayTimeInMillis)
    :
    m_dwDelayTimeInMillis(dwDelayTimeInMillis)
  {
    m_p_create_gui_process = new Windows_API::CreateProcess(
      r_create_gui_process);

  }
  ~CreateProcessAttributesAndDelayTime()
  {
    if( m_p_create_gui_process)
      delete m_p_create_gui_process;
  }
};

DWORD
#ifdef _WIN32 //under Linux g++ error if "__stdcall"
__stdcall
#endif
StartGUIdelayed_ThreadFunc(void * p_v)
{
  LOGN( "begin" )
  //  Windows_API::CreateProcess * p_create_gui_process =
  //    (Windows_API::CreateProcess *) p_v;
  CreateProcessAttributesAndDelayTime * p_createprocessattributesanddelaytime =
      (CreateProcessAttributesAndDelayTime *) p_v;
  if ( //p_create_gui_process
      p_createprocessattributesanddelaytime)
    {
      LOGN( "before sleeping " <<
        p_createprocessattributesanddelaytime->m_dwDelayTimeInMillis
        << " milliseconds" )
      ::Sleep(//p_create_gui_process->m_
          p_createprocessattributesanddelaytime->m_dwDelayTimeInMillis);
      LOGN( "after sleeping " <<
        p_createprocessattributesanddelaytime->m_dwDelayTimeInMillis
        << " milliseconds" )
      p_createprocessattributesanddelaytime->m_p_create_gui_process->
        StartProcess(
        //      pwtssession_notification->dwSessionId
        );
      //Was created for this function on heap before calling this function, so
      //free it now.
      delete p_createprocessattributesanddelaytime;
    }
  return 0;
}

void StartGUIprocessDelayedSynchronous(
  ServiceAttributes & r_service_attributes,
  DWORD dwSessionID
  )
{
//  CreateProcessAsUserAttributesW createprocessasuserattributesw;
//  createprocessasuserattributesw.m_stdwstrCommandLine
//      = r_service_attributes.m_stdwstrPathToGUIexe;
//  createprocessasuserattributesw.m_stdwstrCurrentDirectory
//      = r_service_attributes.m_stdwstrGUICurrentDirFullPathTo;
//  createprocessasuserattributesw.m_dwSessionID = dwSessionID;
  CreateProcessAsUserAttributesW createprocessasuserattributesw(
    r_service_attributes.m_stdwstrPathToGUIexe,
    r_service_attributes.m_stdwstrGUICurrentDirFullPathTo,
    createprocessasuserattributesw.m_dwSessionID
    );
  Windows_API::CreateProcess create_gui_process(
    createprocessasuserattributesw);
  create_gui_process.StartProcess(
  //    pwtssession_notification->dwSessionId
  );
}

void
StartGUIprocessDelayedAsync(ServiceAttributes & r_service_attributes,
    DWORD dwSessionID)
{
  LOGN( "begin" )
//  CreateProcessAsUserAttributesW * p_createprocessasuserattributesw =
//    new CreateProcessAsUserAttributesW();
//  p_createprocessasuserattributesw->m_stdwstrCommandLine
//      = r_service_attributes.m_stdwstrPathToGUIexe;
//  p_createprocessasuserattributesw->m_stdwstrCurrentDirectory
//      = r_service_attributes.m_stdwstrGUICurrentDirFullPathTo;
//  p_createprocessasuserattributesw->m_dwSessionID = dwSessionID;
  CreateProcessAsUserAttributesW createprocessasuserattributesw(
    r_service_attributes.m_stdwstrPathToGUIexe,
    r_service_attributes.m_stdwstrGUICurrentDirFullPathTo,
    dwSessionID
    );

  //Must be created on heap because it should be valid after leaving _this_
  //block.
  Windows_API::CreateProcess * p_create_gui_process =
    new Windows_API::CreateProcess(
//      * p_createprocessasuserattributesw
      createprocessasuserattributesw
      );
  CreateProcessAttributesAndDelayTime * p_createprocessattributesanddelaytime =
    new CreateProcessAttributesAndDelayTime(
      p_create_gui_process,
      r_service_attributes.m_dwStartGUIdelayTimeInMillis);

  //http://docs.wxwidgets.org/2.8/wx_wxthread.html#typeswxthread:
  //"Detached threads delete themselves once they have completed, either by
  //themselves when they complete processing or through a call to
  //wxThread::Delete, and thus must be created on the heap"
//  wxThreadBasedI_Thread * p_wxthreadbasedi_threadStartGUIdelayed = new
//    wxThreadBasedI_Thread(I_Thread::detached);
//  p_wxthreadbasedi_threadStartGUIdelayed->start(
//  wxThreadBasedI_Thread wxthreadbasedi_threadStartGUIdelayed(
//    I_Thread::detached);
//  wxthreadbasedi_threadStartGUIdelayed.start(
//  //create_gui_process.StartProcess,
//      StartGUIdelayed_ThreadFunc,
//      //    pwtssession_notification->dwSessionId
//      //    createprocessasuserattributesw,
//      //p_create_gui_process
//      (void *) p_createprocessattributesanddelaytime);

  ::CreateThread(
    NULL,
    //"The initial size of the stack, in bytes."
    //"If this parameter is zero, the new thread uses the default size for
    //the executable.
    0,
    StartGUIdelayed_ThreadFunc,
    (void *) p_createprocessattributesanddelaytime,
    0, //dwCreationFlags: 0=The thread runs immediately after creation.
    NULL //"If this parameter is NULL, the thread identifier is not returned."
    );
}
