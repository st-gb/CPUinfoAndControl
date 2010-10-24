/* 
 * File:   MainController.cpp
 * Author: Stefan
 * 
 * Created on 28. August 2009, 21:22
 */

#include "MainController.hpp"
//GetStdString(const std::wstring & cr_wstr )
#include <Controller/character_string/stdtstr.hpp>
#include <Controller/character_string/stdstring_format.hpp> //to_stdstring(...)
#include <Controller/CPU-related/I_CPUcontroller.hpp> //class I_CPUcontroller
#include <Controller/CPUcontrolBase.hpp> //class CPUcontrolBase
#include <Controller/I_CPUaccess.hpp> //class I_CPUaccess
//#include <Controller/ReadFileContent.hpp> //ReadFileContent(...)
////for VoltageSafetyException
//#include <Controller/X86InfoAndControlExceptions.hpp>
#ifdef COMPILE_WITH_AMD_GRIFFIN
//#include <Controller/CPU-related/AMD/Griffin/GriffinController.hpp>
//#include <Controller/CPU-related/AMD/Griffin/ClocksNotHaltedCPUcoreUsageGetter.hpp>
#endif //#ifdef COMPILE_WITH_AMD_GRIFFIN
//#include <Controller/CPU-related/Intel/PentiumM/PentiumM_Controller.hpp>
//#include <Controller/CPU-related/Intel/PentiumM/PentiumM_ClocksNotHaltedCPUcoreUsageGetter.hpp>
//#include <Controller/CPU-related/Intel/Nehalem/NehalemController.hpp>
//#include <Controller/CPU-related/Intel/Nehalem/NehalemClocksNotHaltedCPUcoreUsageGetter.hpp>
#include <Controller/GetNumberOfLogicalCPUcores.h>//GetNumberOfLogicalCPUcores()
#ifdef COMPILE_WITH_MSR_EXAMINATION
  //only useful for user interface
  #include <Xerces/SAX2_CPUspecificHandler.hpp>
#endif
#include <Xerces/SAX2MainConfigHandler.hpp> //class SAX2MainConfigHandler
//class SAX2DefaultVoltageForFrequency
#include <Xerces/SAX2DefaultVoltageForFrequency.hpp>
#include <Xerces/XMLAccess.hpp> //ReadXMLfileInitAndTermXerces(...)

////compiling with pre-declarations is faster than with "#include"s 
//class GriffinController ;
//class PentiumM_Controller ;

//class CPU_VendorFamilyModelTable
//{
//  std::vector vectable ;
//  void Insert(
//    std::string vendor_id,
//    fam
//    ,model
//    ,ctlr_name
//    ) ;
//  void GetControllerName(vendorid,fam,model)
//  {
//    return 
//  }
//} ;

//CPU_VendorFamilyModelTable g_table ;

//class CPUcontrolBase ;

extern CPUcontrolBase * gp_cpucontrolbase ;

#define INSERT_INTO_TABLE(vendor_id,fam,model,ctlr_name) \
  g_table.Insert(vendor_id,fam,model,ctlr_name)
#define GET_FROM_TABLE(vendor_id,fam,model) \
  g_table.GetControllerName(vendorid,fam,model)
    
MainController::MainController( UserInterface * p_userinterface )
  //C++ style inits
  :
  mp_cpuaccess( NULL )
  , mp_cpucontroller (NULL)
  , mp_model (NULL)
  , mp_userinterface ( p_userinterface )
{
  //INSERT_INTO_TABLE("AuthenticAMD",17,4,"GriffinController")
  //INSERT_INTO_TABLE("GenuineIntel",6,13,"PentiumM_Controller")
  //INSERT_INTO_TABLE("GenuineIntel",6,13,/*Insert the constructor */PentiumM_Conrtoller)
}

MainController::MainController(const MainController& orig) {
}

MainController::~MainController() {
  LOGN("~MainController")
}

