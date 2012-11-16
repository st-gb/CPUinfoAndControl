/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#include "ModelData.hpp"
//#include "RegisterData.hpp" //class MSRdata
#include <Controller/MSVC_adaption/tchar.h>

#define _T_LITERAL_PROGRAM_NAME _T("x86InfoAndControl")
#define CONFIG_FILE_PATH "configuration"
//#define _T_LITERAL_PROGRAM_NAME "x86InfoAndControl"

//Model::Model()
//  //C++ style inits:
//  :
//  //Initialize in the same order as textual in the declaration?
//  //(to avoid g++ warnings)
//  m_cpucoredata( * this)
//  , m_bStopFindingLowestStableCPUcoreVoltageRequestedViaUI(false)
//  , m_bCollectPstatesAsDefault(false)
//  , m_bEnableOvervoltageProtection(true)
//  , m_bSkipCPUtypeCheck(false)
//  , m_bSyncGUIshowDataWithService ( false )
////  , m_bTruncateLogFileForEveryStartup(true)
//  , m_StableCPUcoreVoltageWaitTimeInMillis(10000)
//  , mp_cpucontroller (NULL)
//  , m_stdtstrProgramName( _T_LITERAL_PROGRAM_NAME )
//  , m_std_strConfigFilePath( CONFIG_FILE_PATH )
////  , m_std_strLogFilePath( LOG_FILE_PATH )
////  , m_uiNumberOfSecondsToWaitUntilVoltageIsReduced(60)
//  {
//    //m_cpucoredata
//#ifdef _DEBUG
////      const Model * pmodel = this ;
//#endif
////    m_dPreviousPERF_CTRvalue = 0.0 ;
//  }

//  void Model::AddMaxVoltageForFreq(WORD wFreqInMHz,float fValue)
//  {
//    //m_vecmaxvoltageforfreq.push_back(MaxVoltageForFreq(fValue,wFreqInMHz) );
//    m_setmaxvoltageforfreq.insert(MaxVoltageForFreq(fValue,wFreqInMHz) ) ;
//  }

//  void Model::AddValueTableRow(
//    //std::vector<RegisterDataTable>::iterator iter_regdatatable ,
//    std::vector<RegisterDataTable> & r_stdvector_registerdatatable ,
//    //std::vector<std::string [2]> & stdvecstdstrAttributeNameAndValue
//    ValueTableRow & r_vte )
//  {
//    bool bTableFound = false ;
//    std::vector<RegisterDataTable>::iterator iter_regdatatable ;
//    std::string stdstrAttName ;
//    iter_regdatatable = r_stdvector_registerdatatable.begin() ;
//    while( iter_regdatatable != r_stdvector_registerdatatable.end() )
//    {
////      if( iter_regdatatable->m_wCols ==
////         stdvecstdstrAttributeNameAndValue.size()
////         && iter_regdatatable->m_wCols > 0
////         && iter_regdatatable->m_wRows > 0 
////        )
//      {
//        for( WORD wIndex = 0 ;wIndex < iter_regdatatable->m_wCols ; ++ wIndex )
//        {
//          stdstrAttName = iter_regdatatable->m_ararstdstr[wIndex][0] ;
//          if( stdstrAttName != stdvecstdstrAttributeNameAndValue.at(wIndex) )
//          {
//            break ;
//          }
//          if( ( wIndex + 1 ) == iter_regdatatable->m_wCols )
//          {
//            bTableFound = true ;
//            //TODO insert into table
//            iter_regdatatable = r_stdvector_registerdatatable.end() ;
//            //next ++ it gets r_stdvector_registerdatatable.end()
//            -- iter_regdatatable ;
//          }
//        }
//      }
//      ++ iter_regdatatable ;
//    }
//    if( ! bTableFound )
//      ;//TODO create new table and insert row
//  }
//  
//  void Model::AddValueTableRow(
//    std::vector<MSRdata>::reverse_iterator rev_iter_msrdata ,
//    std::vector<std::string [2]> & stdvecstdstrAttributeNameAndValue)
//  {
//    if( rev_iter_msrdata != m_stdvector_msrdata.rend() )
//      AddValueTableRow( 
//        rev_iter_msrdata->m_stdvector_registerdatatable.begin() ,
//        stdvecstdstrAttributeNameAndValue ) ;
//    else
//    {
//      std::vector<MSRdata>::iterator iter_msrdata = m_stdvector_msrdata.begin() ;
//      for( WORD wIndex = 0 ; wIndex < m_stdvector_msrdata.size() ; ++ wIndex )
//      while ( iter_msrdata != m_stdvector_msrdata.end() )
//      {
//        //MSRdata msrdata = m_stdvector_msrdata.at()
//  //      if( iter_msrdata->m_stdvector_registerdatatable )
//        AddValueTableRow(stdvecstdstrAttributeNameAndValue ) ;
//        ++ iter_msrdata ;
//      }
//    }
//  }

  BYTE Model::GetNumberOfCPUCores()
  {
    //return m_byNumberOfCPUCores ;
    return m_cpucoredata.m_byNumberOfCPUCores ;
  }

  void Model::SetNumberOfCPUCores(BYTE byNumberOfCPUCores)
  {
    //m_byNumberOfCPUCores = byNumberOfCPUCores ;
    //m_cpucoredata.m_byNumberOfCPUCores = byNumberOfCPUCores ;
    m_cpucoredata.SetCPUcoreNumber(byNumberOfCPUCores) ;
  }

  void Model::SetSkipCPUTypeCheck(bool bSkipCPUtypeCheck)
  {
    m_bSkipCPUtypeCheck = bSkipCPUtypeCheck ;
  }
//Remain a blank line after this line for no error/warning with "g++" compiler
