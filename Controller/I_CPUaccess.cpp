/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
//#include "ISpecificController.hpp"
#include "I_CPUaccess.hpp"
//for BITMASK_FOR_LOWMOST_7BIT
#include <preprocessor_macros/bitmasks.h>
#include <Controller/CPU-related/ReadTimeStampCounter.h>
#include <ModelData/ModelData.hpp> //class Model
#include <string.h> //strcat(...)
#include <windef.h> //for DWORD
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN(...)
#include <hardware/CPU/x86/CPUID_addresses.h> //EXTENDED_L2_CACHE_FEATURES
#include <Aladdin_Enterprises/md5_implementation/GenerateMD5checksum.hpp>

/*//ULONG ulECX ;
NTSTATUS
//int
ReadMsr(void	*lpInBuffer, //index/offset as 4 byte value.
		ULONG	nInBufferSize,
		void	*lpOutBuffer,
		ULONG	nOutBufferSize,
		ULONG	*lpBytesReturned)
{
	ULONG ulECX = *(ULONG*)lpInBuffer;
	DEBUG("ulECX (MSR offset): %x\n", ulECX) ;
	//"The EDX register is loaded with the high-order 32 bits of the MSR and the EAX register is loaded with the low-order 32 bits."
	ULONG ulEAX = 0;//High or low 4 byte
	ULONG ulEDX = 0;//other 4 byte.

#ifdef WIN32
	__asm
	{
		mov ecx, ulECX //tell the offset
	}
	DEBUG("ulECX: %x before readmsr\n", ulECX) ;
	__asm
	{
		rdmsr //Assembler-Befehl fr MSR lesen
	}
	DEBUG("After readmsr\n") ;
	__asm
	{
		mov ulEAX, eax
		mov ulEDX, edx
	}
#else
	//gcc -S -masm=intel test.c
	//With gcc/g++: compile with switch "-S -masm=intel" .
	//>> "Op-code dst src" in Intel syntax changes to
	//"Op-code src dst" in AT&T syntax. <<
	//asm(".intel_syntax noprefix\n");
	//asm("mov ecx, ulECX\n"); //tell the offset
	// If we have more than one instructions, we write one per line in
	//double quotes, and also suffix a \n? and ?\t to the instruction.
	//This is because gcc sends each instruction as a string to as(GAS)
	//and by using the newline&sol;tab we send correctly formatted lines
	//to the assembler.
//	asm("mov ulECX, %ecx\n\t" //tell the offset
//		"rdmsr\n\t" //Assembler-Befehl fr MSR lesen
//		"mov %eax, ulEAX \n\t"
//		"mov %edx, ulEDX ");
	DEBUG("Before changing registers\n") ;
	asm("mov %0, %%ecx \n\t"
		:
		: "a"
		(ulECX)
		:"ecx"
		) ;
	asm("mov %%ecx, %0\n\t"
		: "=a" (ulEAX)
		);
	DEBUG("ulEAX: %x before readmsr\n", ulEAX) ;
	//asm("rdmsr;");
	asm(".byte 0x0F, 0x32;");
	DEBUG("after readmsr\n") ;
	asm("mov %%eax, %0\n\t"
		: "=a" (ulEAX)
		);
	asm("mov %%eDx, %0\n\t"
		: "=a" (ulEDX)
		);
#endif
	memcpy((PULONG)lpOutBuffer, &ulEAX, 4);
	memcpy((PULONG)lpOutBuffer+1, &ulEDX, 4);

	*lpBytesReturned = 8;
    return STATUS_SUCCESS;
}*/

////http://www.ccsl.carleton.ca/~jamuir/rdtscpm1.pdf:
////This functions can be used to synchronize rdtsc ("read Time Stamp Counter")
////instruction.
//void I_CPUaccess::CPUID()
//{
//  //TODO no need to save the contents of the EAX etc. regiser values (->faster)
//  DWORD dwEAX ;
//  //from http://www.ibm.com/developerworks/library/l-ia.html:
////  asm ("cpuid"
////        : "=a" (dwEAX)
////        : "a" (0));
//}

