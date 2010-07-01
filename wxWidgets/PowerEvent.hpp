
//This file is e.g. for the service for detecting power events.
//It is not necessary for Windows where power events can be retrieved in the
//service's control handler.

//#include "wx/window.h"
//#include "wx/power.h" //for power mgmt notification (wxPowerType et.c),
//  EVT_POWER_RESUME

////from wx/event.h:
////error C2374: 'sm_eventTableEntries': Neudefinition; Mehrfachinitialisierung
////static const wxEventTableEntry sm_eventTableEntries[];
////static const wxEventTable        sm_eventTable;
////error C2270: 'GetEventTable': Modifizierer f�r Funktionen, die keine
//  Memberfunktionen sind, nicht zul�ssig
////virtual const wxEventTable*      GetEventTable() const;
////error C2374: 'sm_eventHashTable': Neudefinition; Mehrfachinitialisierung
////static wxEventHashTable          sm_eventHashTable;
////error C2270: 'GetEventHashTable': Modifizierer f�r Funktionen, die keine
//   Memberfunktionen sind, nicht zul�ssig
////virtual wxEventHashTable&        GetEventHashTable() const;
//
////void OnResume(wxPowerEvent& WXUNUSED(event)) ;
////
////const wxEventTableEntry sm_eventTableEntries[] = {
//// #ifdef wxHAS_POWER_EVENTS
////  //EVT_POWER_SUSPENDING(MyFrame::OnSuspending)
////  //EVT_POWER_SUSPENDED(MyFrame::OnSuspended)
////  //EVT_POWER_SUSPEND_CANCEL(MyFrame::OnSuspendCancel)
////#endif // wxHAS_POWER_EVENTS
////  EVT_POWER_RESUME(OnResume)
////    END_EVENT_TABLE()
////    //DECLARE_EVENT_TABLE_ENTRY( wxEVT_NULL, 0, 0, 0, 0 )
////  //};
////const wxEventTable sm_eventTable = { //&wxWindow::sm_eventTable,
////    sm_eventTableEntries[0] };
//////error C2270: 'GetEventTable': Modifizierer f�r Funktionen, die keine
//   Memberfunktionen sind, nicht zul�ssig
//////const wxEventTable GetEventTable() const
//////{
//////    return & sm_eventTable;
//////}
////wxEventHashTable sm_eventHashTable( sm_eventTable);
////error C2270: 'GetEventHashTable': Modifizierer f�r Funktionen, die keine
//  Memberfunktionen sind, nicht zul�ssig
////wxEventHashTable & GetEventHashTable() const
////{
////    return sm_eventHashTable;
////}
