
////with Linux I can't compile with xerces yet.
////#ifdef WIN32 
//#ifdef _WINDOWS
//  #define COMPILE_WITH_XERCES
//#endif

#ifdef COMPILE_WITH_XERCES
//If not included: compiler error "C1010" with MSVC++.
#include "../stdafx.h"
#include "XMLAccess.h"
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
	
	//Needed for verzichten auf the exact namspace.
	XERCES_CPP_NAMESPACE_USE
	
	using namespace std;
	
  // ---------------------------------------------------------------------------
  //  This is a simple class that lets us do easy (though not terribly efficient)
  //  trancoding of char* data to XMLCh data.
  // ---------------------------------------------------------------------------
  class XStr
  {
  public :
      // -----------------------------------------------------------------------
      //  Constructors and Destructor
      // -----------------------------------------------------------------------
      XStr(const char* const toTranscode)
      {
          // Call the private transcoding method
          fUnicodeForm = XMLString::transcode(toTranscode);
      }

      ~XStr()
      {
          XMLString::release(&fUnicodeForm);
      }
      // -----------------------------------------------------------------------
      //  Getter methods
      // -----------------------------------------------------------------------
      const XMLCh* unicodeForm() const
      {
          return fUnicodeForm;
      }
  private :
      // -----------------------------------------------------------------------
      //  Private data members
      //
      //  fUnicodeForm
      //      This is the Unicode XMLCh format of the string.
      // -----------------------------------------------------------------------
      XMLCh*   fUnicodeForm;
  };

  #define X(str) XStr(str).unicodeForm()

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
	
	  SAX2XMLReader * p_sax2xmlreader = XMLReaderFactory::createXMLReader();
      if( p_sax2xmlreader )
      {
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
	        char * message = XMLString::transcode(toCatch.getMessage());
	        cout << "XML exception. Message is: \n"
	             << message << "\n";
          p_userinterface->Confirm( std::string( "XML exception in file" ) + 
            xmlFile + ":" + message );
	        XMLString::release(&message);
            //http://xerces.apache.org/xerces-c/apiDocs-2/classXMLReaderFactory.html:
            //"The parser object returned by XMLReaderFactory is owned by the calling users, and it's the responsiblity of the users to delete that parser object, once they no longer need it."
            delete p_sax2xmlreader;
//	        return FAILURE;
	      }
	      catch (const SAXParseException & r_saxparseexception )
	      {
	        char * pchMessage = XMLString::transcode( r_saxparseexception.getMessage() );
	        cout << "Exception message is: \n"
	             << pchMessage << "\n";
          std::string strMessage = "XML error in file: \"" + 
            std::string(xmlFile) 
            + "\", line " + to_stdstring( r_saxparseexception.getLineNumber() ) 
            + ", column " + to_stdstring( r_saxparseexception.getColumnNumber() ) 
            + ": " + pchMessage 
            + "\nIn order to solve this problem you may look into the XML "
            "specifications for element names etc" ;
	        p_userinterface->Confirm(//pchMessage
            strMessage);
	        XMLString::release( & pchMessage );
            //http://xerces.apache.org/xerces-c/apiDocs-2/classXMLReaderFactory.html:
            //"The parser object returned by XMLReaderFactory is owned by the calling users, and it's the responsiblity of the users to delete that parser object, once they no longer need it."
          delete p_sax2xmlreader;
//	        return FAILURE;
	      }
	      catch (...) 
	      {
	        DEBUG_COUT( << "Unexpected Exception at parsing XML\n" ) ;
	        p_userinterface->Confirm("Unexpected Exception parsing the XML document\n");
            //http://xerces.apache.org/xerces-c/apiDocs-2/classXMLReaderFactory.html:
            //"The parser object returned by XMLReaderFactory is owned by the calling users, and it's the responsiblity of the users to delete that parser object, once they no longer need it."
            delete p_sax2xmlreader;
//	        return FAILURE;
	      }
	      //__finally
       //   {
       ////        //http://xerces.apache.org/xerces-c/apiDocs-2/classXMLReaderFactory.html#_details:
       //   //"The parser object returned by XMLReaderFactory is owned by the calling users, and it's the responsiblity of the users to delete that parser object, once they no longer need it."
       //   delete p_sax2xmlreader;
       //   }
    	  //delete p_defaultHandler;
      }

    //http://xerces.apache.org/xerces-c/program-3.html:
    //"Independent of the API you want to use, DOM, SAX, or SAX2, your
    //application must [...] and terminate it after you are done.
    XMLPlatformUtils::Terminate();
