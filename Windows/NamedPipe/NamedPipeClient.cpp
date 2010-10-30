#include "NamedPipeClient.hpp"
#include <global.h> //LOGN
#include <windows.h>
//for format_output_data(...)
#include <Controller/character_string/format_as_string.hpp>
#include <ModelData/ModelData.hpp> //class Model
#include <Windows/LocalLanguageMessageFromErrorCode.h>
#include <sstream> //class std::ostringstream
//including specstrings.h lead to error messages for std::string include files?!
//#include <specstrings.h> //for __out
//ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/ipc/base/named_pipe_client.htm

#define BUFSIZE 512
#define NAMED_PIPE_NAME_ANSI "\\\\.\\pipe\\CPUcontrollerService"

void NamedPipeClient::Disconnect()
{
  Disconnect_Inline() ;
}

bool NamedPipeClient::GetConnectionStateViaGetNamedPipeHandleState()
{
  bool bConnected = false ;
  BOOL bool_ =
//    m_bool =
    //If this method is called by thread A while thread B is waiting for data
    //via ::ReadFile(...) it waits as long until ::ReadFile(...) returns.
    ::GetNamedPipeHandleState(
    m_handleClientPipe , //__in       HANDLE hNamedPipe,
    & m_dwState , //__out_opt  LPDWORD lpState,
    //http://msdn.microsoft.com/en-us/library/aa365443%28VS.85%29.aspx:
    //"A pointer to a variable that receives the number of current pipe
    //instances."
    & m_dwCurInstances ,//__out_opt  LPDWORD lpCurInstances,
    //http://msdn.microsoft.com/en-us/library/aa365443%28v=VS.85%29.aspx:
    //"This parameter must be NULL if the specified pipe handle is to the
    //server end of a named pipe or if
    //client and server processes are on the same computer.
    //This parameter can be NULL if this information is not required."
    NULL , //__out_opt  LPDWORD lpMaxCollectionCount,
    //http://msdn.microsoft.com/en-us/library/aa365443%28v=VS.85%29.aspx:
    //"This parameter must be NULL if the specified pipe handle is to the
    //server end of a named pipe or if
    //client and server processes are on the same computer.
    //This parameter can be NULL if this information is not required."
    NULL ,//__out_opt  LPDWORD lpCollectDataTimeout,
    //"This parameter must be NULL if the specified pipe handle is to the
    //client end of a named pipe."
    NULL ,//__out_opt  LPTSTR lpUserName,
    m_dwMaxUserNameSize //__in       DWORD nMaxUserNameSize
    );
  if( bool_ )
  {
    DEBUGN("pipe state:" << m_dwState << "# of current instances:"
      << m_dwCurInstances )
//    DEBUGN("already connected to the service's pipe")
    if( //The assumption dwCurInstances > 0 -> connected did not always work:
        //if the service was stopped, there # instance was "1"
        m_dwCurInstances > 0 )
    {
      bConnected = true ;
      LOGN("# instances > 0 (" << m_dwCurInstances << ")-> IsConnected==true")
    }
  }
  else //-> failed when it was not connected
  {
    DWORD dwErrorCode = ::GetLastError() ;
//    DEBUGN("GetNamedPipeHandleState failed")
    //Error code if not connected--avoid (too many) messages for this error
    //code.
    if( dwErrorCode != ERROR_INVALID_HANDLE )
      LOGN("Getting pipe info failed:" <<
        ::LocalLanguageMessageAndErrorCodeA(dwErrorCode) )
  }
  return //bool_ ;
    bConnected ;
}