/** http://en.wikipedia.org/wiki/CPUID#Calling_CPUID:
 * "In assembly language the CPUID instruction takes no parameters as CPUID
 * implicitly uses the EAX register. The EAX register should be loaded with a
 * value specifying what information to return.
 * CPUID should be called with EAX = 0 first, as this will return the highest
 * calling parameter that the CPU supports.
 * To obtain extended function information CPUID should be called with bit 31
 * of EAX set. To determine the highest extended function calling parameter,
 * call CPUID with EAX = 80000000h."
 * ->
 *
 * Implement the CPUID instruction here.
* It may be overridden for subclasses that allow ring0 access because the
* "cpuid" instruction may be restricted to ring0 access. */
BOOL I_CPUaccess::CpuidEx(
  DWORD dwIndex,
  PDWORD p_dwEAX,
  PDWORD p_dwEBX,
  PDWORD p_dwECX,
  PDWORD p_dwEDX,
  DWORD_PTR affinityMask
  )
{
  //from http://www.cs.usfca.edu/~cruse/cs686s07/cpuid.cpp:
//    int reg_eax, reg_ebx, reg_ecx, reg_edx;
  //TODO creates error code 988 when a release DLL created with this code was loaded
//#ifdef _DEBUG
//    asm(" movl %0, %%eax " :: "m" (dwIndex) );
//    DEBUGN("before executing \"cpuid\" instruction" )
//    asm(" cpuid ");
//    asm(" mov %%eax, %0 " : "=m" (*p_dwEAX) );
//    DEBUGN("after \"cpuid\": eax:" << *p_dwEAX )
//    asm(" mov %%ebx, %0 " : "=m" (*p_dwEBX) );
//    asm(" mov %%ecx, %0 " : "=m" (*p_dwECX) );
//    asm(" mov %%edx, %0 " : "=m" (*p_dwEDX) );
//    return TRUE ;
//#else
  ////When using "*p_dwEAX" etc. directly inside the CPUID instruction
  //// the stepping was "
  //from http://www.ibm.com/developerworks/library/l-ia.html:
  asm ("cpuid"
        : "=a" (*p_dwEAX),
          "=b" (*p_dwEBX),
          "=c" (*p_dwECX),
          "=d" (*p_dwEDX)
        : "a" (dwIndex));
//    *p_dwEAX = reg_eax ;
  DEBUGN("after \"cpuid\": eax:" << *p_dwEAX << "stepping:" << (*p_dwEAX & 0xF) )
  return TRUE ;
}

/** @brief get CPU Level 2 cache size in _Kibi_ (1024) bytes.
 *  May be useful for buffer size for instable CPU core operation detection
 *  see http://en.wikipedia.org/wiki/CPUID#EAX.3D80000006h:_Extended_L2_Cache_Features
 * */
fastestUnsignedDataType I_CPUaccess::GetL2cacheSizeInKiB()
{
  DWORD ECX, dummy, EAX;
  fastestUnsignedDataType L2cacheSizeInKiB = 0;
//  CpuidEx(GET_HIGHEST_EXTENDED_FUNCTION_SUPPORTED, &EAX, &dummy, &dummy, &dummy, 1);
  EAX = GetHighestExtendedFunctionSupported();
  if( EAX >= EXTENDED_L2_CACHE_FEATURES )
  {
    /** http://en.wikipedia.org/wiki/CPUID#EAX.3D80000006h:_Extended_L2_Cache_Features:
    * "Returns details of the L2 cache in ECX, including the line size in
    * bytes, type of associativity (encoded by a 4 bits) and the cache size." */
    CpuidEx(EXTENDED_L2_CACHE_FEATURES, &dummy, &dummy, &ECX, &dummy, 1);
    //Intel® Processor Identification and the CPUID Instruction Application
    //Note 485 August 2009 Order Number: 241618-036  :
    //"ECX[7:0] L2 Cache Line Size in bytes."
    //"ECX[31:16] L2 Cache size described in 1024-byte units."
    fastestUnsignedDataType L2cacheSizeInKiB = ECX >> 16;
  }
  return L2cacheSizeInKiB;
}

