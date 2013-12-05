/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once

#include <string>
//#include "StdAfx.h"
//#include <global.h> //for DWORD, ...
//"c:/mingw/bin/../lib/gcc/mingw32/3.4.5/../../../../include/winnt.h:107:
// error: `CONST' does not name a type"
#include <windef.h> //for CONST for winnt.h
#include <winnt.h> //for ULONGLONG
#include <vector>
//#include <windows.h> //ULONGLONG
#include <preprocessor_macros/Windows_compatible_typedefs.h> //for ULONGLONG

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
  enum metricPrefixes{ none, kilo, Mega, Giga};
  bool m_bCalculateDiff ;
  /** unsigned is fastest data type. */
  unsigned m_metricPrefix;
  ULONGLONG m_ullPreviousValue ;
  std::string m_strDataName ;
  std::vector<BitRange> m_stdvec_bitrange ;
  RegisterData(const std::string & str)
    : m_bCalculateDiff (false)
    , m_metricPrefix(none)
    , m_ullPreviousValue (0)
  {
      m_strDataName = str ;
  }
  void add(unsigned char byStartBit,  unsigned char byBitLength)
  {
      m_stdvec_bitrange.push_back(BitRange(byStartBit, byBitLength)) ;
  }
  void SetMetricPrefix(const std::string & metricPrefix)
  {
    if( metricPrefix == "M" )
      m_metricPrefix = Mega;
    else if( metricPrefix == "G" )
      m_metricPrefix = Giga;
  }
  //Assignment ("=") operator adapted from:
  //http://www.cs.caltech.edu/courses/cs11/material/cpp/donnie/cpp-ops.html:
//  RegisterData & operator = (const RegisterData & cr_registerdataToCopyFrom )
//  {
////    m_stdvec_bitrange = cr_registerdataToCopyFrom.m_stdvec_bitrange.
//    return *this;  // Return a reference to myself.
//  }
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

/** @brief common attributes shared between different CPU register data types
 *  (MSR, CPUID)*/
class CPUregisterProperties
{
public:
  /** register index, like MSR number xx, CPUID function/ code xx */
  unsigned m_dwIndex ;
  /** name for a CPU register, like "PERF_STATUS" for "0x198" for Intel MSR */
  std::string m_stdstrRegisterName ;
  CPUregisterProperties(unsigned index, const std::string & stdstrRegisterName)
    : m_dwIndex(index)
    , m_stdstrRegisterName(stdstrRegisterName)
  {

  }
};