//NamedPipeClient::NamedPipeClient(
//  LPTSTR lpszPipename //= TEXT("\\\\.\\pipe\\mynamedpipe"); 
//  )
BYTE NamedPipeClient::Init(std::string & r_stdstrMessage)
{
  m_bConnected = false ;
  LPTSTR lptstrPipename ;
  if( m_r_model.m_stdwstrPipeName.empty() )
  {
    std::tstring stdtstrPipeName = TEXT(NAMED_PIPE_NAME_ANSI) ;
    lptstrPipename = //_tcscpy(lptstrPipename, TEXT(NAMED_PIPE_NAME_ANSI) ) ;
      //Else: "error: invalid conversion from `const wchar_t*' to `TCHAR*'"
      (TCHAR*)
      stdtstrPipeName.c_str() ;
  }
  else
    lptstrPipename =
      //Else: "error: invalid conversion from `const wchar_t*' to `TCHAR*'"
      (TCHAR*) Getstdtstring( m_r_model.m_stdwstrPipeName ).c_str() ;
  //LPTSTR lpvMessage=TEXT("Default message from client"); 
  //TCHAR chBuf[BUFSIZE]; 
  //BOOL fSuccess; 
  //DWORD cbRead, dwMode; 
  DWORD dwLastError ;
 
  // Try to open a named pipe; wait for it, if necessary. 
  do
  { 
    m_handleClientPipe =
      //http://msdn.microsoft.com/en-us/library/aa363858%28VS.85%29.aspx:
      //"If the function fails, the return value is INVALID_HANDLE_VALUE.
      //To get extended error information, call GetLastError."
      ::CreateFile(
      lptstrPipename,   // pipe name 
      GENERIC_READ |  // read and write access 
      GENERIC_WRITE, 
      0,              // no sharing 
      NULL,           // default security attributes
      OPEN_EXISTING,  // opens existing pipe 
      0,              // default attributes 
      NULL);          // no template file 
 
      //If the function fails, the return value is INVALID_HANDLE_VALUE.
      if ( m_handleClientPipe != INVALID_HANDLE_VALUE )
      {
        m_bConnected = true ;
        LOGN("got handle to the service's pipe at address " <<
          m_handleClientPipe << "--connected to it")
        // Break if the pipe handle is valid. 
        break;
      }
 
      dwLastError = ::GetLastError() ;
      // Exit if an error other than ERROR_PIPE_BUSY occurs. 
      if ( dwLastError != ERROR_PIPE_BUSY )
      {
        std::ostringstream stdostringstreamMessage ;
        stdostringstreamMessage << "Could not open pipe \""
//          << NAMED_PIPE_NAME_ANSI
          << GetStdString( std::tstring( lptstrPipename) )
          << "\": "
          << ::LocalLanguageMessageFromErrorCodeA(dwLastError) ;
        if( dwLastError ==
          //http://msdn.microsoft.com/en-us/library/aa363858%28VS.85%29.aspx:
          //"If the CreateNamedPipe function was not successfully called on
          //the server prior to this operation, a pipe will not exist and
          //CreateFile will fail with ERROR_FILE_NOT_FOUND."
            ERROR_FILE_NOT_FOUND )
          stdostringstreamMessage << "\nThis probably means that the service "
            "has not been started." ;
        r_stdstrMessage = stdostringstreamMessage.str() ;
        LOGN( r_stdstrMessage);
        return 0;
      }
 
      // All pipe instances are busy, so wait for 20 seconds. 
 
      if ( ! ::WaitNamedPipe(lptstrPipename, 20000)) 
      { 
         dwLastError = ::GetLastError() ;
         LOGN("Could not open pipe:" <<
           ::LocalLanguageMessageFromErrorCodeA(dwLastError) );
         return 0;
      } 
   } while (1) ;
 
   //// The pipe connected; change to message-read mode. 
   //dwMode = PIPE_READMODE_MESSAGE;

   //fSuccess = ::SetNamedPipeHandleState( 
   //   m_handleClientPipe ,    // pipe handle 
   //   & dwMode ,  // new pipe mode 
   //   NULL,     // don't set maximum bytes 
   //   NULL );    // don't set maximum time 
   //if (!fSuccess) 
   //{
   //   LOGN("SetNamedPipeHandleState failed"); 
   //   return 0;
   //}
 
   return 1; 
}

bool NamedPipeClient::IsConnected()
{
  LOGN("IsConnected begin")
  bool bConnected = false ;
//  LOGN("IsConnected before GetNamedPipeHandleState")

  m_criticalsection_typeIOandIsconnected.Enter() ;

  //"m_vbIsReadingOrWriting" is (thread-safe via
  //  m_criticalsection_typeIOandIsconnected)
  // set to true before reading to or writing from the pipe.
  if( m_vbIsReadingOrWriting )
  {
    //Assume it is connected because the probability is high that we were not
    //in the timespan where an unconnected pipe was between begin of
    //::ReadFile(...) and return of ::ReadFile(...).
    //If the pipe server disconnects, also due to an error, ::ReadFile(...)
    // and WriteFile(...) return.
    bConnected = true ;
    LOGN("currently reading from or writing to pipe -> IsConnected==true")
    m_criticalsection_typeIOandIsconnected.Leave() ;
  }
  else
  {
    bConnected = m_bConnected ;
//  bConnected = IsConnectedViaGetNamedPipeHandleState() ;
    //Leave the critical section after calling GetNamedPipeHandleState
    //because else may happen: m_vbIsReadingOrWriting is false, next instruction
    //in other thread is "ReadFile() -> blocks the execution of
    //GetNamedPipeHandleState
    m_criticalsection_typeIOandIsconnected.Leave() ;
    //    LOGN("IsConnected after GetNamedPipeHandleState")
    return bConnected ;
  }
  LOGN("IsConnected end")
  return bConnected ;
}

