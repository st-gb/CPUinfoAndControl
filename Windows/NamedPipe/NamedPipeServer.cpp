#include "NamedPipeServer.hpp"
#include <aclapi.h>
#include <Windows/DiscretionaryAccessControlList.h>
#ifdef __CYGWIN__
  #include <mingw/tchar.h> //for _T(...)
#else
  #include <tchar.h> //for _T(...)
#endif
//ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/ipc/base/multithreaded_pipe_server.htm

//struct PipeClientThreadStruct
//{
//  HANDLE hPipe
//  mp_serverprocess
//};

void OutputPipeInfo(HANDLE handlePipe)
{
  PSID psidOwner ;
  PSID psidGroup ;
  PACL paclDACL ;
  PACL paclSACL ;
  PSECURITY_DESCRIPTOR psecurity_descriptor ;
  
  DWORD dw = ::GetSecurityInfo(
    handlePipe , //HANDLE handle,
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
    PACL pNewDACL = NULL;

    PSID_IDENTIFIER_AUTHORITY psid_identifier_authority =
      GetSidIdentifierAuthority(
        psidOwner
      );
    LOGN("SID ID auth: " << *((DWORD*)psid_identifier_authority) )

    EXPLICIT_ACCESS ea;
    ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
    ea.grfAccessPermissions = GENERIC_ALL ;
    ea.grfAccessMode = GRANT_ACCESS ;
    ea.grfInheritance= NO_INHERITANCE ;
    ea.Trustee.TrusteeForm = TRUSTEE_IS_NAME ;
    ea.Trustee.ptstrName = //char or wide char string 
      _T("CURRENT_USER") ;

    // Create a new ACL that merges the new ACE
    // into the existing DACL.

    DWORD dwRes = 0;
    dwRes = SetEntriesInAcl(
      1, 
      & ea, 
      paclDACL , //pOldDACL, 
      & pNewDACL
      );
    if (ERROR_SUCCESS != dwRes)
    {
      LOGN( "SetEntriesInAcl Error:" << dwRes );
      goto Cleanup; 
    } 

    // Attach the new ACL as the object's DACL.

    dwRes = SetSecurityInfo(
      handlePipe, 
      SE_SERVICE, 
      DACL_SECURITY_INFORMATION,
      NULL, 
      NULL, 
      pNewDACL, 
      NULL );
    if (ERROR_SUCCESS != dwRes)
    {
      LOGN( "SetNamedSecurityInfo Error:" << dwRes );
      goto Cleanup; 
    } 

    Cleanup:
    //if(pSD != NULL) 
    //  LocalFree((HLOCAL) pSD); 
    if(pNewDACL != NULL) 
      LocalFree((HLOCAL) pNewDACL); 
      
    LocalFree(psecurity_descriptor) ;
  }
  //  LOGN("error" << GetLastError
}

VOID PipeClientThread(LPVOID lpvParam) 
{ 
   //TCHAR chRequest[PIPE_BUFFER_SIZE]; 
   //TCHAR chReply[PIPE_BUFFER_SIZE]; 
   BYTE byCommand ;
   DWORD cbBytesRead, cbReplyBytes, cbWritten; 
   BOOL fSuccess; 
   HANDLE hPipe; 
   LOGN("pipe thread for client")
 
   // The thread's parameter is a handle to a pipe instance. 
   //hPipe = (HANDLE) lpvParam; 
   NamedPipeServer * p_namedpipeserver = (NamedPipeServer *) lpvParam ;
   DEBUGN("p_namedpipeserver: " << p_namedpipeserver)
   if( p_namedpipeserver )
   {
     hPipe = p_namedpipeserver->m_handlePipe ;
     DEBUGN("pipe handle: " << hPipe )
     while (1) 
     { 
        LOGN("waiting blocked for reading from pipe client")
       // Read client requests from the pipe. 
        fSuccess = ::ReadFile(
           hPipe        // handle to pipe 
           //chRequest,    // buffer to receive data 
           , & byCommand
           //PIPE_BUFFER_SIZE * sizeof(TCHAR), // size of buffer 
           , 1
           , & cbBytesRead, // number of bytes read 
           NULL);        // not overlapped I/O 
        //OutputPipeInfo();
        if (! fSuccess || cbBytesRead == 0) 
           break; 
        //switch( byCommand )
        //{
        //  case stop_service:
        //}
        //}
        LOGN("received client command: " << (WORD) byCommand )
        p_namedpipeserver->mp_serverprocess->IPC_Message(byCommand) ;
        //GetAnswerToRequest(chRequest, chReply, &cbReplyBytes); 
   
     //// Write the reply to the pipe. 
     //   fSuccess = WriteFile( 
     //      hPipe,        // handle to pipe 
     //      chReply,      // buffer to write from 
     //      cbReplyBytes, // number of bytes to write 
     //      &cbWritten,   // number of bytes written 
     //      NULL);        // not overlapped I/O 

     //   if (! fSuccess || cbReplyBytes != cbWritten) break; 
    } 
   
  // Flush the pipe to allow the client to read the pipe's contents 
  // before disconnecting. Then disconnect the pipe, and close the 
  // handle to this pipe instance. 
   
     ::FlushFileBuffers(hPipe); 
     ::DisconnectNamedPipe(hPipe); 
     ::CloseHandle(hPipe); 
   }
}

