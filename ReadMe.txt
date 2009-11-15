========================================================================
    KONSOLENANWENDUNG: PumaStateCtrl-Projekt�bersicht
========================================================================

//With Xerces < Version 2.8:
//  Add "XML_LIBRARY" to "Preprocessor Definitions" to compile with Xerces statically (else many "LNK2001" and "LNK2019" and linker errors).
//with Xerces 3.0: "XERCES_STATIC_LIBRARY"
//And: Add "Advapi32.lib" as additional dependancy for the linker.
//zu Linker->Bibliothek ignorieren hinzuf�gen:"LIBCMT.lib", sonst: "LIBCMT.lib(_ctype.obj) : error LNK2005: _isspace ist bereits in MSVCRT.lib(MSVCR80.dll) definiert."

die CPU usage wird oft erst nach dem 2ten Programmstart erfolgreich geholt.
Beim 1ten Start wird p_ienumwbemclassobject->Next(...) (in 
CPUcoreUsageGetterIWbemServices::GetValues(...) nicht fertig.

Ich konnte nur die Release-Version in Vista ausf�hren, da die VC8Debug-DLL
anscheinend nicht da war.

Die Taktfrequenz konnt ich erfolgreich von 550 (FID:14,DID:2) auf 575 
(FID:15,DID:2) MHz erh�hen. Vielleicht geht das nicht (so gut), wenn ich statt dessen
auch die DID ver�ndert h�tte (z.B.: FID:5,DID:1= 600 MHz).

ein Werte-Wrap muss vermieden werden bei VID,DID, und FID: wenn man 254 (bin�r: ..10) eingibt, 
darf nicht eine kleinere Zahl (z.B. "10"bin= 2dec) daraus werden

Diese PumaStateCtrl-Anwendung wurde vom Anwendungs-Assistenten
f�r Sie erstellt.  

Diese Datei bietet eine �bersicht �ber den Inhalt der einzelnen Dateien,
aus denen Ihre PumaStateCtrl�Anwendung besteht.


PumaStateCtrl.vcproj
    Dies ist die Hauptprojektdatei f�r VC++-Projekte, die mit dem Anwendungs-
    Assistenten generiert werden. 
    Sie enth�lt Informationen �ber die Version von Visual C++, mit der die
    Datei generiert wurde, sowie �ber die Plattformen, Konfigurationen und 
    Projektfeatures, die im Anwendungs-Assistenten ausgew�hlt wurden.

PumaStateCtrl.cpp
    Dies ist die Hauptquelldatei der Anwendung.

/////////////////////////////////////////////////////////////////////////////
Weitere Standarddateien:

StdAfx.h, StdAfx.cpp
    Mit diesen Dateien werden eine vorkompilierte Header (PCH)-Datei
    mit dem Namen "PumaStateCtrl.pch" und eine 
    vorkompilierte Typendatei mit dem Namen "StdAfx.obj" erstellt.

/////////////////////////////////////////////////////////////////////////////
Weitere Hinweise:

Der Anwendungs-Assistent weist Sie mit "TODO:"-Kommentaren auf Teile des
Quellcodes hin, die Sie erg�nzen oder anpassen sollten.

/////////////////////////////////////////////////////////////////////////////