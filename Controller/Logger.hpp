#ifndef LOGGER_HPP
  #define LOGGER_HPP

  //#include <fstream> //for class std::ofstream ;
  //#include <strstream> //for class std::ostrstream
  #include <string> //for std::wstring
  //#include <sstream>
  #include <Controller/stdtstr.hpp>
  #include <set> //for class std::set
#ifdef _WINDOWS
  //Use "windows.h" for g++ because the case matters.
  #include <windows.h> //for SYSTEMTIME
#endif
  //class std::ofstream ;

  class Logger
  {
#ifdef _WINDOWS
    //Use member variable: so it does not need to be created on stack for
    //every call to "Log(...)".
    SYSTEMTIME systemtime ;
#endif
  public:
    enum log_class
    {
      sync = 0
    };
    std::set<WORD> m_stdsetLogClass ;
    std::set<std::string> m_stdsetstdstrExcludeFromLogging ;
    //std::ofstream m_ofstream ;
    std::ofstream * mp_ofstream ;
    Logger( //const std::set<std::string> & cr_stdsetstdstrExcludeFromLogging
      ) ;
    ~Logger() ;
    Logger( std::string & stdstrFilePath ) ;

    bool IsOpen() ;
    //void Log(//ostream & ostr
    //    std::ostrstream & r_ostrstream
    //    ) ;

    void Log(//ostream & ostr
        std::string & r_stdstr
        ) ;
    void Log(//ostream & ostr
        std::string & r_stdstr
        , WORD wType
        ) ;
    void Log(//ostream & ostr
        char * p_ch
        ) ;
    #ifdef COMPILE_WITH_WSTRING
    //wide char version for output of Windows power scheme names etc.
    void LogW(//ostream & ostr
      std::wstring & r_stdstr
      ) ;
    #endif //#ifdef COMPILE_WITH_WSTRING
    //void
    bool OpenFile( //std::string & r_stdstrFilePath
      std::tstring & r_stdtstrFilePath ) ;
    bool OpenFile2( std::string & r_stdstrFilePath ) ;

    void TruncateFileToZeroAndRewrite() ;
  };

#endif //LOGGER_HPP