NamedPipeServer::NamedPipeServer(I_ServerProcess * p_serverprocess)
  : mp_security_descriptor (NULL)
{
  mp_serverprocess = p_serverprocess ;
}

NamedPipeServer::~NamedPipeServer()
{
  DEBUGN("~NamedPipeServer()")
   if ( mp_security_descriptor )
   {
      free( mp_security_descriptor );
      DEBUGN("freed the sec desc")
   }
}

BYTE NamedPipeServer::CreateSecAttributes(
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

    DWORD dwRes, dwDisposition;
    PSID pEveryoneSID = NULL, pAdminSID = NULL;
    PACL pACL = NULL;
    PSECURITY_DESCRIPTOR pSD = NULL;
    EXPLICIT_ACCESS ea[2];
    SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
    SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
    //SECURITY_ATTRIBUTES sa;
    LONG lRes;
    HKEY hkSub = NULL;

    // Create a well-known SID for the Everyone group.
    if(!AllocateAndInitializeSid(&SIDAuthWorld, 1,
                     SECURITY_WORLD_RID,
                     0, 0, 0, 0, 0, 0, 0,
                     &pEveryoneSID))
    {
        printf("AllocateAndInitializeSid Error %u\n", GetLastError());
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
        printf("AllocateAndInitializeSid Error %u\n", GetLastError());
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
        printf("SetEntriesInAcl Error %u\n", GetLastError());
        goto Cleanup;
    }

    // Initialize a security descriptor.  
    pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, 
                             SECURITY_DESCRIPTOR_MIN_LENGTH); 
    if (NULL == pSD) 
    { 
        printf("LocalAlloc Error %u\n", GetLastError());
        goto Cleanup; 
    } 
 
    if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)) 
    {  
        printf("InitializeSecurityDescriptor Error %u\n",
                                GetLastError());
        goto Cleanup; 
    } 
 
    // Add the ACL to the security descriptor. 
    if (!SetSecurityDescriptorDacl(pSD, 
            TRUE,     // bDaclPresent flag   
            pACL, 
            FALSE))   // not a default DACL 
    {  
        printf("SetSecurityDescriptorDacl Error %u\n", GetLastError());
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

//e.g. create shared memory or create socket and listen on socket.
BYTE NamedPipeServer::Init(
  //LPTSTR lpszPipename //= "\\\\.\\pipe\\CPUcontrollerService" 
  )
{
  LOGN("init. pipe server ");
  LPTSTR lpszPipename = _T("\\\\.\\pipe\\CPUcontrollerService") ;

  //SECURITY_DESCRIPTOR sd ;
  SECURITY_ATTRIBUTES sa ;
  //SID_IDENTIFIER_AUTHORITY siaLocal = SECURITY_LOCAL_SID_AUTHORITY;
  //if( !AllocateAndInitializeSid( 
  //  & siaLocal, 
  //  1,    
  //  SECURITY_LOCAL_RID,    
  //  0, 0, 0, 0, 0, 0, 0,     
  //  & pSidLocal ) 
  //  )
//http://www.codeproject.com/KB/vista-security/interaction-in-vista.aspx
  //{    break;}
  //if( CreateSecAttributes( sa )
  //  )
  ////http://www.codeguru.com/forum/archive/index.php/t-301326.html:
  //SECURITY_ATTRIBUTES sa;
  //SECURITY_DESCRIPTOR sd;

  //InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
  //SetSecurityDescriptorDacl(&sd,TRUE,NULL,FALSE);
  //SetSecurityDescriptorGroup(&sd,NULL, FALSE ); 
  //SetSecurityDescriptorSacl(&sd, FALSE, NULL, FALSE );

  //sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  //sa.lpSecurityDescriptor = &sd;
  //sa.bInheritHandle = TRUE;

    //from MS platform SDK v6.1\Samples\WinBase\Service:
   // create a security descriptor that allows anyone to write to
   //  the pipe...
   //
   mp_security_descriptor = (PSECURITY_DESCRIPTOR) malloc( 
     // = (sizeof(SECURITY_DESCRIPTOR))
     SECURITY_DESCRIPTOR_MIN_LENGTH );

  if ( mp_security_descriptor //)
   //{
     //if ( 
     && InitializeSecurityDescriptor(mp_security_descriptor, 
          SECURITY_DESCRIPTOR_REVISION)
     )
  {
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = mp_security_descriptor ;

    if( DowngradeDACL(&sa) )
    //{
    //}
    {
      //http://msdn.microsoft.com/en-us/library/ms707085(VS.85).aspx
      //SECURITY_WORLD_SID_AUTHORITY
      //Windows Me/98/95:  Named pipes cannot be created.
      m_handlePipe = 
        //If the function succeeds, the return value is a handle to the 
        //server end of a named pipe instance.
        //http://msdn.microsoft.com/en-us/library/aa365150%28VS.85%29.aspx:
        ::CreateNamedPipe(
          //The string must have the following form: \\.\pipe\pipename
          //"\\\\.\\pipe\\CPUcontrollerService" //LPCTSTR lpName,
          lpszPipename

          , PIPE_ACCESS_DUPLEX //DWORD dwOpenMode,
            //"The caller will have write access to the named pipe's
            //discretionary access control list (ACL)."
            | WRITE_DAC //necessary for changing the sec desc after creation?!
          , PIPE_TYPE_BYTE //DWORD dwPipeMode,
            | PIPE_WAIT        // blocking mode 
          , PIPE_UNLIMITED_INSTANCES //DWORD nMaxInstances,
          , PIPE_BUFFER_SIZE //DWORD nOutBufferSize,
          , PIPE_BUFFER_SIZE  //DWORD nInBufferSize,
          //DWORD nDefaultTimeOut,
          , NMPWAIT_USE_DEFAULT_WAIT // client time-out 
          //LPSECURITY_ATTRIBUTES lpSecurityAttributes
          //http://msdn.microsoft.com/en-us/library/aa365150%28VS.85%29.aspx:
          //"If lpSecurityAttributes is NULL, the named pipe gets a default
          //security descriptor and the handle cannot be inherited."
          //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/secauthz/security/security_descriptor.htm
          //"The ACLs in the default security descriptor for a named pipe grant
          //full control to the LocalSystem account, administrators, and the
          //creator owner. They also grant read access to members of the
          //Everyone group and the anonymous account."
//          , NULL
          , & sa
        );
      //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/secauthz/security/impersonatenamedpipeclient.htm:
      //ImpersonateNamedPipeClient

      //DWORD GetSecurityInfo(
      //  m_handlePipe ,//HANDLE handle,
      //  SE_OBJECT_TYPE ObjectType,
      //  SECURITY_INFORMATION SecurityInfo,
      //  PSID* ppsidOwner,
      //  PSID* ppsidGroup,
      //  PACL* ppDacl,
      //  PACL* ppSacl,
      //  PSECURITY_DESCRIPTOR* ppSecurityDescriptor
      //);

      if ( m_handlePipe == INVALID_HANDLE_VALUE) 
      {
          LOGN("CreatePipe failed"); 
      }
      else
      {
        LOGN("CreatePipe succeeded"); 
//        AddLowPrivilegedACE(m_handlePipe) ;
//        OutputPipeInfo(m_handlePipe) ;
        LOGN("Waiting for incoming client connections")
        // Wait for the client to connect; if it succeeds, 
        // the function returns a nonzero value. If the function
        // returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 
        BOOL fConnected = ::ConnectNamedPipe( 
          m_handlePipe, //handle to pipe
          NULL //lpOverlapped
          ) ? 
          TRUE : (::GetLastError() == ERROR_PIPE_CONNECTED); 

        if (fConnected) 
        { 
          DWORD dwThreadId ;
          LOGN("pipe server: A client connected") ;
        // Create a thread for this client. 
          HANDLE hThread = ::CreateThread( 
              NULL,              // no security attribute 
              0,                 // default stack size 
              (LPTHREAD_START_ROUTINE) PipeClientThread, 
              //(LPVOID) m_handlePipe,    // thread parameter 
              this ,
              0,                 // not suspended 
              & dwThreadId // returns thread ID 
              );

           if ( hThread == NULL )
           {
              LOGN("pipe server: CreateThread failed");
              return 0;
           }
           else ::CloseHandle(hThread); 
        } 
        else 
          // The client could not connect, so close the pipe. 
          ::CloseHandle(m_handlePipe); 
      }
    }
  }
  return 1 ;
}
////connects e.g. to the shared memory
//NamedPipeServer::ConnectToProvider()
//{
//
//}

