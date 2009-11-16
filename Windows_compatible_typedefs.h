/* 
 * File:   Windows_compatible_typedefs.h
 * Author: sgebauer
 *
 * Created on 15. November 2009, 15:20
 */

#ifndef _WINDOWS_COMPATIBLE_TYPEDEFS_H
#define	_WINDOWS_COMPATIBLE_TYPEDEFS_H

#ifdef	__cplusplus
extern "C" {
#endif

//see winnt.h
typedef unsigned long long ULONGLONG ;
//see basetsd.h
typedef unsigned long DWORD_PTR ;
typedef unsigned char BYTE ;
typedef unsigned short WORD ;
typedef unsigned long DWORD ;
#define WINAPI __stdcall
typedef unsigned long long __int64 ;
typedef void * LPVOID;
//typedef ULONG_MAX ;

#ifdef	__cplusplus
}
#endif

#endif	/* _WINDOWS_COMPATIBLE_TYPEDEFS_H */

