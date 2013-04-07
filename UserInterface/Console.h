/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#include <string>
#include "UserInterface.hpp"
#include "Controller/PumaStateCtrl.h"
#include "global.h"
//#include "AMD_family17.h" //for

//extern pfnRdMsrExDef g_pfnRdMsrEx ;

class Console: public UserInterface
{
  void showHelp(){};
//  void  outputPstate(//char *msrfile,
//    DWORD_PTR core_id,
//    unsigned char byCurrentP_state,
//    unsigned long msr_register_number ,
//    int *vid)
//  {
//    BOOL nSuccess = FALSE ;
//	  unsigned char CpuFid,CpuDid,CpuVid, CPU_FID_plus8 ;
//	  unsigned long low = 0;
//	  unsigned long high = 0;
//	DEBUG("outputPstate(Core#:%lu,%u,%lx\n",core_id,byCurrentP_state,msr_register_number);
//	  //unsigned long long ullMSR_RegisterValue ;
//	  //ULONG	ulBytesReturned ;
//    //pfnRdmsrDef pfnRdmsr ;
//    //pfnRdMsrExDef pfnRdMsrEx ;
//	  //((*ptr_msr_read)(msrfile, msr_register_number, &low, &high)) ;
//  //	ReadMsr((void *)&msr_register_number,4,
//  //          (void *)&ullMSR_RegisterValue,8,
//  //          &ulBytesReturned) ;
//	  //low = (unsigned long) ullMSR_RegisterValue ;
//	  //high = (unsigned long) ullMSR_RegisterValue>>32 ;
//  //Wrmsr =					(_Wrmsr)				GetProcAddress (*hModule, "Wrmsr");
//    //pfnRdmsr = (pfnRdmsrDef)GetProcAddress(g_hinstanceWinRingDLL,"Rdmsr");
//    //(*(void (*)(9900,))pfn)();
//    if(//pfnRdmsr == NULL ||
//      //g_pfnRdMsrEx == NULL
//      true
//      )
//    {
//      printf("NO function RdmsrEx\n");
//    }
//    else
//    {
//      //printf("function RdmsrEx exists\n");
//      //(*(void (*)())pfn)();
//      //msr_register_number =
//      //  //time counter
//      //  0x00000010 ;
//      //If the function succeeds, the return value is TRUE.
//      //nSuccess = (*pfnRdmsr)(msr_register_number,&low, &high) ;
//	    //index = 0x00000010; // Time Stamp Counter
////	    nSuccess = (*g_pfnRdMsrEx)(msr_register_number, &low, &high, 1<<core_id);
//    }
//    DEBUG("low: %lu, high:%lu\n",low,high);
//    if( nSuccess )
//    {
//	    DEBUG(" Register fuer p-State \"%u\":%lx enabled:%lu current divisor:%lu current value:%lu\n",
//		    byCurrentP_state , msr_register_number,
//		    high>>31,
//        high & BITMASK_FOR_CURRENT_DIVISOR >> 8 ,
//        high & BITMASK_FOR_CURRENT_VALUE );
//	    CpuFid = (BYTE)low&63/*bits 0 to 5*/ ;
//	    CpuDid = (BYTE) (
//		    (low&448/*=111000000bin*/)>>START_BIT_FOR_CPU_CORE_DIVISOR_ID ); //<=>bits 6-8 shifted to the right
//	    CpuVid = (BYTE) (
//		    (low&BITMASK_FOR_CPU_CORE_VOLTAGE_ID/*=1111111000000000bin*/)>>9 ); //<=>bits 9-15 shifted to the right
//	    *vid = CpuVid ;
//	    /*AMD: "The CPU COF specified by MSRC001_00[6B:64][CpuFid,CpuDid] is
//	     * ((100 MHz * (CpuFid + 08h)) / (2^CpuDid)).
//	     */
//	    CPU_FID_plus8 = CpuFid + 0x8 ;
//	    DEBUG("  CpuVid:%u->%.3fV; core FID is "
//        "%u, CpuDid:%u->frequency="
//        "(100*(FID+8=%u)=%u)/(CpuDid**2=%u)=%u\n",
//        CpuVid,
//        //Vid=60: 0.8V, Vid=48: 0.95V -> 0.15V^=deltaVID 12 <=>0.15V:12 / VID
//
//        (float)(60-CpuVid)*0.0125f+0.8f,
//        CpuFid,
//        CpuDid,
//		    CPU_FID_plus8,
//        100*CPU_FID_plus8,
//        1<<CpuDid,
//        (100 * ((unsigned short) CPU_FID_plus8 ) ) / (//pow(2.0,(double)CpuDid)
//		    1<<CpuDid)
//        );
//    }
//    else
//      printf("Calling ReadMSR for msr_register_number%lx and "
//        "core id %lu failed. This is maybe you have not enough priveleges "
//        "to execute the ring 0 code. Try executing this prog as admin. \n",
//        msr_register_number,core_id);
//    //DEBUG("end of outputPstate(...)\n");
//  }
//
//  bool getCurrentPstate(BYTE & rbyCurrentPstate,DWORD dwCoreID)
//  {
//    bool bSuccess = false ;
//    DWORD dwMSRlow, dwMSRghigh ;
//	  //unsigned char byFreqID ;
//
//    //if(g_pfnRdMsrEx)
//    if( false )
//    {
//      //DEBUG("the low 32 bits: %s\n", getBinaryRepresentation(arch,dwMSRlow) );
//	    //printf("  would write:  %s to MSR %lx\n", getBinaryRepresentation(&msrvalue,arch), msr_register_number);
//	    //waitForEnter("um in MSR zu schreiben") ;
//	    //if ((msr_write(msrfile, msr_register_number, &msrvalue)) != OK)
//	    //	printf("MSR write failed\n");
//      if( (*g_pfnRdMsrEx)(
//        0xC0010062,
//        &dwMSRlow,
//        &dwMSRghigh,
//        dwCoreID
//        )
//        )
//      {
//        bSuccess = true ;
//        rbyCurrentPstate = (BYTE) dwMSRlow ;
//        DEBUG("Succ. read from 0xC0010062\n");
//      }
//      else
//        printf("error reading from MSR addr. 0xC0010062\n");
//    }
//    else
//      DEBUG("g_pfnRdMsrEx == NULL\n");
//    return bSuccess ;
//  }
//
//  void outputAllPstates(//char *msrfile,
//    DWORD_PTR core_id,
//    unsigned char byCurrentP_state,
//    int & vid)
//  {
//	  unsigned char byP_stateIndex = 0;
//	  unsigned long msr_register_number ;
//	  int nTempVID ;
//	  for(;byP_stateIndex < 8 ;byP_stateIndex++)
//	  {
//		msr_register_number = //0xC0010064 + byP_stateIndex ;
//        PumaStateCtrl::GetMSRregisterForPstate(byP_stateIndex) ;
//		  //DEBUG("before outputPstate\n")
//		outputPstate(//msrfile
//          core_id,
//          byP_stateIndex,
//          msr_register_number,
//          &nTempVID) ;
//      //DEBUG("after outputPstate(...)\n");
//		  if(byCurrentP_state == byP_stateIndex )
//			  vid = nTempVID ;
//	  }
//  }
  void outputAllPstates(unsigned char byCurrentP_state, int & vid)
  {
//    BYTE byCurrentPstate ;
//    //BYTE byCPUcoreID = 0 ;
//    printf("------------------------Core # %u:\n", 0);
//    outputAllPstates(//char *msrfile,
//      0,
//      byCurrentP_state,
//      vid) ;
//    getCurrentPstate(byCurrentPstate,0);
//    printf("Current pState: %u\n", byCurrentPstate);
//    printf("------------------------Core # %u:\n", 1);
//    outputAllPstates(//char *msrfile,
//      1,
//      byCurrentP_state,
//      vid) ;
//    getCurrentPstate(byCurrentPstate,1);
//    printf("Current pState: %u\n", byCurrentPstate);
  }
  void outputOtherRelevantVoltageInfo();//{};
  //Make it pure ("= 0") virtual so that it needs to be implemented by subclasses.
  bool Confirm(const std::string & str) 
  {
  	std::cout << str ;
  	return true;
  }
	bool Confirm(std::ostrstream & r_ostrstream)
	{
		std::string str(r_ostrstream.str(),r_ostrstream.pcount()) ;
		Confirm( str ) ;
		return true ;
	}
};
