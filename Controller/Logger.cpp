//#if defined( _MSC_VER) 
#include "StdAfx.h"
//#endif //#ifdef _MSC_VER

//If MS compiler etc.
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__CYGWIN__)
  //#include <Windows.h> //for SYSTEMTIME
  //Use "windows.h" for g++ because the case matters.
  #include <windows.h> //for SYSTEMTIME
#endif
#include "Logger.hpp"
//#include <Controller/tchar_conversion.h> //GetCharPointer()
#include <Controller/stdtstr.hpp> //GetStdString(...)
#include <fstream> //for class std::ofstream

Logger::Logger(
//  const std::set<std::string> & cr_stdsetstdstrExcludeFromLogging
  )
  //C++ style initialisation.
  : mp_ofstream(NULL)
//  , m_p_stdsetstdstrExcludeFromLogging( gp_cpucontrolbase)
  {}

Logger::Logger( std::string & stdstrFilePath )
{
    //m_ofstream.open(stdstrFilePath.c_str() );
  m_stdsetLogClass.insert(Logger::sync) ;
}

Logger::~Logger()
{
    delete mp_ofstream ;
}

bool Logger::IsOpen()
{
  return mp_ofstream //!= NULL
    && mp_ofstream->is_open() ;
}

//void Logger::Log(//ostream & ostr
//    std::ostrstream & r_ostrstream 
//    )
//{
//    //std::stringstream s;
//    //s.
//    if( m_ofstream.good() )
//    {
//      //Must set this, else text may follow after the string we want.
//      //I had program crashes with the following method:
//      //pch[r_ostrstream.pcount()] = '\0' ;
//      //r_ostrstream.ends();
//      r_ostrstream.put('\0'); //the same as "ends()" does.
//      char *pch = r_ostrstream.str() ;
//        m_ofstream << pch ;
//        m_ofstream.flush() ;
//    }
//}

void Logger::Log(//ostream & ostr
    std::string & r_stdstr
    )
{
//  if(r_stdstr.end().)
  if( //If NOT in the container.
      m_stdsetstdstrExcludeFromLogging.find(r_stdstr) ==
      m_stdsetstdstrExcludeFromLogging.end()
  //  )
  //if( //m_ofstream.good()
    &&
    mp_ofstream && mp_ofstream->good()
    )
  {
  #ifdef _WINDOWS
//    SYSTEMTIME systemtime ;
    //GetSystemTime(&systemtime);              // gets current time
    //gets the same time as the Windows clock.
//    ::GetLocalTime( & systemtime );
    ::GetLocalTime( & systemtime );
  #else

  #endif
    //m_ofstream << r_stdstr ;
    *mp_ofstream
      #ifdef _WINDOWS
      << systemtime.wYear << "."
      << systemtime.wMonth << "."
      << systemtime.wDay << " "
      << systemtime.wHour << "h:"
      << systemtime.wMinute << "min "
      << systemtime.wSecond << "s "
      << systemtime.wMilliseconds << "ms"
      << " thread ID:" << ::GetCurrentThreadId()
      << ":"
      #else
      #endif
      << r_stdstr
      << "\n"
      ;
    //m_ofstream.flush() ;
    mp_ofstream->flush() ;

    //TODO write into RAM for having 2 possibilties:
    //-truncate the log file to zero for every startup
    //-append to log file at every startup

    //->Write into RAM until the config file that determines which of these
    //2 possibilities to use is completely read.
    //If the config says: always append: simply delete the RAM buffer.
    //If the config says: always truncate:
    // 1. truncate the log file to zero
    // 2. write the RAM buffer contents ( that are the same as in
    //   log file written from startup till then)
    // 3. delete the RAM buffer.
    //buffer.add( ofstream.str() ) ;
  }
}

void Logger::Log(//ostream & ostr
    std::string & r_stdstr
    , WORD wType
    )
{
//  if( mp_model )
//  {
//    if( mp_model->
//        )
//  }
  if( m_stdsetLogClass.find(wType) != m_stdsetLogClass.end() )
    Log(r_stdstr) ;
}

