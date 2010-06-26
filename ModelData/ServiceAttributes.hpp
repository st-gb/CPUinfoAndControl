/*
 * ServiceAttributes.h
 *
 *  Created on: May 23, 2010
 *      Author: Stefan
 */

#ifndef SERVICEATTRIBUTES_H_
#define SERVICEATTRIBUTES_H_

#include <string> //std::wstring

class ServiceAttributes
{
public:
  //wstring because if charset > 1 byte
  std::wstring m_stdwstrPathToGUIexe ;
  //With different current dirs the GUI(s) can be executed with different
  //configurations.
  std::wstring m_stdwstrGUICurrentDirFullPathTo ;
  ServiceAttributes()
//    :
//      m_stdwstrPathToGUIexe( L"C:\\Users\\Stefan\\x86InfoAndControl\\"
//      "x86InfoAndControl_GUI_MinGW32bit.exe")
//    , m_stdwstrGUICurrentDirFullPathTo(
//      L"C:\\Users\\Stefan\\x86InfoAndControl\\")
    {}
//  virtual
//  ~ServiceAttributes();
};

#endif /* SERVICEATTRIBUTES_H_ */
