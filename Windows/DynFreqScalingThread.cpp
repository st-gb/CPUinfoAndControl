//the following includes must be here above, else compiler errors
#define _WIN32_DCOM
#define _WIN32_WINNT 0x0501
//typedef __int64 long long ;
//#if defined(COMPILER_1)
//typedef long long long64;
//#elif defined(__CYGWIN32__)
//typedef __int64 long64;
//#else
//#error This compiler is not supported.
//#endif

//Indirectly inclued files use __int64 that is not supported on cygwin.
#  ifndef __int64        /* some versions seem to #define it already */
#    define __int64 long long
#  endif
#if defined(__CYGWIN32__)
  //#define __thiscall Thiscall
  #define __thiscall
  #define __forceinline inline
  #define _MSC_VER 1200
#endif
//#ifndef __GNUC_VA_LIST
//#define __GNUC_VA_LIST
//typedef __builtin_va_list __gnuc_va_list;
//#endif
//#include <stdarg.h>
#include <iostream> //for "cout"
using namespace std;
#include <Windows.h> //for CreateThread
#include "DynFreqScalingThread.hpp"

using namespace Windows_API ;

DWORD WINAPI DynFreqScalingThreadProc(LPVOID lpParameter)
{
  DynFreqScalingThread * cp_dynfreqscalingthread =
    (DynFreqScalingThread * ) lpParameter ;
  if( cp_dynfreqscalingthread )
  {
    cp_dynfreqscalingthread->Entry() ;
    //while(cp_dynfreqscalingthread->m_vbRun ) 
    //{
    //  Sleep(700) ;
    //  MessageBeep( (WORD ) -1 ) ;
    //}
  }
  return 0 ;
}

//int
DWORD DynFreqScalingThread::Run()
{
  DWORD dwRet = ERROR_SUCCESS ;
  DWORD dwThreadId ;
  //Call the base class' method that does the work needed for subclasses that
  //(re-)start a DVFS thread.
  DynFreqScalingThreadBase::Start() ;
  HANDLE handleThread =
    //http://msdn.microsoft.com/en-us/library/ms682453%28VS.85%29.aspx :
    //"If the function fails, the return value is NULL.
    //To get extended error information, call GetLastError."
    ::CreateThread(
    NULL,                   // default security attributes
    0,                      // use default stack size  
    DynFreqScalingThreadProc ,       // thread function name
    this ,          // argument to thread function 
    0,                      // use default creation flags 
    & dwThreadId
    );   // returns the thread identifier 
  if( handleThread )
  {
    LOGN("Creating the DVFS thread succeeded.")
    //http://msdn.microsoft.com/en-us/library/ms686724%28v=VS.85%29.aspx:
    //"When a thread terminates, its thread object is not freed until all open
    //handles to the thread are closed."
    //http://msdn.microsoft.com/en-us/library/ms724211%28v=VS.85%29.aspx:
    //"Closing a thread handle does not terminate the associated thread or remove
    //the thread object."
    //Close the thread handle here (waiting for the end of the thread via
    // WaitForSingleObject() would need another thread->not so good.)
    ::CloseHandle(handleThread ) ;
}
  else
  {
    LOGN("Creating the DVFS thread failed.")
    dwRet = GetLastError() ;
  }
  //wxThread->Run() also returns 0 on success
  return //hThread != 0 ;
    dwRet ;
}

//BYTE
DWORD DynFreqScalingThread::Start()
{
  return Run() ;
}

DynFreqScalingThread::DynFreqScalingThread(
  ICPUcoreUsageGetter * p_icpu
  //, GriffinController * p_pumastatectrl
  , I_CPUcontroller * p_cpucontroller
  , CPUcoreData & r_cpucoredata
  )
  : DynFreqScalingThreadBase(
      p_icpu
      //, GriffinController * p_pumastatectrl
      , p_cpucontroller
      , r_cpucoredata
      )
{
}

