#pragma once

//#include "StdAfx.h" //for DWORD, ...
#include "../global.h" //for DWORD, ...
#include <vector>
#include <string>
//#include <windows.h> //ULONGLONG
#include <Windows_compatible_typedefs.h> //for ULONGLONG

class BitRange
{
public:
    unsigned char m_byStartBit ;  
    unsigned char m_byBitLength ;
    BitRange(unsigned char byStartBit,  unsigned char byBitLength)
    {
        m_byStartBit = byStartBit ;
        m_byBitLength = byBitLength ;
    }
};

class RegisterData
{
public:
  bool m_bCalculateDiff ;
  ULONGLONG m_ullPreviousValue ;
  std::string m_strDataName ;
  std::vector<BitRange> m_stdvec_bitrange ;
  RegisterData(const std::string & str)
    : m_bCalculateDiff (false)
    , m_ullPreviousValue (0)
  {
      m_strDataName = str ;
  }
  void add(unsigned char byStartBit,  unsigned char byBitLength)
  {
      m_stdvec_bitrange.push_back(BitRange(byStartBit, byBitLength)) ;
  }
  //void setTable() {};
};

class RegisterDataTable
{
public:
  std::string //[][] 
    * m_ararstdstr ;
  //WORD 
    unsigned short m_wCols; 
  //WORD 
    unsigned short m_wRows;
  RegisterDataTable(
    //Even it's a 2-dim array it must declared as 1-dim array because
    //only array with 1 dimension needn't be given the length.
    std::string //[][] 
    * ararstdstr , 
    //WORD 
    unsigned short wCols, 
    //WORD 
    unsigned short wRows)
  {
    m_ararstdstr = ararstdstr ;
    m_wCols = wCols ;
    m_wRows = wRows ;
  }
};

class MSRdata
{
  public:
  BYTE m_byCoreID ;
  DWORD m_dwIndex ;
  std::string m_stdstrRegisterName ;
  std::vector<RegisterData> m_stdvec_registerdata ;
  //std::vector<std::string [][]> m_stdvector_ararstdstr ;
  std::vector<RegisterDataTable> m_stdvector_registerdatatable ;
  MSRdata(//DWORD
    unsigned long dwIndex)
    : m_byCoreID(0)
  {
      m_dwIndex = dwIndex ;
  }
  MSRdata(//DWORD
    unsigned long dwIndex
    , std::string & stdstrRegisterName ) ;

  void add(RegisterData & regdata)
  {
      m_stdvec_registerdata.push_back(regdata);
  }
  //std::string
  bool //GetTableContainingDataName
    GetTableAttributeValue(
    std::vector<RegisterDataTable>::iterator iter_registerdatatable ,
    const std::vector<std::string> & stdvector_stdstringAttributeValue ,
    std::string & strValue ) ;
  //An object of THIS class may contain more than 1 table.
  //ex.: looking for: "MHz"
  //true: table for "strDataName" exists
  bool GetTableAttributeValue(
    const std::vector<std::string> & stdvector_stdstringAttributeValue, 
    //const std::string & strDataName ,
    std::vector<RegisterDataTable>::iterator iter_registerdatatable ,
    std::string & strAttrVal ) ;

  void setTable(
    //A string gives high independance instead of e.g. an integer (->no floats possible)
    std::string //[][] 
    * 
    ararstdstr
    , WORD wCols, WORD wRows) ;
};
