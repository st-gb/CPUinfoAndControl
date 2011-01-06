/*
 * PStateConfig.cpp
 *
 *  Created on: Jun 15, 2010
 *      Author: Stefan
 */

#include "PStateConfig.hpp"

//Include "xerces...\include" in the "include path" for the following "#include"s.
//Class XERCES_CPP_NAMESPACE::DOMException etc.
//  #include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>//class XERCES_CPP_NAMESPACE::DOMDocument
#include <xercesc/dom/DOMElement.hpp>//class XERCES_CPP_NAMESPACE::DOMElement
//class XERCES_CPP_NAMESPACE::DOMException
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMLSException.hpp>
#include <xercesc/dom/DOMLSOutput.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/dom/DOMLSParser.hpp>//class XERCES_CPP_NAMESPACE::DOMLSParser
//class XERCES_CPP_NAMESPACE::DOMNamedNodeMap
#include <xercesc/dom/DOMNamedNodeMap.hpp>
//class XERCES_CPP_NAMESPACE::DOMXPathException
#include <xercesc/dom/DOMXPathException.hpp>
#include <xercesc/dom/DOMXPathNSResolver.hpp>
//class XERCES_CPP_NAMESPACE::DOMXPathResult
#include <xercesc/dom/DOMXPathResult.hpp>
//class XERCES_CPP_NAMESPACE::DOMImplementationLS
#include <xercesc/dom/DOMImplementationLS.hpp>
//class XERCES_CPP_NAMESPACE::DOMImplementationRegistry
#include <xercesc/dom/DOMImplementationRegistry.hpp>
//Class XERCES_CPP_NAMESPACE::LocalFileFormatTarget
#include <xercesc/framework/LocalFileFormatTarget.hpp>
//Class XERCES_CPP_NAMESPACE::MemBufFormatTarget
#include <xercesc/framework/MemBufFormatTarget.hpp>
//  #include <xercesc/sax2/DefaultHandler.hpp>
//  #include <xercesc/sax2/SAX2XMLReader.hpp>
//  #include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/XMLString.hpp> //Class XERCES_CPP_NAMESPACE::XMLString
//  //XERCES_CPP_NAMESPACE::XMLPlatformUtils::Terminate()
//  #include <xercesc/util/PlatformUtils.hpp>
//for to_stdstring()
#include <Controller/character_string/stdstring_format.hpp>
#include <ModelData/ModelData.hpp> //class Model
//Class XercesAttributesHelper
#include <Xerces/XercesAttributesHelper.hpp>
//#include <Xerces/XercesHelper.hpp> //for XERCES_STRING_FROM_ANSI_STRING()
#include <Xerces/XercesString.hpp> //for XERCES_STRING_FROM_ANSI_STRING()
//  #include <global.h> //for LOGN()
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)

#define MAKE_WCHAR_STRING(string) L##string
#define FREQ_AND_VOLTAGE_LITERAL "freq_and_voltage"
#define FREQ_AND_VOLTAGE_WCHAR_LITERAL L"freq_and_voltage"
#define FREQUENCY_IN_MHZ_ANSI_LITERAL "frequency_in_MHz"
//#define FREQUENCY_IN_MHZ_WCHAR_LITERAL L##FREQUENCY_IN_MHZ_ANSI_LITERAL
#define FREQUENCY_IN_MHZ_WCHAR_LITERAL L"frequency_in_MHz"

#define XPATH_EXPRESSION_FREQ_AND_VOLTAGE_ANSI "/*/freq_and_voltage"
#define XPATH_EXPRESSION_FREQ_AND_VOLTAGE_WCHAR L"/*/freq_and_voltage"
//#define XPATH_EXPRESSION_FREQ_AND_VOLTAGE_WCHAR \_
//  MAKE_WCHAR_STRING(XPATH_EXPRESSION_FREQ_AND_VOLTAGE_ANSI)

