#pragma once

//from http://www.codeguru.com/forum/showthread.php?t=231056:
#include <string>
#include <sstream>
#include <iostream>

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

//convert std::string to e.g. a float value
template <class T>
//T 
//void * 
void from_stdstring( 
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
