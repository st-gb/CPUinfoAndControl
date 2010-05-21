#pragma once
#include <windows.h> //SECURITY_ATTRIBUTES,...

void AddLowPrivilegedACE( HANDLE & r_handle );
BOOL DowngradeDACL(SECURITY_ATTRIBUTES * pSA) ;
BYTE CreateSecAttributes(SECURITY_ATTRIBUTES & sa ) ;