//void NamedPipeClient::SetDisconnectCallbackFunction(
//  void (* pfnCallback)()  )
//{
//  m_pfnCallback = pfnCallback ;
//}

NamedPipeClient::NamedPipeClient(Model & r_model )
  :
    m_vbIsReadingOrWriting (false) ,
    m_r_model( r_model) ,
    m_vbIsGettingCPUcoreData(false) ,
//      m_vbIsGettingCPUcoreData(true) ,
    m_arbyIPCdata (NULL)
{

}

NamedPipeClient::~NamedPipeClient()
{
  ::CloseHandle(m_handleClientPipe);
  if( m_arbyIPCdata )
  {
    LOGN("freeing IPC data")
    delete [] m_arbyIPCdata ;
  }
  LOGN("end of named pipe client")
}

BOOL NamedPipeClient::Read(
  //__out
  LPVOID lpBuffer,
  //__in
  DWORD nNumberOfBytesToRead,
  //__out_opt
  LPDWORD lpNumberOfBytesRead,
  //__inout_opt
  LPOVERLAPPED lpOverlapped
  )
{
  //If another thread calls "IsConnected"
  m_criticalsection_typeIOandIsconnected.Enter() ;
  m_vbIsReadingOrWriting = true ;
  m_criticalsection_typeIOandIsconnected.Leave() ;
  m_bool =
  //http://msdn.microsoft.com/en-us/library/aa365467%28VS.85%29.aspx:
  // "If the function succeeds, the return value is nonzero (TRUE)."

  //http://msdn.microsoft.com/en-us/library/aa365467%28VS.85%29.aspx:
  // "The ReadFile function returns when one of the following conditions occur:
  //  * The number of bytes requested is read.
  //  * A write operation completes on the write end of the pipe.
  //  * An asynchronous handle is being used and the read is occurring
  //    asynchronously.
  //  * An error occurs.""
  ::ReadFile(
    m_handleClientPipe ,
    lpBuffer,
    nNumberOfBytesToRead,
    lpNumberOfBytesRead,
    lpOverlapped
    ) ;
  m_vbIsReadingOrWriting = false ;

//  if( m_bool )
//  {
//    m_bConnected = true ;
//  }
//  else
//    m_bConnected = false ;

  //IsConnected() may be called concurrently--protect write to variable therefore
  m_criticalsection_typeIOandIsconnected.Enter() ;
  m_bConnected = m_bool ;
  m_criticalsection_typeIOandIsconnected.Leave() ;
  return m_bool ;
}

//void NamedPipeClient::ReadSingleByteLoop()
//{
//  BYTE byValue ;
//  DWORD dwError ;
//  WORD wTotalNumBytesRead = 0 ;
//   DEBUGN("before read from pipe loop")
//   do
//   {
//     DEBUGN("before read from pipe ")
//     // Read the server's response from the pipe.
//     fSuccess =
//       ReadFile(
//       & byValue , //chBuf,    // buffer to receive reply
//       1 ,//BUFSIZE * sizeof(TCHAR),  // size of buffer
//       & dwNumBytesRead,  // number of bytes read
//       NULL // NULL = not overlapped
//       );
//     if ( fSuccess )
//     {
//       DEBUGN("got byte #" << wTotalNumBytesRead << " from pipe: "
//         << (WORD) byValue << byValue )
//       if( dwNumBytesRead > 0 )
//       {
//         wTotalNumBytesRead += dwNumBytesRead ;
//         if( wTotalNumBytesRead % 2 == 0 )
//         {
//           wch |= byValue ;
////           stdwstrMessage += wch ;
//           m_stdwstrMessage += wch ;
//         }
//         else
//         {
////           ((BYTE*) wch) = byValue ;
//           wch = byValue ;
//           wch <<= 8 ;
//         }
//       }
//       else
//       {
//         DEBUGN("no more bytes from pipe")
//         //Got no more bytes-> break loop.
//         break ;
//       }
//     }
//     else
//     {
//       dwError = ::GetLastError() ;
//       if( dwError != ERROR_MORE_DATA )
//       {
//         LOGN( "reading form the pipe failed: " <<
//           LocalLanguageMessageFromErrorCodeA(dwError) )
//         break;
//       }
//     }
//   //   _tprintf( TEXT("%s\n"), chBuf );
//   } while ( //!fSuccess
//       dwNumBytesRead > 0 );  // repeat loop if ERROR_MORE_DATA
//}

