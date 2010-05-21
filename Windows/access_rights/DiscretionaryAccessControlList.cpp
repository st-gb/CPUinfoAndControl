
//for ConvertStringSecurityDescriptorToSecurityDescriptorA in sddl.h
#define _WIN32_WINNT 0x0500

#include "DiscretionaryAccessControlList.h"
#include <Windows/LocalLanguageMessageFromErrorCode.h>

#include <windows.h> //SECURITY_ATTRIBUTES,...
#include <aclapi.h> // GetSecurityInfo() , SetEntriesInAcl, SetSecurityInfo
#include <accctrl.h> //EXPLICIT_ACCESS
#include <Sddl.h> //ConvertStringSecurityDescriptorToSecurityDescriptor(...)
#include "global.h" //for DEBUGN(...)

#ifdef __CYGWIN__
  #include <mingw/tchar.h> //for _T(...)
#else
  #include <tchar.h> //for _T()
#endif
#ifndef _MSC_VER //if not using an MS compiler
  //from Platform SDK's pwrprof.h
  #define SDDL_REVISION_1     1
    BOOL
    //WINAPI
//  ConvertStringSecurityDescriptorToSecurityDescriptor(
  ConvertStringSecurityDescriptorToSecurityDescriptorA(
    LPCSTR StringSecurityDescriptor,
    DWORD StringSDRevision,
    PSECURITY_DESCRIPTOR * SecurityDescriptor,
    PULONG  SecurityDescriptorSize OPTIONAL
    );
#endif
#include <sddl.h> //ConvertStringSecurityDescriptorToSecurityDescriptor

#ifdef _MSC_VER  //MicroSoft compiler knows this pragma
//This pragma can be used instead of including the library in the linker options.
#pragma comment( lib, "Advapi32.lib" )
#endif

//from http://msdn.microsoft.com/en-us/library/aa365150%28VS.85%29.aspx
// community content

//    Permissions necessary to open a named pipe
//    It appears that in order for a client (running under lower privilege rights) to open a named pipe, they must have FILE_READ_ATTRIBUTES and SYNCHRONIZE access, so be sure to grant this access to any users you want to be able to connect.
//
//    Here's some code (C++) on how to do this:
//
//    ------------------------------------------------------------------------------------------------------------------

    // Create a pipe
//    hPipe = CreateNamedPipe(L\\\\.\\pipe\\MyPipeNameHere,
//    PIPE_ACCESS_OUTBOUND | WRITE_DAC,
//        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT, 1,
//
//        1024, 1024, 2000, NULL);
  // An ACE must be added to pipe's DACL so that client processes
  // running under low-privilege accounts are also able to change state
  // of client end of this pipe to Non-Blocking and Message-Mode.
void AddLowPrivilegedACE( HANDLE & r_handle )
{
//    PACL pACL = NULL;
//
//    PACL pNewACL = NULL;
//
//    EXPLICIT_ACCESS explicit_access_list[1];
//
//    TRUSTEE trustee[1];
//
//    GetSecurityInfo(r_handle, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, NULL,
//        NULL, &pACL, NULL, NULL);
//
//    trustee[0].TrusteeForm = TRUSTEE_IS_NAME;
//
//    trustee[0].TrusteeType = TRUSTEE_IS_GROUP;
//
//    trustee[0].ptstrName = _T("Everyone");
//
//    trustee[0].MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
//
//    trustee[0].pMultipleTrustee = NULL;
//
//    ZeroMemory(&explicit_access_list[0],
//    sizeof(EXPLICIT_ACCESS));
//
//    explicit_access_list[0].grfAccessMode = GRANT_ACCESS;
//
//    explicit_access_list[0].grfAccessPermissions =
//        FILE_WRITE_ATTRIBUTES | READ_CONTROL ;
//
//    explicit_access_list[0].grfInheritance = NO_INHERITANCE;
//
//    explicit_access_list[0].Trustee = trustee[0];
//
//    SetEntriesInAcl(1, explicit_access_list, pACL, &pNewACL);
//
//    DWORD dw =
//      SetSecurityInfo(r_handle, SE_KERNEL_OBJECT,
//      DACL_SECURITY_INFORMATION, NULL, NULL, pNewACL, NULL);
////    if( dw == ERROR_SUCCESS )
//    DEBUGN("SetSecurityInfo " << (dw == ERROR_SUCCESS? "succeeded" : "failed" ) )
//
//    LocalFree(pNewACL);
//    -------------------------------------------------------------------------
//    NOTE: Denying access to named pipe from remote users can be done with
//    the same code as above. Just replace L"Everyone" with L"Network" with
//    changes to grfAccessMode and grfAccessPermissions.
//
//    Code on this post is added by Sanje2v. Original post and/or last editor
//    was Joseph Galbraith.
}