//  using namespace std;
//  //To NOT need to prefix the xerces classes with the "xercesc...::"
//  XERCES_CPP_NAMESPACE_USE
//  Do not do "using namespace xercesc..." because so it is clearer that the
  // class names belong to _Xerces_

  //void
  //std::map<WORD,WORD>::const_iterator
  XERCES_CPP_NAMESPACE::DOMElement *
    XercesConfigurationHandler::AddFrequencyToDOMtree(WORD wFreq)
  {
    std::map<WORD,WORD>::const_iterator c_iterFrequenciesStoredInFile ;
    //DOMElement * p_dom_elementFreqnVolt ;
    try
    {
      mp_dom_elementFreqnVolt = mp_dom_document->
        //http://xerces.apache.org/xerces-c/apiDocs-3/classDOMDocument.html
        // #f5e93a6b757adb2544b3f6dffb4b461a:
        // "DOMException   INVALID_CHARACTER_ERR: Raised if the specified name
        //  contains an illegal character."
        createElement(
          //XERCES_STRING_FROM_ANSI_STRING(FREQ_AND_VOLTAGE_LITERAL)
        //Avoid Linux g++ "no matching function for call to ‘xercesc_3_0::
        // DOMDocument::createElement(const wchar_t [17])’"
        (const XMLCh * ) FREQ_AND_VOLTAGE_WCHAR_LITERAL );
      //http://xerces.apache.org/xerces-c/apiDocs-3/classDOMNode.html:
      //"Adds the node newChild to the end of the list of children of this
      //node."
      mp_dom_elementRoot->
        //http://xerces.apache.org/xerces-c/apiDocs-3/classDOMNode.html
        // #504731160f9bfff5bb9cc64afabf0e2f:
        //"     * @exception DOMException"
        appendChild( mp_dom_elementFreqnVolt);

      LOGN("appended " << FREQ_AND_VOLTAGE_LITERAL << "to the XML DOM tree" )
      mp_dom_elementFreqnVolt->
        //http://xerces.apache.org/xerces-c/apiDocs-2/classDOMElement.html
        // #1a607d8c619c4aa4a59bc1a7bc5d4692
        //"@exception DOMException"
        setAttribute(
        //XERCES_STRING_FROM_ANSI_STRING("frequency_in_MHz")
        //Avoid Linux g++ "no matching function for call to
        //‘xercesc_3_0::DOMElement::setAttribute(const wchar_t [17],
        //const XMLCh*)’ "
        (const XMLCh * ) FREQUENCY_IN_MHZ_WCHAR_LITERAL //attribute name
        ,
        //attribute value
        XERCES_STRING_FROM_ANSI_STRING( to_stdstring<WORD>(wFreq).c_str() )
        );
      LOGN("Successfully set attribute value " << wFreq << " for "
        FREQUENCY_IN_MHZ_ANSI_LITERAL )
    }
    catch( const XERCES_CPP_NAMESPACE::DOMException & cr_xercesc_dom_exception )
    {
      LOGN("AddFrequencyToDOMtree: DOMException while creating, appending DOM "
        "element or setting attribute value: "
        << //XercesHelper::ToStdString( cr_xercesc_dom_exception.getMessage() )
        Xerces::ToStdString( cr_xercesc_dom_exception.getMessage() )
        )
    }
//    LOGN("before insert into freq in MHz -> DOM index map")
    //Do not need to add a freq to the index map because every frequency can
    //only be once in the r_stdsetvoltageandfreq param for
    // PossiblyAddOrModifyVoltages(...) ?!
    //TODO verify if the DOM index is correct.
//    c_iterFrequenciesStoredInFile =
//    //http://www.cplusplus.com/reference/stl/map/insert/:
//    //"the insertion operation checks for each element inserted whether another
//    //element exists already in the container with the same key value, if so,
//    //the element is not inserted"
//    //So if "min_voltage" for 600 MHZ and "max_voltage" it the XML element
//    // "freq_and_voltage" _not_ inserted twice.
//    m_stdmapFreqInMHzInDOMtree2DOMindex.insert(
//      std::pair<WORD,WORD>(
//        wFreq,
//        m_stdmapFreqInMHzInDOMtree2DOMindex.size()
//        )
//        ).first ;
//    LOGN("before releasing DOMXPathres ")
//    if( mp_domxpathresult )
//    {
//      //Release mem: a new XPATHres is created in GetDOM_XPathResultForFrequencies()
//      mp_domxpathresult->release();
//    }
////    LOGN("after releasing DOMXPathres ")
//    //The number of elements grew by 1. So redo getting the XPath result.
//    GetDOM_XPathResultForFrequencies() ;
    return //c_iterFrequenciesStoredInFile ;
      mp_dom_elementFreqnVolt ;
  }

  XERCES_CPP_NAMESPACE::DOMXPathNSResolver * XercesConfigurationHandler::
    BuildStdmapFreqInMHzInDOMtree2DOMindex(
//    std::map<WORD,WORD> & r_stdmapFreqInMHzInDOMtree2DOMindex
    )
  {
    //Avoid g++ warning " 'p_dom_xpath_ns_resolver' might be used
    //uninitialized in this function"
    XERCES_CPP_NAMESPACE::DOMXPathNSResolver * p_dom_xpath_ns_resolver = NULL ;
    LOGN("begin of freq in MHz to DOM index")
    // "/*": das Wurzel-Element unabhaengig vom Namen
    //  (jedes wohlgeformte XML-Dokument hat genau ein Wurzel-Element)
//    char * archXPath = XPATH_EXPRESSION_FREQ_AND_VOLTAGE ;
//    XMLCh * p_xmlchXpath = XMLString::transcode(archXPath);
    LOGN("mp_dom_document:" << mp_dom_document)
    //DOMElement * p_dom_elementRoot = p_dom_document->getDocumentElement();
    mp_dom_elementRoot = mp_dom_document->getDocumentElement();
    LOGN("mp_dom_elementRoot: " << mp_dom_elementRoot )
    p_dom_xpath_ns_resolver = mp_dom_document->createNSResolver(
      mp_dom_elementRoot);
    LOGN("p_dom_xpath_ns_resolver: " << p_dom_xpath_ns_resolver )
    //DOMXPathResult * p_domxpathresult = p_dom_document->evaluate(
    PossiblyReleaseDOM_XPathResult() ;
    try
    {
      LOGWN_WSPRINTF(L"evaluating %ls", XPATH_EXPRESSION_FREQ_AND_VOLTAGE_WCHAR)
      //Gets NULL on failure/ exception.
      mp_domxpathresult = mp_dom_document->
        //http://xerces.apache.org/xerces-c/apiDocs-3/classDOMXPathEvaluator.html
        // #8785175b7f816234978a931a60000039:
        // "DOMXPathException", "DOMException"
        // returns NULL on failure/ exception
        evaluate(
        //p_xmlchXpath,
        //Avoid g++ warning "no matching function for call to
        //‘xercesc_3_0::DOMDocument::evaluate(const wchar_t [20],
        //xercesc_3_0::DOMElement*&, xercesc_3_0::DOMXPathNSResolver*&,
        //xercesc_3_0::DOMXPathResult::ResultType, NULL)’ "
//        (const XMLCh * ) XPATH_EXPRESSION_FREQ_AND_VOLTAGE_WCHAR ,
        XERCES_STRING_FROM_ANSI_STRING(XPATH_EXPRESSION_FREQ_AND_VOLTAGE_ANSI) ,
        mp_dom_elementRoot,
        p_dom_xpath_ns_resolver,
        XERCES_CPP_NAMESPACE::DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE,
        NULL
        );
      XMLSize_t nDOMqueryResultsetLength =
        //http://xerces.apache.org/xerces-c/apiDocs-2/classDOMXPathResult.html:
        //"XPathException   TYPE_ERR: raised if resultType is not
        //UNORDERED_NODE_SNAPSHOT_TYPE or ORDERED_NODE_SNAPSHOT_TYPE. "
        mp_domxpathresult->getSnapshotLength();
      LOGN("number of " << //archXPath
        XPATH_EXPRESSION_FREQ_AND_VOLTAGE_ANSI << " elements: "
        << nDOMqueryResultsetLength )
//      //Use "[]" rather than "char *" to avoid g++ Linux compiler warning
//      // "deprecated conversion from string constant to ‘char*’ "
//      char arp_chAttributeName [] = "frequency_in_MHz" ;
//      XMLCh * xmlchAttributeName = XERCES_CPP_NAMESPACE::XMLString::transcode(
//        arp_chAttributeName);
//      if( xmlchAttributeName)
//      {
      XERCES_CPP_NAMESPACE::DOMNamedNodeMap * p_dom_namednodemap ;
      XERCES_CPP_NAMESPACE::DOMNode * p_domnodeAttribute ;
      const XMLCh * cp_xmlchAttrValue ;
      DWORD dwValue ;
//      XercesAttributesHelper xerces_attributes_helper ;

      //Iterate over all "freq_and_voltage" XML elements.
      for( //XMLSize_t
        WORD wResultSetIndex = 0; wResultSetIndex < nDOMqueryResultsetLength;
        wResultSetIndex ++ )
      {
        LOGN("at " << XPATH_EXPRESSION_FREQ_AND_VOLTAGE_ANSI
          << " element " << wResultSetIndex )
        //http://xerces.apache.org/xerces-c/apiDocs-2/classDOMXPathResult.html:
        //"XPathException  TYPE_ERR: raised if resultType is not
        //UNORDERED_NODE_SNAPSHOT_TYPE or ORDERED_NODE_SNAPSHOT_TYPE. "
        mp_domxpathresult->snapshotItem(wResultSetIndex);
        //theSerializer->write(p_domxpathresult->getNodeValue(),
        //  theOutputDesc);
        //Get all attributes for the current XML element.
        p_dom_namednodemap = mp_domxpathresult->getNodeValue()->
          getAttributes() ;
        //for( XMLSize_t xmlsize_tAttIdx = 0 ; p_dom_namednodemap->getLength() ;
        //    ++ xmlsize_tAttIdx )
        //{
        p_domnodeAttribute = p_dom_namednodemap->getNamedItem(
          //"frequency_in_MHz"
//            xmlchAttributeName
          XERCES_STRING_FROM_WCHAR_T_STRING(FREQUENCY_IN_MHZ_WCHAR_LITERAL)
          ) ;
        //Attribute exists.
        if ( p_domnodeAttribute )
        {
          cp_xmlchAttrValue = p_domnodeAttribute->getNodeValue() ;
          std::string stdstr = //XercesHelper::ToStdString(cp_xmlchAttrValue) ;
            Xerces::ToStdString(cp_xmlchAttrValue) ;
          LOGN("attribute name for " << //arp_chAttributeName
            FREQUENCY_IN_MHZ_ANSI_LITERAL
            << ": " << stdstr )
          //cp_xmlchAttrValue = cp_xmlchAttrValue ;
//            xerces_attributes_helper.ToDWORD( stdstr , & dwValue ) ;
          from_stdstring<DWORD>( dwValue, stdstr ) ;
          m_stdmapFreqInMHzInDOMtree2DOMindex.insert(
            std::pair<WORD,WORD> ( (WORD) dwValue, wResultSetIndex ) ) ;
        }
        //}
//        }
//        XERCES_CPP_NAMESPACE::XMLString::release( & xmlchAttributeName );
      }
  //    XMLString::release( & p_xmlchXpath);
    }
    catch( const XERCES_CPP_NAMESPACE::DOMXPathException &
      cr_xercesc_dom_xpath_exception )
    {
      LOGN("BuildStdmapFreqInMHzInDOMtree2DOMindex: DOMXPathException: "
        << //XercesHelper::ToStdString(
            //cr_xercesc_dom_xpath_exception.getMessage() )
        Xerces::ToStdString(cr_xercesc_dom_xpath_exception.getMessage() )
        )
    }
    catch( const XERCES_CPP_NAMESPACE::DOMException & cr_xercesc_dom_exception )
    {
      LOGN("BuildStdmapFreqInMHzInDOMtree2DOMindex: DOMException: "
        << //XercesHelper::ToStdString(cr_xercesc_dom_exception.getMessage() )
        Xerces::ToStdString(cr_xercesc_dom_exception.getMessage() )
        )
    }
    LOGN("mp_domxpathresult: " << mp_domxpathresult )

    LOGN("end of freq in MHz to DOM index")
    return p_dom_xpath_ns_resolver ;
  }

  bool XercesConfigurationHandler::IsConfigurationChanged(
    std::string & r_strPstateSettingsFileName )
  {
    bool bIsConfigurationChanged = false ;
    if( m_bXercesSuccessfullyInitialzed )
    {
      //setting/ assigning the value is Important: for readXMLfileDOM() and
      //writeXML(...)
      mpc_chFullXMLFilePath = r_strPstateSettingsFileName.c_str() ;

  //    BYTE retval ;
      //// "/*": das Wurzel-Element unabh�ngig vom Namen
      ////  (jedes wohlgeformte XML-Dokument hat genau ein Wurzel-Element)
      //char * archXPath = "/*/freq_and_voltage" ;
  //    DOMDocument * p_dom_document ;
  //    DOMLSParser * p_dom_ls_parser ;
  //    DOMImplementation * p_dom_implementation ;
      try
      {
  //      XMLPlatformUtils::Initialize();
        if( readXMLfileDOM(
          //p_chFullXMLFilePath
    //      r_strPstateSettingsFileName.c_str()
    //      , p_dom_document
    //      , p_dom_ls_parser
    //      , p_dom_implementation
            )
          )
        {
          //Is NULL if the XML file could not be read.
          if( mp_dom_document )
          {
      //      mp_dom_document = p_dom_document ;
            //XMLCh * p_xmlchXpath = XMLString::transcode(archXPath);
            //DOMElement * p_dom_elementRoot = p_dom_document->getDocumentElement();
            //mp_dom_elementRoot = p_dom_document->getDocumentElement();

            bIsConfigurationChanged = TestIfCfgIsChangedOrChangeCfg(
              //true: do not change, only test if it would be changed.
              true
              ) ;
            mp_dom_document->release() ;
            mp_dom_document = NULL ;
            //XMLString::release(&p_xmlchXpath);
          }// if( p_dom_document )
          //Is NULL if the XML file could not be read (=may not exist).
          else
          {
            LOGN("file " << mpc_chFullXMLFilePath << " could not be read/ is "
                "not available")
            CPUcoreData & r_cpucoredata = mp_model->m_cpucoredata ;
            if( r_cpucoredata.m_stdsetvoltageandfreqDefault.size() > 0 )
            {
              LOGN("at least 1 default/ max voltage-> cfg is changed")
              bIsConfigurationChanged = true ;
            }
            if( r_cpucoredata.m_stdsetvoltageandfreqLowestStable.size() > 0 )
            {
              LOGN("at least 1 min voltage-> cfg is changed")
              bIsConfigurationChanged = true ;
            }
            if( r_cpucoredata.m_stdsetvoltageandfreqWanted.size() > 0 )
            {
              LOGN("at least 1 min voltage-> cfg is changed")
              bIsConfigurationChanged = true ;
            }
          }
        }
  //      //http://xerces.apache.org/xerces-c/program-3.html:
  //      //"Independent of the API you want to use, DOM, SAX, or SAX2, your
  //      //application must [...] and terminate it after you are done.
  //      //When Terminate() was called in another block (even if in a function that
  //      //is called in the same block) than program crash.
  //      XMLPlatformUtils::Terminate();
      }
      catch (const XERCES_CPP_NAMESPACE::XMLException &
        cr_xercesc_xml_exception)
      {
        char* message = XERCES_CPP_NAMESPACE::XMLString::transcode(
          cr_xercesc_xml_exception.getMessage() );
        LOG( "Error during Xerces initialization! :\n" << message //<< "\n"
          )
        XERCES_CPP_NAMESPACE::XMLString::release(&message);
      }
      LOGN("configuration between " << mpc_chFullXMLFilePath <<
        " and program data differs?" <<
        (m_bDOMtreeDiffersOrDifferedFromModelData ? "yes" : "no") )
    }
    return bIsConfigurationChanged ;
  }
  //XercesConfigurationHandler::AppendDefaultVoltages( //Model & r_model
  //  )
  //{
  //  float fVoltage ;
  //  WORD wFreq ;
  //  std::set<VoltageAndFreq>::const_iterator c_iterator =
  //    mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.begin() ;
  //  //for( WORD wIndex = 0 ; wIndex < wSize ; ++ wIndex )
  //  while( c_iterator !=
  //    mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.end()
  //    )
  //  {
  //    //if( model.m_cpucoredata.m_stdsetvoltageandfreqDefault..find()
  //    wFreq = c_iterator->m_wFreqInMHz ;
  //    fVoltage = c_iterator->m_fVoltageInVolt ;
  //    DOMElement * p_dom_elementFreqnVolt = p_dom_document->createElement(
  //      XERCES_STRING_FROM_ANSI_STRING("freq_and_voltage") );
  //    mp_dom_elementRoot->appendChild(p_dom_elementFreqnVolt);
  //    p_dom_elementFreqnVolt->setAttribute(
  //      XERCES_STRING_FROM_ANSI_STRING("frequency_in_MHz") , //attribute name
  //      XERCES_STRING_FROM_ANSI_STRING( to_stdstring<WORD>(wFreq).c_str() ) //attribute value
  //      );
  //    p_dom_elementFreqnVolt->setAttribute(
  //      XERCES_STRING_FROM_ANSI_STRING("max_voltage_in_Volt") , //attribute name
  //      XERCES_STRING_FROM_ANSI_STRING( to_stdstring<float>(fVoltage).c_str() ) //attribute value
  //      );
  //    //if( stdsetFrequenciesStoredInFile.find(wFreq ) !=
  //    //  stdsetFrequenciesStoredInFile.end()
  //    //  )
  //    //{
  //    //  //AddToDOM(
  //    //  //p_dom_document->
  //    //}
  //    ++ c_iterator ;
  //  }
  //}

  void XercesConfigurationHandler::GetDOM_XPathResultForFrequencies()
  {
    BYTE retval ;
    // "/*": the root element independent of the name
    //  (every well-formed XML document has exactly 1 root element)
    //Use "[]" rather than "char *" to avoid g++ Linux compiler warning
    // "deprecated conversion from string constant to ‘char*’ "
    //TODO use wchar_t literal (this can avoid "XMLString::transcode(...)"
    char archXPath [] = "/*/freq_and_voltage" ;
    //DOMLSParser * p_dom_ls_parser ;
    //DOMImplementation * p_dom_implementation ;

    XMLCh * p_xmlchXpath = XERCES_CPP_NAMESPACE::XMLString::transcode(
      archXPath);
    try
    {
      XERCES_CPP_NAMESPACE::DOMXPathNSResolver * p_dom_xpath_ns_resolver =
        mp_dom_document->createNSResolver( mp_dom_elementRoot);
      //DOMXPathResult * p_domxpathresult = p_dom_document->evaluate(
      PossiblyReleaseDOM_XPathResult() ;
      mp_domxpathresult = mp_dom_document->
        //http://xerces.apache.org/xerces-c/apiDocs-3/
        // classDOMXPathEvaluator.html#8785175b7f816234978a931a60000039:
        // DOMXPathException, DOMException
        evaluate(
        p_xmlchXpath,
        mp_dom_elementRoot,
        p_dom_xpath_ns_resolver,
        XERCES_CPP_NAMESPACE::DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE,
        NULL
        );
      LOGN("after DOMDOc::evaluate")
    }
    catch(const XERCES_CPP_NAMESPACE::DOMXPathException &
      cr_xercesc_dom_xpath_exception)
    {
      LOGN("GetDOM_XPathResultForFrequencies: DOMXPathException. "
        "An error occurred during processing of the "
        "XPath expression." // Msg is:"
        << //XercesHelper::ToStdString( cr_xercesc_dom_xpath_exception.
          //getMessage() )
          Xerces::ToStdString( cr_xercesc_dom_xpath_exception.getMessage() )
        )
//      retval = 4;
    }
    catch(const XERCES_CPP_NAMESPACE::DOMException & cr_xercesc_dom_exception)
    {
      LOGN("GetDOM_XPathResultForFrequencies: "
        "An error occurred during processing of the XPath expression: "
        << //XercesHelper::ToStdString( cr_xercesc_dom_exception.getMessage() )
          Xerces::ToStdString( cr_xercesc_dom_exception.getMessage() )
        )
        retval = 4;
    }
    XERCES_CPP_NAMESPACE::XMLString::release(&p_xmlchXpath);
    //mp_domxpathresult->release();
  }

  void XercesConfigurationHandler::GetFreqnVoltDOMelement(
    WORD wFreqInMHz
    , XERCES_CPP_NAMESPACE::DOMElement * & rp_dom_element
    )
  {
    ////mp_dom_document->getElementById(
    //std::map<WORD,WORD>::const_iterator iter =
    //  m_stdmapFreqInMHzInDOMtree2DOMindex.find(
    //  citer_stdset_voltageandfreq->m_wFreqInMHz) ;
    //if( iter != m_stdmapFreqInMHzInDOMtree2DOMindex.end() )
    //  mp_dom_elementFreqnVolt = mp_dom_elementRoot->

    BYTE retval ;
    rp_dom_element = NULL ;
    // "/*": the root element independent of the name
    //  (every well-formed XML document has exactly 1 root element)
    //Use "[]" rather than "char *" to avoid g++ Linux compiler warning
    // "deprecated conversion from string constant to ‘char*’ "
    //TODO use wchar_t literal (this can avoid "XMLString::transcode(...)"
    char archXPath [] = "/*/freq_and_voltage" ;
    //DOMLSParser * p_dom_ls_parser ;
    //DOMImplementation * p_dom_implementation ;

    XMLCh * p_xmlchXpath = XERCES_CPP_NAMESPACE::XMLString::transcode(
      archXPath);
    XMLCh * p_xmlchAttributeName ;
    try //DOMXPathException, DOMException
    {
      XERCES_CPP_NAMESPACE::DOMXPathNSResolver * p_dom_xpath_ns_resolver =
        mp_dom_document->createNSResolver( mp_dom_elementRoot);
      //DOMXPathResult * p_domxpathresult = p_dom_document->evaluate(
      PossiblyReleaseDOM_XPathResult() ;
      mp_domxpathresult = mp_dom_document->
        //http://xerces.apache.org/xerces-c/apiDocs-3/
        // classDOMXPathEvaluator.html#8785175b7f816234978a931a60000039:
        // DOMXPathException, DOMException
        evaluate(
        p_xmlchXpath,
        mp_dom_elementRoot,
        p_dom_xpath_ns_resolver,
        //Xerces .hpp :
        //"snapshot list of nodes that will be in original document order.
        //Document modification does not invalidate the snapshot but may mean
        //that reevaluation would not yield the same snapshot and nodes in the
        //snapshot may have been altered, moved, or removed from the document."
        XERCES_CPP_NAMESPACE::DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE,
        NULL
        );

      XMLSize_t nNumXMLelements =
        //http://xerces.apache.org/xerces-c/apiDocs-2/classDOMXPathResult.html:
        //"XPathException   TYPE_ERR: raised if resultType is not
        //UNORDERED_NODE_SNAPSHOT_TYPE or ORDERED_NODE_SNAPSHOT_TYPE. "
        mp_domxpathresult->getSnapshotLength();
      XERCES_CPP_NAMESPACE::DOMNamedNodeMap * p_dom_namednodemap ;
      XERCES_CPP_NAMESPACE::DOMNode * p_domnodeAttribute ;
      p_xmlchAttributeName = XERCES_CPP_NAMESPACE::XMLString::transcode(
        "frequency_in_MHz");
      const XMLCh * cp_xmlchAttrValue ;
      DWORD dwValue ;
      XercesAttributesHelper xerces_attributes_helper ;

      //Iterate over all "freq_and_voltage" XML elements.
      for(XMLSize_t i = 0; i < nNumXMLelements ; i++)
      {
        //Set XML element number "i".
        mp_domxpathresult->
          //http://xerces.apache.org/xerces-c/apiDocs-2/classDOMXPathResult.html:
          // "XPathException  TYPE_ERR: raised if resultType is not
          // UNORDERED_NODE_SNAPSHOT_TYPE or ORDERED_NODE_SNAPSHOT_TYPE. "
          snapshotItem(i);
        //theSerializer->write(p_domxpathresult->getNodeValue(),
        //  theOutputDesc);
        //Get all attributes for the current XML element.
        p_dom_namednodemap = mp_domxpathresult->getNodeValue()->
          getAttributes() ;
        //for( XMLSize_t xmlsize_tAttIdx = 0 ; p_dom_namednodemap->getLength() ;
        //    ++ xmlsize_tAttIdx )
        //{
        //All attributes for the XML element number "i".
          p_domnodeAttribute = p_dom_namednodemap->getNamedItem(
            //"frequency_in_MHz"
            p_xmlchAttributeName ) ;
          //Attribute exists.
          if ( p_domnodeAttribute )
          {
            cp_xmlchAttrValue = p_domnodeAttribute->
              //"@exception DOMException"
              getNodeValue() ;
            std::string stdstr =//XercesHelper::ToStdString(cp_xmlchAttrValue) ;
              Xerces::ToStdString(cp_xmlchAttrValue) ;
            //cp_xmlchAttrValue = cp_xmlchAttrValue ;
            xerces_attributes_helper.ToDWORD( stdstr , & dwValue ) ;
            if( dwValue == wFreqInMHz )
              rp_dom_element = (XERCES_CPP_NAMESPACE::DOMElement *)
                mp_domxpathresult->
                //"@exception DOMXPathException"
                getNodeValue() ;
            //m_stdmapFreqInMHzInDOMtree2DOMindex.insert(
            //  std::pair<WORD,WORD> ( dwValue, i ) ) ;
          }
        //}
      }
      //XMLString::release( & p_xmlchAttributeName );
    }
    catch(const XERCES_CPP_NAMESPACE::DOMXPathException &
        cr_xercesc_dom_xpath_exception)
    {
      LOGN( "GetFreqnVoltDOMelement:"
        "An error occurred during processing of the XPath expression. Msg is:"
        << //XercesHelper::ToStdString(cr_xercesc_dom_xpath_exception.
          //getMessage() )
          Xerces::ToStdString(cr_xercesc_dom_xpath_exception.getMessage() )
        )
        retval = 4;
    }
    catch(const XERCES_CPP_NAMESPACE::DOMException & cr_xercesc_dom_exception)
    {
      LOGN( "GetFreqnVoltDOMelement:An error occurred during processing of the "
        "XPath expression. Msg is:"
        << //XercesHelper::ToStdString(cr_xercesc_dom_exception.getMessage() )
          Xerces::ToStdString(cr_xercesc_dom_exception.getMessage() )
        )
        retval = 4;
    }
    XERCES_CPP_NAMESPACE::XMLString::release( & p_xmlchAttributeName );
    XERCES_CPP_NAMESPACE::XMLString::release( & p_xmlchXpath );
//    mp_domxpathresult->release();
  }

  bool XercesConfigurationHandler::PossiblyAddOrModifyVoltages(
    const std::set<VoltageAndFreq> & r_stdsetvoltageandfreq
    , //e.g. "max_voltage_in_Volt"
    const char * const cpc_XMLAttrName
    //"true": only look if there would be changes made (useful to check
    // for the purpose: if there are changes then ask the user whether to
    //save the changes.
    , bool bOnlySimulate
    )
  {
    LOGN("begin of PossiblyAddVoltages(" << cpc_XMLAttrName << ")")
    bool bDOMtreeDifferedFromModelData = false ;

    bool bCreateVoltageAttribute = false ;
    XERCES_CPP_NAMESPACE::DOMElement * p_dom_elementFreqnVolt = NULL ;
    XERCES_CPP_NAMESPACE::DOMNamedNodeMap * p_dom_namednodemap ;
    XERCES_CPP_NAMESPACE::DOMNode * p_domnodeAttribute ;
    mp_dom_elementFreqnVolt = NULL ;
    float fVoltageFromFile ;
    std::map<WORD,WORD>::const_iterator c_iterFrequenciesStoredInDOMtree ;
    std::set<VoltageAndFreq>::const_iterator citer_stdset_voltageandfreq =
      r_stdsetvoltageandfreq.begin() ;
    std::string stdstr ;
    WORD wFreq ;
    const XMLCh * cp_xmlchAttrName ;
    //While the iterator is valid.
    while( citer_stdset_voltageandfreq !=
      r_stdsetvoltageandfreq.end()
      )
    {
      mp_dom_elementFreqnVolt = NULL ;
      bCreateVoltageAttribute = false ;
      //DOMElement * p_dom_elementFreqnVolt ;
      //if( model.m_cpucoredata.m_stdsetvoltageandfreqDefault..find()
      wFreq = citer_stdset_voltageandfreq->m_wFreqInMHz ;
      c_iterFrequenciesStoredInDOMtree =
        m_stdmapFreqInMHzInDOMtree2DOMindex.find(wFreq ) ;
      //The frequency does not exist in the file.
      if( c_iterFrequenciesStoredInDOMtree ==
          m_stdmapFreqInMHzInDOMtree2DOMindex.end()
        )
      {
        LOGN("The frequency " << wFreq << " does not exist in the DOM tree.")
        //AddToDOM(
        //p_dom_document->
        //wFreq = c_iterator->m_wFreqInMHz ;
        //DOMElement * p_dom_elementFreqnVolt = p_dom_document->
        if( ! bOnlySimulate )
        {
          p_dom_elementFreqnVolt = AddFrequencyToDOMtree(wFreq) ;
          mp_dom_elementFreqnVolt = p_dom_elementFreqnVolt ;
        }
        bDOMtreeDifferedFromModelData = true ;
        m_bDOMtreeDiffersOrDifferedFromModelData = true ;
        bCreateVoltageAttribute = true ;
        //p_dom_namednodemap = NULL ;
      }
      else //XML element with "freq_and_voltage" exists.
      {
        LOGN("The frequency " << wFreq << " existing in program memory also "
          "exists in the DOM tree.")
//        const XMLCh * cp_xmlchAttrName ;
        if( citer_stdset_voltageandfreq->m_fVoltageInVolt != 0.0 )
        {
          LOGN("voltage <> 0")
          //If config. file already exists.
          if( mp_domxpathresult )
          {
            LOGN("DOM XPath result <> NULL")
//            DOMNamedNodeMap * p_dom_namednodemap ;
            //http://xerces.apache.org/xerces-c/apiDocs-2/classDOMXPathResult.html:
            //"If index is greater than or equal to the number of nodes in the
            //list, this method returns null."
            //"XPathException  TYPE_ERR: raised if resultType is not
            //UNORDERED_NODE_SNAPSHOT_TYPE or ORDERED_NODE_SNAPSHOT_TYPE. "
            mp_domxpathresult->snapshotItem(
              c_iterFrequenciesStoredInDOMtree->second);
            p_dom_namednodemap = mp_domxpathresult->getNodeValue()->
              getAttributes() ;
            p_dom_elementFreqnVolt = (XERCES_CPP_NAMESPACE::DOMElement * )
              mp_domxpathresult->getNodeValue() ;
            mp_dom_elementFreqnVolt = p_dom_elementFreqnVolt ;
            if( p_dom_namednodemap )
            {
//              DOMNode * p_domnodeAttribute ;
              cp_xmlchAttrName = XERCES_STRING_FROM_ANSI_STRING(
                cpc_XMLAttrName) ;
              p_domnodeAttribute = p_dom_namednodemap->getNamedItem(
                //cp_xmlchAttrName
                XERCES_STRING_FROM_ANSI_STRING(cpc_XMLAttrName) ) ;
              ////Attribute exists.
              if ( p_domnodeAttribute //&& c_iterator->m_fVoltageInVolt !=
                )
              {
                float fVoltageFromProgramMemory = citer_stdset_voltageandfreq->
                    m_fVoltageInVolt ;
                stdstr = //XercesHelper::ToStdString(
                  //p_domnodeAttribute->getNodeValue() ) ;
                Xerces::ToStdString(p_domnodeAttribute->getNodeValue() ) ;
                from_stdstring<float>( fVoltageFromFile ,
                  stdstr
                  //, & fVoltage
                  ) ;
                LOGN("attribute \"" << cpc_XMLAttrName << "\" with value \""
                  << fVoltageFromFile << "\" exists for freq "
                  << wFreq << " in DOM tree")
//                float fVoltageFromFile ;
                //DOMElement * p_dom_elementFreqnVolt =
                mp_dom_elementFreqnVolt =
                  (XERCES_CPP_NAMESPACE::DOMElement *) mp_domxpathresult->
                  getNodeValue() ;
                if( fVoltageFromFile != fVoltageFromProgramMemory )
                {
                  m_bDOMtreeDiffersOrDifferedFromModelData = true ;
                  LOGN("for attribute  \"" << cpc_XMLAttrName
                    << "\": value from program memory: \"" <<
                    fVoltageFromProgramMemory
                    << "\" does not match value from DOM tree" )
                  SetVoltageFromIandCModelData(
                    cpc_XMLAttrName,
                    bDOMtreeDifferedFromModelData ,
                    fVoltageFromProgramMemory ) ;
                }
              }
              else //Attribute does not exist in file/ DOM tree.
              {
                LOGN("attribute " << cpc_XMLAttrName
                  << " does NOT exist for freq"
                  << wFreq << " in DOM tree")
                mp_dom_elementFreqnVolt =
                  (XERCES_CPP_NAMESPACE::DOMElement *) mp_domxpathresult->
                  getNodeValue() ;
                bCreateVoltageAttribute = true ;
              }
            }
          }
          else
          {
            LOGN("DOM XPath result == NULL")
            bCreateVoltageAttribute = true ;
          }
        }
      }
      if( bCreateVoltageAttribute )
      {
        m_bDOMtreeDiffersOrDifferedFromModelData = true ;
        //Used in SetVoltageFromIandCModelData(...) for
        //mp_dom_elementFreqnVolt->setAttribute(...) .
        if( //p_dom_elementFreqnVolt
            mp_dom_elementFreqnVolt
            )
        {
          //DOMElement * p_dom_elementFreqnVolt =
          //  (DOMElement *) p_domxpathresult->getNodeValue() ;
//          if( ! bOnlySimulate )
//          {
//            p_dom_elementFreqnVolt->setAttribute(
//              //cp_xmlchAttrName , //attribute name
//              XERCES_STRING_FROM_ANSI_STRING(cpc_XMLAttrName) ,
//              //attribute value
//              XERCES_STRING_FROM_ANSI_STRING( to_stdstring<float>(
//                citer_stdset_voltageandfreq->m_fVoltageInVolt ).c_str() )
//              );
//          }
//          bDOMtreeModified = true ;
          SetVoltageFromIandCModelData(
            cpc_XMLAttrName,
            bDOMtreeDifferedFromModelData ,
            citer_stdset_voltageandfreq->m_fVoltageInVolt
            ) ;
        }
      }
      ++ citer_stdset_voltageandfreq ;
    }//while
    return bDOMtreeDifferedFromModelData ;
  }

  void XercesConfigurationHandler::PossiblyReleaseDOM_XPathResult()
  {
    if( mp_domxpathresult )
    {
      LOGN("before releasing DOM Xpath result")
      mp_domxpathresult->release() ;
      mp_domxpathresult = NULL ;
    }
  }

  BYTE XercesConfigurationHandler::readXMLfileDOM(
//    const char * p_chFullXMLFilePath
//    , DOMDocument * & p_dom_document
//    , DOMLSParser * & rp_dom_ls_parser
//    , DOMImplementation * & p_dom_implementation
    )
  {
    BYTE byRet = 0 ;
    XMLCh tempStr[100];
    XERCES_CPP_NAMESPACE::XMLString::transcode("LS", tempStr, 99);
    //DOMImplementation *
    mp_dom_implementation = XERCES_CPP_NAMESPACE::DOMImplementationRegistry::
      //"Return the first registered implementation that has the desired features,
      // or null if none is found."
      getDOMImplementation(tempStr);
    if( mp_dom_implementation )
    {
      //DOMLSParser *
      mp_dom_ls_parser = ( (XERCES_CPP_NAMESPACE::DOMImplementationLS * )
        mp_dom_implementation)->createLSParser(
          XERCES_CPP_NAMESPACE::DOMImplementationLS::MODE_SYNCHRONOUS, 0 );
      //// optionally you can set some features on this builder
      //if (rp_dom_ls_parser->getDomConfig()->canSetParameter(XMLUni::fgDOMValidate, true))
      //    rp_dom_ls_parser->getDomConfig()->setParameter(XMLUni::fgDOMValidate, true);
      //if (rp_dom_ls_parser->getDomConfig()->canSetParameter(XMLUni::fgDOMNamespaces, true))
      //    rp_dom_ls_parser->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, true);
      //if (rp_dom_ls_parser->getDomConfig()->canSetParameter(XMLUni::fgDOMDatatypeNormalization, true))
      //    rp_dom_ls_parser->getDomConfig()->setParameter(XMLUni::fgDOMDatatypeNormalization, true);

      //// optionally you can implement your DOMErrorHandler (e.g. MyDOMErrorHandler)
      //// and set it to the builder
      //MyDOMErrorHandler* errHandler = new myDOMErrorHandler();
      //rp_dom_ls_parser->getDomConfig()->setParameter(XMLUni::fgDOMErrorHandler, errHandler);

      //DOMDocument * p_dom_document = 0;
      try
      {
        mp_dom_document =
          //Returns NULL if the XML file could not be read.
          mp_dom_ls_parser->
          //"DOMException", "DOMLSException"
          parseURI( //p_chFullXMLFilePath
            mpc_chFullXMLFilePath );
        LOGN( "File \"" << mpc_chFullXMLFilePath << "\" successfully parsed.")
        //If parseURI throws an exception this instruction is not executed.
        byRet = 1 ;
      }
      catch (const XERCES_CPP_NAMESPACE::XMLException &
        cr_xercesc_xml_exception)
      {
        char * message = XERCES_CPP_NAMESPACE::XMLString::transcode(
          cr_xercesc_xml_exception.getMessage());
        LOGN( "XMLException. message is: \n" << message << "\n")
        XERCES_CPP_NAMESPACE::XMLString::release(&message);
//          return FAILURE ;
      }
      catch(const XERCES_CPP_NAMESPACE::DOMLSException &
        cr_xercesc_domls_exception )
      {
        LOGN( "DOMLSException. message is: \n" << //XercesHelper::ToStdString(
          //cr_xercesc_domls_exception.getMessage() )
          Xerces::ToStdString(cr_xercesc_domls_exception.getMessage() )
          )
          //          return FAILURE ;
      }
      catch (const XERCES_CPP_NAMESPACE::DOMException &
        cr_xercesc_dom_exception)
      {
        char* message = XERCES_CPP_NAMESPACE::XMLString::transcode(
          cr_xercesc_dom_exception.msg);
        LOGN( "DOMException. message is: \n" << message << "\n" )
        XERCES_CPP_NAMESPACE::XMLString::release(&message);
//          return FAILURE ;
      }
      catch (...)
      {
        LOGN( "Unexpected Exception \n" )
//          return FAILURE ;
      }
    } //    if( mp_dom_implementation )
    else
    {
      LOGN("readXMLfileDOM: DOMImplementationRegistry returned NULL")
    }
    //delete errHandler;
    return byRet;
  }

  BYTE * XercesConfigurationHandler::SerializeConfigToMemoryBuffer(
    DWORD & r_dwByteSize )
  {
    LOGN("XercesConfigurationHandler::SerializeConfigToMemoryBuffer")
    BYTE * ar_by = NULL ;
    mp_dom_implementation = XERCES_CPP_NAMESPACE::DOMImplementationRegistry::
      //"Return the first registered implementation that has the desired features,
      // or null if none is found."
      getDOMImplementation(
        //Convert/ cast to "const XMLCh *" to avoid Linux g++ error
        //"no matching function for call to ‘xercesc_3_0::
        //DOMImplementationRegistry::getDOMImplementation(const wchar_t [3])’ "
        (const XMLCh *)
        L"LS" );
//    PossiblyAddVoltages(
//      //true: do not change, only test if it would be changed.
//      false //bool bTest
//      ) ;
    if( ! CreateDOMtree() )
    {
      XERCES_CPP_NAMESPACE::MemBufFormatTarget mem_buf_format_target ;
      WriteDOM(
        mp_dom_elementRoot
  //            , p_chFullXMLFilePath
  //            , p_dom_implementation
        , mem_buf_format_target
        ) ;
      XMLSize_t xmlsize_tBufferLength = mem_buf_format_target.getLen() ;
      const XMLByte * cp_xmlbyteBuffer = mem_buf_format_target.getRawBuffer() ;
      //"user should make its own copy of the
      //  * returned buffer if it intends to keep it independent on the
      //    state of the
      //  * MemBufFormatTarget."!"
      ar_by = new BYTE[ xmlsize_tBufferLength ] ;
      if( ar_by ) //If <> NULL => Allocating the array succeeded.
      {
        memcpy( ar_by, cp_xmlbyteBuffer, xmlsize_tBufferLength ) ;

        //Both the size and the array address must be set thread safe/
        // insed a critical section.
        r_dwByteSize = xmlsize_tBufferLength ;
      }
      mp_dom_document->release() ;
      mp_dom_document = NULL ;
    }
    return ar_by ;
  }

  void XercesConfigurationHandler::SetVoltageFromIandCModelData(
    const char * const cpc_XMLAttrName ,
    bool & bDOMtreeModified ,
    float fVoltageFromModelData
    )
  {
    if( ! m_bOnlySimulate )
    {
      LOGN("before setting attribute value " <<
       fVoltageFromModelData
        << "for attribute name "        << cpc_XMLAttrName
        << " for freq "
        << //XercesHelper::ToStdString( mp_dom_elementFreqnVolt->getAttribute(
          Xerces::ToStdString( mp_dom_elementFreqnVolt->getAttribute(
          //Avoid g++ warning "no matching function for call to
          //‘xercesc_3_0::DOMElement::getAttribute(const wchar_t [17])’ "
          (const XMLCh * ) FREQUENCY_IN_MHZ_WCHAR_LITERAL )
          )
        )
      try
      {
        mp_dom_elementFreqnVolt->setAttribute(
          //cp_xmlchAttrName , //attribute name
          XERCES_STRING_FROM_ANSI_STRING(cpc_XMLAttrName) ,
          //attribute value
          XERCES_STRING_FROM_ANSI_STRING( to_stdstring<float>(
            fVoltageFromModelData ).c_str() )
          );
        LOGN("setting attribute value "
          << fVoltageFromModelData
          << "for attribute name "
          << cpc_XMLAttrName
          << " succeeded" )
        bDOMtreeModified = true ;
        m_bAtLeast1SuccessfullDOMtreeModification = true ;
      }
      catch(const XERCES_CPP_NAMESPACE::DOMException &
        cr_xercesc_dom_exception )
      {
        m_bAtLeast1FailedDOMtreeModification = true ;
        LOGN("setting attribute value "
          << fVoltageFromModelData
          << "for attribute name "
          << cpc_XMLAttrName
          << " failed" )
      }
    }
  }

  short XercesConfigurationHandler::WriteDOM(
    XERCES_CPP_NAMESPACE::DOMNode * p_dom_node
//    , const char * const cpc_chFilePath
    //For creating a DOMLSOutput instance via createLSOutput().
//    , DOMImplementation * p_dom_implementation
    //May be LocalFileFormatTarget->write to a file, MembufTarget etc.
    , XERCES_CPP_NAMESPACE::XMLFormatTarget & r_xmlformattarget
    )
  {
    LOGN("WriteDOM")
    if( mp_dom_implementation )
    {
      XERCES_CPP_NAMESPACE::DOMLSSerializer * p_dom_ls_serializer =
        ( ( XERCES_CPP_NAMESPACE::DOMImplementationLS *)
        mp_dom_implementation )->createLSSerializer();
      XERCES_CPP_NAMESPACE::DOMConfiguration * p_dom_configuration =
        p_dom_ls_serializer->getDomConfig();
      //See http://www.mail-archive.com/c-users@xerces.apache.org/msg03106.html:
      if( p_dom_configuration->canSetParameter(
        XERCES_CPP_NAMESPACE::XMLUni::fgDOMWRTFormatPrettyPrint, //true
          false)
        )
        p_dom_configuration->setParameter(
          XERCES_CPP_NAMESPACE::XMLUni::fgDOMWRTFormatPrettyPrint,
          //true
          false );

//      XMLFormatTarget * p_xml_format_targetFile = new LocalFileFormatTarget (
//        //cpc_chFilePath
//        mpc_chFullXMLFilePath
//        );
      XERCES_CPP_NAMESPACE::DOMLSOutput * p_domls_output = (
       (XERCES_CPP_NAMESPACE::DOMImplementationLS *)
        mp_dom_implementation)->createLSOutput();
      p_domls_output->setByteStream(//p_xml_format_targetFile
        & r_xmlformattarget);
      try
      {
          // do the serialization through DOMLSSerializer::write();
          p_dom_ls_serializer->write(p_dom_node, p_domls_output);
      }
      catch (const XERCES_CPP_NAMESPACE::XMLException &
        cr_xercesc_xml_exception)
      {
        char * message = XERCES_CPP_NAMESPACE::XMLString::transcode(
          cr_xercesc_xml_exception.getMessage());
        LOGN(  "XMLException Exception message is: \n"
             << message << "\n" )
        XERCES_CPP_NAMESPACE::XMLString::release(&message);
        return -1;
      }
      catch (const XERCES_CPP_NAMESPACE::DOMException &
        cr_xercesc_dom_exception)
      {
        char * message = XERCES_CPP_NAMESPACE::XMLString::transcode(
          cr_xercesc_dom_exception.msg);
        LOGN( "DOMException Exception message is: \n"
             << message << "\n" )
        XERCES_CPP_NAMESPACE::XMLString::release(&message);
        return -1;
      }
      catch (...)
      {
        LOGN ( "Unexpected Exception \n"  )
        return -1;
      }
      p_domls_output->release();
      p_dom_ls_serializer->release();
      //delete myErrorHandler;
      //delete myFilter;
//      delete p_xml_format_targetFile;
    }
    return 1 ;
  }

  //Put the "test for change" and the "change" code into the same function
  //because both depends (exactly) on the same attributes.
  bool XercesConfigurationHandler::TestIfCfgIsChangedOrChangeCfg(
    //true: do not change, only test if it would be changed.
    bool bTest
    )
  {
    //Reset because this method may be called more than once for the same
    // XercesConfigurationHandler object.
    bool bDOMtreeModified = false ;
    m_bAtLeast1FailedDOMtreeModification = false ;
    m_bAtLeast1SuccessfullDOMtreeModification = false ;
    m_bDOMtreeDiffersOrDifferedFromModelData = false ;
    m_stdmapFreqInMHzInDOMtree2DOMindex.clear() ;
    if( bTest )
      LOGN( "testing if performance states config. is changed")
    else
      LOGN( "changing performance states config. if differences" )
    //mp_dom_document = p_dom_document ;
    try
    {
      XERCES_CPP_NAMESPACE::DOMXPathNSResolver * p_dom_xpath_ns_resolver =
        BuildStdmapFreqInMHzInDOMtree2DOMindex(
//          m_stdmapFreqInMHzInDOMtree2DOMindex
          ) ;
      //If BuildStdmapFreqInMHzInDOMtree2DOMindex succeeded.
      if( mp_domxpathresult )
      {
  //        bool bCreateMaxVoltageAttribute ;

        //WORD wSize = model.m_cpucoredata.m_stdsetvoltageandfreqDefault.size() ;
  //        WORD wFreq ;
        //for( WORD wIndex = 0 ; wIndex < wSize ; ++ wIndex )

        //if( PossiblyAddDefaultVoltages() )
        //  bDOMtreeModified = true ;
        //if( PossiblyAddLowestStableVoltages() )
        //  bDOMtreeModified = true ;
        //if( PossiblyAddWantedVoltages() )
        //  bDOMtreeModified = true ;
        bDOMtreeModified = PossiblyAddVoltages( bTest ) ;
        if( //bDOMtreeModified
          m_bDOMtreeDiffersOrDifferedFromModelData )
        {
          LOGN( "the DOM tree (from file)" //is modified"
            "differs or differed from the in-program data")
          if( ! bTest )
          {
            XERCES_CPP_NAMESPACE::LocalFileFormatTarget localfileformattarget(
              mpc_chFullXMLFilePath
              );
            WriteDOM(
              mp_dom_elementRoot
  //            , p_chFullXMLFilePath
  //            , p_dom_implementation
              , localfileformattarget
              ) ;
          }
        }
        else
        {

        }
      }
//      mp_dom_document->release();
//      mp_domxpathresult->release();
      //This finally writes to the file.
      p_dom_xpath_ns_resolver->release ();
    }
    catch(const XERCES_CPP_NAMESPACE::DOMXPathException &
      cr_xercesc_dom_xpath_exception)
    {
      LOGN( "TestIfCfgIsChangedOrChangeCfg:"
        "An error occurred during processing of the XPath expression. "
        "Msg is:"
        << //XercesHelper::ToStdString( cr_xercesc_dom_xpath_exception.
          //getMessage() )
          Xerces::ToStdString( cr_xercesc_dom_xpath_exception.getMessage() )
        )
//        retval = 4;
    }
    catch(const XERCES_CPP_NAMESPACE::DOMException & cr_xercesc_dom_exception)
    {
      LOGN( "TestIfCfgIsChangedOrChangeCfg:"
        "An error occurred during processing of the XPath expression. "
        "Msg is:"
        << //XercesHelper::ToStdString( cr_xercesc_dom_exception.getMessage() )
          Xerces::ToStdString( cr_xercesc_dom_exception.getMessage() )
        )
//        retval = 4;
    }
    return //bDOMtreeModified ;
      m_bDOMtreeDiffersOrDifferedFromModelData ;
  }

  //Purpose: this should called by all fcts that change the voltage or test for
  //their changes. This is good if something is changed (e.g. if attributes
  //are added/ removed)
  //So it can be done central (do not repeat principle)
  bool XercesConfigurationHandler::PossiblyAddVoltages(
    //true: do not change, only test if it would be changed.
    bool bTest
    )
  {
    bool bDOMtreeModified = false ;
    m_bOnlySimulate = bTest ;
    LOGN("begin of PossiblyAddVoltages")
    PossiblyReleaseDOM_XPathResult() ;
    //A DOM tree may already exist( if read from an XML file). So build a map
    //for this case.
    BuildStdmapFreqInMHzInDOMtree2DOMindex() ;
    //Prevent insertions or deletions while accessing the STL container
    //for voltages.
    mp_model->m_cpucoredata.m_wxcriticalsection.Enter() ;

    if( PossiblyAddOrModifyVoltages(
      mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault
      , "max_voltage_in_Volt"
      , bTest )
      )
    {
      bDOMtreeModified = true ;
      //The DOM tree was modified: so a snapshot must be taken.
      BuildStdmapFreqInMHzInDOMtree2DOMindex() ;
    }
    if( PossiblyAddOrModifyVoltages(
      mp_model->m_cpucoredata.m_stdsetvoltageandfreqLowestStable
      , "min_voltage_in_Volt"
      , bTest )
      )
    {
      bDOMtreeModified = true ;
      //The DOM tree was modified: so a snapshot must be taken.
      BuildStdmapFreqInMHzInDOMtree2DOMindex() ;
    }
    if( PossiblyAddOrModifyVoltages(
      mp_model->m_cpucoredata.m_stdsetvoltageandfreqWanted
      , "voltage_in_Volt"
      , bTest )
      )
      bDOMtreeModified = true ;
    mp_model->m_cpucoredata.m_wxcriticalsection.Leave() ;
    return bDOMtreeModified ;
  }

  BYTE XercesConfigurationHandler::CreateDOMtree()
  {
    try //DOMException by createDocument(...)
    {
      //http://www.w3.org/TR/1998/REC-xml-19980210#NT-Nmtoken:
      //remove invalid tokens like "("
      //std::string str = RemoveInvalidTokens(
      //  r_strPstateSettingsFileName.c_str() ) ;
      //mp_userinterface->GetTextFromUser(_T("Enter the root XML element name:")) ;
      //DOMDocument * p_dom_document = p_dom_implementation->createDocument(
      mp_dom_document = mp_dom_implementation->
        //"@exception DOMException"
        createDocument(
        0, // root element namespace URI.
        // root element name
        //XERCES_STRING_FROM_ANSI_STRING( str.c_str() ) ,
        XERCES_STRING_FROM_ANSI_STRING("CPU") ,
        0 // document type object (DTD).
        );
      if( mp_dom_document )
      {
        //DOMElement * p_dom_elementRoot = p_dom_document->getDocumentElement();
        mp_dom_elementRoot = mp_dom_document->getDocumentElement();
        //AppendDefaultVoltages(p_dom_elementRoot) ;
        //AppendMiniumVoltages(p_dom_elementRoot) ;
        //AppendWantedVoltages(p_dom_elementRoot) ;
  //          bool bDOMtreeModified = false ;
        //if( PossiblyAddDefaultVoltages() )
        //  bDOMtreeModified = true ;
        //if( PossiblyAddLowestStableVoltages() )
        //  bDOMtreeModified = true ;
        //if( PossiblyAddWantedVoltages() )
        //  bDOMtreeModified = true ;
        PossiblyAddVoltages(
          //if true: do not change, only test if it would be changed.
          false
          ) ;
      }
    }
    catch( const XERCES_CPP_NAMESPACE::DOMException & cr_xercesc_dom_exception)
    {
      //mp_userinterface->Confirm(e.getMessage() ) ;
      LOGN("DOMException while creating p-state DOM tree:"
        << //XercesHelper::ToStdString( cr_xercesc_dom_exception.getMessage() )
          Xerces::ToStdString( cr_xercesc_dom_exception.getMessage() )
        )
      return 1 ;
    }
    return 0 ;
  }

  BYTE XercesConfigurationHandler:://mergeXMLfileDOM(
    MergeWithExistingConfigFile(
    const char * p_chFullXMLFilePath
    , Model & model
    , std::string & r_strPstateSettingsFileName
    )
  {
    //Initialize just to avoid (g++) compiler warning.
    BYTE retval = 0 ;
//    if( m_bXercesSuccessfullyInitialzed )
    {
      //setting/ assigning the value is Important: for readXMLfileDOM() and
      //writeXML(...)
      mpc_chFullXMLFilePath = p_chFullXMLFilePath ;
      //DOMDocument * p_dom_document ;
  //    DOMLSParser * p_dom_ls_parser ;
  //    DOMImplementation * p_dom_implementation ;
      try
      {
  //      XMLPlatformUtils::Initialize();
        readXMLfileDOM(
    //      p_chFullXMLFilePath
    //      , mp_dom_document
    //      , p_dom_ls_parser
    //      , p_dom_implementation
          ) ;

        //The XML file could be read.
        if( mp_dom_document )
        {
          TestIfCfgIsChangedOrChangeCfg(false) ;
          mp_dom_document->release() ;
          mp_dom_document = NULL ;
          //XMLString::release(&p_xmlchXpath);
        }// if( p_dom_document )
        //Is NULL if the XML file could not be read (=may not exist).
        else
        {
          //A clear map causes not to use an existing DOM tree.
          m_stdmapFreqInMHzInDOMtree2DOMindex.clear() ;
          LOGN( "file" << mpc_chFullXMLFilePath << "could not be read")
          //DOMImplementation * p_dom_implementation =
          //  DOMImplementationRegistry::getDOMImplementation(
          //  XERCES_STRING_FROM_ANSI_STRING("Core") );
          if ( //!= NULL
              mp_dom_implementation )
          {
            if( ! CreateDOMtree() )
            {
              XERCES_CPP_NAMESPACE::LocalFileFormatTarget localfileformattarget(
                mpc_chFullXMLFilePath
                );
              //Write the whole XML tree.
              WriteDOM(
                mp_dom_elementRoot
    //            , p_chFullXMLFilePath
    //            , p_dom_implementation
                , localfileformattarget
                ) ;
              mp_dom_document->release() ;
              mp_dom_document = NULL ;
            }
          }
        }
      }
      catch (const XERCES_CPP_NAMESPACE::XMLException &
          cr_xercesc_xml_exception )
      {
        LOG( "MergeWithExistingConfigFile: XMLException:"
          << //XercesHelper::ToStdString( cr_xercesc_xml_exception.getMessage() )
            Xerces::ToStdString( cr_xercesc_xml_exception.getMessage() )
            //<< "\n"
          )
        return 1;
      }
    }
    ////Delete the parser itself. Must be done prior to calling Terminate, below.
    ////This destroys the DOM document DOMDocument got by p_dom_ls_parser->parseURI()
    //p_dom_ls_parser->release();
    //  Delete the parser itself.  Must be done prior to calling Terminate, below.
    //
    //delete p_dom_ls_parser;
    return retval ;
  }

  XercesConfigurationHandler::XercesConfigurationHandler(
    Model * p_model )
    //E.g. for getting an array of all "freq_and_voltage" XML elements.
    :
    m_bAtLeast1SuccessfullDOMtreeModification (false),
    m_bAtLeast1FailedDOMtreeModification(false) ,
    m_bXercesSuccessfullyInitialzed (false) ,
    mp_dom_document( NULL)
    , mp_dom_implementation( NULL)
    , mp_dom_ls_parser (NULL)
    , mp_domxpathresult(NULL)
  {
//    m_bXercesSuccessfullyInitialzed = x86InfoAndControl::InitializeXerces() ;
    m_bXercesSuccessfullyInitialzed = true ;
//    LOGN("Xerces successfully initialized")
    mp_model = p_model ;
  }

  XercesConfigurationHandler::~XercesConfigurationHandler()
  {
    LOGN("~XercesConfigurationHandler")
//    if( mp_dom_ls_parser )
//    {
//      LOGN("Before releasing DOM parser")
//      //Also releases the DOMdocument:
//      //http://xerces.apache.org/xerces-c/apiDocs-3/classDOMLSParser.html
//      // #9fa70302351aecdabddb71c4dfd0d626:
//      //"The parser owns the returned DOMDocument.  It will be deleted
//      // when the parser is released."
//      mp_dom_ls_parser->release() ;
//      LOGN("After releasing DOM parser")
//    }
    //http://xerces.apache.org/xerces-c/apiDocs-3/classDOMLSParser.html
    // #9fa70302351aecdabddb71c4dfd0d626:
    //"The parser owns the returned DOMDocument.  It will be deleted
    // when the parser is released."
    if( mp_dom_document )
    {
      LOGN("before releasing DOM document")
      //http://xerces.apache.org/xerces-c/apiDocs-3/classDOMNode.html:
      //"If this is a document, any nodes it owns (created by
      //DOMDocument::createXXXX()) are also released."
      mp_dom_document->release();
    }
    if( mp_domxpathresult )
    {
      LOGN("before releasing DOMXPathResult")
      mp_domxpathresult->release();
    }
    LOGN("~XercesConfigurationHandler end")
//    x86InfoAndControl::TerminateXerces() ;
  }
