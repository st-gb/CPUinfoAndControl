// Folgender ifdef-Block ist die Standardmethode zum Erstellen von Makros, die das Exportieren 
// aus einer DLL vereinfachen. Alle Dateien in dieser DLL werden mit dem NEHALEMCONTROLLERDLL_EXPORTS-Symbol
// symbol defined on the command line. This symbol should not be defined on any project
// das diese DLL verwendet. Alle anderen Projekte, deren Quelldateien diese Datei beinhalten, erkennen 
// NEHALEMCONTROLLERDLL_API-Funktionen als aus einer DLL importiert, während die DLL
// mit diesem Makro definierte Symbole als exportiert ansieht.
#ifdef NEHALEMCONTROLLERDLL_EXPORTS
#define NEHALEMCONTROLLERDLL_API __declspec(dllexport)
#else
#define NEHALEMCONTROLLERDLL_API __declspec(dllimport)
#endif

// Diese Klasse wird aus NehalemControllerDLL.dll exportiert.
class NEHALEMCONTROLLERDLL_API CNehalemControllerDLL {
public:
	CNehalemControllerDLL(void);
	// TODO: Hier die Methoden hinzufügen.
};

extern NEHALEMCONTROLLERDLL_API int nNehalemControllerDLL;

NEHALEMCONTROLLERDLL_API int fnNehalemControllerDLL(void);