//from MS platform SDK v6.1\Samples\WinBase\Service:
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

//If using the DACL from an object of an administrator then usually a normal
//user is not granted acces to it. So create a DACL with low privileges.
BOOL DowngradeDACL(SECURITY_ATTRIBUTES * p_security_attributes)
{
  if ( p_security_attributes == NULL )
    return FALSE;
  PSECURITY_DESCRIPTOR p_security_descriptor = ( (PSECURITY_DESCRIPTOR)
    p_security_attributes->lpSecurityDescriptor ) ;

//  // Define the SDDL for the DACL.
//  // Modify these values as needed to generate the proper
//  // DACL for your application.
//  TCHAR * szSD =
//    _T("D:")                  // Discretionary ACL
//    _T("D;OICI;GA;;;BG)")     // Deny all access to Built-in Guests
//    _T("D;OICI;GA;;;AN)")     // Deny all access to Anonymous Logon
//    _T("A;OICI;GRGWGX;;;AU)") // Allow read/write/execute to Authenticated Users
//    _T("A;OICI;GA;;;BA)") ;  // Allow full control to Administrators

  //http://msdn.microsoft.com/en-us/library/aa379561%28v=VS.85%29.aspx:
  //"Because the internal format of a security descriptor can vary, we
  //recommend that applications not modify the SECURITY_DESCRIPTOR  structure
  //directly. For creating and manipulating a security descriptor, use the
  //functions listed in See Also."

  //http://msdn.microsoft.com/en-us/library/aa376401%28VS.85%29.aspx:
//    return ConvertStringSecurityDescriptorToSecurityDescriptor(
//    return
//      ConvertStringSecurityDescriptorToSecurityDescriptorA(
//      szSD,
//      SDDL_REVISION_1,
//      //"A pointer to a variable that receives a pointer to the converted
//      //security descriptor. The returned security descriptor is self-relative.
//      //To free the returned buffer, call the LocalFree function. To convert
//      //the security descriptor to an absolute security descriptor, use the
//      //MakeAbsoluteSD function."
//      //cast for g++
//      (PSECURITY_DESCRIPTOR *)
//      &(pSA->lpSecurityDescriptor),
//      //"A pointer to a variable that receives the size, in bytes, of the
//      //converted security descriptor. This parameter can be NULL."
//      NULL
//      );

  DEBUGN("sec desc revision:" << (WORD) p_security_descriptor->Revision )
  //Without setting the revision "SetSecurityDescriptorDacl" fails.
  p_security_descriptor->Revision = SDDL_REVISION_1 ;
  BOOL _bool =
  //http://msdn.microsoft.com/en-us/library/aa379583%28v=VS.85%29.aspx:
  //"If the function succeeds, the function returns nonzero."
  SetSecurityDescriptorDacl (
//    (PSECURITY_DESCRIPTOR)
//    & (p_security_attributes->lpSecurityDescriptor),
    p_security_descriptor ,
    //"A flag that indicates the presence of a DACL in the security
    //descriptor. If this parameter is TRUE, the function sets the
    //SE_DACL_PRESENT flag in the SECURITY_DESCRIPTOR_CONTROL  structure and
    //uses the values in the pDacl and bDaclDefaulted  parameters.
    //If this parameter is FALSE, the function clears the SE_DACL_PRESENT
    //flag, and pDacl and bDaclDefaulted are ignored."
    TRUE ,//bDaclPresent [in]
    //http://msdn.microsoft.com/en-us/library/aa379583%28v=VS.85%29.aspx:
    //"When an object has no DACL (when the pDacl parameter is NULL), no
    //protection is assigned to the object, and all access requests are granted.
    //To help maintain security, restrict access by using a DACL."
    NULL ,
    //"A flag that indicates the source of the DACL. If this flag is TRUE,
    //the DACL has been retrieved by some default mechanism. If FALSE, the
    //DACL has been explicitly specified by a user."
    FALSE
    ) ;
  DEBUGN("result of SetSecurityDescriptorDacl: " << _bool )
  if( ! _bool )
  {
    DWORD dw =
        ::GetLastError() ;
    DEBUGN("SetSecurityDescriptorDacl failed: error code:" << dw
      << " " << LocalLanguageMessageFromErrorCode(dw) )
  }
  return _bool ;
}