//	  return SUCCESS;
    return byReturn ;
	}

  BYTE readXMLfileDOM( 
    const char * p_chFullXMLFilePath 
    , DOMDocument * & p_dom_document
    , DOMLSParser * & parser
    , DOMImplementation * & impl 
    )
  {
    try {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        //cout << "Error during initialization! :\n"
        //     << message << "\n";
        XMLString::release(&message);
        return 1;
    }

    XMLCh tempStr[100];
    XMLString::transcode("LS", tempStr, 99);
    //DOMImplementation * 
    impl = DOMImplementationRegistry::
      getDOMImplementation(tempStr);
    //DOMLSParser * 
    parser = ((DOMImplementationLS*)impl)->createLSParser( 
      DOMImplementationLS::MODE_SYNCHRONOUS, 0);

    //// optionally you can set some features on this builder
    //if (parser->getDomConfig()->canSetParameter(XMLUni::fgDOMValidate, true))
    //    parser->getDomConfig()->setParameter(XMLUni::fgDOMValidate, true);
    //if (parser->getDomConfig()->canSetParameter(XMLUni::fgDOMNamespaces, true))
    //    parser->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, true);
    //if (parser->getDomConfig()->canSetParameter(XMLUni::fgDOMDatatypeNormalization, true))
    //    parser->getDomConfig()->setParameter(XMLUni::fgDOMDatatypeNormalization, true);


    //// optionally you can implement your DOMErrorHandler (e.g. MyDOMErrorHandler)
    //// and set it to the builder
    //MyDOMErrorHandler* errHandler = new myDOMErrorHandler();
    //parser->getDomConfig()->setParameter(XMLUni::fgDOMErrorHandler, errHandler);

    //DOMDocument * p_dom_document = 0;

    try {
        p_dom_document = parser->parseURI(p_chFullXMLFilePath);
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
    catch(const DOMLSException & ex)
    {
        return -1;
    }
    catch (...) {
        cout << "Unexpected Exception \n" ;
        return -1;
    }
    //delete errHandler;
    return 0;
  }

  short WriteDOM( 
    DOMNode * p_dom_node
    , const char * const cpc_chFilePath 
    , DOMImplementation * impl 
    )
  {
    DOMLSSerializer * theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();

    XMLFormatTarget * myFormTarget = new LocalFileFormatTarget ( cpc_chFilePath );
    DOMLSOutput* theOutput = ((DOMImplementationLS*)impl)->createLSOutput();
    theOutput->setByteStream(myFormTarget);
    try {
        // do the serialization through DOMLSSerializer::write();
        theSerializer->write(p_dom_node, theOutput);
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

    theOutput->release();
    theSerializer->release();
    //delete myErrorHandler;
    //delete myFilter;
    delete myFormTarget;
  }

  BYTE mergeXMLfileDOM( const char * p_chFullXMLFilePath 
    , Model & model )
  {
    BYTE retval ;
    // "/*": das Wurzel-Element unabhängig vom Namen 
    //  (jedes wohlgeformte XML-Dokument hat genau ein Wurzel-Element)
    char * archXPath = "/*/freq_and_voltage" ; 
    DOMDocument * p_dom_document ;
    DOMLSParser * parser ;
    DOMImplementation * impl ;
    readXMLfileDOM( 
      p_chFullXMLFilePath 
      , p_dom_document 
      , parser 
      , impl ) ;

    //Is NULL if the XML file could not be read.
    if( p_dom_document )
    {
      XMLCh * xpathStr = XMLString::transcode(archXPath);
      DOMElement * p_dom_elementRoot = p_dom_document->getDocumentElement();
      try
      {
        DOMXPathNSResolver * resolver = p_dom_document->createNSResolver(
          p_dom_elementRoot);
        DOMXPathResult * p_domxpathresult = p_dom_document->evaluate(
          xpathStr,
          p_dom_elementRoot,
          resolver,
          DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE,
          NULL);

        XMLSize_t nLength = p_domxpathresult->getSnapshotLength();
        DOMNamedNodeMap * p_dom_namednodemap ;
        DOMNode * p_domnodeAttribute ;
        XMLCh * xmlchElementName = XMLString::transcode("frequency_in_MHz");
        const XMLCh * cp_xmlchAttrValue ;
        std::set<WORD> stdsetFrequenciesStoredInFile ;
        DWORD dwValue ;
        XercesHelper xerceshelper ;
        for(XMLSize_t i = 0; i < nLength; i++)
        {
          p_domxpathresult->snapshotItem(i);
          //theSerializer->write(p_domxpathresult->getNodeValue(), 
          //  theOutputDesc);
          p_dom_namednodemap = p_domxpathresult->getNodeValue()->getAttributes() ;
          //for( XMLSize_t xmlsize_tAttIdx = 0 ; p_dom_namednodemap->getLength() ;
          //    ++ xmlsize_tAttIdx )
          //{
            p_domnodeAttribute = p_dom_namednodemap->getNamedItem( 
              //"frequency_in_MHz" 
              xmlchElementName ) ;
            //Attribute exists.
            if ( p_domnodeAttribute )
            {
              cp_xmlchAttrValue = p_domnodeAttribute->getNodeValue() ;
              //cp_xmlchAttrValue = cp_xmlchAttrValue ;
              xerceshelper.ToDWORD( 
                  xerceshelper.ToStdString(cp_xmlchAttrValue) , & dwValue
                ) ;
              stdsetFrequenciesStoredInFile.insert( dwValue ) ;
            }
          //}
        }
        XMLString::release( & xmlchElementName );
        bool bAdded = false ;
        //Prevent insertions or deletions while accing the container.
        model.m_cpucoredata.m_wxcriticalsection.Enter() ;
        //WORD wSize = model.m_cpucoredata.m_stdsetvoltageandfreqDefault.size() ;
        std::set<VoltageAndFreq>::const_iterator c_iterator = 
          model.m_cpucoredata.m_stdsetvoltageandfreqDefault.begin() ;
        WORD wFreq ;
        //for( WORD wIndex = 0 ; wIndex < wSize ; ++ wIndex )
        while( c_iterator != 
          model.m_cpucoredata.m_stdsetvoltageandfreqDefault.end() )
        {
          //if( model.m_cpucoredata.m_stdsetvoltageandfreqDefault..find() 
          wFreq = c_iterator->m_wFreqInMHz ;
          if( //The frequency does not exist in the file.
            stdsetFrequenciesStoredInFile.find(wFreq ) == 
            stdsetFrequenciesStoredInFile.end() 
            )
          {
            //AddToDOM(
            //p_dom_document->
            //wFreq = c_iterator->m_wFreqInMHz ;
            DOMElement * p_dom_elementFreqnVolt = p_dom_document->
              createElement( X("freq_and_voltage") );
            p_dom_elementRoot->appendChild(p_dom_elementFreqnVolt);
            p_dom_elementFreqnVolt->setAttribute( 
              X("frequency_in_MHz") , //attribute name
              X( to_stdstring<WORD>(wFreq).c_str() ) //attribute value
              );
            bAdded = true ;
          }
          ++ c_iterator ;
        }
        model.m_cpucoredata.m_wxcriticalsection.Leave() ;
        if( bAdded )
        {
          WriteDOM( p_dom_elementRoot
            , p_chFullXMLFilePath 
            , impl ) ;
        }
        p_dom_document->release();
        p_domxpathresult->release();
        resolver->release ();
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
      XMLString::release(&xpathStr);  
    }// if( p_dom_document )
    else
    {
      //DOMImplementation * impl =  
      //  DOMImplementationRegistry::getDOMImplementation(X("Core"));

      if (impl != NULL)
      {
        DOMDocument * p_dom_document = impl->createDocument(
          0,                    // root element namespace URI.
            X(p_chFullXMLFilePath),         // root element name
          0);                   // document type object (DTD).
        DOMElement * p_dom_elementRoot = p_dom_document->getDocumentElement();

        std::set<VoltageAndFreq>::const_iterator c_iterator = 
          model.m_cpucoredata.m_stdsetvoltageandfreqDefault.begin() ;
        WORD wFreq ;
        //for( WORD wIndex = 0 ; wIndex < wSize ; ++ wIndex )
        while( c_iterator != 
          model.m_cpucoredata.m_stdsetvoltageandfreqDefault.end() 
          )
        {
          //if( model.m_cpucoredata.m_stdsetvoltageandfreqDefault..find() 
          wFreq = c_iterator->m_wFreqInMHz ;
          DOMElement * p_dom_elementFreqnVolt = p_dom_document->createElement(
            X("freq_and_voltage") );
          p_dom_elementRoot->appendChild(p_dom_elementFreqnVolt);
          p_dom_elementFreqnVolt->setAttribute( 
            X("frequency_in_MHz") , //attribute name
            X( to_stdstring<WORD>(wFreq).c_str() ) //attribute value
            );
          //if( stdsetFrequenciesStoredInFile.find(wFreq ) != 
          //  stdsetFrequenciesStoredInFile.end() 
          //  )
          //{
          //  //AddToDOM(
          //  //p_dom_document->
          //}
          ++ c_iterator ;
        }
        WriteDOM( 
          p_dom_elementRoot
          , p_chFullXMLFilePath 
          , impl ) ;
      }
    }
    //Delete the parser itself. Must be done prior to calling Terminate, below.
    //This destroys the DOM document DOMDocument got by parser->parseURI()
    parser->release();
    return retval ;
  }

#endif //#ifdef COMPILE_WITH_XERCES