#ifdef COMPILE_WITH_WSTRING
void Logger::LogW(//ostream & ostr
  std::wstring & r_stdstr
  )
{
  if( //m_ofstream.good() 
      mp_ofstream && mp_ofstream->good() 
      )
  {
    SYSTEMTIME systemtime ;
    //GetSystemTime( & systemtime); // gets current time in UTC / GMT? time zome
    ::GetLocalTime( & systemtime ); //gets the same time as the Windows clock.
    //m_ofstream << r_stdstr ;
    *mp_ofstream
        << systemtime.wYear
        << "." << systemtime.wMonth
        << "." << systemtime.wDay
        << " " << systemtime.wHour
        << "h:" << systemtime.wMinute
        << "min " << systemtime.wSecond
        << "s " << systemtime.wMilliseconds
        << "ms:" << r_stdstr.c_str() ;
    //m_ofstream.flush() ;
    mp_ofstream->flush() ;

    //TODO write into RAM for having 2 possibilties:
    //-truncate the log file to zero for every startup
    //-append to log file at every startup

    //->Write into RAM until the config file that determines which of these
    //2 possibilties to use is completely read.
    //If the config says: always append: simply delete the RAM buffer.
    //If the config says: always truncate:
    // 1. truncate the log file to zero
    // 2. write the RAM buffer contents ( that are the same as in
    //   log file written from startup till then)
    // 3. delete the RAM buffer.
    //buffer.add( ofstream.str() ) ;
  }
}
#endif //#ifdef COMPILE_WITH_WSTRING

void Logger::Log(//ostream & ostr
    char * p_ch
    )
{
    if( //m_ofstream.good() 
       mp_ofstream && mp_ofstream->good() )
    {
        //m_ofstream << p_ch ;
        *mp_ofstream //<< " char * " 
            << p_ch ;
        //m_ofstream.flush() ;
        mp_ofstream->flush() ;
    }
}

bool Logger::OpenFile2( std::string & r_stdstrFilePath
  )
{
    //char arch [10] = "File\r\n" ;
    //When the ofstream was not dynamically created (=on the heap)
    //the was no content within the file even when there was at least
    //1 output via the "<<" operator and this program was terminated
    //(=contents should have been written then).
    //Maybe this was due to a wrong thread/ process affinity for the
    //ofstream object.
    //So I create the ofstream object dynamically.
    if( ! mp_ofstream )
        mp_ofstream = new std::ofstream ;
    //m_ofstream.open(
    mp_ofstream->open(
      r_stdstrFilePath.c_str()
      //GetCharPointer(r_stdtstrFilePath.c_str() )
        //"C:\\Temp\\GriffinSvc_log.txt"
        //, std::ios_base::out | std::ios_base::trunc
        );
#ifdef _DEBUG
//    bool bOfstreamIsGood = mp_ofstream->good() ;
#endif
    return mp_ofstream->good() ;
    //std::ofstream ofstreamTest;
    //ofstreamTest.open( "C:\\Temp\\GriffinSvc_text.txt" );
    //ofstreamTest << "hhh bla" ;
    //ofstreamTest << r_stdstrFilePath << " is " <<
    //    ( m_ofstream.is_open() ? "" : " NOT " ) << "open\n" ;
    //m_ofstream << "File Opened" ;
    //*mp_ofstream << "File Opened" ;
    //m_ofstream.close() ;
    //m_ofstream.write(arch,6) ;
    //m_ofstream.flush() ;
}

//void 
bool Logger::OpenFile( //std::string & r_stdstrFilePath
  std::tstring & r_stdtstrFilePath)
{
  std::string stdstr( //GetCharPointer(r_stdtstrFilePath.c_str() ) ) ;
    GetStdString(r_stdtstrFilePath) ) ;
  return OpenFile2( stdstr ) ;
}

void Logger::TruncateFileToZeroAndRewrite()
{

}
