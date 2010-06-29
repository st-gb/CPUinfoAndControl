#pragma once

//from http://www.codeguru.com/forum/showthread.php?t=231056:
#include <string>
#include <sstream> //for std::istringstream
#include <iostream>

  #include <exception> //for class std::exception
  #ifndef WIN32
    #include <stdexcept> //for class "runtime_error"
  #endif //#ifndef WIN32

  //#ifdef WIN32
  class NumberFormatException
    : public
    #if defined(__CYGWIN__) || defined(__MINGW32__)
    //  ::RuntimeException
      std::exception
    #else
      std::runtime_error
  #endif
  {
  public:
     NumberFormatException(const std::string& s)
      #ifdef __CYGWIN__
      //  ::RuntimeException
      //  std::exception()
      #else
        #ifdef _MSC_VER
        :
        std::runtime_error(s)
        #endif
      #endif
      {}
  };
  //#endif

template <class T>
std::string to_stdstring(T t//, std::ios_base & (*f)(std::ios_base&)
                      )
{
  std::ostringstream oss;
  oss << //f 
    //std::hex << 
    t;
  std::string str = oss.str() ;
  return //oss.str();
    str ;
}

//Convert std::string to e.g. a float value:
//float fValue ; std::string stdstr = "8.0";
// from_stdstring<float>(fValue , stdstr ) ;
//template <class T>
////T
////void *
//void from_stdstring(
//  T & t
//  , std::string & stdstr//, std::ios_base & (*f)(std::ios_base&)
//  //, void * pv
//  )
//{
//  std::istringstream iss;
//  iss.str(stdstr) ;
//  //int i ;
//  //iss >> stdstr ;
//  iss >> //(t) *pv ;
//    t ;
//  //std::string str = iss.str() ;
//  //return //oss.str();
//  //  //str ;
//  //  pv ;
//}

template <class T>
bool from_stdstring(
   T & r_templateType,
   const std::string & r_stdstr
   //,std::ios_base& (*f)(std::ios_base&)
  )
{
  //DEBUG("from_string:%s\n",s);
//  LOG("from_string:" << s << "\n" );
  std::istringstream iss(r_stdstr);
  return !(iss //>> f
    >> r_templateType
    ).fail();
}

////convert std::string to e.g. a float value
//template <class T>
////T
////void *
//void from_stdtstring(
//  T & t
//  , std::wstring & stdwstr//, std::ios_base & (*f)(std::ios_base&)
//  //, void * pv
//  )
//{
//  std::wistringstream iss;
//  iss.str(stdwstr) ;
//  //int i ;
//  //iss >> stdstr ;
//  iss >> //(t) *pv ;
//    t ;
//  //std::string str = iss.str() ;
//  //return //oss.str();
//  //  //str ;
//  //  pv ;
//}

//convert std::string to e.g. a float value
template <class T>
//T
//void *
void from_stdtstring(
  T & t
  , std::string & stdstr//, std::ios_base & (*f)(std::ios_base&)
  //, void * pv
  )
{
  std::istringstream iss;
  iss.str(stdstr) ;
  //int i ;
  //iss >> stdstr ;
  iss >> //(t) *pv ;
    t ;
  //std::string str = iss.str() ;
  //return //oss.str();
  //  //str ;
  //  pv ;
}
