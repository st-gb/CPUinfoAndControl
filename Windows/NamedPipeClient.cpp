#include "NamedPipeClient.hpp"
#include "global.h" //LOGN
#include <windows.h>
#include <Windows/LocalLanguageMessageFromErrorCode.h>

//ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/ipc/base/named_pipe_client.htm

#define BUFSIZE 512

//NamedPipeClient::NamedPipeClient(
//  LPTSTR lpszPipename //= TEXT("\\\\.\\pipe\\mynamedpipe"); 
//  )
BYTE NamedPipeClient::Init()
{
  m_bConnected = false ;
  LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\CPUcontrollerService") ;
  //LPTSTR lpvMessage=TEXT("Default message from client"); 
  TCHAR chBuf[BUFSIZE]; 
  BOOL fSuccess; 
  DWORD cbRead, dwMode; 
  DWORD dwLastError ;
 
  // Try to open a named pipe; wait for it, if necessary. 
  while (1) 
  { 
    m_handleClientPipe = ::CreateFile( 
      lpszPipename,   // pipe name 
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
        // Break if the pipe handle is valid. 
        break;
      }
 
      dwLastError = ::GetLastError() ;
      // Exit if an error other than ERROR_PIPE_BUSY occurs. 
      if ( ::GetLastError() != ERROR_PIPE_BUSY ) 
      {
        LOGN("Could not open pipe:" << 
          ::LocalLanguageMessageFromErrorCode(dwLastError) ); 
         return 0;
      }
 
      // All pipe instances are busy, so wait for 20 seconds. 
 
      if ( ! ::WaitNamedPipe(lpszPipename, 20000)) 
      { 
         LOGN("Could not open pipe"); 
         return 0;
      } 
   } 
 
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

//void NamedPipeClient::SetDisconnectCallbackFunction(
//  void (* pfnCallback)()  )
//{
//  m_pfnCallback = pfnCallback ;
//}

BYTE NamedPipeClient::SendMessage(BYTE byMessage)
{
  DWORD dwNumberOfBytesWritten ;
  // Send a message to the pipe server. 
  BOOL fSuccess = ::WriteFile( 
    m_handleClientPipe        // pipe handle 
    //lpvMessage,             // message 
    //stop_service
    , & byMessage
    // (lstrlen(lpvMessage)+1)*sizeof(TCHAR), // message length 
    , 1
    , & dwNumberOfBytesWritten   // bytes written 
    , NULL );                  // not overlapped 
   if (!fSuccess) 
   {
      LOGN("WriteFile failed"); 
      m_bConnected = false ;
      return 0;
   }
 
   //do 
   //{ 
   //// Read from the pipe. 
 
   //  fSuccess = ::ReadFile( 
   //      m_handleClientPipe ,    // pipe handle 
   //      chBuf,    // buffer to receive reply 
   //      BUFSIZE*sizeof(TCHAR),  // size of buffer 
   //      &cbRead,  // number of bytes read 
   //      NULL);    // not overlapped 
 
   //  if (! fSuccess && ::GetLastError() != ERROR_MORE_DATA) 
   //      break; 
 
   //   _tprintf( TEXT("%s\n"), chBuf ); 
   //} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 
   return 1 ;
}
