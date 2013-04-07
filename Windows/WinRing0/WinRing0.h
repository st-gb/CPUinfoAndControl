/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //Include guard.
#ifndef WINRING0_H
#define WINRING0_H
  typedef void (*pfnInitializeOls)();
  typedef BOOL ( __stdcall *pfnISMSRDef)() ;
  typedef BOOL (*pfnRdmsrDef)(DWORD,PDWORD,PDWORD);
  typedef BOOL ( //The calling convention "__stdcall" is needed because
                //else runtime error: something with CPU register "ESP".
               __stdcall fnRdMsrExDef)(DWORD,PDWORD,PDWORD,DWORD_PTR);
  typedef BOOL ( //The calling convention "__stdcall" may be needed because
                //else runtime error may occur
                __stdcall *pfnReadIoPortByteEx)(WORD,PBYTE);
  typedef BOOL ( //The calling convention "__stdcall" may be needed because
                //else runtime error may occur
                __stdcall *pfnReadIoPortDWordEx)(WORD,PWORD);
  typedef BOOL (//The calling convention "__stdcall" may be needed because
                //else runtime error may occur
                __stdcall *pfnReadPciConfigByteEx)(
                  DWORD //pciAddress,
                  ,DWORD //regAddress,
                  ,PBYTE //value
                );
  typedef BOOL (//The calling convention "__stdcall" may be needed because
                //else runtime error may occur
                __stdcall *pfnReadPciConfigDwordEx)(
                  DWORD //pciAddress,
                  ,DWORD //regAddress,
                  ,PDWORD //value
                );
  typedef BOOL (//The calling convention "__stdcall" may be needed because
                //else runtime error may occur
                __stdcall *pfnWritePciConfigDwordEx)(
                  DWORD //pciAddress,
                  ,DWORD //regAddress,
                  ,DWORD //value
                );
#endif //#ifndef WINRING0_H
