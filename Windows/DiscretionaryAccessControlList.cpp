#define _WIN32_WINNT 0x0500 //for 
// ConvertStringSecurityDescriptorToSecurityDescriptorA in sddl.h

#include "DiscretionaryAccessControlList.h"
#include <windows.h> //SECURITY_ATTRIBUTES,...
#ifndef _MSC_VER
  //from Platform SDK's pwrprof.h
  #define SDDL_REVISION_1     1
    BOOL
    //WINAPI
  ConvertStringSecurityDescriptorToSecurityDescriptor(
    LPCSTR StringSecurityDescriptor,
    DWORD StringSDRevision,
    PSECURITY_DESCRIPTOR * SecurityDescriptor,
    PULONG  SecurityDescriptorSize OPTIONAL
    );
#endif
#include <sddl.h> //ConvertStringSecurityDescriptorToSecurityDescriptor

//This pragma can be used instead of including the library in the linker options.
#pragma comment( lib, "Advapi32.lib" )

//from MS platform SDK v6.1\Samples\WinBase\Service
// CreateMyDACL
// Creates a security descriptor containing the
// desired DACL. This function uses SDDL to make Deny and Allow ACEs.
//
// Parameter:
//     SECURITY_ATTRIBUTES * pSA
// Address to a SECURITY_ATTRIBUTES structure. It is the caller's
// responsibility to properly initialize the structure, and to free 
// the structure's lpSecurityDescriptor member when done (by calling
// the LocalFree function).
// 
// Return value:
//    FALSE if the address to the structure is NULL. 
//    Otherwise, this function returns the value from the
//    ConvertStringSecurityDescriptorToSecurityDescriptor function.
BOOL CreateMyDACL(SECURITY_ATTRIBUTES * pSA)
{
    // Define the SDDL for the DACL. This example sets 
    // the following access:
    //     Built-in guests are denied all access.
    //     Anonymous Logon is denied all access.
    //     Authenticated Users are allowed read/write/execute access.
    //     Administrators are allowed full control.
    // Modify these values as needed to generate the proper
    // DACL for your application. 
    TCHAR * szSD = "D:"                   // Discretionary ACL
                   "(D;OICI;GA;;;BG)"     // Deny access to Built-in Guests
                   "(D;OICI;GA;;;AN)"     // Deny access to Anonymous Logon
                   "(A;OICI;GRGWGX;;;AU)" // Allow read/write/execute to Authenticated Users
                   "(A;OICI;GA;;;BA)";    // Allow full control to Administrators

    if (NULL == pSA)
        return FALSE;

    return ConvertStringSecurityDescriptorToSecurityDescriptor(
      szSD,
      SDDL_REVISION_1,
      //cast for g++
      (PSECURITY_DESCRIPTOR *)
      &(pSA->lpSecurityDescriptor),
      NULL);
}
