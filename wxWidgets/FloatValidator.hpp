#pragma once //incl. guard

#include <wx/validate.h>
class wxValidator ;

class CfloatValidator 
  : public wxValidator
 {
 public:
     CfloatValidator(float *pval, float lBound=FLT_MIN, float hBound=FLT_MAX);
     CfloatValidator(double *pval, double lBound=DBL_MIN, double hBound=DBL_MAX);
 
     CfloatValidator( const CfloatValidator& v );
 
     wxObject *Clone() const;
     bool TransferFromWindow();
     bool TransferToWindow();
     bool Validate(wxWindow* parent);
 
 private:
    union fpt_union {
       double *d;
       float *s;
    } val;
 
    double m_lBound,m_hBound;
    enum { doubleType, singleType } m_type;
 
    CfloatValidator& operator=(const CfloatValidator&);
 };
