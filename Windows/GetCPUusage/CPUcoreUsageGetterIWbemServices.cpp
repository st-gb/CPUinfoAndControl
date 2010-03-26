//#ifndef COMPILE_WITHOUT_IWBEMSERVICES
#ifdef COMPILE_WITH_IWBEMSERVICES

#define TIMESTAMP_SYS100NS_INDEX 1

#include "CPUcoreUsageGetterIWbemServices.hpp"

//MSDN:"You must include the #define _WIN32_DCOM preprocessor directive at the beginning of your code to be able to use CoInitializeEx."
#define _WIN32_DCOM 
  //float 
  BYTE CPUcoreUsageGetterIWbemServices::GetPercentalUsageForBothCores(
    float r_arf[] )
  {
    BYTE byReturn = 0 ;
    DEBUG("Ccugiwb GetPercentalUsageForBothCores begin\n");
    if( m_bSuccesfullyInitialized )
    { 
		  //Get the initial values
		  if( GetValues(//mp_iwbemservices,
        m_ar_ulinitVal,(m_wNumberOfCPUCores + 1) * 2 ))
      {
        DEBUG("CPU usage--GetValues() error\n");
			  //delete ulinitVal;
			  //goto Cleanup;
        //Cleanup();
        //return 1.0;
      }
      else
		  {
        ////Just for testing:
        //ulinitVal[0] = 50.0 ;
        //ulinitVal[1] = 100.0 ;
        //m_arulong[0] = 50.0 ;
        //m_arulong[1] = 200.0 ;
        //ulinitVal[2] = 50.0 ;
        //ulinitVal[3] = 100.0 ;
        //m_arulong[3] = 50.0 ;
        //m_arulong[1] = 200.0 ;
        //This sleep is essential! if not sleep the CPU time is always "100%".
        //->Value of parameter inside Sleep() must be at least "1"
        ::Sleep(m_dwMilliSecondsToSleepBetweenGetWMIdata);

		    //Get the initial Values
		    if(GetValues(//mp_iwbemservices,
          m_arulong,(m_wNumberOfCPUCores+1)*2))
        {
          DEBUG("CPU usage--GetValues() error\n");
        }
        else
        {
		      if((m_arulong[1]- m_ar_ulinitVal[1])>0)
		      {
            r_arf[0] = fabs(100.0- ((float)(m_arulong[0] - m_ar_ulinitVal[0]) /
              (m_arulong[1]- m_ar_ulinitVal[1])) * 100) ;
            //DEBUG("GetPercentalUsageForBothCores() %f\n", r_arf[0]);
            LOG("GetPercentalUsageForBothCores() " << r_arf[0] << "\n" );
			      //sprintf(szVal,"%0.2f",);
			      //cout<< "PercentProcessorTime CPU 0: \t\t" <<szVal<<endl;
		      }
		      else
		      {
			      DEBUG("Divide by zero for Processor 0\n");
		      }

		      //if(m_wNumberOfCPUCores>1)
		      {
			      if((m_arulong[3]- m_ar_ulinitVal[3])>0)
			      {
              r_arf[1] = fabs(100.0- ((float)(m_arulong[2] - m_ar_ulinitVal[2]) /
                (m_arulong[3]- m_ar_ulinitVal[3])) * 100) ;
				      //sprintf(szVal,"%0.2f",);
				      //cout<< "PercentProcessorTime CPU 1:\t\t" <<szVal<<endl;
			      }
			      else
			      {
				      //cout<<"Divide by zero for Processor 1"<<endl;
			      }
      		
			      //if((m_arulong[5]- ulinitVal[5])>0)
			      //{
			      //	sprintf(szVal,"%0.2f",fabs(100.0- ((float)(m_arulong[4] - ulinitVal[4]) / (m_arulong[5]- ulinitVal[5])) * 100));	
			      //	cout<< "PercentProcessorTime  CPU Total:\t" <<szVal<<endl;
			      //}
			      //else
			      //{
			      //	cout<<"Divide by zero for total"<<endl;
			      //}
		      }
		      //else
		      //{
		      //	if((m_arulong[3]- ulinitVal[3])>0)
		      //	{
		      //		sprintf(szVal,"%0.2f",fabs(100.0-((float)(m_arulong[2] - ulinitVal[2]) / (m_arulong[3]- ulinitVal[3])) * 100));
		      //		cout<< "PercentProcessorTime  CPU Total:\t" << szVal<<endl;
		      //	}
		      //	else
		      //	{
		      //		cout<<"Divide by zero for total"<<endl;
		      //	}
		      //}
          byReturn = SUCCESS ;
        }
      }
    }
    else
      DEBUG("Doing nothing because: getting CPU usage not succ. initialized\n");
    DEBUG("GetPercentalUsageForBothCores end\n");
    //return 1.0 ;
    return byReturn ;
  }

  BYTE CPUcoreUsageGetterIWbemServices::GetPercentalUsageForAllCores(
    float r_arf[]
    )
  {
    BYTE byReturn = 0 ;
    DEBUG("Ccugiwb GetPercentalUsageForBothCores begin\n");
    if( m_bSuccesfullyInitialized )
    {
		  //Get the initial Values
		  if( GetValues(//mp_iwbemservices,
        m_ar_ulinitVal,( m_wNumberOfCPUCores + 1 ) * 2 ))
      {
        DEBUG("CPU usage--GetValues() error\n");
			  //delete ulinitVal;
			  //goto Cleanup;
        //Cleanup();
        //return 1.0;
      }
      else
		  {
        ////Just for testing:
        //ulinitVal[0] = 50.0 ;
        //ulinitVal[1] = 100.0 ;
        //m_arulong[0] = 50.0 ;
        //m_arulong[1] = 200.0 ;
        //ulinitVal[2] = 50.0 ;
        //ulinitVal[3] = 100.0 ;
        //m_arulong[3] = 50.0 ;
        //m_arulong[1] = 200.0 ;
        //This sleep is essential! if not sleep the CPU time is always "100%".
        //->Value of parameter inside Sleep() must be at least "1"
        ::Sleep(m_dwMilliSecondsToSleepBetweenGetWMIdata);

		    //Get the initial Values
		    if( GetValues(//mp_iwbemservices,
          m_arulong, (m_wNumberOfCPUCores +
          //+ 1 for overall CPU performance data
          1 ) *
          //2 elements per CPU: %CPU time and time stamp
          2 ))
        {
          DEBUG("CPU usage--GetValues() error\n");
        }
        else
        {
          double dPercentProcessorTimeDiffDivTimeDiff ;
          ULONG ulongTimestampAfter ;
          ULONG ulongTimeDiff ;
          ULONG ulongTimestampBefore ;
          ULONG ulongCPUpercentBefore ;
          ULONG ulongCPUpercentAfter ;
          ULONG ulongPercentProcessorTimeDiff ;
          WORD wPerDataElementIndex = 0 ;
          for( BYTE byCPUindex = 0 ; byCPUindex < m_wNumberOfCPUCores ;
            ++byCPUindex 
             )
          {
            wPerDataElementIndex = byCPUindex * 2 + TIMESTAMP_SYS100NS_INDEX ;
            ulongTimestampBefore = m_ar_ulinitVal[ wPerDataElementIndex ] ;
            ulongTimestampAfter = m_arulong[ wPerDataElementIndex ] ;
            ulongTimeDiff = ulongTimestampAfter - ulongTimestampBefore ;
            if( //( m_arulong[ wPerDataElementIndex ] -
                 //m_ar_ulinitVal[ wPerDataElementIndex ] ) 
                 ulongTimeDiff > 0
              )
            {
              ulongCPUpercentBefore = m_ar_ulinitVal[ byCPUindex * 2 ] ;
              ulongCPUpercentAfter = m_arulong[ byCPUindex * 2 ] ;
              ulongPercentProcessorTimeDiff = ulongCPUpercentAfter - 
                ulongCPUpercentBefore ;
              dPercentProcessorTimeDiffDivTimeDiff = 
                (double) ulongPercentProcessorTimeDiff
                /
                (double) ulongTimeDiff ;
              r_arf[ byCPUindex ] = fabs( 
                100.0 - ((float)
                //(
                //m_arulong[ byCPUindex * 2 ] -
                //m_ar_ulinitVal[ byCPUindex * 2 ]
                //ulongCPUpercentAfter - ulongCPUpercentBefore
                //ulongPercentProcessorTimeDiff
                //)
                // /
                //(m_arulong[ wPerDataElementIndex ] -
                //m_ar_ulinitVal[ wPerDataElementIndex ])
                //ulongTimeDiff 
                dPercentProcessorTimeDiffDivTimeDiff
                ) * 100
                ) ;
              //DEBUG("GetPercentalUsageForBothCores() %f\n", r_arf[0]);
              LOG("GetPercentalUsageForBothCores() " << r_arf[0] << "\n" );
              //sprintf(szVal,"%0.2f",);
              //cout<< "PercentProcessorTime CPU 0: \t\t" <<szVal<<endl;
            }
            else
            {
              DEBUG("WMI: timespan between measuring CPU % is 0\n");
            }
          }
          byReturn = SUCCESS ;
		    }
      }
    }
    else
      DEBUG("Doing nothing because: getting CPU usage not succ. initialized\n");
    DEBUG("GetPercentalUsageForBothCores end\n");
    //return 1.0 ;
    return byReturn ;
  }

