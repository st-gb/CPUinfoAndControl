// NehalemControllerDLL.cpp : Definiert die exportierten Funktionen für die DLL-Anwendung.
//

//#include "stdafx.h"
#include "NehalemControllerDLL.h"


// Dies ist das Beispiel einer exportierten Variable.
NEHALEMCONTROLLERDLL_API int nNehalemControllerDLL=0;

// Dies ist das Beispiel einer exportierten Funktion.
NEHALEMCONTROLLERDLL_API int fnNehalemControllerDLL(void)
{
	return 42;
}

// Dies ist der Konstruktor einer Klasse, die exportiert wurde.
// Siehe NehalemControllerDLL.h für die Klassendefinition.
CNehalemControllerDLL::CNehalemControllerDLL()
{
	return;
}