void I_CPUaccess::GetMD5checkSum(void * beginOfMD5InputData,
  unsigned numBytesMD5InputData, BYTE arbyMD5checksum[16])
{
  Aladdin_Enterprises::md5_implementation::GenerateMD5checkSum(
    beginOfMD5InputData, numBytesMD5InputData, arbyMD5checksum);
}

//Is the same for AMD and Intel.
bool //ISpecificController
  I_CPUaccess::GetProcessorNameByCPUID( std::string & r_stdstr )
{
  bool bSuccess ;
  char * archCPUID ;
  bSuccess = GetProcessorNameByCPUID(archCPUID) ;
  if( bSuccess )
  {
    r_stdstr = std::string( archCPUID ) ;
    //Was allocated on heap inside "GetProcessorNameByCPUID(char * &)".
    delete [] archCPUID ;
  }
  return bSuccess ;
}

bool I_CPUaccess::GetProcessorNameWithoutLeadingSpaces( std::string & r_stdstr ) 
{
  bool bSuccess ;
  char * archCPUID ;
  bSuccess = GetProcessorNameByCPUID(archCPUID) ;
  if( bSuccess )
  {
    //Skip leading space characters.
    while( * archCPUID == ' ' )
      ++ archCPUID;
    r_stdstr = std::string( archCPUID ) ;
    //Was allocated on heap inside "GetProcessorNameByCPUID(char * &)".
    delete [] archCPUID ;
//    while( r_stdstr.size() > 0 && r_stdstr.at(0) == ' '  )
//      r_stdstr.erase ( 0 ,
//      //delete 1 char.
//      1 );
  }
  return bSuccess ;
}

//http://en.wikipedia.org/wiki/CPUID#EAX.3D80000000h:_Get_Highest_Extended_Function_Supported
/** @return data type must have at least 32 bit:= 80000000h: */
fastestUnsignedDataType I_CPUaccess::GetHighestExtendedFunctionSupported()
{
  DWORD EAX, dummy;
  CpuidEx(GET_HIGHEST_EXTENDED_FUNCTION_SUPPORTED, & EAX, & dummy, & dummy, & dummy, 1);
  return EAX;
}

