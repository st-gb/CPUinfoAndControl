
////with Linux I can't compile with xerces yet.
////#ifdef WIN32 
//#ifdef _WINDOWS
//  #define COMPILE_WITH_XERCES
//#endif

#ifdef COMPILE_WITH_XERCES
//If not included: compiler error "C1010" with MSVC++.
#include "../stdafx.h"
#include "XMLAccess.hpp"
#include "../global.h" //for if "COMPILE_WITH_XERCES" is defined or not
#include "XercesHelper.hpp" 

	#include <iostream>
	//#include "SAX2Handler.hpp"
	#include "../global.h" //for SUCCESS, FAILURE
	//#include "../Model.hpp"
	
	//Include "xerces...\include" in the "include path" for the following "#include"s.
	#include <xercesc/sax2/SAX2XMLReader.hpp>
	#include <xercesc/sax2/XMLReaderFactory.hpp>
	#include <xercesc/sax2/DefaultHandler.hpp>
	#include <xercesc/util/XMLString.hpp>
  #include <xercesc/dom/DOM.hpp>
  #include <xercesc/dom/DOMElement.hpp>
  #include <xercesc/util/XMLString.hpp>
  #include <xercesc/util/PlatformUtils.hpp>
  #include <xercesc/framework/LocalFileFormatTarget.hpp>

  using namespace std;
  using namespace xercesc;

  //#include <Controller/Logger.hpp>
  #include <Controller/Logger.hpp>
  #include <Controller/stdstring_format.hpp> //for to_strstring()
  #include "../ModelData/ModelData.hpp"
  #include "../UserInterface.hpp"

