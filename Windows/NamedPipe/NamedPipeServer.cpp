#include "NamedPipeServer.hpp"
#include <aclapi.h>
#include <Windows/access_rights/DiscretionaryAccessControlList.h>
#include <Windows/LocalLanguageMessageFromErrorCode.h>
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
class PipeClientThreadAttributes
{
public:
  HANDLE m_handlePipe ;
  NamedPipeServer * mp_namedpipeserver ;
  PipeClientThreadAttributes(
    HANDLE handlePipe
    , NamedPipeServer * p_namedpipeserver
    )
    :
      m_handlePipe ( handlePipe)
      , mp_namedpipeserver ( p_namedpipeserver )
  {

  }
};

VOID PipeClientThread(LPVOID lpvParam) 
{ 
   //TCHAR chRequest[PIPE_BUFFER_SIZE]; 
   //TCHAR chReply[PIPE_BUFFER_SIZE]; 
//   DWORD cbBytesRead, cbReplyBytes, cbWritten;
   LOGN("pipe thread for client")
 
   // The thread's parameter is a handle to a pipe instance. 
   //hPipe = (HANDLE) lpvParam; 
//   NamedPipeServer * p_namedpipeserver = (NamedPipeServer *) lpvParam ;
   PipeClientThreadAttributes * p_pipeclientthreadattributes =
     (PipeClientThreadAttributes * ) lpvParam ;
//   DEBUGN("p_namedpipeserver: " << p_namedpipeserver)
   DEBUGN("p_pipeclientthreadattributes: " << p_pipeclientthreadattributes)
   if( //p_namedpipeserver
       p_pipeclientthreadattributes )
   {
     BOOL fSuccess;
     BYTE * arbyPipeDataToSend ;
     BYTE byCommand ;
     DWORD dwBytesRead ;
     DWORD dwByteSize ;
     DWORD dwNumBytesWritten ;
     HANDLE hPipe;
     NamedPipeServer * p_namedpipeserver =
         p_pipeclientthreadattributes->mp_namedpipeserver ;
//     std::wstring stdwstrMessage ;
//     hPipe = p_namedpipeserver->m_handlePipe ;
     hPipe = p_pipeclientthreadattributes->m_handlePipe ;
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
           , & dwBytesRead, // number of bytes read
           NULL);        // not overlapped I/O 
        //OutputPipeInfo();
        if (! fSuccess || dwBytesRead == 0)
        {
          DWORD dw = ::GetLastError() ;
          LOGN("reading from pipe failed: " <<
            LocalLanguageMessageFromErrorCode(dw))
          break;
        }
        //switch( byCommand )
        //{
        //  case stop_service:
        //}
        //}
        LOGN("received client command: " << (WORD) byCommand )
        dwByteSize =
        p_namedpipeserver->mp_serverprocess->IPC_Message(
          byCommand ,
//          stdwstrMessage
          arbyPipeDataToSend
          ) ;
//        DWORD dwByteSize = p_namedpipeserver->mr_ipc_datahandler.GetResponse(
//          byCommand ,
//          arbyPipeDataToSend
//          ) ;
        //GetAnswerToRequest(chRequest, chReply, &cbReplyBytes); 
   
        DEBUGN("before write 4 bytes to pipe")
     // Write the reply to the pipe.
        fSuccess = ::WriteFile(
                   hPipe,        // handle to pipe
                   & dwByteSize , //chReply,      // buffer to write from
                   4 ,//cbReplyBytes, // number of bytes to write
                   & dwNumBytesWritten,   // number of bytes written
                   NULL // not overlapped I/O
                   );
        ::FlushFileBuffers(hPipe);
        DEBUGN( dwNumBytesWritten << " bytes written to pipe")
        if( fSuccess )
          DEBUGN( " successfully written to pipe")
        else
          DEBUGN( "error writing to pipe")

        if (! fSuccess )
        {
          DWORD dw = ::GetLastError() ;
          LOGN("error writing to pipe:"
            << LocalLanguageMessageFromErrorCode(dw) )
          delete [] arbyPipeDataToSend ;
          break;
        }
        if( fSuccess  )
        {
          DEBUGN("before write " << dwByteSize << " bytes to pipe")
          fSuccess = ::WriteFile(
             hPipe,        // handle to pipe
             arbyPipeDataToSend , //chReply,      // buffer to write from
             dwByteSize ,//cbReplyBytes, // number of bytes to write
             & dwNumBytesWritten,   // number of bytes written
             NULL // not overlapped I/O
             );
          ::FlushFileBuffers(hPipe);
          DEBUGN(dwNumBytesWritten << " bytes written to pipe")
          if( fSuccess )
            DEBUGN( " successfully written to pipe")
          else
            DEBUGN( "error writing to pipe")
          if (! fSuccess || dwByteSize != dwNumBytesWritten )
          {
            DWORD dw = ::GetLastError() ;
            LOGN("error writing to pipe:"
              << LocalLanguageMessageFromErrorCode(dw) )
            delete [] arbyPipeDataToSend ;
            break;
          }
        }
        delete [] arbyPipeDataToSend ;
//        //Writing 0 bytes signals the end of the data
//        ::WriteFile(
//           hPipe,        // handle to pipe
//           arbyPipeDataToSend , //chReply,      // buffer to write from
//           0 ,//cbReplyBytes, // number of bytes to write
//           & dwNumBytesWritten,   // number of bytes written
//           NULL
//           );
    } //while
     //was created on heap by thread for "create pipe client thread "
     delete p_pipeclientthreadattributes ;
  // Flush the pipe to allow the client to read the pipe's contents 
  // before disconnecting. Then disconnect the pipe, and close the 
  // handle to this pipe instance. 
   
     ::FlushFileBuffers(hPipe); 
     ::DisconnectNamedPipe(hPipe); 
     ::CloseHandle(hPipe); 
   }
   LOGN("end of pipe client thread with thread ID:" << ::GetCurrentThreadId() )
}

