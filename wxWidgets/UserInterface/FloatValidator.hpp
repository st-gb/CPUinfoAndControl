/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //Include guard

//from http://wiki.wxwidgets.org/WxValidator
#include <wx/validate.h> //Base class wxValidator
#include <limits> //for __FLT_MIN__ etc.
#ifndef _MSC_VER //If not MicroSoft compiler
  #define FLT_MIN __FLT_MIN__
  #define FLT_MAX __FLT_MAX__
  #define DBL_MIN __DBL_MIN__
  #define DBL_MAX __DBL_MAX__
#endif

//class wxValidator ;

class CfloatValidator 
  : public wxValidator
{
 public:
  CfloatValidator(float * pval, float lBound = FLT_MIN,
    float hBound = FLT_MAX );
  CfloatValidator(double * pval, double lBound = DBL_MIN,
    double hBound = DBL_MAX );

  CfloatValidator( const CfloatValidator & v );

  wxObject * Clone() const;
  bool TransferFromWindow();
  bool TransferToWindow();
  bool Validate(wxWindow * parent);
 
 private:
  union floating_point_union {
    double * p_dDoublePrecision ;
    float * p_fSinglePrecision ;
  } m_floating_point_unionValue;

  double m_lBound,m_hBound;
  enum { doubleType, singleType } m_type;

  CfloatValidator & operator = (const CfloatValidator & );

//  DECLARE_DYNAMIC_CLASS( CfloatValidator ) //GetClassInfo()
//  DECLARE_EVENT_TABLE() //GetEventTable()
};
