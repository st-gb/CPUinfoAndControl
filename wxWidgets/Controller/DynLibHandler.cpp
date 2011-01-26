/*
 * DynLibHandler.cpp
 *
 *  Created on: Sep 7, 2010
 *      Author: Stefan
 */

#include <Controller/CPUcontrolBase.hpp>
//GetNumberOfLogicalCPUcores()
#include <Controller/GetNumberOfLogicalCPUcores.h>
#include <wxWidgets/Controller/DynLibHandler.hpp>
#include <wxWidgets/Controller/wxDynLibCPUcontroller.hpp>
#include <wxWidgets/Controller/wxDynLibCPUcoreUsageGetter.hpp>
//for GetStdString(wxString &), getwxString(...)
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>

#include <wx/dynlib.h> //for wxDynamicLibrary::GetDllExt()

namespace wxWidgets
{

//  DynLibHandler::DynLibHandler()
//    :
//    mp_wxdynlibcpucontroller (NULL)
//    , mp_wxdynlibcpucoreusagegetter ( NULL )
//  {
//
//  }

  DynLibHandler::~DynLibHandler()
  {
    // TODO Auto-generated destructor stub
  }

  bool DynLibHandler::CreateDynLibCPUcontroller(
    std::string & r_stdstrDynLibFilePath
//    ,
//    I_CPUaccess * p_i_cpuaccess,
//    I_UserInterface * mp_userinterface
    )
  {
    bool bSuccess = false ;
    wxString wxstrFilePath = getwxString( r_stdstrDynLibFilePath );
    try
    {
      //r_p_cpucontroller = new wxDynLibCPUcontroller(
//      mp_wxdynlibcpucontroller =
      mr_cpucontrolbase.m_p_cpucontrollerDynLib =
        new wxDynLibCPUcontroller(
        wxstrFilePath
//        , mp_i_cpuaccess
        , mr_cpucontrolbase.mp_i_cpuaccess
//        , mp_userinterface
        , mr_cpucontrolbase.mp_userinterface
        ) ;
      if( mr_cpucontrolbase.m_p_cpucontrollerDynLib )
      {
        mr_cpucontrolbase.m_model.m_cpucoredata.mp_cpucontroller =
          mr_cpucontrolbase.m_p_cpucontrollerDynLib;
//        mr_cpucontrolbase.m_model.m_cpucoredata.InterpolateDefaultVoltages();
      }
      bSuccess = true ;
  //        LOGN("before DLL::GetAvailableMultipliers")
  //        gp_cpucontrolbase->mp_wxdynlibcpucontroller->GetAvailableMultipliers(
  //          mp_model->m_cpucoredata.m_stdset_floatAvailableMultipliers) ;
  //        mp_model->m_cpucoredata.AvailableMultipliersToArray() ;
  //        LOGN("after DLL::GetAvailableMultipliers")
  //
  //        gp_cpucontrolbase->mp_wxdynlibcpucontroller->GetAvailableVoltagesInVolt(
  //          mp_model->m_cpucoredata.m_stdset_floatAvailableVoltagesInVolt) ;
  //        mp_model->m_cpucoredata.AvailableVoltagesToArray() ;
      LOGN("CPU controller DynLib "
        << mr_cpucontrolbase.m_model.m_stdstrCPUcontrollerDynLibPath
        << ": successfully loaded and function pointers to it assigned.")
  //        gp_cpucontrolbase->SetCPUcontroller( //p_wxdynlibcpucontroller
  //           //mp_wxdynlibcpucontroller
  //           gp_cpucontrolbase->mp_wxdynlibcpucontroller  ) ;
      //gp_cpucontrolbase->mp_wxdynlibcpucontroller = r_p_cpucontroller ;
//      r_p_cpucontroller = mp_wxdynlibcpucontroller ;

      //This number is important for CPU core creating usage getter.
      WORD wNumberOfLogicalCPUcores =
        mr_cpucontrolbase.m_p_cpucontrollerDynLib->GetNumberOfCPUcores() ;
      if( wNumberOfLogicalCPUcores ) // <> 0
        mr_cpucontrolbase.m_model.m_cpucoredata.m_byNumberOfCPUCores =
          wNumberOfLogicalCPUcores ;
      else
        mr_cpucontrolbase.m_model.m_cpucoredata.m_byNumberOfCPUCores =
          GetNumberOfLogicalCPUcores() ;
    }
    //Catch because: if executed from GUI it can continue.
    catch( ... )
    {
      LOGN("exception when creating the CPU controller. "
        "mr_cpucontrolbase.m_p_cpucontrollerDynLib"
        << mr_cpucontrolbase.m_p_cpucontrollerDynLib )
//      r_p_cpucontroller = NULL ;
    }
    return bSuccess ;
  }

  bool DynLibHandler::CreateDynLibCPUcoreUsageGetter(
    std::string & r_stdstrDynLibFilePath
//    ,
//    I_CPUaccess * p_i_cpuaccess,
//    I_UserInterface * mp_userinterface
    )
  {
    bool bSuccess = false ;
    try
    {
      wxString wxstrDynLibFilePath = getwxString( r_stdstrDynLibFilePath ) ;
      //r_p_icpucoreusagegetter = new wxDynLibCPUcoreUsageGetter (
//      mp_wxdynlibcpucoreusagegetter =
      mr_cpucontrolbase.m_p_cpucoreusagegetterDynLib =
        new wxDynLibCPUcoreUsageGetter (
        wxstrDynLibFilePath
//        , mp_i_cpuaccess
        , mr_cpucontrolbase.mp_i_cpuaccess
//        , * p_cpucoredata
        , mr_cpucontrolbase.m_model.m_cpucoredata
        ) ;
      bSuccess = true ;
      LOGN("after allocating dyn lib CPU core usage getter")
      //gp_cpucontrolbase->mp_wxdynlibcpucoreusagegetter = r_p_icpucoreusagegetter ;
//      r_p_icpucoreusagegetter = mr_cpucontrolbase.
//        mp_wxdynlibcpucoreusagegetter ;
  //        //no CPU controller DLL should be loaded or loading it failed it is
  //        //"0".
  //        if( p_cpucoredata->m_byNumberOfCPUCores == 0 )
  //        {
  //          WORD wNumberOfLogicalCPUcores =   gp_cpucontrolbase->
  //            mp_wxdynlibcpucoreusagegetter->GetNumberOfLogicalCPUcores() ;
  //          if( wNumberOfLogicalCPUcores == 0 )
  //            wNumberOfLogicalCPUcores = 1 ;
  //          //TODO correct CPU core number
  //          //Set -> allocate array for OnPaint()
  //          p_cpucoredata->SetCPUcoreNumber(wNumberOfLogicalCPUcores) ;
  //        }
    }
    //Catch because: if executed from GUI it can continue.
    catch( ... )
    {
//      r_p_icpucoreusagegetter = NULL ;
    }
    return bSuccess ;
  }

  std::string DynLibHandler::GetDynLibPath(const std::string & cr_strFileName )
  {
    wxString wxstr( wxDynamicLibrary::GetDllExt() ) ;
    //http://wiki.wxwidgets.org/Converting_everything_to_and_from_wxString
    // #wxString_to_std::string
    return cr_strFileName + GetStdString( wxstr ) ;
  }
}