void MainController::CreateBuiltInCPUcontrollerAndUsageGetter(
  )
{
#ifdef COMPILE_WITH_AMD_GRIFFIN
//  if( p_cpucoredata->m_strVendorID == "AuthenticAMD"
//    && p_cpucoredata->m_wFamily == 17
//    && p_cpucoredata->m_byModel ==  3
//    )
//  {
//    r_p_cpucontroller = new GriffinController( mp_model , mp_cpuaccess ) ;
//    r_p_icpucoreusagegetter = new ClocksNotHaltedCPUcoreUsageGetter(
//      0
//      , (GriffinController*) r_p_cpucontroller
//      //, *mp_model
//      ) ;
//    //return //new GriffinController() ;
//    //  p_gc ;
//  }
#endif //  #ifdef COMPILE_WITH_AMD_GRIFFIN
//  if( p_cpucoredata->m_strVendorID == "GenuineIntel"
//    && p_cpucoredata->m_wFamily == 6
//    && p_cpucoredata->m_byModel == 13
//    )
//  {
//    r_p_cpucontroller = new PentiumM_Controller() ;
//    r_p_icpucoreusagegetter = new PentiumM::ClocksNotHaltedCPUcoreUsageGetter(
//      0, (PentiumM_Controller *) r_p_cpucontroller ) ;
//  }
//if( p_cpucoredata->m_strVendorID == "GenuineIntel"
//  && p_cpucoredata->m_wFamily == 6
//  && p_cpucoredata->m_byModel == 30
//  )
//{
//  r_p_cpucontroller = new NehalemController() ;
//  r_p_icpucoreusagegetter = new Nehalem::ClocksNotHaltedCPUcoreUsageGetter(
//    0, (NehalemController *) r_p_cpucontroller ) ;
//}
}

//May be called from GUI object or console's main() function.
//Implements the factory software pattern.
BYTE MainController::CreateCPUcontrollerAndUsageGetter(
  I_CPUcontroller * & r_p_cpucontroller
  ,ICPUcoreUsageGetter * & r_p_icpucoreusagegetter)
{
  r_p_cpucontroller = NULL ;
  r_p_icpucoreusagegetter = NULL ;
  CPUcoreData * p_cpucoredata = & mp_model->m_cpucoredata ;

  std::string stdstrCPUtypeRelativeDirPath ;
  if( GetPstatesDirPath( stdstrCPUtypeRelativeDirPath ) )
  {
    stdstrCPUtypeRelativeDirPath += "/" ;
    gp_cpucontrolbase->CreateDynLibCPUcontroller(
      stdstrCPUtypeRelativeDirPath
//        , r_p_cpucontroller
      ) ;
    r_p_cpucontroller = gp_cpucontrolbase->m_p_cpucontrollerDynLib ;
    gp_cpucontrolbase->CreateDynLibCPUcoreUsageGetter(
      stdstrCPUtypeRelativeDirPath
//        , r_p_icpucoreusagegetter
      ) ;
    r_p_icpucoreusagegetter = gp_cpucontrolbase->
        m_p_cpucoreusagegetterDynLib ;
  } //if( GetPstatesDirPath( stdstrCPUtypeRelativeDirPath ) )

//  LOGN("number of CPU cores:"
//    << mp_model->m_cpucoredata.m_byNumberOfCPUCores )
  //If neither a built-in or dynamically linked CPU controller or usage getter
  //returned a core number > 0
  if( mp_model->m_cpucoredata.m_byNumberOfCPUCores == 0 )
  {
    LOGN("number of CPU cores not set neither by CPU controller nor by "
        "usage getter -> getting it from the Operating System")
//#ifdef _WINDOWS
    mp_model->m_cpucoredata.m_byNumberOfCPUCores =
      GetNumberOfLogicalCPUcores() ;
    LOGN("number of CPU cores reported by the Operating System:"
      <<
      //Cast to integer to not output the value as character
      (WORD)
      mp_model->m_cpucoredata.m_byNumberOfCPUCores )
//#endif
  }
  else
    LOGN("number of CPU cores:"
      <<
      //Cast to integer to not output the value as character
      (WORD) mp_model->m_cpucoredata.m_byNumberOfCPUCores )

  //SetCPUcoreNumber(...) -> allocate array for OnPaint() / DVFS
  p_cpucoredata->SetCPUcoreNumber(
    mp_model->m_cpucoredata.m_byNumberOfCPUCores) ;

  if( r_p_cpucontroller )
  {
    //Init(): For Pentium Ms e.g. it enables SpeedStep.
    r_p_cpucontroller->Init() ;
    if( r_p_icpucoreusagegetter )
    {
      r_p_cpucontroller->mp_icpucoreusagegetter = r_p_icpucoreusagegetter ;
      //Init(): Starts performance counting etc.
      r_p_icpucoreusagegetter->Init() ;
      r_p_icpucoreusagegetter->SetAttributeData( mp_model ) ;
    }
  }
  LOGN("end of creating/ allocating CPU controller and usage getter objects")
  if( r_p_cpucontroller //&& r_p_icpucoreusagegetter
    )
  {
    return 1 ;
  }
  else
    return 0 ;
  //  //Needed for drawing the voltage-frequency curves.
  //  r_p_cpucontroller->GetMaximumFrequencyInMHz() ;
}