int CPUcoreUsageGetterIWbemServices::GetValues(//IWbemServices *pSvc,
    ULONG ar_ulongPercentCPUTime [],
    int nNumberOfArrayElements
    )
  {
	  int nError = 0;
    DEBUG("ccugiwb usage-GV begin\n");
    //
	 //  // Step 6: --------------------------------------------------
  //    // Use the IWbemServices pointer to make requests of WMI ----
  //  // For example, get the name of the operating system
    //IEnumWbemClassObject * p_ienumwbemclassobject = NULL;
    mp_ienumwbemclassobject = NULL ;
    //HRESULT hres = pSvc->ExecQuery(
    m_hresult = mp_iwbemservices->ExecQuery(
      //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/wmisdk/wmi/iwbemservices_execquery.htm:
      //"This must be "WQL", the acronym for WMI Query Language."
      bstr_t("WQL"), 
      bstr_t("SELECT * FROM Win32_PerfRawData_PerfOS_Processor"),
      //"Forward-only enumerators are generally much faster and use less 
      //memory than conventional enumerators but do not allow calls to 
      //Clone or Reset."
      WBEM_FLAG_FORWARD_ONLY | 
      //"This flag causes this to be a semisynchronous call. "
      WBEM_FLAG_RETURN_IMMEDIATELY, 
      NULL, //IWbemContext * pCtx; Typically NULL.
      //IEnumWbemClassObject** ppEnum
      &mp_ienumwbemclassobject
      );
    
    //DEBUG("ExecQuery's hresult:%d\n",hresult);
    LOG( "ExecQuery's hresult:" << m_hresult << "\n" );
    if (FAILED(m_hresult))
    {
      //cout << "Query for operating system name failed."
      //    << " Error code = 0x" 
      //    << hex << hres << endl;
      DEBUG("ccugiwb usage-GetValues 28.02.2009.16:52\n");
      return 1;               // Fxn has failed.
    }

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------
    //IWbemClassObject * p_iwbemclassobject;
    ULONG uReturn = 0;
    int nCurrentArrayIndex = 0;
    //DEBUG("cpu usage-GetValues #array eles:%d\n",nNumberOfArrayElements);
    LOG( "cpu usage-GetValues #array eles:" << nNumberOfArrayElements 
        << "\n" );
  
    m_hresult = WBEM_S_NO_ERROR ;
    //TODO some sourcecode causes loss of RAM (memory leak?!).
    //I think its because of "p_ienumwbemclassobject" is not deleted
    while ( nCurrentArrayIndex < nNumberOfArrayElements 
      && mp_ienumwbemclassobject
      //Used to check whether reached the end of the enumeration
      && m_hresult == WBEM_S_NO_ERROR
      )
    {
      //DEBUG("ccugiwb usage-GetValues current ele:%d\n",nCurrentArrayIndex);
      LOG("ccugiwb usage-GetValues current ele:" << nCurrentArrayIndex << 
          "\n" );
      //IWbemClassObject * arp_iwbemclassobject[1];
      //IWbemClassObject * p_iwbemclassobject;
      //p_ienumwbemclassobject->BeginEnumeration(WBEM_FLAG_SYSTEM_ONLY);
      //HRESULT hr = p_ienumwbemclassobject->Next(
      //TODO error here:
      m_hresult = mp_ienumwbemclassobject->Next(
        //TODO : idea: replace WBEM_INFINITE by a ~1 second timeout.
        //maximum amount of time in milliseconds that the call blocks before returning
        //The thread hung at this method call. This seems to be so beause of the
        //"WBEM_INFINITE".
        //WBEM_INFINITE, 
        1000,
        1, //uCount 
        //[out] Pointer to enough storage to hold the number of IWbemClassObject interface pointers specified by uCount. This storage must be supplied by the caller. This parameter cannot be NULL. The caller must call Release on each of the received interface pointers when they are no longer needed.
        //=array of pointers?!
        & mp_iwbemclassobject,
        //&&iwbemclassobject, 
        //arp_iwbemclassobject,
        & uReturn);
      //DEBUG("ccugiwb usage-GetValues Next's hresult:%d\n",hresult);
      LOG( "ccugiwb usage-GetValues Next's hresult:" << m_hresult << "\n" );
      //because this program hung at the above instruction 
      //"p_ienumwbemclassobject->Next(...)" with a timeout of 
      //"WBEM_INFINITE" as parameter for the 1st time this method was 
      //executed I excpect that there will be a timeout sometimes.
      //As a reaction to this timeout I want to call the method again.
      if(m_hresult == WBEM_S_TIMEDOUT )
      {
        DEBUG("Getting objects from WMI enumeration timed out. Trying again.\n");
        m_hresult = mp_ienumwbemclassobject->Next(
          //TODO : idea: replace WBEM_INFINITE by a ~1 second timeout.
          //maximum amount of time in milliseconds that the call blocks before returning
          //WBEM_INFINITE, 
          1000,
          1, //uCount 
          //[out] Pointer to enough storage to hold the number of IWbemClassObject interface pointers specified by uCount. This storage must be supplied by the caller. This parameter cannot be NULL. The caller must call Release on each of the received interface pointers when they are no longer needed.
          //=array of pointers?!
          & mp_iwbemclassobject,
          //&&iwbemclassobject, 
          //arp_iwbemclassobject,
          & uReturn
          );
      }
      //This error appeared after a resume from standby (because the CPU
        //was at 100% load with also other tasks?) 
      if( m_hresult != WBEM_S_NO_ERROR )
        //TODO it's better when this message disappears after it succeeded
        //the next time.
        ShowMessage("Error getting the CPU core load");
      //p_ienumwbemclassobject->
      if( uReturn == 0 )
      {
        DEBUG("Didn't get any WMI enumeration objects\n");
        nError = IEnumWbemClassObject_NextReturnedNoObject ;
        break;
      }

      //VARIANT variant
      //  //, variant2
      //  ;
      //"The VariantInit function initializes the VARIANTARG by setting the vt member to VT_EMPTY." 
      //"Use VariantInit to initialize new local variables of type VARIANTARG (or VARIANT)."
      //VariantInit(&variant);
      VariantInit(& m_variant);
      DEBUG("cpu usage-GetValues 2009 02 28 15:38:40\n");

	    //hr = p_iwbemclassobject->Get(L"PercentProcessorTime", 0, &variant, 0, 0);
	    m_hresult = mp_iwbemclassobject->Get( L"PercentProcessorTime", 0,
        & m_variant, 0, 0);
      //DEBUG("cpu usage-GetValues percent proc time:%s\n",m_variant.bstrVal);
      LOG( "cpu usage-GetValues percent proc time:" << m_variant.bstrVal 
          << "\n" );
  //		arulong[nCurrentArrayIndex] = _wtol(variant.bstrVal);
      //http://www.codeproject.com/KB/system/MultiCPUUsage.aspx:
      //"The problem is that he is using wtol which converts to a long and
      //there is an overflow so it is setting to the max of a long every time.
      //If you change _wtol to _wtof it will work. "
	    //arulong[nCurrentArrayIndex] = _wtof(variant.bstrVal);
	    ar_ulongPercentCPUTime[nCurrentArrayIndex] = _wtof(m_variant.bstrVal);
      //DEBUG("PercentProcessorTime: %lu\n",arulong[nCurrentArrayIndex]);
      LOG( "PercentProcessorTime: " << ar_ulongPercentCPUTime[nCurrentArrayIndex] << "\n" );
      //"Use this function to clear variables of type VARIANTARG (or VARIANT) 
      //before the memory that contains the VARIANTARG is freed (as when a
      //local variable goes out of scope).
      //The function clears a VARIANTARG by setting the vt member to 
      //VT_EMPTY and the wReserved member to 0. The current contents of the
      //VARIANTARG are released first."
      //VariantClear(&variant);
      VariantClear(&m_variant);
      DEBUG("cpu usage-GetValues 2009 02 28 15:37\n");

      //VariantInit(&variant);
      VariantInit(&m_variant);
	    //hr = p_iwbemclassobject->Get(L"TimeStamp_Sys100NS", 0, &variant, 0, 0);
	    m_hresult = mp_iwbemclassobject->Get(L"TimeStamp_Sys100NS", 0, &m_variant, 0, 0);
	    //arulong[nCurrentArrayIndex+1] = _wtol(variant.bstrVal);
      
      //http://www.codeproject.com/KB/system/MultiCPUUsage.aspx:
      //"The problem is that he is using wtol which converts to a long and
      //there is an overflow so it is setting to the max of a long every time.
      //If you change _wtol to _wtof it will work. "
	    //arulong[nCurrentArrayIndex+1] = _wtof(variant.bstrVal);
	    ar_ulongPercentCPUTime[nCurrentArrayIndex+1] = _wtof(m_variant.bstrVal);
      //"Use this function to clear variables of type VARIANTARG (or VARIANT) before the memory that contains the VARIANTARG is freed (as when a local variable goes out of scope).
      //The function clears a VARIANTARG by setting the vt member to VT_EMPTY and the wReserved member to 0. The current contents of the VARIANTARG are released first."
	    //VariantClear(&variant);
	    VariantClear(&m_variant);
      DEBUG("cpu usage-GetValues 2009 02 28 15:38\n");
  //    VariantInit(&variant2);
	 //   hr = p_iwbemclassobject->Get(L"TimeStamp_Sys100NS", 0, &variant2, 0, 0);
	 //   arulong[nCurrentArrayIndex+1] = _wtof(variant2.bstrVal);
	 //   VariantClear(&variant2);

      //2 elements: "PercentProcessorTime", "TimeStamp_Sys100NS"
	    nCurrentArrayIndex += 2 ;
      //delete p_iwbemclassobject ;
      //arp_iwbemclassobject[0]->Release();
      mp_iwbemclassobject->Release();
      //delete arp_iwbemclassobject[0];
    }
    //if( uReturn )
    //{
	   // //p_iwbemclassobject->Release();
    //  //arp_iwbemclassobject[0]->Release();
    //  //delete arp_iwbemclassobject[0];
    //  //p_iwbemclassobject->Delete();
    //  //Release memory to the OS.
    //  //delete p_iwbemclassobject ;
    //}
    //Release MUST be called, else: memory leaks increase by some KB every time.
	  mp_ienumwbemclassobject->Release();
    //delete p_ienumwbemclassobject ;
    DEBUG("cpu usage-GetValues end\n");
	  return nError;
  }

