#ifdef COMPILE_WITH_SERVICE_CAPABILITY

#include "../global.h" //for DEBUG(...) etc.

//for running as a Windows service, begin
SERVICE_STATUS          MyServiceStatus; 
SERVICE_STATUS_HANDLE   MyServiceStatusHandle; 
//for running as a Windows service, end

// Stub initialization function. 
DWORD MyServiceInitialization(DWORD   argc, LPTSTR  *argv, 
    DWORD *specificError) 
{ 
    argv; 
    argc; 
    specificError; 
    return(0); 
}

//The MyServiceCtrlHandler function in the following example is the Handler function. When this function is called by the dispatcher thread, it handles the control code passed in the Opcode parameter and then calls the SetServiceStatus function to update the service's status. Every time a Handler function receives a control code, it is appropriate to return status with a call to SetServiceStatus regardless of whether the service acts on the control.
VOID WINAPI MyServiceCtrlHandler (DWORD Opcode) 
{ 
   DWORD status; 
 
   switch(Opcode) 
   { 
      case SERVICE_CONTROL_PAUSE: 
      // Do whatever it takes to pause here. 
         MyServiceStatus.dwCurrentState = SERVICE_PAUSED; 
         break; 
 
      case SERVICE_CONTROL_CONTINUE: 
      // Do whatever it takes to continue here. 
         MyServiceStatus.dwCurrentState = SERVICE_RUNNING; 
         break; 
 
      case SERVICE_CONTROL_STOP: 
      // Do whatever it takes to stop here. 
         MyServiceStatus.dwWin32ExitCode = 0; 
         MyServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
         MyServiceStatus.dwCheckPoint    = 0; 
         MyServiceStatus.dwWaitHint      = 0; 

         if (!SetServiceStatus (MyServiceStatusHandle, 
           &MyServiceStatus))
         { 
            status = GetLastError(); 
            DEBUG(" [MY_SERVICE] SetServiceStatus error %ld\n", 
               status); 
         } 
 
         DEBUG(" [MY_SERVICE] Leaving MyService \n",0); 
         return; 
 
      case SERVICE_CONTROL_INTERROGATE: 
      // Fall through to send current status. 
         break; 
 
      default: 
         DEBUG(" [MY_SERVICE] Unrecognized opcode %ld\n", 
             Opcode); 
   } 
 
   // Send current status. 
   if (!SetServiceStatus (MyServiceStatusHandle,  &MyServiceStatus)) 
   { 
      status = GetLastError(); 
      DEBUG(" [MY_SERVICE] SetServiceStatus error %ld\n", 
         status); 
   } 
   return; 
}

//"The MyServiceStart function in the following example is the ServiceMain function for the service. MyServiceStart has access to the command-line arguments, in the way that the main function of a console application does. The first parameter contains the number of arguments being passed to the service. There will always be at least one argument. The second parameter is a pointer to an array of string pointers. The first item in the array always points to the service name."
void WINAPI MyServiceStart (DWORD argc, LPTSTR *argv) 
{ 
  DWORD status; 
  DWORD specificError; 
  DEBUG("MyServiceStart--begin\n");
  //The MyServiceStart function first fills in the SERVICE_STATUS structure including the control codes that it accepts.
  MyServiceStatus.dwServiceType        = SERVICE_WIN32; 
  MyServiceStatus.dwCurrentState       = SERVICE_START_PENDING; 
  MyServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | 
      SERVICE_ACCEPT_PAUSE_CONTINUE; 
  MyServiceStatus.dwWin32ExitCode      = 0; 
  MyServiceStatus.dwServiceSpecificExitCode = 0; 
  MyServiceStatus.dwCheckPoint         = 0; 
  MyServiceStatus.dwWaitHint           = 0; 

  //The MyServiceStart function then calls the RegisterServiceCtrlHandler function to register MyService as the service's Handler function and begin initialization. The following sample initialization function, MyServiceInitialization, is included for illustration purposes; it does not perform any initialization tasks such as creating additional threads. If your service's initialization performs tasks that are expected to take longer than one second, your code must call the SetServiceStatus function periodically to send out wait hints and check points indicating that progress is being made.
  MyServiceStatusHandle = RegisterServiceCtrlHandler( 
      "MyService", 
      MyServiceCtrlHandler); 

  if (
    //If the function fails, the return value is zero.
    MyServiceStatusHandle == (SERVICE_STATUS_HANDLE)0) 
  { 
    DWORD dwErrorCode = ::GetLastError() ;
    DEBUG(" [MY_SERVICE] RegisterServiceCtrlHandler "
      "failed: %lu\n", dwErrorCode ); 
    switch(//(DWORD)MyServiceStatusHandle
      dwErrorCode )
    {
    case ERROR_INVALID_NAME :
      DEBUG("The specified service name is invalid.\n");
      break;
    case ERROR_SERVICE_DOES_NOT_EXIST:
      DEBUG("The specified service does not exist.\n");
      break;
    default:
      DEBUG("error code was probably be set by the registry functions that are called by the service control manager.\n");

    }
    return; 
  }
  else
    DEBUG("RegisterServiceCtrlHandler succ.\n");

  // Initialization code goes here. 
  status = MyServiceInitialization(argc,argv, &specificError); 

  // Handle error condition 
  if (status != NO_ERROR) 
  //If an error has occurred in initialization, MyServiceStart reports SERVICE_STOPPED with the SetServiceStatus function and returns.
  {
    DEBUG("MyServiceInitialization succ.\n");
      MyServiceStatus.dwCurrentState       = SERVICE_STOPPED; 
      MyServiceStatus.dwCheckPoint         = 0; 
      MyServiceStatus.dwWaitHint           = 0; 
      MyServiceStatus.dwWin32ExitCode      = status; 
      MyServiceStatus.dwServiceSpecificExitCode = specificError; 

      SetServiceStatus (MyServiceStatusHandle, &MyServiceStatus); 
      return; 
  } 

  // Initialization complete - report running status. 
  //When initialization has completed successfully, the example calls SetServiceStatus with a status of SERVICE_RUNNING and the service continues with its work.
  MyServiceStatus.dwCurrentState       = SERVICE_RUNNING; 
  MyServiceStatus.dwCheckPoint         = 0; 
  MyServiceStatus.dwWaitHint           = 0; 

  if (!SetServiceStatus (MyServiceStatusHandle, &MyServiceStatus)) 
  { 
      status = GetLastError(); 
      DEBUG(" [MY_SERVICE] SetServiceStatus error"
          "%ld\n",status); 
  } 

  // This is where the service does its work. 
  DEBUG(" [MY_SERVICE] Returning the Main Thread \n",0); 

  //Because this sample service does not complete any real tasks, MyServiceStart simply returns control to the caller. However, your service should use this thread to complete whatever tasks it was designed to do. 
  // It is important for the function to return, rather than call the ExitThread function, because returning allows for cleanup of the memory allocated for the arguments.
  //You can provide for additional cleanup by calling the RegisterWaitForSingleObject function on an event before returning. The thread that is running the ServiceMain function terminates, but the service itself continues to run.
  return; 
} 
#endif //#ifdef COMPILE_WITH_SERVICE_CAPABILITY
