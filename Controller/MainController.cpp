/* 
 * File:   MainController.cpp
 * Author: Stefan
 * 
 * Created on 28. August 2009, 21:22
 */

#include "MainController.hpp"
#include <Controller/I_CPUaccess.hpp>
#include <Xerces/SAX2_CPUspecificHandler.hpp>
#include <Xerces/SAX2MainConfigHandler.hpp>
#include <Xerces/SAX2DefaultVoltageForFrequency.hpp>
#include <Xerces/XMLAccess.h>
#include <Controller/stdstring_format.hpp>
#include "I_CPUcontroller.hpp"
#include <Controller/AMD/Griffin/GriffinController.hpp>
#include <Controller/Intel/PentiumM/PentiumM_Controller.hpp>
#include <Controller/AMD/Griffin/ClocksNotHaltedCPUcoreUsageGetter.hpp>
#include <Controller/Intel/PentiumM/PentiumM_ClocksNotHaltedCPUcoreUsageGetter.hpp>

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

#define INSERT_INTO_TABLE(vendor_id,fam,model,ctlr_name) \
  g_table.Insert(vendor_id,fam,model,ctlr_name)
#define GET_FROM_TABLE(vendor_id,fam,model) \
  g_table.GetControllerName(vendorid,fam,model)
    
MainController::MainController() 
  //C++ style inits
  : mp_model (NULL)
  , mp_cpucontroller (NULL)
{
  //INSERT_INTO_TABLE("AuthenticAMD",17,4,"GriffinController")
  //INSERT_INTO_TABLE("GenuineIntel",6,13,"PentiumM_Controller")
  //INSERT_INTO_TABLE("GenuineIntel",6,13,/*Insert the construtor */PentiumM_Conrtoller)
}

MainController::MainController(const MainController& orig) {
}

MainController::~MainController() {
}

//May be called from GUI object or console's main() funciton.
//Implements the factory software pattern: returns an object.
I_CPUcontroller * MainController::CreateCPUcontrollerAndUsageGetter(
  ICPUcoreUsageGetter * & r_p_icpucoreusagegetter)
{
  CPUcoreData * p_cpucoredata = & mp_model->m_cpucoredata ;
  if( p_cpucoredata->m_strVendorID == "AuthenticAMD" 
    && p_cpucoredata->m_wFamily == 17 
    && p_cpucoredata->m_byModel ==  3 
    )
  {
    GriffinController * p_gc = new GriffinController() ;
    r_p_icpucoreusagegetter = new ClocksNotHaltedCPUcoreUsageGetter(0,p_gc) ;
    p_gc->mp_icpucoreusagegetter = r_p_icpucoreusagegetter ;
    return //new GriffinController() ;
      p_gc ;
  }
  if( p_cpucoredata->m_strVendorID == "GenuineIntel" 
    && p_cpucoredata->m_wFamily == 6 
    && p_cpucoredata->m_byModel == 13 
    )
  {
    PentiumM_Controller * p_pmc = new PentiumM_Controller() ;
    r_p_icpucoreusagegetter = new PentiumM::ClocksNotHaltedCPUcoreUsageGetter(
      0, p_pmc
      ) ;
    p_pmc->mp_icpucoreusagegetter = r_p_icpucoreusagegetter ;
    return p_pmc ;
  }
  //return new GET_FROM_TABLE("GenuineIntel",6,13)
  return NULL ;
}

//May be called from GUI object or console's main() funciton.
//Implements the factory software pattern.
BYTE MainController::CreateCPUcontrollerAndUsageGetter(
  I_CPUcontroller * & r_p_cpucontroller
  ,ICPUcoreUsageGetter * & r_p_icpucoreusagegetter)
{
  r_p_cpucontroller = NULL ;
  CPUcoreData * p_cpucoredata = & mp_model->m_cpucoredata ;
  if( p_cpucoredata->m_strVendorID == "AuthenticAMD" 
    && p_cpucoredata->m_wFamily == 17 
    && p_cpucoredata->m_byModel ==  3 
    )
  {
    r_p_cpucontroller = new GriffinController() ;
    r_p_icpucoreusagegetter = new ClocksNotHaltedCPUcoreUsageGetter(
      0
      , (GriffinController*) r_p_cpucontroller
      ) ;
    //return //new GriffinController() ;
    //  p_gc ;
  }
  if( p_cpucoredata->m_strVendorID == "GenuineIntel" 
    && p_cpucoredata->m_wFamily == 6 
    && p_cpucoredata->m_byModel == 13 
    )
  {
    r_p_cpucontroller = new PentiumM_Controller() ;
    r_p_icpucoreusagegetter = new PentiumM::ClocksNotHaltedCPUcoreUsageGetter(
      0, (PentiumM_Controller *) r_p_cpucontroller ) ;
  }
  if( r_p_cpucontroller )
    //For Pentium Ms e.g. it enables SpeedStep.
    r_p_cpucontroller->Init() ;
  //  //Needed for drawing the voltage-frequency curves.
  //  r_p_cpucontroller->GetMaximumFrequencyInMHz() ;
  return 1 ;
}

