/*
 * wxThreadBasedI_Thread.hpp
 *
 *  Created on: Jun 16, 2010
 *      Author: Stefan
 */

#ifndef WXTHREADBASEDI_THREAD_HPP_
#define WXTHREADBASEDI_THREAD_HPP_

#include <Controller/multithread/I_Thread.hpp>
#include <wx/thread.h>

class wxThreadFuncStarterThread
  : public wxThread
{
  pfnThreadFunc m_thread_func ;
  void * mp_v ;
public:
  wxThreadFuncStarterThread(
    pfnThreadFunc pfn_threadfunc ,
    void * p_v ,
    BYTE byThreadType) ;
  void * Entry() ;
};

class wxThreadBasedI_Thread
  : public I_Thread
{
  wxThreadFuncStarterThread * mp_wxthreadfuncstarterthread ;
public:
    wxThreadBasedI_Thread( BYTE byThreadType ) ;
  void Delete() ;
  //static
    BYTE start( pfnThreadFunc, void * p_v ) ;
  void * WaitForTermination() ;
};

namespace x86IandC
{
  typedef wxThreadBasedI_Thread thread_type ;
}

#endif /* WXTHREADBASEDI_THREAD_HPP_ */
