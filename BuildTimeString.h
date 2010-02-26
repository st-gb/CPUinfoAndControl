
//This file must be processed for every built event, so 

#ifdef __CYGWIN__
    #define BUILT_TIME "Build: " __DATE__" " __TIME__ " GMT + 1\n\n"
#else
  //We need a _T() macro (wide char-> L"", char->"") for EACH 
  //line to make it compatible between char and wide char.
  //#define BUILT_TIME _T("Build: ") ## __DATE__ ## _T(" ") ## __TIME__ ## _T(" GMT + 1\n\n")
  //#define BUILT_TIME _T("Build: ")__DATE__ _T(" ") __TIME__ _T(" GMT + 1\n\n")
  //MSVC 2005 and gcc can compile this, but not MSVC 2010 as far as I remember.
  #define BUILT_TIME _T("Build: ")_T(__DATE__) _T(" ") _T(__TIME__) _T(" GMT + 1\n\n")
  //"Build: " __DATE__ " " __TIME__ " GMT\n\n"
  //
#endif
