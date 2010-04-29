/*
 * GetCurrentProcessExeFileNameWithoutDirs.cpp
 *
 *  Created on: Apr 27, 2010
 *      Author: Stefan
 */
//#include <string>
#include "StdAfx.h"
#include "GetCurrentProcessExeFileNameWithoutDirs.hpp"
#include <WinDef.h> //DWORD etc.
#include <windows.h> //for GetModuleFileName()
#include <tchar.h> //_tcsrchr

std::tstring GetExeFileNameWithoutDirs()
{
  std::tstring stdstr ;
  TCHAR artchThisExesPath [ MAX_PATH //plus term. 0 char
      + 1] ;
  DWORD dwModuleFileNameReturn =
  ::GetModuleFileName
    (
      //If this parameter is NULL, GetModuleFileName retrieves the path
      //of the executable file of the current process.
      NULL,
      artchThisExesPath ,
      //Size of the lpFilename buffer, in TCHARs.
      MAX_PATH
    ) ;
  //WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
  //    "Size of path: " << dwModuleFileNameReturn ) ;
  if ( //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/getmodulefilename.htm:
      //"If the function succeeds, the return value is the length of the
      //string that is copied to the buffer, in TCHARs.
      //If the function fails, the return value is 0 (zero). To get extended error information, call GetLastError."
      dwModuleFileNameReturn == MAX_PATH || dwModuleFileNameReturn == 0
      )
  {
    //DEBUG("GetModuleFileName failed (%d)\n", GetLastError());
    //LOG(
//    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
//        "Getting file path for THIS executable file failed: " <<
//        LocalLanguageMessageFromErrorCode( ::GetLastError() ) << ")" //<< \n"
//        );
    //return FALSE;
  }
  else
  {
    TCHAR * ptchLastBackslash =
        //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.VisualStudio.v80.de/dv_vccrt/html/75cf2664-758e-49bb-bf6b-8a139cd474d2.htm:
        //"Scan a string for the last occurrence of a character."
        _tcsrchr(artchThisExesPath,'\\' ) ;
    //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.VisualStudio.v80.de/dv_vccrt/html/75cf2664-758e-49bb-bf6b-8a139cd474d2.htm:
    //"Returns a pointer to the last occurrence of c in str, or
    //NULL if c is not found."
    if( ptchLastBackslash )
    {
      artchThisExesPath[ //pointer arithmetics:
          //address of last backslash minus address of first char:
          //If e.g. "C\", the diff is "1"-> string[1] ^= '\0' <=> "C"
          ptchLastBackslash - artchThisExesPath ] = '\0' ;
      stdstr = std::tstring(
        ptchLastBackslash + 1) ;
    }
  }
  return stdstr ;
}