/** @brief get processor name/ "Processor Brand String"
 * Is the same for AMD and Intel.
* Use the method with std::string parameter rather than this
* version with "char * &" where the
* memory is allocated inside the function! */
bool //ISpecificController
  I_CPUaccess::GetProcessorNameByCPUID(
  //Use a pointer to an array in order to allocate the array within 
  //this method.
  char * & archCPUID 
  )
{
  bool bSuccess = false ;
  DWORD dwEAX ;
  DWORD dw ;
  //Intel CPUID (doc # 241618) August 2009: for brand string:
  //"1. Execute the CPUID instruction with EAX=80000000h"
  if( CpuidEx( 
      GET_HIGHEST_EXTENDED_FUNCTION_SUPPORTED
      , & dwEAX
      , & dw
      , & dw
      , & dw 
      , 1
      )
    )
  {
    //Intel CPUID (doc # 241618) August 2009: for brand string:
    //"3. The processor brand string feature is supported if EAX >= 80000004h"
    if( dwEAX >= LAST_PROCESSOR_BRAND_STRING_INDEX )
    {
      BYTE byCPUID_Address = 0, byCharIndex = 0;
      //char archCPUID[//4*4
      //  CPUID_PROCESSOR_NAME_CHAR_NUMBER
      //  //For string terminating "\0" .
      //  + 1 ] ;
      archCPUID = new char [ CPUID_PROCESSOR_NAME_CHAR_NUMBER
        //For string terminating "\0" .
        + 1 ] ;
      if( archCPUID )
      {
        bSuccess = true ;
        archCPUID[ CPUID_PROCESSOR_NAME_CHAR_NUMBER ] = '\0';
        for( ; byCPUID_Address < 3 ; ++ byCPUID_Address )
        {
          if( CpuidEx(
            //http://en.wikipedia.org/wiki/CPUID
            // #EAX.3D80000002h.2C80000003h.2C80000004h:_Processor_Brand_String:
            // "EAX=80000002h,80000003h,80000004h: Processor Brand String"
            //AMD: "CPUID Fn8000_000[4:2] Processor Name String Identifier"
            FIRST_PROCESSOR_BRAND_STRING_INDEX + byCPUID_Address,
            ((DWORD *)(archCPUID + byCharIndex) ),
            ((DWORD *)(archCPUID + byCharIndex + 4 )),
            ((DWORD *)(archCPUID + byCharIndex + 8 )),
            ((DWORD *)(archCPUID + byCharIndex + 12 ))
            , 1
            )
            )
          {
            byCharIndex += NUMBER_OF_CPUID_REGISTER_BYTES ;
            //DEBUG("CPUID address:%lu "
            //  //"EAX:%lu,EBX:%lu ECX:%lu,EDX:%lu "
            //  //"%s\n"
            //  "\n"
            //  ,
            //  (0x80000002+byCPUID_Address)//,
            //  //dwEAX, dwEBX, dwECX, dwEDX,
            //  //archCPUID
            //  );
            //LOG("CPUID address:" << ( 0x80000002 + byCPUID_Address ) << " \n" );
            //archProcessorName[
          }
          else
          {
            //DEBUG("Error getting processor name of this CPU\n");
            bSuccess = false ;
            break ;
          }
        }//end for-loop
      }
//      if( bSuccess )
//        //DEBUG("processor name of this CPU is: %s\n", archCPUID );
//        LOG("Processor name of this CPU is: " << archCPUID << "\n" );
    }
  }
  return bSuccess ;
}

/** It makes sense to implement the get family and model as a
* method of the base CPU access class (instead of e.g. as a method
* of a CPU controller class) for the following reason:
* the family and model are the PREREQUISITE to select the CPU
* controller class, e.g for vendor AMD, family 17 use the
* "GriffinController" class.
 *
 * @param r_str the vendor ID (manufacturer name)
 * @return
 */
bool //ISpecificController
  I_CPUaccess::GetVendorID( std::string & r_str )
{
  bool bRet = false ;
  char archCPUID[ 
    //3 words a 4 bytes = 12 byte + NULL char = 13
    13 ] ;
  char archEBX[5]="    ", archECX[5] = "    " , archEDX[5] = "    " ;
  archCPUID[12] = '\0' ; //for (DWORD *) (archCPUID + 8)
  archCPUID[0] = '\0' ; //for strcat()
  DWORD dw ;
  //Intel: EBX: "Genu" EDX: "ineI" ECX: "ntel"
  bRet = CpuidEx( 
    VENDOR_ID_INDEX
    , & dw
    //Vendor ID is stored in EBX, ECX, EDX
    , //(DWORD *) archCPUID //EBX
      (DWORD *) archEBX
    ,// ( (DWORD *) (archCPUID + 8)) //ECX
      (DWORD *) archECX
    , //( (DWORD *) (archCPUID + 4)) //EDX
      (DWORD *) archEDX
    , 1
    ) ;
  if( bRet )
  {
    /** http://en.wikipedia.org/wiki/CPUID#EAX.3D0:_Get_vendor_ID:
    * "a twelve character ASCII string stored in EBX, EDX, ECX - in that order."
    * */
    strcat( archCPUID, archEBX ) ;
    strcat( archCPUID, archEDX ) ;
    strcat( archCPUID, archECX ) ;
    r_str = std::string(archCPUID) ;
  }
  return bRet ;
}

