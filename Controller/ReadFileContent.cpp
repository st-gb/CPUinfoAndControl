/*
 * ReadFileContent.cpp
 *
 *  Created on: Jun 12, 2010
 *      Author: Stefan
 */
#include <fstream>
#include <global.h> //LOGN()
//#include <windef.h> //BYTE
typedef unsigned char BYTE ;

BYTE ReadFileContent( std::string & r_stdstrFilePath )
{
  BYTE by = 0 ;
  std::ifstream stdifstream ;
  stdifstream.open( r_stdstrFilePath.c_str()
    , //std::ifstream::in
    // std::ios_base::in
    std::_S_in
    );
  if( stdifstream.is_open() )
  {
    char * buffer ;
    LOGN("successfully opened file \"" << r_stdstrFilePath << "\"" )
    //http://www.cplusplus.com/reference/iostream/istream/seekg/:
    stdifstream.seekg(0, std::ios::end);
    int length = stdifstream.tellg();
    stdifstream.seekg (0, std::ios::beg);
    // allocate memory:
    buffer = new char [length + 1 ];

    // read data as a block:
    stdifstream.read (buffer,length);
    buffer[ length ] =
      //string terminating NULL char.
      '\0' ;
    r_stdstrFilePath = std::string( buffer ) ;
    stdifstream.close();
    delete[] buffer;
    by = 1 ;
  }
  else
  {
    LOGN("failed to open file \"" << r_stdstrFilePath << "\"" )
  }
  return by ;
}
