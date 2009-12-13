//#include "ISpecificController.hpp"
#include "I_CPUaccess.hpp"
#include <preprocessor_helper_macros.h> //for BITMASK_FOR_LOWMOST_7BIT

#define CPUID_PROCESSOR_NAME_CHAR_NUMBER 4*4*3

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

//Is the same for AMD and Intel.
bool //ISpecificController
  I_CPUaccess::GetProcessorNameByCPUID( std::string & r_stdstr )
{
  bool bSuccess ;
  char * archCPUID ;
  bSuccess = GetProcessorNameByCPUID(archCPUID) ;
  r_stdstr = std::string( archCPUID ) ;
  //Was allocated on heap inside "GetProcessorNameByCPUID(char * &)".
  delete archCPUID ;
  return bSuccess ;
}

bool I_CPUaccess::GetProcessorNoLeadingSpaces( std::string & r_stdstr ) 
{
  bool bSuccess ;
  char * archCPUID ;
  bSuccess = GetProcessorNameByCPUID(archCPUID) ;
  r_stdstr = std::string( archCPUID ) ;
  //Was allocated on heap inside "GetProcessorNameByCPUID(char * &)".
  delete archCPUID ;
  while( r_stdstr.size() > 0 && r_stdstr.at(0) == ' '  )
    r_stdstr.erase ( 0 , 
    //delete 1 char.
    1 );
  return bSuccess ;
}

//Is the same for AMD and Intel.
//Use the method with std::string parameter rather than this 
//version with "char * &" where the
//memory is allocated inside the function!
bool //ISpecificController
  I_CPUaccess::GetProcessorNameByCPUID(
  //Use a pointer to an array in order to allocate the array within 
  //this method.
  char * & archCPUID 
  )
{
  bool bSuccess = true ;
  DWORD dwEAX ;
  DWORD dw ;
  if( CpuidEx( 
      0x80000000 
      , & dwEAX
      , & dw
      , & dw
      , & dw 
      , 1
      )
    )
  {
    if( dwEAX >= 0x80000004 )
    {
      BYTE byCPUID_Address = 0, byCharIndex = 0;
      //char archCPUID[//4*4
      //  CPUID_PROCESSOR_NAME_CHAR_NUMBER
      //  //For string terminating "\0" .
      //  + 1 ] ;
      archCPUID = new char [//4*4
        //CPUID_PROCESSOR_NAME_CHAR_NUMBER
        4*4*3
        //For string terminating "\0" .
        + 1 ] ;
      archCPUID[//4*4
        CPUID_PROCESSOR_NAME_CHAR_NUMBER ] = '\0';
      for( ; byCPUID_Address < 3 ; ++ byCPUID_Address )
      {
        if( CpuidEx(
          //AMD: "CPUID Fn8000_000[4:2] Processor Name String Identifier"
          0x80000002 + byCPUID_Address,
          //dwEAX,
          //*((DWORD *)archCPUID),
          ((DWORD *)(archCPUID + byCharIndex) ),
          //dwEBX,
          //*((DWORD *)(archCPUID+4)),
          ((DWORD *)(archCPUID + byCharIndex + 4 )),
          //dwECX,
          //*((DWORD *)(archCPUID+8)),
          ((DWORD *)(archCPUID + byCharIndex + 8 )),
          //dwEDX
          //*((DWORD *)(archCPUID+12))
          ((DWORD *)(archCPUID + byCharIndex + 12 ))
          , 1
          ) 
          )
        {
          byCharIndex += 16 ;
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
          DEBUG("Error getting processor name of this CPU\n");
          bSuccess = false ;
          break ;
        }
      }//end for-loop
      if( bSuccess )
        //DEBUG("processor name of this CPU is: %s\n", archCPUID );
        LOG("Processor name of this CPU is: " << archCPUID << "\n" );
    }
  }
  return bSuccess ;
}

//It makes sense to implement the get family and model as a
//method of the base CPU access class (instead of e.g. as a method 
//of a CPU controller class) for the following reason:
//the family and model is the PREREQUISITE to select the CPU 
//controller class, e.g for vendor AMD, family 17 use the 
//"GriffinController" class.
bool //ISpecificController
  I_CPUaccess::GetVendorID( std::string & r_str )
{
  bool bRet = false ;
  char archCPUID[ 
    //3 words a 4 bytes = 12 byte + NULL char = 13
    13 ] ;
  archCPUID[12] = '\0' ;
  DWORD dw ;
  //Intel: EBX: "Genu" EDX: "ineI" ECX: "ntel"
  bRet = CpuidEx( 
    0x00000000 
    , & dw
    //Vendor ID is stored in EBX, ECX, EDX
    , (DWORD *) archCPUID //EBX
    , ( (DWORD *) (archCPUID + 8)) //ECX
    , ( (DWORD *) (archCPUID + 4)) //EDX
    , 1
    ) ;
  if( bRet )
  {
    r_str = std::string(archCPUID) ;
  }
  return bRet ;
}

//It makes sense to implement the get family and model as a
//method of the base CPU access class (instead of e.g. as a method 
//of a CPU controller class) for the following reason:
//the family and model is the PREREQUISITE to select the CPU 
//controller class, e.g for vendor AMD, family 17 use the 
//"GriffinController" class.
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
      0x00000001
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
      0x00000001
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
  //return 2 ;
  //If CPUID for address Fn8000_0008 fails the core count is 1.
  BYTE byCoreNumber = 1 ;
  DWORD dwEAX;
  DWORD dwEBX;
  DWORD dwECX;
  DWORD dwEDX;
  DEBUG("WRDL--getting number of CPU cores\n");
  if( CpuidEx(
    //AMD: "CPUID Fn8000_0008 Address Size And Physical Core Count Information"
    0x80000008,
    &dwEAX,
    &dwEBX,
    &dwECX,
    &dwEDX,
    1
      )
    )
  {
    byCoreNumber = ( dwECX & BITMASK_FOR_LOWMOST_7BIT )
      //"ECX 7:0 NC: number of physical cores - 1.
      //The number of cores in the processor is NC+1 (e.g., if
      //NC=0, then there is one core).
      //See also section 2.9.2 [Number of Cores and Core Number]."
      + 1 ;
    //DEBUG("Number of CPU cores: %u\n", (WORD) byCoreNumber );
    LOG( "Number of CPU cores: " << (WORD) byCoreNumber << "\n" );
  }
  DEBUG("WRDL--end of getting number of CPU cores\n");
  return byCoreNumber ;
}