//Including <Wbemidl.h and other INDIRECTLY included files makes problems on
//cygwin.
//#ifndef COMPILE_WITHOUT_IWBEMSERVICES
#ifdef COMPILE_WITH_IWBEMSERVICES
#include <comdef.h>
//Prevent indirect inclusion of BaseTsd.h
#define _BASETSD_H_
//for "VC/inlcude/excpt.h" etc.
#define __in
#define __inout
#include <Wbemidl.h> //for IWbemServices, IWbemLocator, ...
#include <conio.h> //getche()
#include <objbase.h> //CoInitializeEx()
//end of: the following includes must be here above, else compiler errors

#include "DynFreqScalingThread.hpp"
#include "UserInterface.hpp"
#include "Controller/ICPUcoreUsageGetter.hpp"
#include <Windows.h> //for ::SetThreadAffinityMask(...) ::CloseHandle(...)
#include <math.h> //for log2()
#include "ModelData/PStates.h"
//#include "stdafx.h"

# pragma comment(lib, "wbemuuid.lib")
//for CoInitializeEx 
# pragma comment(lib, "ole32.lib")

int GetValues(IWbemServices *pSvc,ULONG *ulVal ,int n)
{
	int nError = 0;
	 // Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----

    // For example, get the name of the operating system
    IEnumWbemClassObject* pEnumerator = NULL;
    HRESULT hres = pSvc->ExecQuery(
        bstr_t("WQL"), 
        bstr_t("SELECT * FROM Win32_PerfRawData_PerfOS_Processor"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator);
    
    if (FAILED(hres))
    {
        cout << "Query for operating system name failed."
            << " Error code = 0x" 
            << hex << hres << endl;

        return 1;               // Fxn has failed.
    }

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------
 
    IWbemClassObject *pclsObj;
    ULONG uReturn = 0;
	  int nCtr = 0;
  
    while (nCtr<n)
    {
      HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, 
          &pclsObj, &uReturn);

      if(0 == uReturn)
      {
          break;
      }

      VARIANT vtProp;
      VariantInit(&vtProp);

		  hr = pclsObj->Get(L"PercentProcessorTime", 0, &vtProp, 0, 0);
  //		ulVal[nCtr] = _wtol(vtProp.bstrVal);
      //http://www.codeproject.com/KB/system/MultiCPUUsage.aspx: "The problem is that he is using wtol which converts to a long and there is an overflow so it is setting to the max of a long every time. If you change _wtol to _wtof it will work. "
		  ulVal[nCtr] = _wtof(vtProp.bstrVal);
          VariantClear(&vtProp);
      //DEBUG("PercentProcessorTime: %lu\n",ulVal[nCtr]);
      LOG("PercentProcessorTime: " << ulVal[nCtr] << "\n");
	  hr = pclsObj->Get(L"TimeStamp_Sys100NS", 0, &vtProp, 0, 0);
		  //ulVal[nCtr+1] = _wtol(vtProp.bstrVal);
      //http://www.codeproject.com/KB/system/MultiCPUUsage.aspx: "The problem is that he is using wtol which converts to a long and there is an overflow so it is setting to the max of a long every time. If you change _wtol to _wtof it will work. "
		  ulVal[nCtr+1] = _wtof(vtProp.bstrVal);
		  VariantClear(&vtProp);
		  nCtr+=2;
      pclsObj->Release();
    }
	pEnumerator->Release();
	return nError;
}

int Init()
{
  return 0 ;
}

