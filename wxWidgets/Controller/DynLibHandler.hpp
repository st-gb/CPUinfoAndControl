/*
 * DynLibHandler.hpp
 *
 *  Created on: Sep 7, 2010
 *      Author: Stefan
 */

#ifndef DYNLIBHANDLER_HPP_
#define DYNLIBHANDLER_HPP_

//Forward declarations (faster than #include)
class CPUcontrolBase ;
//class I_UserInterface ;
//class wxDynLibCPUcontroller ;
//class wxDynLibCPUcoreUsageGetter ;

namespace wxWidgets
{
  //Class to separate wxWidgets specific part from the CPUcontrolBase class.
  //Other classes that implement a dynamic library handler should declare/
  //define the same function signatures as in this class--possibly add an
  //interface class (as a base class) for dynamic library handlers.
  class DynLibHandler
  {
    CPUcontrolBase & mr_cpucontrolbase ;
//    //Additional controller Dyn libs to reflect whether there are dyn lib
//    // controller dyn libs available. An alternative would be to use RunTime
//    // Type Information (RTTI); i.e. "if ( typeid(mp_cpucontroller) ==
//    // typeid(wxDynLibCPUcontroller)"
//    wxDynLibCPUcontroller * mp_wxdynlibcpucontroller ;
//    wxDynLibCPUcoreUsageGetter * mp_wxdynlibcpucoreusagegetter ;
  public:
    void CreateDynLibCPUcontroller(
      std::string & strDynLibFilePath
//      ,
//      I_CPUaccess * p_i_cpuaccess,
//      I_UserInterface * mp_userinterface
      ) ;
    void CreateDynLibCPUcoreUsageGetter(
      std::string & strDynLibFilePath
//      ,
//      I_CPUaccess * p_i_cpuaccess,
//      I_UserInterface * mp_userinterface
      ) ;
    DynLibHandler();
    DynLibHandler(CPUcontrolBase & r_cpucontrolbase)
      : mr_cpucontrolbase( r_cpucontrolbase)
    {

    }
    virtual
    ~DynLibHandler();

    std::string GetDynLibPath(const std::string & cr_strFileName ) ;

//    I_CPUcontroller * GetDynLibCPUcontroller()
//    {
//      return (I_CPUcontroller *) mp_wxdynlibcpucontroller ;
//    }
//
//    ICPUcoreUsageGetter * GetDynLibCPUcoreUsageGetter()
//    {
//      return (ICPUcoreUsageGetter *) mp_wxdynlibcpucoreusagegetter ;
//    }
  };

}

#endif /* DYNLIBHANDLER_HPP_ */