void MainController::SetCPUaccess(
  //ISpecificController 
  I_CPUaccess * p_cpuaccessmethod 
  )
{
  mp_cpuaccess = p_cpuaccessmethod ;
}

//This method is also used for save file dialog.
BYTE MainController::GetPstatesDirPath(
  std::string & r_strCPUtypeRelativeDirPath )
{
  BYTE byRet = 0 ;
  std::string strVendorID ;
  if( //May be NULL
      mp_cpuaccess && mp_cpuaccess->
    GetVendorID(strVendorID)
    )
  {
    BYTE byModel ;
    BYTE byStepping ;
    WORD wFamily ;
    mp_model->m_cpucoredata.m_strVendorID = strVendorID ;
    //std::string strModel ; 
    //std::string strFamily ; 
    //mp_cpuaccess->GetModel(strModel) ;
    //mp_cpuaccess->GetFamily(strFamily) ;
    if( mp_cpuaccess->//GetFamilyAndModel( //byFamily
      //wFamily , byModel ) 
        GetFamilyAndModelAndStepping(
          wFamily , byModel , byStepping ) 
      )
    {
      WORD wModel = (WORD) byModel ;
      WORD wStepping = (WORD) byStepping ;
      r_strCPUtypeRelativeDirPath = 
        "configuration/" 
        + strVendorID 
        + "/" 
        + //strFamily 
        to_stdstring<WORD>( wFamily //, std::hex
          ) 
        + "/" +
        to_stdstring<WORD>( //(WORD) byModel 
          wModel //, std::hex
          ) 
        + "/" +
        //The stepping should be included because e.g. the Phenom 965 exists
        //in Stepping C2 and  C3--the C3 has lower voltages, so the
        //"default_voltage_in_Volt"
        //attribute values are lower than the ones of the C2 stepping.
        to_stdstring<WORD>( //(WORD) byModel 
          wStepping //, std::hex
          ) 
        ;
      byRet = 1 ;
    }
  }
  return byRet ;
}

BYTE MainController::GetPstateSettingsFileName( 
  std::string & r_strPstateSettingsFileName )
{
  BYTE byRet = 0 ;
  if( mp_cpuaccess->
    //Because file name must not begin with spaces in NTFS.
    GetProcessorNameWithoutLeadingSpaces( //byFamily
      r_strPstateSettingsFileName 
    )
  )
  {
    r_strPstateSettingsFileName += ".xml" ;
    byRet = 1 ;
  }
  return byRet ;
}