/** It makes sense to implement the get family and model as a
* method of the base CPU access class (instead of e.g. as a method
* of a CPU controller class) for the following reason:
* the family and model is the PREREQUISITE to select the CPU
* controller class, e.g for vendor "AuthenticAMD", family "17" use the
* "GriffinController" class. */
bool I_CPUaccess::GetFamilyAndModel(
  //BYTE & byFamily
  // values may be > 8 bits : bits 20:27 = 8 bits, + 4 bit value
  WORD & r_wFamily
  // values may be <= 8 bits : extended model = bits 19:16 = 4 bits 
  //  -> max value = 1111bin
  // base model = bits 7:4 = 4 bit value
  // model = extended model + base model << 4 -> 1111bin << 4 = 11110000bin
  //  4 bit << 4 = 8 bit value, 
  , BYTE & byModel
  )
{
  DWORD dwEAX, dwEBX, dwECX, dwEDX ;
  bool bRet = CpuidEx
    (
      //Query CPUID Function 0000_0001 for CPU model and family.
      PROCESSOR_INFO_AND_FEATURE_BITS
      , & dwEAX
      , & dwEBX
      , & dwECX
      , & dwEDX
      , 1
    ) ;
  if( bRet )
  {
    //BYTE byModel = 0, byFamily = 0 ;
    BYTE byBaseFamily = 0 , byExtendedFamily = 0 ;
    BYTE byExtendedModel = 0 ;
    BYTE byBaseModel = 0 ;
    //"Model is an 8-bit value and is defined as: Model[7:0] = 
    //{ExtendedModel[3:0], BaseModel[3:0]}".
    byExtendedModel = 
    //from "AMD Family 11h Processor BKDG", "41256 Rev 3.00 - July 07, 2008"
    //   -"F3xFC CPUID Family/Model Register" /
    //   -"CPUID Fn0000_0001_EAX Family, Model, Stepping Identifiers"
    //    :"Family is an 8-bit value and is defined as: Family[7:0] = 
    //      ({0000b,BaseFamily[3:0]} + ExtendedFamily[7:0]).
    //      E.g. If BaseFamily[3:0]=Fh and ExtendedFamily[7:0]=02h, 
    //      then Family[7:0]=11h. This document applies only to family 
    //      11h processors."
    //    "19:16 ExtendedModel."
    //byModel = 
      (BYTE) ( ( dwEAX 
        //Bits 19:16 are the ExtendedModel, so shift 16 bits right.
        >> 16 ) & 
        //0xF = 1111 bin
        0xF) ;

    //DEBUG("ExtendedModel: %u ",byModel)
      //LOG("Extended CPU Model: " <<           
      //  //Convert to integer in order to output as number 
      //  (WORD) //byModel 
      //  byExtendedModel << " ")
    //byModel <<= 4 ;
    //byModel |= 
    byBaseModel = (dwEAX >> 4 & 0xF) ; //"7:4 BaseModel."
    //  "Model is an 8-bit value and is defined as: Model[7:0] = 
    //      {ExtendedModel[3:0], BaseModel[3:0]}. 
    //     E.g. If ExtendedModel[3:0]=Eh and BaseModel[3:0]=8h, 
    //        then Model[7:0] = E8h. 
    //     Model numbers vary with product.
    byModel = ( byExtendedModel << 4 ) | byBaseModel ;
    //DEBUG("Base Model: %u ",dwEAX>>4 & 0xF)
    //LOG("Base Model: " << ( dwEAX >> 4 & 0xF ) << " ")
    //byFamily 
    byBaseFamily = (BYTE) ( ( dwEAX >> 8 ) & 0xF ) ; //"11:8 BaseFamily: Fh."
    //byFamily += 
    byExtendedFamily = (BYTE) ( ( dwEAX 
        ////"27:20 ExtendedFamily: 02h."
        >> 20 ) & 0xFF ) ; 
    //"Family is an 8-bit value and is defined as: 
    //    Family[7:0] = ({0000b,BaseFamily[3:0]} + ExtendedFamily[7:0])."
    //byFamily 
    r_wFamily = byBaseFamily + byExtendedFamily ;
    //DEBUG(" model: %u family:%u\n",byModel,byFamily)
    //LOG( "Extended CPU model:" 
    //    << (WORD) byExtendedModel 
    //    << ", base model: " 
    //    << (WORD) byBaseModel 
    //    << "->model:" 
    //    << (WORD) byModel 
    //    << "; extended CPU family:" 
    //    << (WORD) byExtendedFamily
    //    << "base family:"
    //    << (WORD) byBaseFamily
    //    << "->family:" 
    //    << (WORD) byFamily 
    //    << ";stepping: "
    //    //3:0 Stepping: processor stepping (revision) for a specific model.
    //    << ( dwEAX & 0xF )
    //    << "\n" 
    //    ) ;
  }
  return bRet ;
}

