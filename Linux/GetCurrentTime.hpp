/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/* 
 * File:   GetCurrentTime.hpp
 * Author: sgebauer
 *
 * Created on 22. November 2009, 23:44
 */

#ifndef _GETCURRENTTIME_HPP
#define	_GETCURRENTTIME_HPP

#include <sys/time.h>
#include <time.h>

GetCurrentTime()
{
  struct timeval timeval_ ;
  struct timezone timezone_ ;
  int n = gettimeofday( & timeval_ , & timezone_ );

//  std::string str = systemtime.wYear << "."
//  << timeval_.tv_sec.  .wMonth << "."
//  << systemtime.wDay << " "
//  << systemtime.wHour << "h:"
//  << systemtime.wMinute << "min "
//  << systemtime.wSecond << "s "
//  << timeval_.tv_usec << "us:"
}

#endif	/* _GETCURRENTTIME_HPP */