int InitExecRelease(float arfCPUcorePercentage [] )
{
    HRESULT hres;
    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------
	int nproc=1;
    hres =  CoInitializeEx(0, COINIT_MULTITHREADED); 
    if (FAILED(hres))
    {
        //DEBUG ("Failed to initialize COM library. Error code = 0x" 
        //    "%x\n",hres);
        LOG ("Failed to initialize COM library. Error code = "//0x" 
            << hres << "\n");
        return 1;                  // Program has failed.
    }

	//cout << "Please enter the # of processors : ";
	//cin>>nproc;
    nproc = 1;
	ULONG *ulinitVal = new ULONG[(nproc+1)*2];//+1 for the total
	int i=0;
	ULONG *ulVal = new ULONG[(nproc+1)*2];//+1 for the total
    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------
    // Note: If you are using Windows 2000, you need to specify -
    // the default authentication credentials for a user by using
    // a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
    // parameter of CoInitializeSecurity ------------------------

    hres =  CoInitializeSecurity(
        NULL, 
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
        );
                      
    if (FAILED(hres))
    {
        LOG( "Failed to initialize security. Error code = "//0x" 
            << hres << "\n" );
        //DEBUG( "Failed to initialize security. Error code = 0x" 
        //    "%x\n",hres);
        switch(hres)
        {
        case S_OK:
          DEBUG("error code means: success\n");
          break;
        case RPC_E_TOO_LATE :
          DEBUG("error code means: CoInitializeSecurity has already been called.\n");
          //CoUninitialize();
          //hres =  CoInitializeSecurity(
          //    NULL, 
          //    -1,                          // COM authentication
          //    NULL,                        // Authentication services
          //    NULL,                        // Reserved
          //    RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
          //    RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
          //    NULL,                        // Authentication info
          //    EOAC_NONE,                   // Additional capabilities 
          //    NULL                         // Reserved
          //    );
          //DEBUG( "initialize security return code = 0x" 
          //  "%x\n",hres);
          LOG( "initialize security return code = " //0x" 
            << hres << "\n" );
          break;
        case RPC_E_NO_GOOD_SECURITY_PACKAGES :
          DEBUG("error code means: asAuthSvc was not NULL, and none of the authentication services in the list could be registered. Check the results saved in asAuthSvc for authentication service�specific error codes. \n");
          break;
        //case E_OUT_OF_MEMORY:
        //  DEBUG("error code means: Out of memory.\n");
        //  break;
        }
        if( hres != RPC_E_TOO_LATE )
        {
          CoUninitialize();
          return 1;                    // Program has failed.
        }
    }
    else
      DEBUG( "CoInitializeSecurity succ.\n");
    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------

    IWbemLocator *pLoc = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &pLoc);
 
    //DEBUG("result of CoCreateInstance: %x\n",hres);
    LOG("result of CoCreateInstance: " << hres << "\n" );
    if (FAILED(hres))
    {
        //DEBUG( "Failed to create IWbemLocator object."
        //    " Err code = 0x%x\n", hres );
        LOG( "Failed to create IWbemLocator object."
            " Err code = "//0x" 
            << hres << "\n" );
        CoUninitialize();
        return 1;                 // Program has failed.
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    IWbemServices *pSvc = NULL;
	
    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = pLoc->ConnectServer(
         _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
         NULL,                    // User name. NULL = current user
         NULL,                    // User password. NULL = current
         0,                       // Locale. NULL indicates current
         NULL,                    // Security flags.
         0,                       // Authority (e.g. Kerberos)
         0,                       // Context object 
         &pSvc                    // pointer to IWbemServices proxy
         );
    
    if (FAILED(hres))
    {
        cout << "Could not connect. Error code = 0x" 
             << hex << hres << endl;
        pLoc->Release();     
        CoUninitialize();
        return 1;                // Program has failed.
    }

    DEBUG ( "Connected to ROOT\\CIMV2 WMI namespace\n" );


    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------

    hres = CoSetProxyBlanket(
       pSvc,                        // Indicates the proxy to set
       RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
       RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
       NULL,                        // Server principal name 
       RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
       RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
       NULL,                        // client identity
       EOAC_NONE                    // proxy capabilities 
    );

    if (FAILED(hres))
    {
        cout << "Could not set proxy blanket. Error code = 0x" 
            << hex << hres << endl;
        pSvc->Release();
        pLoc->Release();     
        CoUninitialize();
        return 1;               // Program has failed.
    }
	
	char szVal[64];
	
	//while(i++<1000)
  //while(1)
	{
		//Get the initial Values
		if(GetValues(pSvc,ulinitVal,(nproc+1)*2))
		{
			delete ulinitVal;
			goto Cleanup;
		}

    //This sleep is essential! if not sleep the CPU time is always "100%".
		Sleep(1000);

		//Get the initial Values
		GetValues(pSvc,ulVal,(nproc+1)*2);

    float fPercentage ;
		if((ulVal[1]- ulinitVal[1])>0)
		{
      fPercentage = fabs(100.0- ((float)(ulVal[0] - ulinitVal[0]) / 
        (ulVal[1]- ulinitVal[1])) * 100) ;
			sprintf(szVal,"%0.2f",fPercentage);
      arfCPUcorePercentage[0] = fPercentage ;
			//DEBUG ("PercentProcessorTime CPU 0: \t\t%s", szVal );
			LOG ("PercentProcessorTime CPU 0: \t\t" << szVal );
		}
		else
		{
			cout<<"Divide by zero for Processor 0"<<endl;
		}

		if(nproc>1)
		{
			if((ulVal[3]- ulinitVal[3])>0)
			{
        fPercentage = fabs(100.0- ((float)(ulVal[2] - ulinitVal[2]) / 
          (ulVal[3]- ulinitVal[3])) * 100) ;
				sprintf(szVal,"%0.2f",fPercentage);
        arfCPUcorePercentage[1] = fPercentage ;
				cout<< "PercentProcessorTime CPU 1:\t\t" <<szVal<<endl;
			}
			else
			{
				cout<<"Divide by zero for Processor 1"<<endl;
			}		
			//if((ulVal[5]- ulinitVal[5])>0)
			//{
			//	sprintf(szVal,"%0.2f",fabs(100.0- ((float)(ulVal[4] - 
   //       ulinitVal[4]) / (ulVal[5]- ulinitVal[5])) * 100));	
			//	cout<< "PercentProcessorTime  CPU Total:\t" <<szVal<<endl;
			//}
			//else
			//{
			//	cout<<"Divide by zero for total"<<endl;
			//}
		}
		else
		{
			if((ulVal[3]- ulinitVal[3])>0)
			{
				sprintf(szVal,"%0.2f",fabs(100.0-((float)(ulVal[2] - ulinitVal[2]) / (ulVal[3]- ulinitVal[3])) * 100));
				cout<< "PercentProcessorTime  CPU Total:\t" << szVal<<endl;
			}
			else
			{
				cout<<"Divide by zero for total"<<endl;
			}
		}
	}
	delete ulinitVal;
	delete ulVal;

Cleanup:
   
    // Cleanup
    // ========
    
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();

	getchar();
    return 0;   // Program successfully completed.
}

