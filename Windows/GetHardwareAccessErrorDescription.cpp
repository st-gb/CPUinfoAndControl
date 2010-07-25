/*
 * GetHardwareAccessErrorDescription.cpp
 *
 *  Created on: Jun 26, 2010
 *      Author: Stefan
 */
#include "GetHardwareAccessErrorDescription.hpp"
#include <Controller/stdstring_format.hpp>
//#include <Windows/ErrorCodeFromGetLastErrorToString.h>
#include <Windows/LocalLanguageMessageFromErrorCode.h>
#include <winerror.h> //for ERROR_GEN_FAILURE

//Gets the description for PCconfig , IO , MSR etc. access
//May be used if different hardware accesses (WinRing0 versions, ...) fail.
std::string GetHardwareAccessErrorDescription(
  DWORD dwErrorCode,
  const //std::string &
  char * stdstrHardwareType ,
  DWORD dwIndex
  )
{
  std::string strErrorMessage ;
  //::GetLastErrorMessageString(
//    dwErrorCode ,
//    strErrorMessage
//    ) ;
    strErrorMessage = ::LocalLanguageMessageAndErrorCodeA(dwErrorCode) ;
//  std::string stdstrAdditionalInfo = "" ;
  switch(dwErrorCode)
  {
  case ERROR_GEN_FAILURE:
    strErrorMessage += "\nThis error code could mean that " +
    std::string(stdstrHardwareType) + " address \""
      + to_stdstring<DWORD>(dwIndex) + "\" is not available/ accessible "
      //"on the CPU"
      ;
      break ;
  }
  return strErrorMessage ;
}
