#include "CurrentDir.h"
#include <windows.h> //for TCHAR
#include <Windows/LocalLanguageMessageFromErrorCode.h>
#include <global.h> //for WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE
#ifdef __CYGWIN__ //does not have a "tchar.h" shipped.
    #include <Controller/MSVC_adaption/tchar.h> //_tcsrchr()
#else //MSC, MINGW (,...)
    #include <tchar.h> //_tcsrchr()
#endif

//Services get their current working dir set to "%WINDIR%/System32", 
//e.g. C:\WINDOWS\System32, but this is often unwanted because if wanting 
//to read or write to files:
//-this path often has less privileges
//-the configuration file should be chosen from the same dir as where the exe
// resides by default
//So change the path of the current working dir to the path where the 
//executable resides.
void SetExePathAsCurrentDir()
{
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
  if (//ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/dllproc/base/
      //getmodulefilename.htm:
      //"If the function succeeds, the return value is the length of the 
      //string that is copied to the buffer, in TCHARs.
      //If the function fails, the return value is 0 (zero).
      //To get extended error information, call GetLastError."
      dwModuleFileNameReturn == MAX_PATH || dwModuleFileNameReturn == 0
      )
  {
    //DEBUG("GetModuleFileName failed (%d)\n", GetLastError()); 
    //LOG( 
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( 
        "Getting file path for THIS executable file failed: " << 
        LocalLanguageMessageFromErrorCodeA( ::GetLastError() ) << ")" //<< \n" 
        );
    //return FALSE;
  }
  else
  {
    TCHAR * ptchLastBackslash = 
      //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.VisualStudio.v80.de/dv_vccrt/
      //html/75cf2664-758e-49bb-bf6b-8a139cd474d2.htm:
      //"Scan a string for the last occurrence of a character."
      _tcsrchr(artchThisExesPath,'\\' ) ;
    //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.VisualStudio.v80.de/dv_vccrt/html/
    //75cf2664-758e-49bb-bf6b-8a139cd474d2.htm:
    //"Returns a pointer to the last occurrence of c in str, or 
    //NULL if c is not found."
    if( ptchLastBackslash )
    {
      artchThisExesPath[ //pointer arithmetics: 
          //address of last backslash minus address of first char:
          //If e.g. "C\", the diff is "1"-> string[1] ^= '\0' <=> "C"
          ptchLastBackslash - artchThisExesPath ] = '\0' ;
      if( 
          ::SetCurrentDirectory(
              artchThisExesPath ) 
        )
      {
          //std::cout << "succ set curr path to " << artchThisExesPath 
          //  << "\n" ;
      }
      else
      {
        //WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( 
        //    "Setting current dir to" << artchThisExesPath << "failed" //\n" 
        //    );
      }
    }
  }
}
