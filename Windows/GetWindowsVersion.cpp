#include "GetWindowsVersion.h"

//from http://support.microsoft.com/kb/307394
#include "stdafx.h"
#include <Windows.h>
#include <tchar.h> //for _T()

//#using <mscorlib.dll>

//using namespace System;

//// This is the entry point for this application
//#ifdef _UNICODE
//int wmain(void)
//#else
//int main(void)
//#endif
std::string GetWindowsVersion(DWORD & dwMajor ,DWORD & dwMinor)
{
  std::string strVersion ;
  OSVERSIONINFO osversioninfo ;

  //setting the dwOSVersionInfoSize is needed, else GetVersionEx() failed
  //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/sysinfo/base/getversionex.htm:
  osversioninfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  //get OperatingSystem info from the system namespace
  if( ::GetVersionEx(
      & osversioninfo   
      )
    )
  {
   //is the platform win9x or NT
   switch(osversioninfo.dwPlatformId )
   {
      //platform is win9x
      case VER_PLATFORM_WIN32_WINDOWS :
    
         switch (osversioninfo.dwMinorVersion )
         {
              case 0:
                 strVersion = _T("Windows 95");
              break;
              case 10:
                if( std::string( osversioninfo.szCSDVersion ) == "2222A")
                 strVersion = _T("Windows 98 Second Edition");
              else
                 strVersion = _T("Windows 98");
              break;
              case  90:
                 strVersion = _T("Windows ME");
              break;
         }
         break;
			
         //platform is NT
         case VER_PLATFORM_WIN32_NT :

            switch(osversioninfo.dwMajorVersion )
            {
               case 3:
                  strVersion = _T("Windows NT 3.51");
                 break;
               case 4:
                  strVersion = _T("Windows NT 4.0");
                 break;
               case 5:
                 if ( osversioninfo.dwMinorVersion == 0 ) 
                    strVersion = _T("Windows 2000");
                 else
                    strVersion = _T("Windows XP");
                 break;
               case 6:
                 break;
            }break;
    }
  }
  return strVersion ;
}
