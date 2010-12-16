/*
 * thread_type.hpp
 *
 *  Created on: Aug 17, 2010
 *      Author: Stefan
 */

#ifndef THREAD_TYPE_HPP_
#define THREAD_TYPE_HPP_

#define USE_PTHREAD

#if defined(__linux__) && defined(USE_PTHREAD)
  //class phreadBasedI_Thread
  #include <Linux/multithread/pthreadBasedI_Thread.hpp>
  typedef Linux::pthreadBasedI_Thread thread_type ;
#else
  //class wxThreadBasedI_Thread ;
  #include <wxWidgets/multithread/wxThreadBasedI_Thread.hpp>
  //#include <wx/thread.h> //class wxCriticalSectionLocker
#endif

namespace x86IandC
{
#if defined(__linux__) && defined(USE_PTHREAD)
  typedef Linux::pthreadBasedI_Thread thread_type ;
#else
  typedef wxThreadBasedI_Thread thread_type ;
#endif
  //typedef critical_section_locker_type wxCriticalSectionLocker ;
}

#endif /* THREAD_TYPE_HPP_ */