//As seen in a log file: while a thread called ::ReadFile() on the pipe handle
// another thread can NOT write concurrently to the same pipe.
BYTE NamedPipeClient::SendCommandAndGetResponse(BYTE byMessage)
{
  LOGN("SendCommandAndGetResponse begin")
  DWORD dwNumberOfBytesWritten ;
  DWORD dwNumBytesRead ;
//  DWORD dwNumberOfBytes ;
//  std::wstring stdwstrMessage ;
//  wchar_t wch ;
  //clear before every pipe write.
  m_stdwstrMessage = L"" ;
  LOGN("before write message to pipe "
#ifdef _DEBUG //because the own logger can only filter strings that matcb
    //full log messages
    << m_handleClientPipe
#endif
    )
  // Send a message to the pipe server.
  BOOL fSuccess = //::WriteFile(
//    m_handleClientPipe        // pipe handle
    Write(
    //lpvMessage,             // message
    //stop_service
    & byMessage
    // (lstrlen(lpvMessage)+1)*sizeof(TCHAR), // message length
    , 1
    , & dwNumberOfBytesWritten   // bytes written
    , NULL );                  // not overlapped
   if ( ! fSuccess )
   {
     m_vbIsGettingCPUcoreData = false ;
#ifdef COMPILE_WITH_LOG
     DWORD dwLastError = ::GetLastError() ;
#endif //#ifdef COMPILE_WITH_LOG
     //http://msdn.microsoft.com/en-us/library/aa365747%28VS.85%29.aspx:
     //"To get extended error information, call the GetLastError function."
      LOGN("WriteFile failed:" << LocalLanguageMessageFromErrorCodeA(
        dwLastError) );
      m_bConnected = false ;
      return 0;
   }
   //TODO Maybe use asynchronous read or write to a avoid blocking thread while
   // waiting for reading from or writing to the pipe

   LOGN("before read message size from pipe "
#ifdef _DEBUG //because the own logger can only filter strings that matcb
     //full log messages
     << m_handleClientPipe
#endif
     )
   fSuccess = //::ReadFile(
     //m_handleClientPipe ,    // pipe handle
     Read(
     // buffer to receive reply
//     & dwNumberOfBytes , //chBuf,
     & m_dwSizeInByte ,
     // size of buffer
     4 ,//BUFSIZE * sizeof(TCHAR),
     & dwNumBytesRead,  // number of bytes read
     NULL // NULL = not overlapped
     );
   LOGN( (fSuccess ? "successfully got " : "failed to read ")
#ifdef _DEBUG //because the own logger can only filter strings that matcb
     //full log messages
      << dwNumBytesRead
#endif
      << " bytes from pipe " //<< //dwNumberOfBytes
#ifdef _DEBUG //because the own logger can only filter strings that match
      << ":" << m_dwSizeInByte
#endif
   )
   if( fSuccess
       // do NOT read 0 bytes! this blocks at ReadFile although the server
       // finished to write 0 bytes
       && //dwNumberOfBytes
       m_dwSizeInByte
     )
   {
//     m_vbIsGettingCPUcoreData = true ;
//     BYTE * arby = new BYTE[dwValue + 2 ] ;
     //This array is accessed later in order to process the data.
     if( m_arbyIPCdata )
       delete [] m_arbyIPCdata ;
     m_arbyIPCdata = new BYTE[ m_dwSizeInByte ] ;
     LOGN("before read "
#ifdef _DEBUG //because the own logger can only filter strings that matcb
    //full log messages
       << m_dwSizeInByte
#endif
       << " bytes from pipe ")
     fSuccess =
//     ::ReadFile(
//      m_handleClientPipe ,    // pipe handle
       Read(
//      arby , //chBuf,    // buffer to receive reply
        m_arbyIPCdata ,
        // size of buffer
  //      dwNumberOfBytes  ,//BUFSIZE * sizeof(TCHAR),
        m_dwSizeInByte ,
        & dwNumBytesRead,  // number of bytes read
        NULL // NULL = not overlapped
        );
     LOGN( (fSuccess ? "successfully got" : "failed to read")
#ifdef _DEBUG //because the own logger can only filter strings that matcb
    //full log messages
        << dwNumBytesRead
#endif
      << " bytes from pipe")
     if( fSuccess )
     {
       m_vbIsGettingCPUcoreData = true ;
       std::string stdstrBytes //( (char *) m_arbyIPCdata, m_dwSizeInByte )
         = format_output_data( m_arbyIPCdata, m_dwSizeInByte, 100) ;
       DEBUGN("data from pipe:" << stdstrBytes ) ;
//       //Term. NULL char
//       arby [dwNumberOfBytes ] = 0 ;
//       arby [dwNumberOfBytes + 1 ] = 0 ;
       m_stdwstrMessage = std::wstring( (wchar_t*) //arby
         m_arbyIPCdata
         //http://www.cplusplus.com/reference/string/string/string/:
         //"Content is initialized to a copy of the string formed by the first
         //n  characters in the array of characters pointed by s."
         , m_dwSizeInByte / 2
         ) ;
//       DEBUGWN_WSPRINTF(L"got message from pipe:%ls", m_stdwstrMessage.c_str() )
     }
     else
       m_vbIsGettingCPUcoreData = false ;
//     delete [] arby ;
   }
   else
     m_vbIsGettingCPUcoreData = false ;
   LOGN("SendCommandAndGetResponse end")
   return 1 ;
}