void MainController::SetCPUaccess(
  //ISpecificController 
  I_CPUaccess * p_ispecificcontroller 
  )
{
  mp_ispecificcontroller = p_ispecificcontroller ;
}

void MainController::Init(
  Model & model 
  , UserInterface * p_userinterface 
  )
{
  //mp_userinterface = 
  std::string strVendorID ;
  if( mp_ispecificcontroller->
    //CpuidEx(
    //0, //CPUID function 0 is vendor ID
    //PDWORD eax,
    //PDWORD ebx,
    //PDWORD ecx,
    //PDWORD edx,
    //DWORD_PTR affinityMask
    //)
    GetVendorID(strVendorID)
    )
  {
    BYTE //byFamily , 
      byModel ;
    BYTE byStepping ;
    WORD wFamily ;
    mp_model = & model ;
    mp_model->m_cpucoredata.m_strVendorID = strVendorID ;
    //std::string strModel ; 
    //std::string strFamily ; 
    //mp_ispecificcontroller->GetModel(strModel) ;
    //mp_ispecificcontroller->GetFamily(strFamily) ;
    if( mp_ispecificcontroller->//GetFamilyAndModel( //byFamily
      //wFamily , byModel ) 
        GetFamilyAndModelAndStepping(
          wFamily , byModel , byStepping ) 
      )
    {
      mp_model->m_cpucoredata.m_byModel = byModel ;
      mp_model->m_cpucoredata.m_wFamily = wFamily ;
      mp_model->m_cpucoredata.m_byStepping = byStepping ;
    }
    SAX2_CPUspecificHandler sax2handler( *p_userinterface, model);
    WORD wModel = (WORD) byModel ;
    WORD wStepping = (WORD) byStepping ;
    std::string strCPUtypeRelativeDirPath = 
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
      //The stepping should be included because e.g. the Phenom 965 exists in Stepping
      //C2 and  C3--the C3 has lower voltages, so the "default_voltage_in_Volt"
      //attribute values are lower than the ones of the C2 stepping.
      to_stdstring<WORD>( //(WORD) byModel 
        wStepping //, std::hex
        ) 
      ;
    std::string strFamilyAndModelFilePath = strCPUtypeRelativeDirPath + ".xml" ;
    std::string strProcessorName ;
    if( mp_ispecificcontroller->
        //Because file name must not begin with spaces in NTFS.
        GetProcessorNoLeadingSpaces( //byFamily
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
    //SAX2MainConfigHandler sax2mainconfighandler( *p_userinterface, model);
    SAX2DefaultVoltageForFrequency sax2defaultvoltageforfrequency( 
      *p_userinterface, model );
    if( readXMLConfig(
        //const char* xmlFile
        strFamilyAndModelFilePath.c_str()
	      , model
	      , p_userinterface
     //   PumaStateCtrl * p_pumastatectrl 
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
    if( readXMLConfig(
        //const char* xmlFile
        strProcessorFilePath.c_str()
	      , model
	      , p_userinterface
     //   PumaStateCtrl * p_pumastatectrl 
        //Base class of implementing Xerces XML handlers.
        //This is useful because there may be more than one XML file to read.
        //So one calls this functions with different handlers passed.
        //DefaultHandler & r_defaulthandler
        , sax2defaultvoltageforfrequency
        )
      )
	  {
      
    }
    //mp_cpucontroller->mp_model = & model ;
    //if( mp_cpucontroller )
    //  //Needed for drawing the voltage-frequency curves.
    //  mp_cpucontroller->GetMaximumFrequencyInMHz() ;
  }
}
