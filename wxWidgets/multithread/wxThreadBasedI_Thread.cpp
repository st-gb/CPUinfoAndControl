/*
 * wxThreadBasedI_Thread.cpp
 *
 *  Created on: Jun 16, 2010
 *      Author: Stefan
 */
#include "wxThreadBasedI_Thread.hpp"
#include <global.h>

wxThreadFuncStarterThread::wxThreadFuncStarterThread(
  pfnThreadFunc pfn_threadfunc ,
  void * p_v ,
  BYTE byThreadType
  )
  : wxThread ( //wxTHREAD_JOINABLE
    (wxThreadKind) byThreadType )
{
  m_thread_func = pfn_threadfunc ;
  mp_v = p_v ;
}

void *
//ExitCode
wxThreadFuncStarterThread::Entry()
{
  return (void*) m_thread_func( mp_v ) ;
}

//  BYTE wxThreadBasedI_Thread::Entry( )
//  {
//
//  }

wxThreadBasedI_Thread::wxThreadBasedI_Thread( BYTE byThreadType )
  : mp_wxthreadfuncstarterthread( NULL )
{
  m_byThreadType = byThreadType ;
}

void wxThreadBasedI_Thread::Delete()
{
  DEBUGN("wxThreadBasedI_Thread::Delete()")
  if( mp_wxthreadfuncstarterthread)
  {
    LOGN("before deleting the thread with ID "
      << mp_wxthreadfuncstarterthread->GetId() )
    delete mp_wxthreadfuncstarterthread ;
  }
}

  //static
  BYTE wxThreadBasedI_Thread::start( pfnThreadFunc pfn_threadfunc, void * p_v )
  {
    //make the same thread type like Win32 threads.
    //http://docs.wxwidgets.org/2.6/wx_wxthread.html#wxthreadentry:
    //"[...]unlike Win32 threads where all threads are joinable[...]
    //"Detached threads (the default type) cannot be waited for."
//    wxThread wxthread(wxTHREAD_JOINABLE) ;
    //"all detached threads must be created on the heap"
//    wxThreadFuncStarterThread wxthreadfuncstarterthread ( pfn_threadfunc , p_v );
//    wxThreadFuncStarterThread * p_wxthreadfuncstarterthread = new
    mp_wxthreadfuncstarterthread = new
        wxThreadFuncStarterThread( pfn_threadfunc , p_v , m_byThreadType );
    wxThreadFuncStarterThread * p_wxthreadfuncstarterthread =
        mp_wxthreadfuncstarterthread ;
    if( p_wxthreadfuncstarterthread )
    {
      //http://docs.wxwidgets.org/2.6/wx_wxthread.html#wxthreadcreate:s
      //"Creates a new thread. The thread object is created in the suspended
      //state, and you should call Run  to start running it."
  //    wxThreadError wxthreaderror = wxthread.Create() ;
      wxThreadError wxthreaderror = //wxthreadfuncstarterthread.Create() ;
          p_wxthreadfuncstarterthread->Create() ;
      if( wxthreaderror == wxTHREAD_NO_ERROR )
      {
        //"Starts the thread execution. Should be called after Create."
  //      wxthread.Run() ;
        return //wxthreadfuncstarterthread.Run() ;
          p_wxthreadfuncstarterthread->Run() ;
      }
    }
    return 0 ;
  }

  void * wxThreadBasedI_Thread::Wait()
  {
    DEBUGN("wxThreadBasedI_Thread::Wait()")
    if( mp_wxthreadfuncstarterthread)
      return mp_wxthreadfuncstarterthread->Wait() ;
    return 0 ;
  }
//  BYTE wxThreadBasedI_Thread::start( pfnThreadFunc, void * p_v )
//  {
//    m_thread_func = pfnThreadFunc ;
//    mp_v = p_v ;
//    Run() ; //calls "Entry()"
//  }
