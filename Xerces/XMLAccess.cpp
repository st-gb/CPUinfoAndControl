
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

	#include <iostream>
	//#include "SAX2Handler.hpp"
	#include "../global.h" //for SUCCESS, FAILURE
	//#include "../Model.hpp"
	
	//Include "xerces...\include" in the "include path" for the following "#include"s.
	#include <xercesc/sax2/SAX2XMLReader.hpp>
	#include <xercesc/sax2/XMLReaderFactory.hpp>
	#include <xercesc/sax2/DefaultHandler.hpp>
	#include <xercesc/util/XMLString.hpp>
  //#include <Controller/Logger.hpp>
  #include <Controller/Logger.hpp>
  #include <Controller/stdstring_format.hpp> //for to_strstring()
  #include "../ModelData/ModelData.hpp"
  #include "../UserInterface.hpp"

extern Logger g_logger ;
	
	//Needed for verzichten auf the exact namspace.
	XERCES_CPP_NAMESPACE_USE
	
	using namespace std;
	
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
#endif //#ifdef COMPILE_WITH_XERCES
