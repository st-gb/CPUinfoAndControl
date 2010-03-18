#pragma once

#include <string>
#include <WinError.h> //ERROR_BAD_EXE_FORMAT

class DLLloadError
{
public:
  static std::string GetPossibleSolution( DWORD dwWindowsErrorCode ) 
  {
    std::string strSol = "Possible solution(s):\n" ;
    switch( dwWindowsErrorCode )
    {
    case ERROR_INVALID_PARAMETER :
    case ERROR_BAD_EXE_FORMAT :
      strSol += "-this may be due to this is an unsupported binary file format (32 bit<->64 bit).\n"
        "So only attach 64 bit DLLs under 64 Bit Windows, 32 bit DLLs under 32 Bit Windows\n" ;
      break ;
    }
    return strSol ;
  }
} ;