BOOL NamedPipeClient::Write(
  LPCVOID lpBuffer,
  //__in
  DWORD nNumberOfBytesToWrite,
  //__out_opt
    LPDWORD lpNumberOfBytesWritten,
  //__inout_opt
    LPOVERLAPPED lpOverlapped
  )
{
  //If another thread calls "IsConnected"
  m_criticalsection_typeIOandIsconnected.Enter() ;
  m_vbIsReadingOrWriting = true ;
  m_criticalsection_typeIOandIsconnected.Leave() ;
  m_bool =
  ::WriteFile(
    m_handleClientPipe ,
    lpBuffer,
    nNumberOfBytesToWrite,
    lpNumberOfBytesWritten,
    lpOverlapped
    ) ;
  m_vbIsReadingOrWriting = false ;

  m_criticalsection_typeIOandIsconnected.Enter() ;
  m_bConnected = m_bool ;
  m_criticalsection_typeIOandIsconnected.Leave() ;

  return m_bool ;
}

//BYTE NamedPipeClient::SendCommandAndGetResponse(
//  BYTE [] arbyMessage , WORD wByteSize)
//{
//  DWORD dwNumberOfBytesWritten ;
//  // Send a message to the pipe server.
//  BOOL fSuccess = ::WriteFile(
//    m_handleClientPipe        // pipe handle
//    //lpvMessage,             // message
//    //stop_service
//    , arbyMessage
//    // (lstrlen(lpvMessage)+1)*sizeof(TCHAR), // message length
//    , wByteSize
//    , & dwNumberOfBytesWritten   // bytes written
//    , NULL );                  // not overlapped
//   if ( ! fSuccess )
//   {
//      LOGN("WriteFile failed");
//      m_bConnected = false ;
//      return 0;
//   }
//
//   //do
//   //{
//   //// Read from the pipe.
//
//   //  fSuccess = ::ReadFile(
//   //      m_handleClientPipe ,    // pipe handle
//   //      chBuf,    // buffer to receive reply
//   //      BUFSIZE*sizeof(TCHAR),  // size of buffer
//   //      &cbRead,  // number of bytes read
//   //      NULL);    // not overlapped
//
//   //  if (! fSuccess && ::GetLastError() != ERROR_MORE_DATA)
//   //      break;
//
//   //   _tprintf( TEXT("%s\n"), chBuf );
//   //} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA
//   return 1 ;
//}
