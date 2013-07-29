/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * GetSecurityInfo.cpp
 *
 *  Created on: May 19, 2010
 *      Author: Stefan
 */

#include <aclapi.h> //::GetSecurityInfo(...)
#include <global.h>

void OutputPipeInfo(HANDLE handle)
{
  PSID psidOwner ;
  PSID psidGroup ;
  PACL paclDACL ;
  PACL paclSACL ;
  PSECURITY_DESCRIPTOR psecurity_descriptor ;

  //http://msdn.microsoft.com/en-us/library/aa446654%28VS.85%29.aspx:
//  "You can use the GetSecurityInfo function with the following types of objects:
//
//      * Local or remote files or directories on an NTFS file system
//      * Named pipes
//      * Local or remote printers
//      * Local or remote Windows services
//      * Network shares
//      * Registry keys
//      * Semaphores, events, mutexes, and waitable timers
//      * Processes, threads, jobs, and file-mapping objects
//      * Interactive service window stations and desktops
//      * Directory service objects"

  DWORD dw = ::GetSecurityInfo(
    handle , //HANDLE handle,
    SE_SERVICE , // SE_OBJECT_TYPE ObjectType,
    //SECURITY_INFORMATION SecurityInfo,
      DACL_SECURITY_INFORMATION
      | SACL_SECURITY_INFORMATION
      | OWNER_SECURITY_INFORMATION
      | GROUP_SECURITY_INFORMATION
    , & psidOwner //PSID* ppsidOwner,
    , & psidGroup //PSID* ppsidGroup,
    , & paclDACL //PACL* ppDacl,
    , & paclSACL //PACL* ppSacl,
    , & psecurity_descriptor //PSECURITY_DESCRIPTOR* ppSecurityDescriptor
  );
  LOGN("GetSecurityInfo return value :" << dw )
  if( dw == ERROR_SUCCESS )
  {
//    PACL pNewDACL = NULL;
//
//    PSID_IDENTIFIER_AUTHORITY psid_identifier_authority =
//      GetSidIdentifierAuthority(
//        psidOwner
//      );
//    LOGN("SID Identifier authority: " << *((DWORD*)psid_identifier_authority) )
//
//    EXPLICIT_ACCESS ea;
//    ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
//    ea.grfAccessPermissions = GENERIC_ALL ;
//    ea.grfAccessMode = GRANT_ACCESS ;
//    ea.grfInheritance= NO_INHERITANCE ;
//    ea.Trustee.TrusteeForm = TRUSTEE_IS_NAME ;
//    ea.Trustee.ptstrName = //char or wide char string
//      _T("CURRENT_USER") ;
//
//    // Create a new ACL that merges the new ACE
//    // into the existing DACL.
//
//    DWORD dwRes = 0;
//    dwRes = SetEntriesInAcl(
//      1,
//      & ea,
//      paclDACL , //pOldDACL,
//      & pNewDACL
//      );
//    if (ERROR_SUCCESS != dwRes)
//    {
//      LOGN( "SetEntriesInAcl Error:" << dwRes );
//      goto Cleanup;
//    }
//
//    // Attach the new ACL as the object's DACL.
//
//    dwRes = SetSecurityInfo(
//      handle,
//      SE_SERVICE,
//      DACL_SECURITY_INFORMATION,
//      NULL,
//      NULL,
//      pNewDACL,
//      NULL );
//    if (ERROR_SUCCESS != dwRes)
//    {
//      LOGN( "SetNamedSecurityInfo Error:" << dwRes );
//      goto Cleanup;
//    }
//
//    Cleanup:
//    //if(pSD != NULL)
//    //  LocalFree((HLOCAL) pSD);
//    if(pNewDACL != NULL)
//      LocalFree((HLOCAL) pNewDACL);

    LocalFree(psecurity_descriptor) ;
  }
  //  LOGN("error" << GetLastError
}

