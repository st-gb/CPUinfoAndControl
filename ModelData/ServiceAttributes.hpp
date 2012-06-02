/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * ServiceAttributes.h
 *
 *  Created on: May 23, 2010
 *      Author: Stefan
 */

#ifndef SERVICEATTRIBUTES_H_
#define SERVICEATTRIBUTES_H_

#include <string> //std::wstring
typedef unsigned long DWORD;

class ServiceAttributes
{
public:
  DWORD m_dwStartGUIdelayTimeInMillis;
//  std::string m_stdstrPathToGUIexe ;
  //Use std::wstring because there may be cases where the character set > 1 byte
  std::wstring m_stdwstrPathToGUIexe ;
  //Intension for this attribute: with different "current directories" the
  //GUI(s) can be executed with different configurations.
  std::wstring m_stdwstrGUICurrentDirFullPathTo ;

  ServiceAttributes() ;
 //  virtual
//  ~ServiceAttributes();
};

#endif /* SERVICEATTRIBUTES_H_ */
