/*
 * CreateProcess.hpp
 *
 *  Created on: May 22, 2010
 *      Author: Stefan
 */

#ifndef CREATEPROCESS_HPP_
#define CREATEPROCESS_HPP_

void InvokeApp(DWORD dwSessionID) ;

class ServiceAttributes ;

//Using a class and store attributes that are are among fcuntions is faster than
// to copy them as parameters (global variables as alternative do not have the
// encapsulation advantage)
class CreateGUIprocess
{
//  STARTUPINFO startupinfo;
  STARTUPINFOW startupinfo;
  PROCESS_INFORMATION process_information;
  ServiceAttributes & m_r_service_attrs ;
public:
  CreateGUIprocess(ServiceAttributes & r_service_attrs )
    :
    m_r_service_attrs (r_service_attrs) {}
  BOOL CreateProcessAsNeeded(
    //Use param here for less error-prone change ( so the param value
    // can be switched easily)
    HANDLE handleToken
    ) ;
  void StartProcess(DWORD dwSessionID) ;
};

#endif /* CREATEPROCESS_HPP_ */
