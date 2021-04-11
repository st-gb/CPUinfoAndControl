
///Stefan Gebauer's (361095 TU Berlin) common_sourcecode repo:
///OperatingSystem::GetErrorMessageFromErrorCodeA(...)
#include <OperatingSystem/GetErrorMessageFromErrorCode.h>
#include <OperatingSystem/GetLastErrorCode.hpp>//OperatingSystem::GetLastErrorCode()
#include <OperatingSystem/Windows/ErrorCode/LocalLanguageMessageFromErrorCode.h>
//Pre-defined preprocessor macro under MSVC, MinGW for 32 and 64 bit Windows.
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
//  #include <Windows/ErrorCodeFromGetLastErrorToString.h>
  ///DLLloadError::GetPossibleSolution(...)
  #include <OperatingSystem/Windows/DLLloadError.hpp>
#endif //#ifdef _WIN32

///This repository's files:
#include <Controller/I_CPUaccess.hpp>
#include <Controller/DynLibCPUcoreUsageGetter.hpp>///typedef _GetCPUcoreUsage
#include <Controller/Logger/LogLevel.hpp>

///Standard C(++) headers:
#include <string>///class std::string
#include <iostream>///std::cout

inline void AssignFunctionPointers(
  HMODULE hinstDynLib,
  _GetCPUcoreUsage & pfnGetCPUcoreUsage,
  dll_usage_getter_init_type & pfn_dll_init_type
  )
{
   ///https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getprocaddress
  pfnGetCPUcoreUsage = (_GetCPUcoreUsage) ::GetProcAddress(
    hinstDynLib,//HMODULE hModule,
    "GetCPUcoreUsage"//LPCSTR lpProcName
    );

//  pfn_dll_usage_getter_num_logical_cpu_cores_type = ::GetProcAddress(
//    "GetNumberOfLogicalCPUcores");

  pfn_dll_init_type =
    (dll_usage_getter_init_type) ::GetProcAddress(hinstDynLib, "Init");
}

/** @brief template function for ease to assign function pointers from a dyn lib
 *  @param p_function must be passed in order to get its type */
template <typename func_type> void PossiblyAssignFunctionPointer(
  HMODULE hinstDynLib,
  const std::string & strFuncName,
  func_type & p_function//, func_type functyp
  )
{
  p_function = (func_type) ::GetProcAddress(
    hinstDynLib,//HMODULE hModule,
    strFuncName.c_str()//LPCSTR lpProcName
    );
//  if(p_function)
}

int main(int argCnt, char ** args)
{
  if(argCnt < 2){
    std::cerr << "Specify a DLL path as 1st command line parameter" <<std::endl;
    return 1;
  }
  std::cout << "Using DLL " << args[1] << std::endl;
  /** https://docs.microsoft.com/de-de/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibraryw
   * "If the function fails, the return value is NULL." */
  /*HINSTANCE*/HMODULE  hinstDynLib = ::LoadLibraryA(args[1]);
  if(hinstDynLib == NULL){
    DWORD dw = GetLastError();
    std::cerr << "Failed to load " << args[1] << " OS error code:" << dw
      << std::endl
      //<< LocalLanguageMessageFromErrorCodeA(dw)
      << DLLloadError::GetPossibleSolution(dw)
      << std::endl;
    return 2;
  }
  std::cout << "Successfully loaded " << args[1] << std::endl;

  _GetCPUcoreUsage pfnGetCPUcoreUsage;
  dll_usage_getter_init_type pfn_dll_init_type;
  AssignFunctionPointers(hinstDynLib, pfnGetCPUcoreUsage,
    pfn_dll_init_type);
  
  if(pfn_dll_init_type == NULL){
    std::cerr << "No valid function pointer to DLL's \"Init\" function" <<
      std::endl; 
    return 3;
  }
  std::cout << "Function pointer to DLL's \"Init\" function:"
    << pfn_dll_init_type << std::endl;   

//  CPUaccess cpuaccess;
//  p_cpuaccess = & cpuaccess;

//    if( ! p_cpuaccess )
//      LOGN_TYPE( "pointer to hardware access object is "
//        "0->may crash when dyn lib's \"" << INIT_LITERAL << "\" function "
//        "derefers it", LogLevel::warning)
  std::cout << "calling DLL's Init fn" << std::endl;
  BYTE byRet = (*pfn_dll_init_type) ( //p_cpuaccess
    //p_cpuacces->wNumLogicalCPUcores 
    //cpucoredata.m_byNumberOfCPUCores

    //By passing a pointer to an object the DLL that receives this pointer
    //has to interpret the struct (an object is a data/ a struct +
    //functions) in the same way as this executable.
    //This means that the members of the struct must both have the
    //same alignment and (relative) position.
    //For instance there were crashes when exe was compiled with MinGW
    //and the DLL was compiled with MSVC because of the "vtable"?:
    //http://de.wikipedia.org/wiki/Tabelle_virtueller_Methoden  /
    //http://en.wikipedia.org/wiki/Virtual_method_table
    // meaning (some) function addresses of I_CPUaccess between MSVC and g++
    // were probably different.
//#ifndef _DEBUG
//        //TODO
//        0 //just for testing what happens if the DLL wants to call operations
        // of I_CPUaccess on a NULL pointer
//#else
    //p_cpuaccess
    NULL
//#endif
    );
  std::cout << "After calling DLL's Init fn:" << (unsigned) byRet << std::endl;
  if(byRet == 0)
  {
    float CPUcoreUsage;
    while(1){
      CPUcoreUsage = pfnGetCPUcoreUsage(0);
      std::cout << "CPU core usage:" << CPUcoreUsage << std::endl;
      Sleep(1000);
    };
  }
  return 0;
}