BYTE MainController::ReadPstateConfig(
  Model & model 
  , UserInterface * p_userinterface 
  )
{
  BYTE byRet = 0 ;
  std::string strCPUtypeRelativeDirPath ;
  if( GetPstatesDirPath(strCPUtypeRelativeDirPath) )
  {
    BYTE byModel ;
    BYTE byStepping ;
    //SAX2_CPUspecificHandler sax2handler( * p_userinterface, model );
    std::string strFamilyAndModelFilePath = strCPUtypeRelativeDirPath + ".xml" ;
    std::string strProcessorName ;
    WORD wFamily ;
    if( mp_cpuaccess->GetFamilyAndModelAndStepping(
        wFamily , byModel , byStepping )
      )
    {
      mp_model->m_cpucoredata.m_byModel = byModel ;
      mp_model->m_cpucoredata.m_wFamily = wFamily ;
      mp_model->m_cpucoredata.m_byStepping = byStepping ;
    }
    if( mp_cpuaccess->
        //Because file name must not begin with spaces in NTFS.
        GetProcessorNameWithoutLeadingSpaces( //byFamily
          strProcessorName 
        )
      )
      mp_model->m_strProcessorName = strProcessorName ;
    std::string strProcessorFilePath = //"configuration/" + strVendorID + "/" 
      //+ //strFamily 
      //to_stdstring<WORD>( wFamily //, std::hex
      //  ) + "/" + //strModel 
      //to_stdstring<WORD>( //(WORD) byModel 
      //  wModel //, std::hex
      //  ) 
      strCPUtypeRelativeDirPath
        + "/" + strProcessorName + ".xml"
      ;
    SAX2DefaultVoltageForFrequency sax2defaultvoltageforfrequency( 
      * p_userinterface, model );
    //#ifdef COMPILE_WITH_REGISTER_EXAMINATION
    #ifdef COMPILE_WITH_MSR_EXAMINATION
    ReadRegisterDataConfig( strFamilyAndModelFilePath, p_userinterface ) ;
    #endif //#ifdef COMPILE_WITH_REGISTER_EXAMINATION
    if( ! ReadXMLfileWithoutInitAndTermXercesInline(
        //const char* xmlFile
        strProcessorFilePath.c_str()
//        , model
        , p_userinterface
        //Base class of implementing Xerces XML handlers.
        //This is useful because there may be more than one XML file to read.
        //So one calls this functions with different handlers passed.
        //DefaultHandler & r_defaulthandler
        , sax2defaultvoltageforfrequency
        )
      )
    {
      byRet = 1 ;
    }
    else
    {
      //std::tstring tstr(
      //  _T("Running this program is unsafe because there was an error ") ) ;
      //tstr +=  _T("with the file containg the maximum voltages (") ;
      //tstr = tstr + strProcessorFilePath ;
      //tstr += _T(")") ;
      //throw VoltageSafetyException(
      //  tstr ) ;
      byRet = 1 ;
    }
    //mp_i_cpucontroller->mp_model = & model ;
    //if( mp_i_cpucontroller )
    //  //Needed for drawing the voltage-frequency curves.
    //  mp_i_cpucontroller->GetMaximumFrequencyInMHz() ;
    //byRet = 1 ;
  }
  return byRet ;
}

BYTE MainController::ReadMainAndPstateConfig(
  Model & model
  , UserInterface * p_userinterface
  )
{
  BYTE byRet = 255 ;
  mp_model = & model ;
  ReadMainConfig(model, p_userinterface) ;
  byRet = ReadPstateConfig(model, p_userinterface) ;
  return byRet ;
}

void MainController::ReadMainConfig(
  Model & model
  , UserInterface * p_userinterface
  )
{
  std::string stdstrMainConfigFileName = GetStdString( model.
    m_stdtstrProgramName ) + "_config.xml" ;
  //The main cfg also contains the exclusion log message filter--load it in any
  //case.
  SAX2MainConfigHandler sax2mainconfighandler( model, p_userinterface );
  DEBUGN( "number of Trie nodes: " << g_logger.m_trie.m_dwNumberOfNodes )
  ReadXMLfileInitAndTermXerces(
    //const char* xmlFile
    stdstrMainConfigFileName.c_str()
    , model
    , p_userinterface
    //Base class of implementing Xerces XML handlers.
    //This is useful because there may be more than one XML file to read.
    //So one calls this functions with different handlers passed.
    //DefaultHandler & r_defaulthandler
    , sax2mainconfighandler
    ) ;
}

//Called by MainController::Init() and by the CPU register dialog(s)
void MainController::ReadRegisterDataConfig(
  std::string & strFamilyAndModelFilePath
  , UserInterface * p_userinterface
  )
{
#ifdef COMPILE_WITH_MSR_EXAMINATION
  SAX2_CPUspecificHandler sax2handler( * p_userinterface, * mp_model );
   if( ReadXMLfileInitAndTermXerces(
      //const char* xmlFile
      strFamilyAndModelFilePath.c_str()
      , * mp_model
      , p_userinterface
      //Base class of implementing Xerces XML handlers.
      //This is useful because there may be more than one XML file to read.
      //So one calls this functions with different handlers passed.
      //DefaultHandler & r_defaulthandler
      //, sax2mainconfighandler
      , sax2handler
      )
    )
	  {

    }
#endif //#ifdef COMPILE_AS_SERVICE
}