void Windows_API::DynFreqScalingThread::SetMembers(
  ICPUcoreUsageGetter * p_icpucoreusagegetter,
  //PumaStateCtrl * p_pumastatectrl
  GriffinController * p_pumastatectrl)
{
  mp_icpucoreusagegetter = p_icpucoreusagegetter ;
  mp_pumastatectrl = p_pumastatectrl ;
}

void Windows_API::DynFreqScalingThread::Start()
{
  DWORD dwThreadID ;
  //mp_icpucoreusagegetter->Init();
  HANDLE m_hThread = 
    //If the function fails, the return value is NULL. 
    ::CreateThread(
    //LPSECURITY_ATTRIBUTES lpThreadAttributes,
    //"If lpThreadAttributes is NULL, the thread gets a default security descriptor."
    NULL,
    //SIZE_T dwStackSize,
    //If this parameter is zero, the new thread uses the default size for the executable.
    0,
    //LPTHREAD_START_ROUTINE lpStartAddress,
    DynFreqScalingThreadProc,
    //LPVOID lpParameter,
    //NULL,
    this,
    //DWORD dwCreationFlags,
    //If this value is zero, the thread runs immediately after creation. 
    0,
    //LPDWORD lpThreadId
    &dwThreadID
    );
}
#endif //#ifndef COMPILE_WITHOUT_IWBEMSERVICES
