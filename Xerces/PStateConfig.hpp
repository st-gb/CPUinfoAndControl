/*
 * PStateConfig.hpp
 *
 *  Created on: Jun 15, 2010
 *      Author: Stefan
 */

#ifndef PSTATECONFIG_HPP_
#define PSTATECONFIG_HPP_

#include <xercesc/util/XercesDefs.hpp> //XERCES_CPP_NAMESPACE_BEGIN
#include <xercesc/util/XercesVersion.hpp> //XERCES_CPP_NAMESPACE
#include <map> //class std::map
#include <set> //class std::set
#include <string> //std::string
#include <windef.h> //BYTE, WORD

//  XERCES_CPP_NAMESPACE_USE

  //Forward declarations (faster than #include)
  XERCES_CPP_NAMESPACE_BEGIN
  class //XERCES_CPP_NAMESPACE::
    DOMDocument ;
  class //XERCES_CPP_NAMESPACE::
    DOMElement ;
  class //XERCES_CPP_NAMESPACE::
    DOMImplementation ;
  class //XERCES_CPP_NAMESPACE::
    DOMLSParser ;
  class //XERCES_CPP_NAMESPACE::
    DOMNode ;
  class //XERCES_CPP_NAMESPACE::
    DOMXPathNSResolver ;
  class //XERCES_CPP_NAMESPACE::
    DOMXPathResult ;
  class XMLFormatTarget ;
  XERCES_CPP_NAMESPACE_END
  class Model ;
  class VoltageAndFreq ;

  class XercesConfigurationHandler
    //: public I_ConfigurationHandler
  {
  private:
    bool m_bAtLeast1SuccessfullDOMtreeModification ;
    bool m_bAtLeast1FailedDOMtreeModification ;
    bool m_bDOMtreeDiffersOrDifferedFromModelData ;
    bool m_bOnlySimulate ;
    bool m_bXercesSuccessfullyInitialzed ;
    const char * mpc_chFullXMLFilePath ;
    std::map<WORD,WORD> m_stdmapFreqInMHzInDOMtree2DOMindex ;
    XERCES_CPP_NAMESPACE::DOMDocument * mp_dom_document ;
    XERCES_CPP_NAMESPACE::DOMElement * mp_dom_elementRoot ;
    XERCES_CPP_NAMESPACE::DOMElement * mp_dom_elementFreqnVolt ;
    XERCES_CPP_NAMESPACE::DOMImplementation * mp_dom_implementation ;
    XERCES_CPP_NAMESPACE::DOMLSParser * mp_dom_ls_parser ;
    XERCES_CPP_NAMESPACE::DOMXPathResult * mp_domxpathresult ;
    //    , DOMDocument * & p_dom_document

    Model * mp_model ;
    //void
    //std::map<WORD,WORD>::const_iterator AddFrequencyToDOMtree(WORD wFreq) ;

    XERCES_CPP_NAMESPACE::DOMElement * AddFrequencyToDOMtree(WORD wFreq) ;
    void AppendDefaultVoltages() ;
    XERCES_CPP_NAMESPACE::DOMXPathNSResolver *
      BuildStdmapFreqInMHzInDOMtree2DOMindex(
//      std::map<WORD,WORD> & r_stdmapFreqInMHzInDOMtree2DOMindex
      ) ;
    BYTE CreateDOMtree() ;
    void GetDOM_XPathResultForFrequencies() ;
    void GetFreqnVoltDOMelement( WORD wFreqInMHz
      , XERCES_CPP_NAMESPACE::DOMElement * & mp_dom_element ) ;
//    bool PossiblyAddDefaultVoltages() ;
//    bool PossiblyAddLowestStableVoltages() ;
    bool PossiblyAddOrModifyVoltages(
      const std::set<VoltageAndFreq> & r_stdsetvoltageandfreq
      , //e.g. "max_voltage_in_Volt"
      const char * const cpc_XMLAttrName
      //"true": only look if there would be changes made (useful to check
      // for the purpose: if there are changes then ask the user whether to
      //save the changes.
      , bool bOnlySimulate
      ) ;
    bool PossiblyAddVoltages(
      //true: do not change, only test if it would be changed.
      bool bTest
      ) ;
    inline void PossiblyReleaseDOM_XPathResult() ;
//    bool PossiblyAddWantedVoltages() ;
    inline void SetVoltageFromIandCModelData(
      const char * const cpc_XMLAttrName ,
      bool & bDOMtreeModified ,
      float fVoltageFromDOMtree
      ) ;
    short WriteDOM(
      XERCES_CPP_NAMESPACE::DOMNode * p_dom_node
//        , const char * const cpc_chFilePath
        //For creating a DOMLSOutput instance via createLSOutput().
//        , DOMImplementation * p_dom_implementation
      //XMLFormatTarget may be LocalFileFormatTarget->write to a file,
      //MembufTarget etc.
      , XERCES_CPP_NAMESPACE::XMLFormatTarget & r_xmlformattarget
      ) ;
  public:
    bool IsConfigurationChanged(std::string & r_strPstateSettingsFileName ) ;
    XercesConfigurationHandler(Model * p_model ) ;
    ~XercesConfigurationHandler() ;
    BYTE //mergeXMLfileDOM(
      MergeWithExistingConfigFile(
      const char * p_chFullXMLFilePath
      , Model & r_model,
      std::string & r_strPstateSettingsFileName
      ) ;
    BYTE readXMLfileDOM(
//      const char * p_chFullXMLFilePath
//      , XERCES_CPP_NAMESPACE::DOMDocument * & p_dom_document
//      , XERCES_CPP_NAMESPACE::DOMLSParser * & parser
//      , XERCES_CPP_NAMESPACE::DOMImplementation * & p_dom_implementation
      ) ;
    BYTE * SerializeConfigToMemoryBuffer( DWORD & r_dwByteSize ) ;
    bool TestIfCfgIsChangedOrChangeCfg(
      //true: do not change, only test if it would be changed.
      bool bTest
      ) ;
  } ;

#endif /* PSTATECONFIG_HPP_ */