//It makes sense to implement the get family and model as a
//method of the base CPU access class (instead of e.g. as a method 
//of a CPU controller class) for the following reason:
//the family and model is the PREREQUISITE to select the CPU 
//controller class, e.g for vendor AMD, family 17 use the 
//"GriffinController" class.
bool I_CPUaccess::GetFamilyAndModelAndStepping(
  //BYTE & byFamily
  // values may be > 8 bits : bits 20:27 = 8 bits, + 4 bit value -> 9 bit
  WORD & r_wFamily
  // values may be <= 8 bits : extended model = bits 19:16 = 4 bits 
  //  -> max value = 1111bin
  // base model = bits 7:4 = 4 bit value
  // model = extended model + base model << 4 -> 1111bin << 4 = 11110000bin
  //  4 bit << 4 = 8 bit value, 
  , BYTE & r_byModel
  , BYTE & r_byStepping
  )
{
  DWORD dwEAX, dwEBX, dwECX, dwEDX ;
  bool bRet = CpuidEx
    (
      //Query CPUID Function 0000_0001 for CPU model and family.
      PROCESSOR_INFO_AND_FEATURE_BITS
      , & dwEAX
      , & dwEBX
      , & dwECX
      , & dwEDX
      , 1
    ) ;
  if( bRet )
  {
    //BYTE byModel = 0, byFamily = 0 ;
    BYTE byBaseFamily = 0 , byExtendedFamily = 0 ;
    BYTE byExtendedModel = 0 ;
    BYTE byBaseModel = 0 ;
    //"Model is an 8-bit value and is defined as: Model[7:0] = 
    //{ExtendedModel[3:0], BaseModel[3:0]}".
    byExtendedModel = 
    //from "AMD Family 11h Processor BKDG", "41256 Rev 3.00 - July 07, 2008"
    //   -"F3xFC CPUID Family/Model Register" /
    //   -"CPUID Fn0000_0001_EAX Family, Model, Stepping Identifiers"
    //    :"Family is an 8-bit value and is defined as: Family[7:0] = 
    //      ({0000b,BaseFamily[3:0]} + ExtendedFamily[7:0]).
    //      E.g. If BaseFamily[3:0]=Fh and ExtendedFamily[7:0]=02h, 
    //      then Family[7:0]=11h. This document applies only to family 
    //      11h processors."
    //    "19:16 ExtendedModel."
    //byModel = 
      (BYTE) ( ( dwEAX 
        //Bits 19:16 are the ExtendedModel, so shift 16 bits right.
        >> 16 ) & 
        //0xF = 1111 bin
        0xF) ;
    //byModel <<= 4 ;
    //byModel |= 
    byBaseModel = (dwEAX >> 4 & 0xF) ; //"7:4 BaseModel."
    //  "Model is an 8-bit value and is defined as: Model[7:0] = 
    //      {ExtendedModel[3:0], BaseModel[3:0]}. 
    //     E.g. If ExtendedModel[3:0]=Eh and BaseModel[3:0]=8h, 
    //        then Model[7:0] = E8h. 
    //     Model numbers vary with product.
    r_byModel = ( byExtendedModel << 4 ) | byBaseModel ;
    //DEBUG("Base Model: %u ",dwEAX>>4 & 0xF)
    //LOG("Base Model: " << ( dwEAX >> 4 & 0xF ) << " ")
    //byFamily 
    byBaseFamily = (BYTE) ( ( dwEAX >> 8 ) & 0xF ) ; //"11:8 BaseFamily: Fh."
    //byFamily += 
    byExtendedFamily = (BYTE) ( ( dwEAX 
        ////"27:20 ExtendedFamily: 02h."
        >> 20 ) & 0xFF ) ; 
    //"Family is an 8-bit value and is defined as: 
    //    Family[7:0] = ({0000b,BaseFamily[3:0]} + ExtendedFamily[7:0])."
    //byFamily 
    r_wFamily = byBaseFamily + byExtendedFamily ;
    //DEBUG(" model: %u family:%u\n",byModel,byFamily)
    r_byStepping =
      //3:0 Stepping: processor stepping (revision) for a specific model.
      ( dwEAX & 0xF ) ;
  }
  return bRet ;
}

