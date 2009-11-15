#pragma once //include guard

#include <sstream> //for ostringstream
#include "../global.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h> //open()

//Use preprocessor macro substitution instead of class derived from interface
//because then there is no additional overhead.
#define SLEEP_1_MILLI_SECOND usleep(1000);

#ifdef _WINDOWS
	typedef _W64 unsigned long DWORD_PTR;
#else
	typedef unsigned long DWORD_PTR;
#endif

const char * getMSRFile(DWORD_PTR affinityMask)
//{
//  BYTE byCoreID ;
//  std::ostringstream ostrstr;
//
//  if(affinityMask==1)
//    byCoreID = 0 ;
//  else
//    byCoreID = 1 ;
//  ostrstr<<"/dev/cpu/"<<(WORD)byCoreID<<"/msr";
//
//  return ostrstr.str().c_str() ;
//}
	;

int 
#ifdef _WINDOWS 
	__stdcall 
#endif //#ifdef _WINDOWS 
readMSR(DWORD dwRegisterNumber, DWORD * p_dwLow, DWORD * p_dwHigh, 
  DWORD_PTR affinityMask )
//{
//  const char * msrfile ;
//  int fd;
//	int result;
// 	unsigned long long msrvalue;
//
//  msrfile = getMSRFile(affinityMask) ;
//
//	if ((fd = open(msrfile, O_RDONLY)) == -1)
//	{
//		printf("Can't open file %s for reading!\n", msrfile);
//		return FAILURE;
//	}	
//
//	if (lseek(fd, dwRegisterNumber, SEEK_SET) == -1)
//	{
//		printf("Seeking failed in file %s!\n", msrfile);
//		return FAILURE; 
//	}
//
//	result = (int)read(fd, &msrvalue, (size_t)8);
//
//	if (close(fd) < 0 || result == -1)
//	{
//		printf("Read or close error in file %s!\n", msrfile);
//		return FAILURE;
//	}
//
//	*p_dwHigh = msrvalue >> 32;
//	*p_dwLow = msrvalue;
//
//	return SUCCESS;
//}
	;

int 
#ifdef _WINDOWS 
	__stdcall 
#endif //#ifdef _WINDOWS 
msr_write(
    DWORD dwRegisterNumber,
    DWORD dwLow,
    DWORD dwHigh,
    DWORD_PTR affinityMask)
//{
////  BYTE byCoreID ;
//  const char *msrfile ;
//	int fd;
//	int result;
//  unsigned long long ullValue = 0;
//
//  msrfile = getMSRFile(affinityMask) ;
//
//	if ((fd = open(msrfile, O_WRONLY)) == -1)
//	{
//		printf("Can't open file %s for writing!\n", msrfile);
//		return FAILURE;
//	}
//
//	if (lseek(fd, dwRegisterNumber, SEEK_SET) == -1)
//	{
//		printf("Seeking failed in file %s!\n", msrfile);
//		return FAILURE; 
//	}
//
//  ullValue = dwHigh << 32 | dwLow ;
//  result = (int)write(fd, &ullValue, (size_t)8);
//
//	if (close(fd) < 0 || result == -1)
//	{
//		printf("Write or close error in file %s!\n", msrfile);
//		return FAILURE;
//	}
//
//	return SUCCESS;
//}
	;

void writeMSR(unsigned long ulECX, unsigned long ulTest) ;

	//asm("mov (%0), %%ecx \n\t" //"%0" ist Platzhalter fr Operand
	//	:  //output
	//	:"a" //"a": Identifier fr ein Register.
	//		(ulECX)
	//	: "ecx" //"ecx" steht in der Clobberlist weils verndert wird.
	//	 ); //tell the offset
	// /* Wenn du statt der Adresse den Inhalt der Speicherzelle bentigst,
	//  ** dann musst du das ganze mit runden Klammern schreiben:
	//  **
	//  ** asm("movl (%0), %%esi;"::"r"(message));
	//  */
	//asm("mov %%ecx, (%0) \n\t" //"%0" ist Platzhalter fr Operand
	//	: //1ter Doppelpunkt: output; "a": Identifier fr ein Register.
	//	 "=a" //2ter Doppelpunkt: input; "a": Identifier fr ein Register.
	//		(ulTest)
	//	:
	//	//: "a" //"ecx" steht in der Clobberlist weils verndert wird.
	//	 ); //tell the offset
	//printf("ulTest: %lu\n", ulTest) ;
