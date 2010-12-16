#include "NamedPipeServer.hpp"
//#include <aclapi.h>
#include <preprocessor_macros/logging_preprocessor_macros.h> //for DEBUGN(...)
//for format_output_data(...)
#include <Controller/character_string/format_as_string.hpp>
#include <Windows/access_rights/DiscretionaryAccessControlList.h>
#include <Windows/ErrorCode/LocalLanguageMessageFromErrorCode.h>
#ifdef __CYGWIN__
  #include <mingw/tchar.h> //for _T(...)
#else
  #include <tchar.h> //for _T(...)
#endif
//ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/ipc/base/
// multithreaded_pipe_server.htm

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
        LOGN("waiting blocked for reading from pipe client"
          //" with handle " << hPipe
          )
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
            LocalLanguageMessageFromErrorCodeA(dw))
          break;
        }
        LOGN("read message from pipe " //<< hPipe
          )
        //switch( byCommand )
        //{
        //  case stop_service:
        //}
        //}
//        LOGN("received client command: " << (WORD) byCommand )
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
   
//        DEBUGN("before write 4 bytes to pipe")
        LOGN("before write the size(4 bytes) "
#ifdef _DEBUG
          << dwByteSize <<
#endif
          "to pipe "// << hPipe
          )
     // Write the reply to the pipe.
        fSuccess = ::WriteFile(
                   hPipe,        // handle to pipe
                   & dwByteSize , //chReply,      // buffer to write from
                   4 ,//cbReplyBytes, // number of bytes to write
                   & dwNumBytesWritten,   // number of bytes written
                   NULL // not overlapped I/O
                   );
        ::FlushFileBuffers(hPipe);
//        DEBUGN( dwNumBytesWritten << " bytes written to pipe " << hPipe )
        LOGN( dwNumBytesWritten << " bytes written to pipe " //<< hPipe
          )
        if( fSuccess )
          DEBUGN( " successfully written to pipe")
        else
          DEBUGN( "error writing to pipe")
        if (! fSuccess )
        {
          DWORD dw = ::GetLastError() ;
          LOGN("error writing to pipe:"
            << LocalLanguageMessageFromErrorCodeA(dw) )
//          delete [] arbyPipeDataToSend ;
          break;
        }
        if( fSuccess  )
        {
          if( //> 0 bytes, array <> NULL
            dwByteSize && arbyPipeDataToSend )
          {
  //          DEBUGN("before write " << dwByteSize << " bytes to pipe")
            LOGN("before writing the actual data to the pipe " //<< hPipe
#ifdef _DEBUG
              "( " << dwByteSize << "byte) "
#endif
              )
            std::string stdstrBytes //( (char *) arbyPipeDataToSend, dwByteSize ) ;
              = format_output_data( arbyPipeDataToSend, dwByteSize, 100) ;
            DEBUGN("send data to pipe:" << stdstrBytes ) ;
            fSuccess = ::WriteFile(
               hPipe,        // handle to pipe
               arbyPipeDataToSend , //chReply,      // buffer to write from
               dwByteSize ,//cbReplyBytes, // number of bytes to write
               & dwNumBytesWritten,   // number of bytes written
               NULL // not overlapped I/O
               );
            ::FlushFileBuffers(hPipe);
            //The array is copied for every pipe client--delete it after sending
            //now.
            delete [] arbyPipeDataToSend ;
//          DEBUGN(dwNumBytesWritten << " bytes written to pipe")
            LOGN( //dwNumBytesWritten <<
              " bytes written to pipe " //<< hPipe
              )
            if( fSuccess )
              //DEBUGN( " successfully written to pipe")
              LOGN( "successfully written data to pipe")
            else
  //            DEBUGN( "error writing to pipe")
              LOGN( "error writing to pipe")
            if (! fSuccess || dwByteSize != dwNumBytesWritten )
            {
              DWORD dw = ::GetLastError() ;
              LOGN("error writing to pipe:"
                << LocalLanguageMessageFromErrorCodeA(dw) )
  //            delete [] arbyPipeDataToSend ;
              break;
            }
          }
        }
//        delete [] arbyPipeDataToSend ;
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
//   criticalsection_locker_type csl(
//     m_criticalsection_typeNumberOfConnectedClients ) ;
//   ++ m_wNumberOfConnectedClients ;
}

NamedPipeServer::NamedPipeServer(
  I_ServerProcess * p_serverprocess
// , I_IPC_DataHandler & r_ipc_datahandler
  )
  :
  mp_ipc_datahandler( NULL)
  , mp_security_descriptor (NULL)
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
    //cites from:
    //http://msdn.microsoft.com/en-us/library/aa365150%28VS.85%29.aspx
    ::CreateNamedPipe(
      //The string must have the following form: \\.\pipe\pipename
      //"\\\\.\\pipe\\CPUcontrollerService" //LPCTSTR lpName,
//      lpszPipename
      m_lpszPipename
      //"The pipe is bi-directional; both server and client processes can
      // read from and write to the pipe."
      , PIPE_ACCESS_DUPLEX //DWORD dwOpenMode,
        //"The caller will have write access to the named pipe's
        //discretionary access control list (ACL)."
        //necessary for changing the security descriptor after creation?!
        | WRITE_DAC
        //error message " A required privilege is not held by the client."
        //when ACCESS_SYSTEM_SECURITY was specified.
//        //"The caller will have write access to the named pipe's SACL."
//        | ACCESS_SYSTEM_SECURITY

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
      //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/secauthz/
      //  security/security_descriptor.htm
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
        //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/secauthz/
        //  security/impersonatenamedpipeclient.htm:
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
          LOGN("CreatePipe failed: " <<
            //http://msdn.microsoft.com/en-us/library/aa365150%28VS.85%29.aspx:
            //"To get extended error information, call GetLastError."
            ::LocalLanguageMessageAndErrorCodeA( ::GetLastError() ) );
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
              //Every pipe client thread must use its own/ a different pipe
              // (handle)
              //Must create on heap because else it would get invalid after
              // leaving this block.
              PipeClientThreadAttributes * pcta = new
                PipeClientThreadAttributes(
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
                 //http://msdn.microsoft.com/en-us/library/
                 // ms686724%28v=VS.85%29.aspx:
                 //"When a thread terminates, its thread object is not freed
                 //until all open handles to the thread are closed."
                 //http://msdn.microsoft.com/en-us/library/
                 // ms724211%28v=VS.85%29.aspx:
                 //"Closing a thread handle does not terminate the associated
                 //thread or remove the thread object."
                 //Close the thread handle here (waiting for the end of the
                 //thread via WaitForSingleObject() would need another
                 //thread->not so good.)
                 ::CloseHandle(hThread);
                 LOGN("spawned thread for pipe client with thread ID "
                   << dwThreadId ) ;
               }
            }
            else
            {
              DWORD dw = ::GetLastError() ;
              LOGN("connecting named pipe failed:" <<
                LocalLanguageMessageFromErrorCodeA(dw) )
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
