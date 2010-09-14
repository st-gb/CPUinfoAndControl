// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
//#include <Controller/character_string/tchar_conversion.h> //GetCharPointer()
#include <Controller/character_string/stdstring_format.hpp> //from_stdstring(...)
#include <wxWidgets/Controller/wxStringHelper.hpp> //GetStdString(wxString & )
// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "FloatValidator.hpp"

//class wxTextCtrl ;
//class wxMessageDialog ;

//from http://wiki.wxwidgets.org/WxValidator:
CfloatValidator::CfloatValidator(
  //The pointer must point to a valid storage.
  float * pval,
  float lBound, float hBound)
{
  m_floating_point_unionValue.p_fSinglePrecision = pval;
  m_type = singleType;

  m_lBound = (double)lBound;
  m_hBound = (double)hBound;
}
 
CfloatValidator::CfloatValidator(
  //The pointer must point to a valid storage.
  double * pval,
  double lBound, double hBound)
{
  m_floating_point_unionValue.p_dDoublePrecision = pval;
  m_type = doubleType;

  m_lBound = lBound;
  m_hBound = hBound;
}
 
CfloatValidator::CfloatValidator( const CfloatValidator & v )
  : wxValidator()
{
  m_type = v.m_type;
  m_lBound = v.m_lBound;
  m_hBound = v.m_hBound;
  if (v.m_type == singleType)
    m_floating_point_unionValue.p_fSinglePrecision =
      v.m_floating_point_unionValue.p_fSinglePrecision;
  else
    m_floating_point_unionValue.p_dDoublePrecision =
      v.m_floating_point_unionValue.p_dDoublePrecision;
}
 
// Note the 'const' here
wxObject * CfloatValidator::Clone() const
{
  if (m_type == singleType)
    return new CfloatValidator(m_floating_point_unionValue.p_fSinglePrecision,
      (float) m_lBound, (float) m_hBound);
  else
    return new CfloatValidator(m_floating_point_unionValue.p_dDoublePrecision,
      m_lBound, m_hBound);
}
 
bool CfloatValidator::TransferFromWindow()
{
  wxString wxstr = ((wxTextCtrl * ) m_validatorWindow)->GetValue();
  bool bSuccess;
  double dNumber ;

  std::string stdstr = GetStdString(wxstr) ;

  bSuccess = from_stdstring<double>( dNumber, stdstr ) ;

  if ( bSuccess )
  {
    if ( m_type == singleType )
      //The pointer points to the address of the constructor parameter.
      * m_floating_point_unionValue.p_fSinglePrecision = (float) dNumber ;
    else
      //The pointer points to the address of the constructor parameter.
      * m_floating_point_unionValue.p_dDoublePrecision = dNumber ;
    return true;
  }
  return false;
}
 
bool CfloatValidator::TransferToWindow()
{
  wxString wxstrTransferToWindow;
 
  if (m_type == singleType)
    wxstrTransferToWindow = wxString::Format( wxT("%g"),
      //The pointer points to the address of the constructor parameter.
      * m_floating_point_unionValue.p_fSinglePrecision );
  else
    wxstrTransferToWindow = wxString::Format( wxT("%g"),
      //The pointer points to the address of the constructor parameter.
      * m_floating_point_unionValue.p_dDoublePrecision );
 
  ((wxTextCtrl * ) m_validatorWindow)->SetValue(wxstrTransferToWindow);
  return true;
}
 
bool CfloatValidator::Validate(wxWindow * parent)
{
  wxString wxstr = ((wxTextCtrl * ) m_validatorWindow)->GetValue();
  bool ret;
  double dNumber ;

  std::string stdstr = GetStdString(wxstr) ;

  ret = ( from_stdstring<double>( dNumber, stdstr ) &&
    ( dNumber >= m_lBound ) && ( dNumber <= m_hBound ) ) ;

  if ( ! ret )
  {
    wxString wxstrErrMsg = wxString::Format(
      wxT("You must enter a number between %g and %g!"), m_lBound, m_hBound
      ) ;
    wxMessageDialog dlg( parent, wxstrErrMsg, wxT("Bad input"), wxOK );
    dlg.ShowModal();
  }
  return ret;
}
