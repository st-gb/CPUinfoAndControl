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

//template <class T>
//T from_stdstring(std::string stdstr//, std::ios_base & (*f)(std::ios_base&)
//                      )
//{
//  std::istringstream iss;
//  iss. >> stdstr ;
//  std::string str = iss.str() ;
//  return //oss.str();
//    str ;
//}
