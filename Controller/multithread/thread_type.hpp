/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * thread_type.hpp
 *
 *  Created on: Aug 17, 2010
 *      Author: Stefan
 */

#ifndef THREAD_TYPE_HPP_
#define THREAD_TYPE_HPP_

//#define USE_PTHREAD

#define NAMESPACE_X86IANDC(x) namespace x86IandC{ x }

#if defined(__linux__) && defined(USE_PTHREAD)
//  typedef Linux::pthreadBasedI_Thread thread_type ;
#else
  //#include <wx/thread.h> //class wxCriticalSectionLocker
#endif

#if defined(__linux__) && defined(USE_PTHREAD)
  //class phreadBasedI_Thread
  #include <Linux/multithread/pthreadBasedI_Thread.hpp>
  namespace x86IandC { typedef Linux::pthreadBasedI_Thread thread_type ; }
#else
  #ifdef _WIN32 //Defined for 32 and 64 bit Windows on MinGW/ MSVC.
    #include <Windows/multithread/Thread.hpp> //class Windows_API::Thread
    namespace x86IandC{ typedef Windows_API::Thread thread_type ; }
//    NAMESPACE_X86IANDC(typedef Windows_API::Thread thread_type ;)
  #else
    //class wxThreadBasedI_Thread ;
    #include <wxWidgets/multithread/wxThreadBasedI_Thread.hpp>
    namespace x86IandC { typedef wxThreadBasedI_Thread thread_type ; }
  #endif
#endif
  //typedef critical_section_locker_type wxCriticalSectionLocker ;

#endif /* THREAD_TYPE_HPP_ */