class CPUIDdata
  : public CPUregisterProperties
{
public:
  BYTE m_byCoreID ;
  std::string m_stdstrGenPurposeRegName ;
  std::vector<RegisterData> m_stdvec_registerdata ;
  CPUIDdata(//DWORD
    unsigned /*long*/ dwIndex
    )
    : CPUregisterProperties(dwIndex, "")
    , m_byCoreID(0)
  {
    //m_dwIndex = dwIndex ;
  }
  CPUIDdata(//DWORD
    unsigned long dwIndex
    //Name for the corresponding CPUID code. 
    //E.g. CPUID code 0 is the code for getting the CPU manufacturer.
    // so register name could be "manufacturer".
    , std::string & stdstrRegisterName 
    ////Name for the general purpose register whose contents are wanted.
    ////The general purpose register can be either EAX, EBX, ECX or EDX.
    ////This is the general register the bit range applies to.
    //, std::string & stdstrGenPurposeRegName 
    )
    : CPUregisterProperties(dwIndex, stdstrRegisterName)
    , m_byCoreID(0)
  {
//    m_dwIndex = dwIndex ;
//    m_stdstrRegisterName = stdstrRegisterName ;
    //m_stdstrGenPurposeRegName = stdstrGenPurposeRegName ;
  }
  /** The following assignment operator solves the g++ warnings (that appears
  // because class RegisterData also contains a vector?:
  //see http://cboard.cprogramming.com/cplusplus-programming/
  //  101030-warnings-when-using-vector-vector.html
  // "C:/MinGW/bin/../lib/gcc/mingw32/3.4.5/../../../../
  // include/c++/3.4.5/bits/stl_vector.h:
  //  In member function `std::vector<_Tp,
  //   _Alloc>& std::vector<_Tp, _Alloc>::operator=(
  //    const std::vector<_Tp, _Alloc>&) [with _Tp = RegisterData,
  //    _Alloc = std::allocator<RegisterData>]':
  //C:/MinGW/bin/../lib/gcc/mingw32/3.4.5/../../../../include/c++/3.4.5/bits/
  // stl_vector.h:715:
  //  warning: '__result' might be used uninitialized in this function
  //C:/MinGW/bin/../lib/gcc/mingw32/3.4.5/../../../../include/c++/3.4.5/bits/
  //  stl_uninitialized.h:82: warning:
  // '__cur' might be used uninitialized in this function
  //C:/MinGW/bin/../lib/gcc/mingw32/3.4.5/../../../../include/c++/3.4.5/bits/
  // stl_uninitialized.h:82:
  // warning: '__cur' might be used uninitialized in this function */

  //Assignment ("=") operator adapted from:
  //http://www.cs.caltech.edu/courses/cs11/material/cpp/donnie/cpp-ops.html:
  CPUIDdata & operator = (const CPUIDdata & cr_cpuiddata )
  {
//    m_stdvec_bitrange = cr_registerdataToCopyFrom.m_stdvec_bitrange.
    return *this;  // Return a reference to myself.
  }
} ;

class MSRdata
  : public CPUregisterProperties
{
  public:
  BYTE m_byCoreID ;
  std::vector<RegisterData> m_stdvec_registerdata ;
  //std::vector<std::string [][]> m_stdvector_ararstdstr ;
  std::vector<RegisterDataTable> m_stdvector_registerdatatable ;
  MSRdata(//DWORD
    unsigned /*long*/ dwIndex)
    : CPUregisterProperties(dwIndex, "")
    ,  m_byCoreID(0)
  {
    //m_dwIndex = dwIndex ;
  }
  MSRdata(//DWORD
    unsigned /*long*/ dwIndex
    , std::string & stdstrRegisterName ) ;
  //The following assignment operator solves the g++ warnings (that appears
  // because class RegisterData also contains a vector?:
  //see http://cboard.cprogramming.com/cplusplus-programming/
  //  101030-warnings-when-using-vector-vector.html
  // "C:/MinGW/bin/../lib/gcc/mingw32/3.4.5/../../../../
  // include/c++/3.4.5/bits/stl_vector.h:
  //  In member function `std::vector<_Tp,
  //   _Alloc>& std::vector<_Tp, _Alloc>::operator=(
  //    const std::vector<_Tp, _Alloc>&) [with _Tp = RegisterData,
  //    _Alloc = std::allocator<RegisterData>]':
  //C:/MinGW/bin/../lib/gcc/mingw32/3.4.5/../../../../include/c++/3.4.5/bits/
  // stl_vector.h:715:
  //  warning: '__result' might be used uninitialized in this function
  //C:/MinGW/bin/../lib/gcc/mingw32/3.4.5/../../../../include/c++/3.4.5/bits/
  //  stl_uninitialized.h:82: warning:
  // '__cur' might be used uninitialized in this function
  //C:/MinGW/bin/../lib/gcc/mingw32/3.4.5/../../../../include/c++/3.4.5/bits/
  // stl_uninitialized.h:82:
  // warning: '__cur' might be used uninitialized in this function

  //Assignment ("=") operator adapted from:
  //http://www.cs.caltech.edu/courses/cs11/material/cpp/donnie/cpp-ops.html:
  MSRdata & operator = (const MSRdata & cr_msrdata ) ;

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
    //A string gives high independence instead of e.g. an integer (->no floats
    //possible)
    std::string //[][] 
    * 
    ararstdstr
    , WORD wCols, WORD wRows) ;
};
