/*
 * CommandLineParams.h
 *
 *  Created on: 30.01.2013
 *      Author: Stefan
 */

#ifndef COMMANDLINEPARAMS_H_
#define COMMANDLINEPARAMS_H_

#if defined(_UNICODE) || defined(UNICODE)
  #include <wchar.h> //wcscmp(...)
#else
  #include <strings.h> //strcmp(...)
#endif
#include <windef.h> //BYTE

bool IsWithinCmdLineArgs( int argc, //char
  TCHAR * argv[], //LPCTSTR p_tstr
  //LPCSTR
  TCHAR * pc_chCompareTo )
{
    bool bIsWithinCmdLineArgs = false ;
    BYTE byIndex = 1 ;
    for( ; byIndex < argc ; ++ byIndex )
    {
      if (
#if defined(_UNICODE) || defined(UNICODE)
        ::wcscmp(
#else
        ::strcmp(
#endif
          argv[byIndex], pc_chCompareTo) == 0
        )
        {
            bIsWithinCmdLineArgs = true ;
            break ;
        }
    }
    return bIsWithinCmdLineArgs ;
}

#endif /* COMMANDLINEPARAMS_H_ */