NamedPipeServer::NamedPipeServer(
  I_ServerProcess * p_serverprocess
// , I_IPC_DataHandler & r_ipc_datahandler
  )
  : mp_security_descriptor (NULL)
  , mp_ipc_datahandler( NULL)
  , m_lpszPipename ( _T("\\\\.\\pipe\\CPUcontrollerService") )
//  , mr_ipc_datahandler (r_ipc_datahandler)
{
  mp_serverprocess = p_serverprocess ;
}

NamedPipeServer::NamedPipeServer(
//      I_ServerProcess * p_serverprocess
  I_IPC_DataHandler & r_ipc_datahandler
  )
  :
  mp_ipc_datahandler ( & r_ipc_datahandler )
  , mp_security_descriptor( NULL )
{

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

void NamedPipeServer::CreateDownPrivilegedPipe()
{
  LOGN("create named pipe \"" << m_lpszPipename << "\"" )
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
//      lpszPipename
      m_lpszPipename
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
      , //& security_attributes
      & m_security_attributes
    );
}

//e.g. create shared memory or create socket and listen on socket.
BYTE NamedPipeServer::Init(
  //LPTSTR lpszPipename //= "\\\\.\\pipe\\CPUcontrollerService" 
  )
{
  LOGN("init. pipe server ");
//  LPTSTR lpszPipename = _T("\\\\.\\pipe\\CPUcontrollerService") ;

  //SECURITY_DESCRIPTOR sd ;
//  SECURITY_ATTRIBUTES security_attributes ;
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

  //http://msdn.microsoft.com/en-us/library/aa379561%28VS.85%29.aspx:
   //"Several functions that use the SECURITY_DESCRIPTOR structure require that
   //this structure be aligned on a valid pointer boundary in memory.
  //These boundaries vary depending on the type of processor used.
  //Memory allocation functions such as malloc  and LocalAlloc return properly
  //aligned pointers."
   mp_security_descriptor = (PSECURITY_DESCRIPTOR) malloc( 
     // = (sizeof(SECURITY_DESCRIPTOR))
     SECURITY_DESCRIPTOR_MIN_LENGTH );

  if ( mp_security_descriptor //)
   //{
     //if ( 
     && InitializeSecurityDescriptor(
       mp_security_descriptor,
       SECURITY_DESCRIPTOR_REVISION
       )
     )
  {
//    security_attributes.nLength = sizeof(security_attributes);
//    security_attributes.bInheritHandle = TRUE;
//    security_attributes.lpSecurityDescriptor = mp_security_descriptor ;
    m_security_attributes.nLength = sizeof(m_security_attributes);
    m_security_attributes.bInheritHandle = TRUE;
    m_security_attributes.lpSecurityDescriptor = mp_security_descriptor ;

    //ImpersonateNamedPipeClient() also allows less-privileged clients to
    //connect?
    if( DowngradeDACL( //& security_attributes
      & m_security_attributes )
      )
    //{
    //}
    {
      do
      {
        CreateDownPrivilegedPipe() ;
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
            BOOL fConnected =
              //http://msdn.microsoft.com/en-us/library/aa365146%28VS.85%29.aspx:
              //"If hNamedPipe was not opened with FILE_FLAG_OVERLAPPED, the
              //function does not return until a client is connected or an error
              //occurs."
              //if synchronous:
              //"If the function succeeds, the return value is nonzero. If the
              //function fails, the return value is zero."
              ::ConnectNamedPipe(
              m_handlePipe, //handle to pipe
              //TODO maybe use asynchronous mode. This would make it possible to
              // exit THIS thread gracefully (correctly free the ressources)
              //when the program is requested to exit.
              NULL //lpOverlapped
              ) ?
              TRUE : (::GetLastError() == ERROR_PIPE_CONNECTED);

            if (fConnected)
            {
              DWORD dwThreadId ;
              LOGN("pipe server: A client connected") ;
              //Every pipe client thread must use its own/ a different pipe (handle)
              //Must create on heap because else it would get invalid after
              // leaving this block.
              PipeClientThreadAttributes * pcta = new PipeClientThreadAttributes(
                m_handlePipe
                , this) ;
            // Create a thread for this client.
              HANDLE hThread = ::CreateThread(
                  NULL,              // no security attribute
                  0,                 // default stack size
                  (LPTHREAD_START_ROUTINE) PipeClientThread,
                  //(LPVOID) m_handlePipe,    // thread parameter
//                  this ,
                  pcta ,
                  0,                 // not suspended
                  & dwThreadId // returns thread ID
                  );
               if ( hThread == NULL )
               {
                  LOGN("pipe server: CreateThread failed");
                  return 0;
               }
               else
               {
                 ::CloseHandle(hThread);
                 LOGN("spawned thread for pipe client with thread ID "
                   << dwThreadId ) ;
               }
            }
            else
            {
              DWORD dw = ::GetLastError() ;
              LOGN("connecting named pipe failed:" <<
                LocalLanguageMessageFromErrorCode(dw) )
              // The client could not connect, so close the pipe.
              ::CloseHandle(m_handlePipe);
              break;
            }
          }
      }while(1) ;
      LOGN("after end of loop for spawning pipe client threads")
    } //DowngradeDACL
  }
  return 1 ;
}
////connects e.g. to the shared memory
//NamedPipeServer::ConnectToProvider()
//{
//
//}

