/*
 * CPUcontrolBase.cpp
 *
 *  Created on: May 1, 2010
 *      Author: Stefan
 */
#include "CPUcontrolBase.hpp"
#include <Controller/ReadFileContent.h>
#include <UserInterface/UserInterface.hpp>

  CPUcontrolBase::CPUcontrolBase( )
    :
    mp_cpucontroller( NULL)
    , mp_cpucoreusagegetter( NULL)
    , mp_userinterface (NULL)
    , mp_wxdynlibcpucontroller (NULL)
    , mp_wxdynlibcpucoreusagegetter ( NULL )
  {
  }

//  //May be called from GUI object or console's main() function.
//  //Implements the factory software pattern.
//  BYTE CPUcontrolBase::CreateCPUcontrollerAndUsageGetter(
//    I_CPUcontroller * & r_p_cpucontroller
//    ,ICPUcoreUsageGetter * & r_p_icpucoreusagegetter)
//  {
//    r_p_cpucontroller = NULL ;
//    r_p_icpucoreusagegetter = NULL ;
//    CPUcoreData * p_cpucoredata = & mp_model->m_cpucoredata ;
//    #ifdef COMPILE_WITH_AMD_GRIFFIN
//  //  if( p_cpucoredata->m_strVendorID == "AuthenticAMD"
//  //    && p_cpucoredata->m_wFamily == 17
//  //    && p_cpucoredata->m_byModel ==  3
//  //    )
//  //  {
//  //    r_p_cpucontroller = new GriffinController( mp_model , mp_cpuaccess ) ;
//  //    r_p_icpucoreusagegetter = new ClocksNotHaltedCPUcoreUsageGetter(
//  //      0
//  //      , (GriffinController*) r_p_cpucontroller
//  //      //, *mp_model
//  //      ) ;
//  //    //return //new GriffinController() ;
//  //    //  p_gc ;
//  //  }
//    #endif //  #ifdef COMPILE_WITH_AMD_GRIFFIN
//  //  if( p_cpucoredata->m_strVendorID == "GenuineIntel"
//  //    && p_cpucoredata->m_wFamily == 6
//  //    && p_cpucoredata->m_byModel == 13
//  //    )
//  //  {
//  //    r_p_cpucontroller = new PentiumM_Controller() ;
//  //    r_p_icpucoreusagegetter = new PentiumM::ClocksNotHaltedCPUcoreUsageGetter(
//  //      0, (PentiumM_Controller *) r_p_cpucontroller ) ;
//  //  }
//    //if( p_cpucoredata->m_strVendorID == "GenuineIntel"
//    //  && p_cpucoredata->m_wFamily == 6
//    //  && p_cpucoredata->m_byModel == 30
//    //  )
//    //{
//    //  r_p_cpucontroller = new NehalemController() ;
//    //  r_p_icpucoreusagegetter = new Nehalem::ClocksNotHaltedCPUcoreUsageGetter(
//    //    0, (NehalemController *) r_p_cpucontroller ) ;
//    //}
//  //  else
//    {
//      std::string stdstrCPUtypeRelativeDirPath ;
//      GetPstatesDirPath( stdstrCPUtypeRelativeDirPath ) ;
//      stdstrCPUtypeRelativeDirPath += "/" ;
//      std::string stdstr = stdstrCPUtypeRelativeDirPath + "CPUcontroller.cfg" ;
//      if( ReadFileContent( stdstr ) )
//      {
//        wxString wxstrFilePath = wxT("CPUcontrollerDynLibs/") + //wxString( stdstr ) ;
//            getwxString(stdstr ) ;
//        wxstrFilePath += wxDynamicLibrary::GetDllExt() ;
//        //http://wiki.wxwidgets.org/Converting_everything_to_and_from_wxString#wxString_to_std::string
//        std::string stlstring = std::string(wxstrFilePath.mb_str());
//        LOGN("should load/ attach " << //wxstrFilePath
//          stlstring << " as CPU controller" )
//        try
//        {
//          //r_p_cpucontroller = new wxDynLibCPUcontroller(
//          mp_wxdynlibcpucontroller =
//            new wxDynLibCPUcontroller(
//            wxstrFilePath
//            , GetCPUaccess()
//            , mp_userinterface
//            ) ;
//  //        LOGN("before DLL::GetAvailableMultipliers")
//  //        gp_cpucontrolbase->mp_wxdynlibcpucontroller->GetAvailableMultipliers(
//  //          mp_model->m_cpucoredata.m_stdset_floatAvailableMultipliers) ;
//  //        mp_model->m_cpucoredata.AvailableMultipliersToArray() ;
//  //        LOGN("after DLL::GetAvailableMultipliers")
//  //
//  //        gp_cpucontrolbase->mp_wxdynlibcpucontroller->GetAvailableVoltagesInVolt(
//  //          mp_model->m_cpucoredata.m_stdset_floatAvailableVoltagesInVolt) ;
//  //        mp_model->m_cpucoredata.AvailableVoltagesToArray() ;
//
//          mp_model->m_stdstrCPUcontrollerDynLibPath = //stdstr ;
//              getstdstring( wxstrFilePath ) ;
//          LOGN("CPU controller DynLib " <<
//            mp_model->m_stdstrCPUcontrollerDynLibPath <<
//            ": successfully loaded and function pointers to it assigned.")
//  //        gp_cpucontrolbase->SetCPUcontroller( //p_wxdynlibcpucontroller
//  //           //mp_wxdynlibcpucontroller
//  //           gp_cpucontrolbase->mp_wxdynlibcpucontroller  ) ;
//          //gp_cpucontrolbase->mp_wxdynlibcpucontroller = r_p_cpucontroller ;
//          r_p_cpucontroller = mp_wxdynlibcpucontroller ;
//          //This number is important for CPU core creating usage getter.
//          p_cpucoredata->m_byNumberOfCPUCores = r_p_cpucontroller->
//            GetNumberOfCPUcores() ;
//        }
//        //Catch because: if executed from GUI it can continue.
//        catch( ... )
//        {
//          r_p_cpucontroller = NULL ;
//        }
//      }
//      stdstr = stdstrCPUtypeRelativeDirPath + "CPUcoreUsageGetter.cfg" ;
//      if( ReadFileContent( stdstr ) )
//      {
//        wxString wxstrFilePath = wxT("CPUcoreUsageGetterDynLibs/") + ( stdstr ) ;
//        wxstrFilePath += wxDynamicLibrary::GetDllExt() ;
//        LOGN("should load/ attach " << getstdstring( wxstrFilePath )
//          << " as CPU core usage getter" )
//        try
//        {
//          //r_p_icpucoreusagegetter = new wxDynLibCPUcoreUsageGetter (
//          mp_wxdynlibcpucoreusagegetter =
//            new wxDynLibCPUcoreUsageGetter (
//            wxstrFilePath
//            , GetCPUaccess() ,
//            * p_cpucoredata
//            ) ;
//          mp_model->m_stdstrCPUcoreUsageGetterDynLibPath = //stdstr ;
//              getstdstring( wxstrFilePath ) ;
//          //gp_cpucontrolbase->mp_wxdynlibcpucoreusagegetter = r_p_icpucoreusagegetter ;
//          r_p_icpucoreusagegetter = mp_wxdynlibcpucoreusagegetter ;
//  //        //no CPU controller DLL should be loaded or loading it failed it is
//  //        //"0".
//  //        if( p_cpucoredata->m_byNumberOfCPUCores == 0 )
//  //        {
//  //          WORD wNumberOfLogicalCPUcores = gp_cpucontrolbase->
//  //            mp_wxdynlibcpucoreusagegetter->GetNumberOfLogicalCPUcores() ;
//  //          if( wNumberOfLogicalCPUcores == 0 )
//  //            wNumberOfLogicalCPUcores = 1 ;
//  //          //TODO correct CPU core number
//  //          //Set -> allocate array for OnPaint()
//  //          p_cpucoredata->SetCPUcoreNumber(wNumberOfLogicalCPUcores) ;
//  //        }
//        }
//        //Catch because: if executed from GUI it can continue.
//        catch( ... )
//        {
//          r_p_icpucoreusagegetter = NULL ;
//        }
//      }
//    }
//  //  LOGN("number of CPU cores:"
//  //    << mp_model->m_cpucoredata.m_byNumberOfCPUCores )
//    //If neither a built-in or dynamically linked CPU controller or usage getter
//    //returned a core number > 0
//    if( mp_model->m_cpucoredata.m_byNumberOfCPUCores == 0 )
//    {
//      LOGN("number of CPU cores not set neither by CPU controller nor by "
//          "usage getter -> getting it from Windows")
//  #ifdef _WINDOWS
//      mp_model->m_cpucoredata.m_byNumberOfCPUCores = GetNumLogicalCPUs() ;
//      LOGN("number of CPU core reported by Windows:"
//        << (WORD) mp_model->m_cpucoredata.m_byNumberOfCPUCores )
//  #endif
//    }
//    else
//      LOGN("number of CPU cores:"
//        << (WORD) mp_model->m_cpucoredata.m_byNumberOfCPUCores )
//
//    //Set -> allocate array for OnPaint() / DVFS
//    p_cpucoredata->SetCPUcoreNumber(mp_model->m_cpucoredata.m_byNumberOfCPUCores) ;
//
//    if( r_p_cpucontroller )
//    {
//      //For Pentium Ms e.g. it enables SpeedStep.
//      r_p_cpucontroller->Init() ;
//      if( r_p_icpucoreusagegetter )
//      {
//        r_p_cpucontroller->mp_icpucoreusagegetter = r_p_icpucoreusagegetter ;
//        //Start performance counting etc.
//        r_p_icpucoreusagegetter->Init() ;
//        r_p_icpucoreusagegetter->SetAttributeData( mp_model ) ;
//      }
//    }
//    if( r_p_cpucontroller //&& r_p_icpucoreusagegetter
//      )
//    {
//      return 1 ;
//    }
//    else
//      return 0 ;
//    //  //Needed for drawing the voltage-frequency curves.
//    //  r_p_cpucontroller->GetMaximumFrequencyInMHz() ;
//  }

  void CPUcontrolBase::DeleteCPUcontroller()
  {
    LOGN("CPUcontrolBase::DeleteCPUcontroller cpu controller:" <<
        mp_cpucontroller )
    if( mp_cpucontroller )
      //Release memory.
      delete mp_cpucontroller ;
    mp_cpucontroller = NULL ;
    //May be NULL at startup.
    if( mp_cpucoreusagegetter )
      mp_cpucoreusagegetter->SetCPUcontroller( NULL ) ;
    LOGN("CPUcontrolBase::DeleteCPUcontroller() end:" )
  }

  void CPUcontrolBase::PossiblyDeleteCPUcontrollerDynLib()
  {
    LOGN("PossiblyDeleteCPUcontroller cpu controller:" << mp_cpucontroller
        << "dyn lib controller:" << mp_wxdynlibcpucontroller )
    if( //gp_cpucontrolbase->
      //mp_wxdynlibcpucontroller
      //This may either point to a built-in CPU controller or to a dyn lib
      //CPU controller.
      mp_cpucontroller &&
      (I_CPUcontroller * ) mp_wxdynlibcpucontroller ==  mp_cpucontroller
      )
    {
      EndDVFS() ;
      //mp_wxx86infoandcontrolapp->
//        SetCPUcontroller( NULL ) ;
      //mp_wxx86infoandcontrolapp->
        DeleteCPUcontroller() ;
//      delete //mp_wxx86infoandcontrolapp->
//        //mp_wxdynlibcpucontroller ;
//        //This may either point to a built-in CPU controller or to a dyn lib
//        //CPU controller.
//        mp_cpucontroller ;
      //delete mp_wxdynlibcpucontroller ;

      //If the CPU controller was a dyn lib CPU controller this pointer also
      //has do be set to NULL.
      //gp_cpucontrolbase->
        mp_wxdynlibcpucontroller = NULL ;
      if( mp_userinterface )
        //E.g. do stuff like disable "unload dyn lib CPU controller" in menue.
        mp_userinterface->CPUcontrollerDeleted() ;
    }
    LOGN("CPUcontrolBase::PossiblyDeleteCPUcontroller() end" )
  }

  void CPUcontrolBase::PossiblyDeleteCPUcoreUsageGetter()
  {
    if( //gp_cpucontrolbase->
        mp_wxdynlibcpucoreusagegetter )
    {
      //mp_wxx86infoandcontrolapp->SetCPUcontroller( NULL ) ;
      //mp_wxx86infoandcontrolapp->DeleteCPUcontroller() ;
      EndDVFS() ;
      //mp_wxx86infoandcontrolapp->
      LOGN("deleting existing CPU core usage getter")
      //Must delete instance of ICPUcoreUsageGetter (base class of
      //wxDynLibCPUcoreUsageGetter).
      //Else the destructor of wxDynLibCPUcoreUsageGetter was not called and
      //so the DLL was not unloaded.
      delete mp_cpucoreusagegetter ;
        mp_cpucoreusagegetter = NULL ;
//      delete //gp_cpucontrolbase->
//        mp_wxdynlibcpucoreusagegetter ;
      //gp_cpucontrolbase->
        mp_wxdynlibcpucoreusagegetter = NULL ;
      //delete mp_wxdynlibcpucontroller ;
      //mp_wxdynlibcpucontroller = NULL ;
      //PossiblyReleaseMemForCPUcontrollerUIcontrols() ;
      //mp_model->m_cpucoredata.ClearCPUcontrollerSpecificAtts() ;
      if( mp_userinterface )
        //E.g. do stuff like disable "unload dyn lib CPU controller" in menue.
        mp_userinterface->CPUcoreUsageGetterDeleted() ;
    }
  }
