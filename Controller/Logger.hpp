#ifndef LOGGER_HPP
    #define LOGGER_HPP

    #include <fstream> //for class std::ofstream ;
    #include <strstream> //for class std::ostrstream
    #include <string> //for std::wstring
    #include <sstream>

    class Logger
    {
    public:
        //std::ofstream m_ofstream ;
        std::ofstream * mp_ofstream ;
        Logger() ;
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
            char * p_ch
            ) ;
        #ifdef COMPILE_WITH_WSTRING
        //wide char version for output of Windows power scheme names etc.
        void LogW(//ostream & ostr
          std::wstring & r_stdstr
          ) ;
        #endif //#ifdef COMPILE_WITH_WSTRING
        //void 
        bool OpenFile( std::string & r_stdstrFilePath ) ;

        void TruncateFileToZeroAndRewrite() ;
    };

#endif //LOGGER_HPP