extern Logger g_logger ;
	
	//To NOT need to prefix the xerces classes with the "xerces::"
	XERCES_CPP_NAMESPACE_USE
	
	using namespace std;

  //void 
  //std::map<WORD,WORD>::const_iterator
  DOMElement *
    XercesConfigurationHandler::AddFrequencyToDOMtree(WORD wFreq)
  {
    std::map<WORD,WORD>::const_iterator c_iterFrequenciesStoredInFile ;
    //DOMElement * p_dom_elementFreqnVolt ;
    mp_dom_elementFreqnVolt = mp_dom_document->
      createElement( X("freq_and_voltage") );
    mp_dom_elementRoot->appendChild( mp_dom_elementFreqnVolt);
    mp_dom_elementFreqnVolt->setAttribute( 
      X("frequency_in_MHz") , //attribute name
      X( to_stdstring<WORD>(wFreq).c_str() ) //attribute value
      );
    //TODO verify if the DOM index is correct.
    c_iterFrequenciesStoredInFile = 
    m_stdmapFreqInMHzInDOMtree2DOMindex.insert(
      std::pair<WORD,WORD>( 
        wFreq, 
        m_stdmapFreqInMHzInDOMtree2DOMindex.size() 
        )
        ).first ;
    if( mp_domxpathresult )
    {
      mp_domxpathresult->release();
    }
    //The number of elements grew by 1. So redo getting the XPath result.
    GetDOM_XPathResultForFrequencies() ;
    return //c_iterFrequenciesStoredInFile ;
      mp_dom_elementFreqnVolt ;
  }

  DOMXPathNSResolver * XercesConfigurationHandler::
    BuildStdmapFreqInMHzInDOMtree2DOMindex(
    std::map<WORD,WORD> & r_stdmapFreqInMHzInDOMtree2DOMindex
    )
  {
    // "/*": das Wurzel-Element unabhaengig vom Namen 
    //  (jedes wohlgeformte XML-Dokument hat genau ein Wurzel-Element)
    char * archXPath = "/*/freq_and_voltage" ; 
    XMLCh * p_xmlchXpath = XMLString::transcode(archXPath);
    //DOMElement * p_dom_elementRoot = p_dom_document->getDocumentElement();
    mp_dom_elementRoot = mp_dom_document->getDocumentElement();

    DOMXPathNSResolver * p_dom_xpath_ns_resolver = mp_dom_document->
      createNSResolver( mp_dom_elementRoot);
    //DOMXPathResult * p_domxpathresult = p_dom_document->evaluate(
    mp_domxpathresult = mp_dom_document->evaluate(
      p_xmlchXpath,
      mp_dom_elementRoot,
      p_dom_xpath_ns_resolver,
      DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE,
      NULL 
      );

    XMLSize_t nDOMqueryResultsetLength = mp_domxpathresult->getSnapshotLength();
    DOMNamedNodeMap * p_dom_namednodemap ;
    DOMNode * p_domnodeAttribute ;
    XMLCh * xmlchAttributeName = XMLString::transcode("frequency_in_MHz");
    const XMLCh * cp_xmlchAttrValue ;
    DWORD dwValue ;
    XercesHelper xerceshelper ;

    //Iterate over all "freq_and_voltage" XML elements.
    for( //XMLSize_t 
      WORD wResultSetIndex = 0; wResultSetIndex < nDOMqueryResultsetLength; 
      wResultSetIndex ++ )
    {
      mp_domxpathresult->snapshotItem(wResultSetIndex);
      //theSerializer->write(p_domxpathresult->getNodeValue(), 
      //  theOutputDesc);
      //Get all attributes for the current XML element.
      p_dom_namednodemap = mp_domxpathresult->getNodeValue()->getAttributes() ;
      //for( XMLSize_t xmlsize_tAttIdx = 0 ; p_dom_namednodemap->getLength() ;
      //    ++ xmlsize_tAttIdx )
      //{
        p_domnodeAttribute = p_dom_namednodemap->getNamedItem( 
          //"frequency_in_MHz" 
          xmlchAttributeName ) ;
        //Attribute exists.
        if ( p_domnodeAttribute )
        {
          cp_xmlchAttrValue = p_domnodeAttribute->getNodeValue() ;
          std::string stdstr = xerceshelper.ToStdString(
            cp_xmlchAttrValue) ;
          //cp_xmlchAttrValue = cp_xmlchAttrValue ;
          xerceshelper.ToDWORD( stdstr , & dwValue ) ;
          m_stdmapFreqInMHzInDOMtree2DOMindex.insert( 
            std::pair<WORD,WORD> ( (WORD) dwValue, wResultSetIndex ) ) ;
        }
      //}
    }
    XMLString::release( & xmlchAttributeName );
    XMLString::release( & p_xmlchXpath);
    return p_dom_xpath_ns_resolver ;
  }

  bool XercesConfigurationHandler::IsConfigurationChanged(
    std::string & r_strPstateSettingsFileName )
  {
    bool bIsConfigurationChanged = false ;
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

    readXMLfileDOM( 
      //p_chFullXMLFilePath 
//      r_strPstateSettingsFileName.c_str()
//      , p_dom_document
//      , p_dom_ls_parser
//      , p_dom_implementation
      ) ;

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

      //XMLString::release(&p_xmlchXpath);
    }// if( p_dom_document )
    //Is NULL if the XML file could not be read (=may not exist).
    else
    {
      CPUcoreData & r_cpucoredata = mp_model->m_cpucoredata ;
      if( r_cpucoredata.m_stdsetvoltageandfreqDefault.size() > 0 )
        bIsConfigurationChanged = true ;
      if( r_cpucoredata.m_stdsetvoltageandfreqLowestStable.size() > 0 )
        bIsConfigurationChanged = true ;
      if( r_cpucoredata.m_stdsetvoltageandfreqWanted.size() > 0 )
        bIsConfigurationChanged = true ;
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
  //      X("freq_and_voltage") );
  //    mp_dom_elementRoot->appendChild(p_dom_elementFreqnVolt);
  //    p_dom_elementFreqnVolt->setAttribute( 
  //      X("frequency_in_MHz") , //attribute name
  //      X( to_stdstring<WORD>(wFreq).c_str() ) //attribute value
  //      );
  //    p_dom_elementFreqnVolt->setAttribute( 
  //      X("max_voltage_in_Volt") , //attribute name
  //      X( to_stdstring<float>(fVoltage).c_str() ) //attribute value
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
   // "/*": das Wurzel-Element unabhaengig vom Namen 
    //  (jedes wohlgeformte XML-Dokument hat genau ein Wurzel-Element)
    char * archXPath = "/*/freq_and_voltage" ; 
    //DOMLSParser * p_dom_ls_parser ;
    //DOMImplementation * p_dom_implementation ;

    XMLCh * p_xmlchXpath = XMLString::transcode(archXPath);
    try
    {
      DOMXPathNSResolver * p_dom_xpath_ns_resolver = mp_dom_document->createNSResolver(
        mp_dom_elementRoot);
      //DOMXPathResult * p_domxpathresult = p_dom_document->evaluate(
      mp_domxpathresult = mp_dom_document->evaluate(
        p_xmlchXpath,
        mp_dom_elementRoot,
        p_dom_xpath_ns_resolver,
        DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE,
        NULL 
        );
    }
    catch(const DOMXPathException& e)
    {
        //XERCES_STD_QUALIFIER cerr << "An error occurred during processing of the XPath expression. Msg is:"
        //    << XERCES_STD_QUALIFIER endl
        //    << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
        retval = 4;
    }
    catch(const DOMException& e)
    {
        //XERCES_STD_QUALIFIER cerr << "An error occurred during processing of the XPath expression. Msg is:"
        //    << XERCES_STD_QUALIFIER endl
        //    << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
        retval = 4;
    }
    XMLString::release(&p_xmlchXpath);
    //mp_domxpathresult->release();
  }

  void XercesConfigurationHandler::GetFreqnVoltDOMelement(
    WORD wFreqInMHz 
    , DOMElement * & rp_dom_element 
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
    // "/*": das Wurzel-Element unabhaengig vom Namen 
    //  (jedes wohlgeformte XML-Dokument hat genau ein Wurzel-Element)
    char * archXPath = "/*/freq_and_voltage" ; 
    //DOMLSParser * p_dom_ls_parser ;
    //DOMImplementation * p_dom_implementation ;

    XMLCh * p_xmlchXpath = XMLString::transcode(archXPath);
    XMLCh * p_xmlchAttributeName ;
    try
    {
      DOMXPathNSResolver * p_dom_xpath_ns_resolver = mp_dom_document->createNSResolver(
        mp_dom_elementRoot);
      //DOMXPathResult * p_domxpathresult = p_dom_document->evaluate(
      mp_domxpathresult = mp_dom_document->evaluate(
        p_xmlchXpath,
        mp_dom_elementRoot,
        p_dom_xpath_ns_resolver,
        DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE,
        NULL 
        );

      XMLSize_t nNumXMLelements = mp_domxpathresult->getSnapshotLength();
      DOMNamedNodeMap * p_dom_namednodemap ;
      DOMNode * p_domnodeAttribute ;
      p_xmlchAttributeName = XMLString::transcode("frequency_in_MHz");
      const XMLCh * cp_xmlchAttrValue ;
      DWORD dwValue ;
      XercesHelper xerceshelper ;

      //Iterate over all "freq_and_voltage" XML elements.
      for(XMLSize_t i = 0; i < nNumXMLelements ; i++)
      {
        //Set XML element number "i".
        mp_domxpathresult->snapshotItem(i);
        //theSerializer->write(p_domxpathresult->getNodeValue(), 
        //  theOutputDesc);
        //Get all attributes for the current XML element.
        p_dom_namednodemap = mp_domxpathresult->getNodeValue()->getAttributes() ;
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
            cp_xmlchAttrValue = p_domnodeAttribute->getNodeValue() ;
            std::string stdstr = xerceshelper.ToStdString(
              cp_xmlchAttrValue) ;
            //cp_xmlchAttrValue = cp_xmlchAttrValue ;
            xerceshelper.ToDWORD( stdstr , & dwValue ) ;
            if( dwValue == wFreqInMHz )
              rp_dom_element = (DOMElement *) mp_domxpathresult->getNodeValue() ;
            //m_stdmapFreqInMHzInDOMtree2DOMindex.insert( 
            //  std::pair<WORD,WORD> ( dwValue, i ) ) ;
          }
        //}
      }
      //XMLString::release( & p_xmlchAttributeName );
    }
    catch(const DOMXPathException& e)
    {
        //XERCES_STD_QUALIFIER cerr << "An error occurred during processing of the XPath expression. Msg is:"
        //    << XERCES_STD_QUALIFIER endl
        //    << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
        retval = 4;
    }
    catch(const DOMException& e)
    {
        //XERCES_STD_QUALIFIER cerr << "An error occurred during processing of the XPath expression. Msg is:"
        //    << XERCES_STD_QUALIFIER endl
        //    << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
        retval = 4;
    }
   XMLString::release( & p_xmlchAttributeName );
   XMLString::release(&p_xmlchXpath);
    mp_domxpathresult->release();
  }

  bool XercesConfigurationHandler::PossiblyAddVoltages(
    const std::set<VoltageAndFreq> & r_stdsetvoltageandfreq 
    , //e.g. "max_voltage_in_Volt"
    const char * const cpc_XMLAttrName 
    //"true": only look if there would be changes made (useful to check
    // for the purpose: if there are changes then ask the user whether to 
    //save the changes.
    , bool bOnlySimulate 
    )
  {
    bool bDOMtreeModified = false ;

    bool bCreateVoltageAttribute = false ;
    DOMElement * p_dom_elementFreqnVolt = NULL ;
    DOMNamedNodeMap * p_dom_namednodemap ;
    DOMNode * p_domnodeAttribute ;
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
        //AddToDOM(
        //p_dom_document->
        //wFreq = c_iterator->m_wFreqInMHz ;
        //DOMElement * p_dom_elementFreqnVolt = p_dom_document->
        if( ! bOnlySimulate )
        {
          p_dom_elementFreqnVolt = AddFrequencyToDOMtree(wFreq) ;
        }
        bDOMtreeModified = true ;
        bCreateVoltageAttribute = true ;
        //p_dom_namednodemap = NULL ;
      }
      else //XML element with "freq_and_voltage" exists.
      {
//        const XMLCh * cp_xmlchAttrName ;
        if( citer_stdset_voltageandfreq->m_fVoltageInVolt != 0.0 )
        {
          //If config. file already exists.
          if( mp_domxpathresult )
          {
//            DOMNamedNodeMap * p_dom_namednodemap ;
            mp_domxpathresult->snapshotItem( 
              c_iterFrequenciesStoredInDOMtree->second);
            p_dom_namednodemap = mp_domxpathresult->getNodeValue()->
              getAttributes() ;
            p_dom_elementFreqnVolt = (DOMElement * ) 
              mp_domxpathresult->getNodeValue() ;
            if( p_dom_namednodemap )
            {
//              DOMNode * p_domnodeAttribute ;
              cp_xmlchAttrName = X(cpc_XMLAttrName) ;
              p_domnodeAttribute = p_dom_namednodemap->getNamedItem( 
                //cp_xmlchAttrName 
                X(cpc_XMLAttrName) ) ;
              ////Attribute exists.
              if ( p_domnodeAttribute //&& c_iterator->m_fVoltageInVolt != 
                )
              {
//                float fVoltageFromFile ;
                stdstr = XercesHelper::ToStdString(
                  p_domnodeAttribute->getNodeValue() ) ;
                from_stdstring<float>( fVoltageFromFile ,
                  stdstr
                  //, & fVoltage
                  ) ;
                //DOMElement * p_dom_elementFreqnVolt = 
                mp_dom_elementFreqnVolt =
                  (DOMElement *) mp_domxpathresult->getNodeValue() ;
                if( fVoltageFromFile != citer_stdset_voltageandfreq->
                  m_fVoltageInVolt )
                {
                  if( ! bOnlySimulate )
                  {
                    mp_dom_elementFreqnVolt->setAttribute( 
                      //cp_xmlchAttrName , //attribute name
                      X(cpc_XMLAttrName) ,
                      //attribute value
                      X( to_stdstring<float>( 
                        citer_stdset_voltageandfreq->m_fVoltageInVolt ).c_str() ) 
                      );
                  }
                  bDOMtreeModified = true ;
                }
              }
              else //Attribute does not exist in file.
              {
                mp_dom_elementFreqnVolt =
                  (DOMElement *) mp_domxpathresult->getNodeValue() ;
                bCreateVoltageAttribute = true ;
              }
            }
          }
          else
            bCreateVoltageAttribute = true ;
        }
      }
      if( bCreateVoltageAttribute )
      {
        if( p_dom_elementFreqnVolt )
        {
          //DOMElement * p_dom_elementFreqnVolt = 
          //  (DOMElement *) p_domxpathresult->getNodeValue() ;
          if( ! bOnlySimulate )
          {
            p_dom_elementFreqnVolt->setAttribute( 
              //cp_xmlchAttrName , //attribute name
              X(cpc_XMLAttrName) ,
              //attribute value
              X( to_stdstring<float>( 
                citer_stdset_voltageandfreq->m_fVoltageInVolt ).c_str() ) 
              );
          }
          bDOMtreeModified = true ;
        }
      }
      ++ citer_stdset_voltageandfreq ;
    }//while
    return bDOMtreeModified ;
  }

  char readXMLConfig(
    const char * xmlFile, //PStates & pstates
	  Model & model, 
	  UserInterface * p_userinterface ,
   //   PumaStateCtrl * p_pumastatectrl 

   //Base class of implementing Xerces XML handlers.
   //This is useful because there may be more than one XML file to read.
   //So one calls this functions with different handlers passed.
    DefaultHandler & r_defaulthandler
    )
	{
    BYTE byReturn = FAILURE ;
      //DEBUG("readXMLConfig begin--filename:%s\n",xmlFile);
      LOG( "read XML configuration begin--filename: \"" << xmlFile << "\"\n" );
	  try 
	  {
      //http://xerces.apache.org/xerces-c/program-3.html:
      //"Independent of the API you want to use, DOM, SAX, or SAX2, your
      //application must initialize the Xerces system before using the API[...]"
	    XMLPlatformUtils::Initialize();
      LOG( "XML successfully initialized\n" );
	  }
	  catch (const XMLException & toCatch )
	  {
	    char* message = XMLString::transcode(toCatch.getMessage());
	    cout << "Error during initialization! :\n";
	    cout << "Exception message is: \n"
	         << message << "\n";
        //DEBUG("XML error:%s\n",message);
        LOG( "XML error:" << message << "\n" );
	    XMLString::release(&message);
//	    return FAILURE;
	  }
    //Initialize to NULL just to avoid (g++) compiler warning.
	  SAX2XMLReader * p_sax2xmlreader = NULL ;
	  p_sax2xmlreader = XMLReaderFactory::createXMLReader();
    if( p_sax2xmlreader )
    {
      LOGN( "XML reader successfully created" );
      p_sax2xmlreader->setFeature(XMLUni::fgSAX2CoreValidation, true);
      p_sax2xmlreader->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional

      //DefaultHandler * p_defaultHandler = new DefaultHandler();
      DefaultHandler defaultHandler;
      //p_sax2xmlreader->setContentHandler(defaultHandler);
//	      SAX2MainConfigHandler sax2(//pstates
//	        model ,
//            p_userinterface ,
//            p_pumastatectrl
//            );
      p_sax2xmlreader->setContentHandler(//&sax2
        & r_defaulthandler );
      p_sax2xmlreader->setErrorHandler(//p_defaultHandler
        & defaultHandler );

      try
      {
        LOGN( "before parsing XML file" );
        p_sax2xmlreader->parse(xmlFile);
        DEBUG("-----------End of loading from config file\n") ;
         //http://xerces.apache.org/xerces-c/apiDocs-2/classXMLReaderFactory.html#_details:
         //"The parser object returned by XMLReaderFactory is owned by the
          //calling users, and it's the responsiblity of the users to delete
          //that parser object, once they no longer need it."
            delete p_sax2xmlreader;
          if( model.m_bTruncateLogFileForEveryStartup )
              g_logger.TruncateFileToZeroAndRewrite() ;
         byReturn = SUCCESS;
      }
      catch (const XMLException& toCatch)
      {
        LOGN( "XMLException" );
        char * message = XMLString::transcode(toCatch.getMessage());
        cout << "XML exception. Message is: \n"
             << message << "\n";
        p_userinterface->Confirm( std::string( "XML exception in file" ) +
          xmlFile + ":" + message );
        XMLString::release(&message);
          //http://xerces.apache.org/xerces-c/apiDocs-2/classXMLReaderFactory.html:
          //"The parser object returned by XMLReaderFactory is owned by
          //the calling users, and it's the responsiblity of the users to
          //delete that parser object, once they no longer need it."
          delete p_sax2xmlreader;
//	        return FAILURE;
      }
      catch ( const SAXParseException & r_saxparseexception
         //const SAXException & r_saxexception
         )
      {
        LOGN( "SAXParseException" );
//        LOGN( "SAXException" );
        char * pchMessage = XMLString::transcode(
            r_saxparseexception.getMessage()
//            r_saxexception.getMessage()
          );
        cout << "Exception message is: \n"
             << pchMessage << "\n";
        std::string strMessage = "XML error in file: \"" +
          std::string(xmlFile)
          + "\", line " + to_stdstring( r_saxparseexception.getLineNumber() )
          + ", column " + to_stdstring( r_saxparseexception.getColumnNumber() )
//          + "\", line " + to_stdstring( r_saxexception.getLineNumber() )
//          + ", column " + to_stdstring( r_saxexception.getColumnNumber() )
          + ": " + pchMessage
          + "\nIn order to solve this problem you may look into the XML "
          "specifications for element names etc" ;
        p_userinterface->Confirm(//pchMessage
          strMessage);
        XMLString::release( & pchMessage );
          //http://xerces.apache.org/xerces-c/apiDocs-2/classXMLReaderFactory.html:
          //"The parser object returned by XMLReaderFactory is owned by
          //the calling users, and it's the responsiblity of the users to
          //delete that parser object, once they no longer need it."
        delete p_sax2xmlreader;
//	        return FAILURE;
      }
      catch (...)
      {
        LOGN( "Unexpected Exception at parsing XML\n" ) ;
        p_userinterface->Confirm("Unexpected Exception parsing the XML document\n");
          //http://xerces.apache.org/xerces-c/apiDocs-2/classXMLReaderFactory.html:
          //"The parser object returned by XMLReaderFactory is owned by
          //the calling users, and it's the responsiblity of the users to
          //delete that parser object, once they no longer need it."
          delete p_sax2xmlreader;
//	        return FAILURE;
      }
      //__finally
     //   {
     ////        //http://xerces.apache.org/xerces-c/apiDocs-2/classXMLReaderFactory.html#_details:
     //   //"The parser object returned by XMLReaderFactory is owned by the
     //   //calling users, and it's the responsiblity of the users to
     //   //delete that parser object, once they no longer need it."
     //   delete p_sax2xmlreader;
     //   }
      //delete p_defaultHandler;
    }
    LOGN( "terminating XML usage" );
    //http://xerces.apache.org/xerces-c/program-3.html:
    //"Independent of the API you want to use, DOM, SAX, or SAX2, your
    //application must [...] and terminate it after you are done.
    XMLPlatformUtils::Terminate();
//	  return SUCCESS;
    return byReturn ;
	}

  BYTE XercesConfigurationHandler::readXMLfileDOM(
//    const char * p_chFullXMLFilePath
//    , DOMDocument * & p_dom_document
//    , DOMLSParser * & rp_dom_ls_parser
//    , DOMImplementation * & p_dom_implementation
    )
  {
//    BYTE byRet = FAILURE ;
    try
    {
      XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch)
    {
        char* message = XMLString::transcode(toCatch.getMessage());
        //cout << "Error during initialization! :\n"
        //     << message << "\n";
        XMLString::release(&message);
        return 1;
    }

    XMLCh tempStr[100];
    XMLString::transcode("LS", tempStr, 99);
    //DOMImplementation * 
    mp_dom_implementation = DOMImplementationRegistry::
      //"Return the first registered implementation that has the desired features,
      // or null if none is found."
      getDOMImplementation(tempStr);

    if( mp_dom_implementation )
    {
      //DOMLSParser *
      mp_dom_ls_parser = ( (DOMImplementationLS * )
        mp_dom_implementation)->createLSParser(
        DOMImplementationLS::MODE_SYNCHRONOUS, 0 );

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
            mp_dom_ls_parser->parseURI( //p_chFullXMLFilePath
              mpc_chFullXMLFilePath );
      }
      catch (const XMLException& toCatch)
      {
          char* message = XMLString::transcode(toCatch.getMessage());
          cout << "Exception message is: \n"
               << message << "\n";
          XMLString::release(&message);
          return FAILURE ;
      }
      catch(const DOMLSException & ex)
      {
          return FAILURE ;
      }
      catch (const DOMException& toCatch)
      {
          char* message = XMLString::transcode(toCatch.msg);
          cout << "Exception message is: \n"
               << message << "\n";
          XMLString::release(&message);
          return FAILURE ;
      }
      catch (...) {
          cout << "Unexpected Exception \n" ;
          return FAILURE ;
      }
    } //    if( mp_dom_implementation )
    else
    {
      LOGN("Xerces: no")
    }

    //delete errHandler;
    return 0;
  }

  short XercesConfigurationHandler::WriteDOM(
    DOMNode * p_dom_node
//    , const char * const cpc_chFilePath
    //For creating a DOMLSOutput instance via createLSOutput().
//    , DOMImplementation * p_dom_implementation
    )
  {
    if( mp_dom_implementation )
    {
      DOMLSSerializer * p_dom_ls_serializer = ((DOMImplementationLS *)
        mp_dom_implementation )->createLSSerializer();
      DOMConfiguration * p_dom_configuration = p_dom_ls_serializer->
        getDomConfig();
      if( p_dom_configuration->canSetParameter(
          XMLUni::fgDOMWRTFormatPrettyPrint, true )
        )
        p_dom_configuration->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint,
          true );

      XMLFormatTarget * p_xml_format_targetFile = new LocalFileFormatTarget (
        //cpc_chFilePath
        mpc_chFullXMLFilePath
        );
      DOMLSOutput * p_domls_output = ( (DOMImplementationLS *)
        mp_dom_implementation)->createLSOutput();
      p_domls_output->setByteStream(p_xml_format_targetFile);
      try
      {
          // do the serialization through DOMLSSerializer::write();
          p_dom_ls_serializer->write(p_dom_node, p_domls_output);
      }
      catch (const XMLException& toCatch) {
          char* message = XMLString::transcode(toCatch.getMessage());
          cout << "Exception message is: \n"
               << message << "\n";
          XMLString::release(&message);
          return -1;
      }
      catch (const DOMException& toCatch) {
          char* message = XMLString::transcode(toCatch.msg);
          cout << "Exception message is: \n"
               << message << "\n";
          XMLString::release(&message);
          return -1;
      }
      catch (...) {
          cout << "Unexpected Exception \n" ;
          return -1;
      }

      p_domls_output->release();
      p_dom_ls_serializer->release();
      //delete myErrorHandler;
      //delete myFilter;
      delete p_xml_format_targetFile;
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
    bool bDOMtreeModified = false ;
    //mp_dom_document = p_dom_document ;
    try
    {
      DOMXPathNSResolver * p_dom_xpath_ns_resolver =
        BuildStdmapFreqInMHzInDOMtree2DOMindex(
          m_stdmapFreqInMHzInDOMtree2DOMindex ) ;

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
      if( bDOMtreeModified )
      {
        if( ! bTest )
          WriteDOM(
            mp_dom_elementRoot
//            , p_chFullXMLFilePath
//            , p_dom_implementation
            ) ;
      }
      mp_dom_document->release();
      mp_domxpathresult->release();
      //This finally writes to the file.
      p_dom_xpath_ns_resolver->release ();
    }
    catch(const DOMXPathException& e)
    {
        //XERCES_STD_QUALIFIER cerr << "An error occurred during processing of the XPath expression. Msg is:"
        //    << XERCES_STD_QUALIFIER endl
        //    << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
//        retval = 4;
    }
    catch(const DOMException& e)
    {
        //XERCES_STD_QUALIFIER cerr << "An error occurred during processing of the XPath expression. Msg is:"
        //    << XERCES_STD_QUALIFIER endl
        //    << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
//        retval = 4;
    }
    return bDOMtreeModified ;
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
    //Prevent insertions or deletions while accessing the STL container
    //for voltages.
    mp_model->m_cpucoredata.m_wxcriticalsection.Enter() ;

    if( PossiblyAddVoltages(
      mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault
      , "max_voltage_in_Volt"
      , bTest )
      )
      bDOMtreeModified = true ;
    if( PossiblyAddVoltages(
      mp_model->m_cpucoredata.m_stdsetvoltageandfreqLowestStable
      , "min_voltage_in_Volt"
      , bTest )
      )
      bDOMtreeModified = true ;
    if( PossiblyAddVoltages(
      mp_model->m_cpucoredata.m_stdsetvoltageandfreqWanted
      , "voltage_in_Volt"
      , bTest )
      )
      bDOMtreeModified = true ;
    mp_model->m_cpucoredata.m_wxcriticalsection.Leave() ;
    return bDOMtreeModified ;
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
    //setting/ assigning the value is Important: for readXMLfileDOM() and
    //writeXML(...)
    mpc_chFullXMLFilePath = p_chFullXMLFilePath ;
    //DOMDocument * p_dom_document ;
//    DOMLSParser * p_dom_ls_parser ;
//    DOMImplementation * p_dom_implementation ;

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
      //XMLString::release(&p_xmlchXpath);  
    }// if( p_dom_document )
    //Is NULL if the XML file could not be read (=may not exist).
    else
    {
      //DOMImplementation * p_dom_implementation =  
      //  DOMImplementationRegistry::getDOMImplementation(X("Core"));

      if ( mp_dom_implementation != NULL)
      {
        try
        {
          //http://www.w3.org/TR/1998/REC-xml-19980210#NT-Nmtoken:
          //remove invalid tokens like "("
          //std::string str = RemoveInvalidTokens(
          //  r_strPstateSettingsFileName.c_str() ) ;
          //mp_userinterface->GetTextFromUser(_T("Enter the root XML element name:")) ;
          //DOMDocument * p_dom_document = p_dom_implementation->createDocument(
          mp_dom_document = mp_dom_implementation->createDocument(
            0,                    // root element namespace URI.
            // root element name
            //X( str.c_str() ) ,
            X("CPU") ,
            0);                   // document type object (DTD).
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
          PossiblyAddVoltages(false) ;

          //Write the whole XML tree.
          WriteDOM( 
            mp_dom_elementRoot
//            , p_chFullXMLFilePath
//            , p_dom_implementation
            ) ;
        }
        catch( const DOMException & e)
        {
          //int i ;
          //mp_userinterface->Confirm(e.getMessage() ) ;
        }
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
    mp_domxpathresult(NULL)
    , mp_dom_document( NULL)
    , mp_dom_implementation( NULL)
    , mp_dom_ls_parser (NULL)
  {
    mp_model = p_model ;
  }

#endif //#ifdef COMPILE_WITH_XERCES
