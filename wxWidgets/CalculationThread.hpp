/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //include guard

#include <wx/thread.h> //for class wxThread

namespace wxWidgets
{
  class CalculationThread
  {
  private:
    wxThread * mp_wxthread ;
  public:
    void StartCalculationThread(BYTE byCore)
    {
      mp_wxthread = new wxThread();
      if( mp_wxthread->Create() == wxTHREAD_NO_ERROR )
        m_wxthread->Run() ;
    }
  };
};
