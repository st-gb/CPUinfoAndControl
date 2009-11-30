// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include <Controller/tchar_conversion.h> //GetCharPointer()
// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "FloatValidator.hpp"

class wxTextCtrl ;
class wxMessageDialog ;

CfloatValidator::CfloatValidator(float *pval, float lBound, float hBound)
 {
     val.s = pval;
     m_type = singleType;
 
     m_lBound = (double)lBound;
     m_hBound = (double)hBound;
 }
 
 CfloatValidator::CfloatValidator(double *pval, double lBound, double hBound)
 {
     val.d = pval;
     m_type = doubleType;
 
     m_lBound = lBound;
     m_hBound = hBound;
 }
 
CfloatValidator::CfloatValidator( const CfloatValidator& v )
    : wxValidator()
{
    m_type = v.m_type;
    m_lBound = v.m_lBound;
    m_hBound = v.m_hBound;
    if (v.m_type == singleType)
        val.s = v.val.s;
    else
        val.d = v.val.d;
 
}
 
// Note the 'const' here
wxObject *CfloatValidator::Clone() const
{
    if (m_type == singleType)
        return new CfloatValidator(val.s, (float)m_lBound, (float)m_hBound);
    else
        return new CfloatValidator(val.d, m_lBound, m_hBound);
}
 
bool CfloatValidator::TransferFromWindow()
{
    wxString wxstr = ((wxTextCtrl*) m_validatorWindow)->GetValue();
    fpt_union fpt_union_ ;
    int r;
 
    if ( m_type == singleType )
      r = sscanf( GetCharPointer( wxstr.GetData() ) ," %f", fpt_union_.s );
    else
      r = sscanf( GetCharPointer( wxstr.GetData() ), " %g", fpt_union_.d );
 
    if ( r == 1 )
    {
        if (m_type == singleType)
            * val.s = * fpt_union_.s;
        else
            * val.d = * fpt_union_.d;
        return true;
    }
 
    return false;
}
 
bool CfloatValidator::TransferToWindow()
{
   wxString buf;
 
   if (m_type == singleType)
       buf=wxString::Format(wxT("%g"),*val.s);
   else
       buf=wxString::Format(wxT("%g"),*val.d);
 
    ((wxTextCtrl*)m_validatorWindow)->SetValue(buf);
 
    return true;
}
 
bool CfloatValidator::Validate(wxWindow* parent)
{
    wxString wxstr = ((wxTextCtrl*)m_validatorWindow)->GetValue();
    fpt_union t;
    bool ret;
 
    if (m_type == singleType) {
        int r = sscanf( GetCharPointer( wxstr.GetData() )," %f", t.s);
        ret = ( (r==1) && ( * t.s > m_lBound ) && ( *t.s < m_hBound ) );
    }
    else {
        int r = sscanf( GetCharPointer( wxstr.GetData() ) ," %g",t.d);
        ret = ( (r==1) && ( * t.d > m_lBound ) && ( *t.d < m_hBound ) );
    }
 
    if (!ret) {
        wxString errmsg = wxString::Format( wxT( 
          "You must enter a number between %g and %g!"), m_lBound, m_hBound);
        wxMessageDialog dlg( parent, errmsg, wxT("Bad input"), wxOK );
        dlg.ShowModal();
    }
 
    return ret;
}
