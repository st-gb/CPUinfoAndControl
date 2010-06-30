/*
 * GetHardwareAccessErrorDescription.hpp
 *
 *  Created on: Jun 26, 2010
 *      Author: Stefan
 */

#ifndef GETCPUACCESSERRORDESCRIPTION_HPP_
#define GETCPUACCESSERRORDESCRIPTION_HPP_

#include <string>

typedef unsigned long DWORD ;

std::string GetHardwareAccessErrorDescription(
  DWORD dwErrorCode,
  const //std::string &
    char * ,
  DWORD dwIndex
  ) ;

#endif /* GETCPUACCESSERRORDESCRIPTION_HPP_ */