BYTE CPUcoreUsageGetterIWbemServices::Init()
{
    BYTE byReturn = 0 ;
  //If Init() is called more than once: do not call the following code 
  //more than necessary.
  if( ! m_bSuccesfullyInitialized )
  {
    HRESULT hres;
	  int i=0;
    DEBUG("Initialisation of CPU usage retrieval--begin\n");
    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

    mp_iwbemservices = NULL;
    hres =  CoInitializeEx(0, 
      //"with" COINIT_MULTITHREADED CoInitializeEx() returned error code 
      //"RPC_E_CHANGED_MODE"
      COINIT_MULTITHREADED

      //MSDN:"Initializes the thread for apartment-threaded object concurrency"
      //MSDN:"Apartment-threading, while allowing for multiple threads of execution, serializes all incoming calls by requiring that calls to methods of objects created by this thread always run on the same thread � the apartment/thread that created them. In addition, calls can arrive only at message-queue boundaries (i.e., only during a PeekMessage, SendMessage, DispatchMessage, etc.). Because of this serialization, it is not typically necessary to write concurrency control into the code for the object, other than to avoid calls to PeekMessage and SendMessage during processing that must not be interrupted by other method invocations or calls to other objects in the same apartment/thread."
      //COINIT_APARTMENTTHREADED
      );
    if (FAILED(hres))
    {
        //cout << "Failed to initialize COM library. Error code = 0x" 
        //    << hex << hres << endl;
      ShowMessage("for getting CPU usage:Initialization of COM library failed");
      DEBUG("for getting CPU usage:Initialization of COM library failed");
      switch(hres)
      {
      case E_INVALIDARG:
        DEBUG(":E_INVALIDARG");
        break;
      case E_OUTOFMEMORY:
        DEBUG(":E_OUTOFMEMORY");
        break;
        //E_UNEXPECTED
      case RPC_E_CHANGED_MODE:
        DEBUG(":Cannot change thread mode after it is set.");
        break;
      default:
        //DEBUG(":result:%xhex=%udec", hres, hres);
        LOG(":result:" << hex << hres << "hex=" << dec << hres << "dec\n" );
        break;
      }
      DEBUG("\n");
        //return -1.0;                  // Program has failed.
      //return CoInitializeExFailed ;
      byReturn = CoInitializeExFailed ;
    }
    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------
    // Note: If you are using Windows 2000, you need to specify -
    // the default authentication credentials for a user by using
    // a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
    // parameter of CoInitializeSecurity ------------------------
    hres =  CoInitializeSecurity(
        NULL, 
        //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/com/html/e0933741-6b75-4ce1-aa63-6240e4a7130f.htm:
        //"A value of -1 tells COM to choose which authentication services to register, and if this is the case, the asAuthSvc parameter must be NULL. However, SChannel will never be chosen as an authentication service if this parameter is -1."
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        //"Reserved for future use; it must be set to NULL. "
        NULL                         // Reserved
        );
                      
    if (FAILED(hres))
    {
      //RPC_E_TOO_LATE occured within this project but not within the
      //original project called "Check". But it is possible to continue
      //in spite of this error code.
      if( hres == RPC_E_TOO_LATE )
        DEBUG("Error: CoInitializeSecurity has already been called."
          "But this should be no problem\n")
      else
      {
        //cout << "Failed to initialize security. Error code = 0x" 
        //    << hex << hres << endl;
        CoUninitialize();
        ShowMessage("for getting CPU usage:Registration of security and "
          "setting the default "
          "security values for the process for COM failed.");
        //DEBUG("for getting CPU usage:Registration of security and "
        //  "setting the default "
        //  "security values for the process for COM failed. result value:%xhex\n",hres);
        LOG("for getting CPU usage:Registration of security and "
          "setting the default "
          "security values for the process for COM failed. result value:" 
          << hex << hres << "hex\n" 
          //Reset to decimal output.
          << dec );
        //return -2.0;                    // Program has failed.
        //return CoInitializeSecurityFailed ;
        byReturn = CoInitializeSecurityFailed ;
      }
    }
    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------

    mp_iwbemlocator = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &mp_iwbemlocator);
 
    if (FAILED(hres))
    {
        //cout << "Failed to create IWbemLocator object."
        //    << " Err code = 0x"
        //    << hex << hres << endl;
        CoUninitialize();
        ShowMessage("for getting CPU usage:CoCreateInstance failed");
        DEBUG("for getting CPU usage:CoCreateInstance failed\n");
        //return -3.0;                 // Program has failed.
        return CoCreateInstanceFailed ;
    }
    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method
	
    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer mp_iwbemservices
    // to make IWbemServices calls.
    hres = mp_iwbemlocator->ConnectServer(
         _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
         NULL,                    // User name. NULL = current user
         NULL,                    // User password. NULL = current
         0,                       // Locale. NULL indicates current
         NULL,                    // Security flags.
         0,                       // Authority (e.g. Kerberos)
         0,                       // Context object 
         &mp_iwbemservices                    // pointer to IWbemServices proxy
         );
    
    if (FAILED(hres))
    {
      cout << "Could not connect. Error code = 0x" 
           << hex << hres << endl;
      //ShowMessage() ;
      mp_iwbemlocator->Release();     
      CoUninitialize();
      ShowMessage("for getting CPU usage:Connect to WMI failed");
      DEBUG("for getting CPU usage:Connect to WMI failed\n");
      //return 1;                // Program has failed.
      return ConnectServerFailed ;
    }

    DEBUG("Connected to ROOT\\CIMV2 WMI namespace\n");

    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------
    hres = CoSetProxyBlanket(
       mp_iwbemservices,                        // Indicates the proxy to set
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
        mp_iwbemservices->Release();
        mp_iwbemlocator->Release();     
        CoUninitialize();
        ShowMessage("for getting CPU usage:Setting the authentication information failed");
        DEBUG("for getting CPU usage:Setting the authentication information failed\n");
        //return 1;               // Program has failed.
        return CoSetProxyBlanketFailed ;
    }
    m_bSuccesfullyInitialized = true ;
    //Allocate if all went ok at first.
	  m_ar_ulinitVal = new ULONG[(m_wNumberOfCPUCores+1)*2];//+1 for the total
	  m_arulong = new ULONG[(m_wNumberOfCPUCores+1)*2];//+1 for the total
    DEBUG("intialisation for getting CPU usage was successful\n");
	  //char szVal[64];
  }//if( ! m_bSuccesfullyInitialized )
  return byReturn ;
}

  void CPUcoreUsageGetterIWbemServices::SetUserInterface(
    UserInterface * p_userinterface)
  {
    mp_userinterface = p_userinterface ;
  }
#endif //#ifndef COMPILE_WITHOUT_IWBEMSERVICES