BYTE CreateSecAttributes(
  SECURITY_ATTRIBUTES & sa )
{
  BYTE byReturn = 0 ;
//  DWORD dwRes, dwDisposition;
//  PSID pEveryoneSID = NULL , pAdminSID = NULL;
//  PACL pACL = NULL;
//  PSECURITY_DESCRIPTOR pSD = NULL;
//  SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
//  SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
//  //SECURITY_ATTRIBUTES sa;
//  LONG lRes;
//  HKEY hkSub = NULL;
//
//  //SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
//  //SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
//  //SECURITY_ATTRIBUTES sa;
//  EXPLICIT_ACCESS explicit_access , ar_explicit_access [2] ;
//
//  // Create a well-known SID for the Everyone group.
//  if( ! AllocateAndInitializeSid(
//      & SIDAuthWorld,
//      1,
//     SECURITY_WORLD_RID,
//     0, 0, 0, 0, 0, 0, 0,
//     &pEveryoneSID )
//    )
//  {
//    LOGN("AllocateAndInitializeSid Error %u\n", GetLastError());
//    goto Cleanup;
//  }
//  ZeroMemory(&explicit_access, sizeof(EXPLICIT_ACCESS) );
//  ZeroMemory(ar_explicit_access, 2 * sizeof(EXPLICIT_ACCESS) );
//  // Initialize an EXPLICIT_ACCESS structure for an ACE.
//  // The ACE will allow Everyone group full access to the key.
//  explicit_access.grfAccessPermissions = KEY_ALL_ACCESS;
//  explicit_access.grfAccessMode = SET_ACCESS;
//  explicit_access.grfInheritance= NO_INHERITANCE;
//  explicit_access.Trustee.TrusteeForm = TRUSTEE_IS_SID;
//  explicit_access.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP ;
//  explicit_access.Trustee.ptstrName  = (LPTSTR) //pAdminSID;
//    pEveryoneSID ;
//  // Create a SID for the BUILTIN\Administrators group.
//  if(! AllocateAndInitializeSid(
//    & SIDAuthNT,
//    2,
//     SECURITY_BUILTIN_DOMAIN_RID,
//     DOMAIN_ALIAS_RID_ADMINS,
//     0, 0, 0, 0, 0, 0,
//     &pAdminSID )
//    )
//  {
//    LOGN("AllocateAndInitializeSid Error %u\n", GetLastError());
//    goto Cleanup;
//  }
//
//  // Initialize an EXPLICIT_ACCESS structure for an ACE.
//  // The ACE will allow the Administrators group full access to the key.
//  ar_explicit_access [1].grfAccessPermissions = KEY_ALL_ACCESS;
//  ar_explicit_access [1].grfAccessMode = SET_ACCESS;
//  ar_explicit_access [1].grfInheritance= NO_INHERITANCE;
//  ar_explicit_access [1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
//  ar_explicit_access [1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
//  ar_explicit_access [1].Trustee.ptstrName  = (LPTSTR) pAdminSID;
//
//  ar_explicit_access [0] = explicit_access ;
//  // Create a new ACL that contains the new ACEs.
//    dwRes =
//      //The SetEntriesInAcl function creates a new access control list
//      //(ACL) by merging new access control or audit control information
//      //into an existing ACL structure.
//      SetEntriesInAcl(
//      //cCountOfExplicitEntries
//      //1
//      2
//      //, & explicit_access
//      //, & ar_explicit_access [1]
//      , ar_explicit_access
//      , NULL //OldAcl
//      , & pACL );
//    if (ERROR_SUCCESS != dwRes)
//    {
//        LOGN("SetEntriesInAcl Error %u\n", GetLastError());
//        goto Cleanup;
//    }
//
//    // Initialize a security descriptor.
//    pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(
//      LPTR,
//      SECURITY_DESCRIPTOR_MIN_LENGTH );
//    if (NULL == pSD)
//    {
//        LOGN("LocalAlloc Error %u\n", GetLastError());
//        goto Cleanup;
//    }
//
//    if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
//    {
//        LOGN("InitializeSecurityDescriptor Error %u\n",
//                                GetLastError());
//        goto Cleanup;
//    }
//    // Add the ACL to the security descriptor.
//    if ( ! SetSecurityDescriptorDacl(
//          pSD,
//          TRUE,     // bDaclPresent flag
//          pACL,
//          FALSE // not a default DACL
//          )
//       )
//    {
//      LOGN("SetSecurityDescriptorDacl Error %u\n", GetLastError());
//      goto Cleanup;
//    }
//    // Initialize a security attributes structure.
//    sa.nLength = sizeof (SECURITY_ATTRIBUTES);
//    sa.lpSecurityDescriptor = pSD;
//    sa.bInheritHandle = //FALSE;
//      TRUE ;
//    byReturn = 1 ;
//
//Cleanup:
//    if (pEveryoneSID)
//        FreeSid(pEveryoneSID);
//    //if (pAdminSID)
//    //    FreeSid(pAdminSID);
//    if (pACL)
//        LocalFree(pACL);
//    if (pSD)
//        LocalFree(pSD);
//    if (hkSub)
//        RegCloseKey(hkSub);
//    LOGN("CreateSecAttributes return: " << (WORD) byReturn )

    DWORD dwRes;
//    DWORD dwDisposition;
    PSID pEveryoneSID = NULL, pAdminSID = NULL;
    PACL pACL = NULL;
    PSECURITY_DESCRIPTOR pSD = NULL;
    EXPLICIT_ACCESS ea[2];
    SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
    SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
    //SECURITY_ATTRIBUTES sa;
//    LONG lRes;
    HKEY hkSub = NULL;

    // Create a well-known SID for the Everyone group.
    if(!AllocateAndInitializeSid(&SIDAuthWorld, 1,
                     SECURITY_WORLD_RID,
                     0, 0, 0, 0, 0, 0, 0,
                     &pEveryoneSID))
    {
        printf("AllocateAndInitializeSid Error %lu\n", GetLastError());
        goto Cleanup;
    }

    // Initialize an EXPLICIT_ACCESS structure for an ACE.
    // The ACE will allow Everyone read access to the key.
    ZeroMemory(&ea, 2 * sizeof(EXPLICIT_ACCESS));
    ea[0].grfAccessPermissions = KEY_READ;
    ea[0].grfAccessMode = SET_ACCESS;
    ea[0].grfInheritance= NO_INHERITANCE;
    ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
    ea[0].Trustee.ptstrName  = (LPTSTR) pEveryoneSID;

    // Create a SID for the BUILTIN\Administrators group.
    if(! AllocateAndInitializeSid(&SIDAuthNT, 2,
                     SECURITY_BUILTIN_DOMAIN_RID,
                     DOMAIN_ALIAS_RID_ADMINS,
                     0, 0, 0, 0, 0, 0,
                     &pAdminSID))
    // Create a SID for the BUILTIN\local system group.
    if(! AllocateAndInitializeSid(
        &SIDAuthNT,
        //http://support.microsoft.com/kb/288900/de:
        //NT Bekannte SIDs (wie LocalSystem ) haben nur einen RID Wert
        //relativ zu der Bezeichnerautorit�t SECURITY_NT_AUTHORITY.
        1,
       SECURITY_LOCAL_SYSTEM_RID,
       0, 0, 0, 0, 0, 0, 0,
       &pAdminSID)
       )
    {
      DEBUG_SPRINTF("AllocateAndInitializeSid Error %lu\n", GetLastError());
        goto Cleanup;
    }

    // Initialize an EXPLICIT_ACCESS structure for an ACE.
    // The ACE will allow the Administrators group full access to the key.
    ea[1].grfAccessPermissions = KEY_ALL_ACCESS;
    ea[1].grfAccessMode = SET_ACCESS;
    ea[1].grfInheritance= NO_INHERITANCE;
    ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
    ea[1].Trustee.ptstrName  = (LPTSTR) pAdminSID;

    // Create a new ACL that contains the new ACEs.
    //dwRes = SetEntriesInAcl(2, ea, NULL, &pACL);
    dwRes = SetEntriesInAcl(1, & ea[1], NULL, &pACL);
    if (ERROR_SUCCESS != dwRes)
    {
      DEBUG_SPRINTF("SetEntriesInAcl Error %lu\n", GetLastError());
        goto Cleanup;
    }

    // Initialize a security descriptor.
    pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR,
                             SECURITY_DESCRIPTOR_MIN_LENGTH);
    if (NULL == pSD)
    {
      DEBUG_SPRINTF("LocalAlloc Error %lu\n", GetLastError());
        goto Cleanup;
    }

    if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
    {
      DEBUG_SPRINTF("InitializeSecurityDescriptor Error %lu\n",
                                GetLastError());
        goto Cleanup;
    }

    // Add the ACL to the security descriptor.
    if (!SetSecurityDescriptorDacl(pSD,
            TRUE,     // bDaclPresent flag
            pACL,
            FALSE))   // not a default DACL
    {
      DEBUG_SPRINTF("SetSecurityDescriptorDacl Error %lu\n", GetLastError());
      goto Cleanup;
    }

    // Initialize a security attributes structure.
    sa.nLength = sizeof (SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = pSD;
    sa.bInheritHandle = FALSE;
    byReturn = 1;

    //// Use the security attributes to set the security descriptor
    //// when you create a key.
    //lRes = RegCreateKeyEx(HKEY_CURRENT_USER, "mykey", 0, "", 0,
    //        KEY_READ | KEY_WRITE, &sa, &hkSub, &dwDisposition);
    //printf("RegCreateKeyEx result %u\n", lRes );

Cleanup:

    if (pEveryoneSID)
        FreeSid(pEveryoneSID);
    if (pAdminSID)
        FreeSid(pAdminSID);
    if (pACL)
        LocalFree(pACL);
    if (pSD)
        LocalFree(pSD);
    if (hkSub)
        RegCloseKey(hkSub);

    return byReturn ;
}