BYTE I_CPUaccess::GetNumberOfCPUCores()
{
//  //return 2 ;
//  //If CPUID for address Fn8000_0008 fails the core count is 1.
  BYTE byCoreNumber = 1 ;
//  DWORD dwEAX;
//  DWORD dwEBX;
//  DWORD dwECX;
//  DWORD dwEDX;
//  //DEBUG("WRDL--getting number of CPU cores\n");
////  if( CpuidEx(
////    //AMD: "CPUID Fn8000_0008 Address Size And Physical Core Count Information"
////    0x80000008,
////    &dwEAX,
////    &dwEBX,
////    &dwECX,
////    &dwEDX,
////    1
////      )
////    )
//  if( CpuidEx(
//    //Intel: "Place core count in BL (originally in EAX[31:26]"
//    //0x0000001,
//		//Thread Level Processor Topology (CPUID Function 0Bh with ECX=0)
//		0xB,
//    &dwEAX,
//    &dwEBX,
//    &dwECX,
//    &dwEDX,
//    1
//      )
//    )
//	{
//    byCoreNumber = ( dwECX & BITMASK_FOR_LOWMOST_7BIT )
//      //"ECX 7:0 NC: number of physical cores - 1.
//      //The number of cores in the processor is NC+1 (e.g., if
//      //NC=0, then there is one core).
//      //See also section 2.9.2 [Number of Cores and Core Number]."
//      + 1 ;
//    //DEBUG("Number of CPU cores: %u\n", (WORD) byCoreNumber );
//    //LOG( "Number of CPU cores: " << (WORD) byCoreNumber << "\n" );
//  }
  //DEBUG("WRDL--end of getting number of CPU cores\n");
  byCoreNumber = mp_model->m_cpucoredata.GetNumberOfCPUcores();
  return byCoreNumber ;
}

/** @brief Implement the rdTSC instruction here.
* It may be overridden for subclasses that allow ring0 access because the
* "rdTSC" instruction may be restricted to ring0 access via a CPU register. */
BOOL I_CPUaccess::ReadTSC(
  DWORD & r_dwLowEAX ,
  DWORD & r_dwHighEDX
  )
{
  ReadTimeStampCounter(r_dwLowEAX, r_dwHighEDX ) ;
  return TRUE ;
}

/** @brief Execute the "read TSC" instruction in order and not out of order.
 *  Important for counting clock tick on out-of-order (able to execute
 *  instructions out of order related to the order of program code
 *  instructions) CPUs. */
BOOL I_CPUaccess::ReadTSCinOrder(
  DWORD & r_dwLowEAX ,
  DWORD & r_dwHighEDX ,
  DWORD dwThreadAffinityMask
  )
{
  if( SetThreadAffinityMask(dwThreadAffinityMask) )
  {
    //http://www.ccsl.carleton.ca/~jamuir/rdtscpm1.pdf:
//    CPUID(); //"force all previous instructions to complete"
    ReadTSC(r_dwLowEAX, r_dwHighEDX ) ;
    return TRUE ;
  }
  return FALSE ;
}

//I_CPUaccess::~I_CPUaccess()
//{
//
//}
