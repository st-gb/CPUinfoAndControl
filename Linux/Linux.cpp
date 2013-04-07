/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#ifndef _WINDOWS
  #include "Linux.h"
  #include <iostream> //for std::cout
  #include <fstream> //for ofstream
  #include <ios> //for ios_base::binary

  const char * getMSRFile(DWORD_PTR affinityMask)
  {
    BYTE byCoreID ;
    std::ostringstream ostrstr;

    if(affinityMask==1)
      byCoreID = 0 ;
    else
      byCoreID = 1 ;
    ostrstr<<"/dev/cpu/"<<(WORD)byCoreID<<"/msr";

    return ostrstr.str().c_str() ;
  }

  int
  #ifdef WIN32
    __stdcall
  #endif
  readMSR(DWORD dwRegisterNumber, DWORD * p_dwLow, DWORD * p_dwHigh,
    DWORD_PTR affinityMask )
  {
    const char * msrfile ;
    int fd;
    int result;
    unsigned long long msrvalue;
    DEBUG("readMSR(%lx,...,%lu)\n",dwRegisterNumber,affinityMask);
    msrfile = getMSRFile(affinityMask) ;

    if ((fd = open(msrfile, O_RDONLY)) == -1)
    {
      printf("Can't open file %s for reading!\n", msrfile);
      return FAILURE;
    }

    if (lseek(fd, dwRegisterNumber, SEEK_SET) == -1)
    {
      printf("Seeking failed in file %s!\n", msrfile);
      return FAILURE;
    }

    result = (int)read(fd, &msrvalue, (size_t)8);

    if (result == -1)
    {
      printf("Read error in file \"%s\"!\n", msrfile);
      return FAILURE;
    }
    if(close(fd) < 0 )
    {
      printf("Close error in file %s!\n", msrfile);
      return FAILURE;
    }

    *p_dwHigh = msrvalue >> 32;
    *p_dwLow = msrvalue;

    return SUCCESS;
  }

  int
  #ifdef WIN32
    __stdcall
  #endif
    msr_write(
      DWORD dwRegisterNumber,
      DWORD dwLow,
      DWORD dwHigh,
      DWORD_PTR affinityMask)
  {
    //BYTE byCoreID ;
    const char *msrfile ;
    int fd;
    int result;
    unsigned long long ullValue = 0;

    writeMSR(55,55);

    msrfile = getMSRFile(affinityMask) ;

  //	std::ofstream ofstreamMSR(msrfile,std::ios_base::binary|std::ios_base::out);
  //
  //	if(ofstreamMSR.fail())
  //		std::cout << "err op file\n";
    if ((fd = open(msrfile, O_WRONLY)) == -1)
    {
      printf("Can't open file %s for writing!\n", msrfile);
      std::cout << "is the module \"msr\" loaded?\nIf not: execute \"modprobe msr\"" ;
      return FAILURE;
    }

    DEBUG("seek to %lu\n",dwRegisterNumber);
    DEBUG("seek to %lx\n",dwRegisterNumber);
  //	ofstreamMSR.seekp(dwRegisterNumber);
    if (lseek(fd, dwRegisterNumber, SEEK_SET) == -1)
    {
      printf("Seeking failed in file %s!\n", msrfile);
      return FAILURE;
    }
  //  if(ofstreamMSR.fail() )
  //  	printf("seek error\n");
  //	DEBUG("current file pos: %x\n",(int)ofstreamMSR.tellp());

    ullValue = dwHigh ;
    ullValue <<= 32 ;
    ullValue |= dwLow ;
    //ullValue = 0 ;
    result = (int)write(fd, &ullValue, (size_t)8);
  //  ofstreamMSR << ullValue ;
    DEBUG("After writing to file\n");

    if (result == -1)
  //  if(ofstreamMSR.fail() )
    {
      printf("Write error in file \"%s\"!\n", msrfile);
      DEBUG("sdös\n");
      return FAILURE;
    }
    else
      printf("NO Write error in file \"%s\"!\n", msrfile);
  //	ofstreamMSR.flush() ;
  //  if(ofstreamMSR.fail() )
  //		std::cout << "err flush file\n";
  //	else
  //		std::cout << "succ flush file\n";
  //	ofstreamMSR.close() ;
  //  if(ofstreamMSR.fail() )
  //		std::cout << "err closing file\n";
    if(close(fd) )
    {
      printf("Close error in file %s!\n", msrfile);
        return FAILURE;
    }

    return SUCCESS;
  }

  void writeMSR(unsigned long ulECX, unsigned long ulTest)
  {
    //asm("rdmsr;");
    //WORD ulEAX, ulEDX;
  //		asm("mov %%eax, %0\n\t"
  //		: "=a" (ulEAX)
  //		:
  //		);
  //	asm("mov %%eDx, %0\n\t"
  //		: "=a" (ulEDX)
  //		);
  //	asm("mov %0, %%ecx \n\t"
  //		:
  //		: "a"
  //		(ulECX)
  //		:"ecx"
  //		) ;
    DEBUG("after readmsr\n");
    asm("mov $0x1, %eax");
    asm("mov $0x0, %ecx");
    asm("wrmsr");
  //	asm("mov (%0), %%ecx \n\t" //"%0" ist Platzhalter fr Operand
  //		:  //output
  //		:"a" //"a": Identifier fr ein Register.
  //			(ulECX)
  //		: "ecx" //"ecx" steht in der Clobberlist weils verndert wird.
  //		 ); //tell the offset
  //	 /* Wenn du statt der Adresse den Inhalt der Speicherzelle bentigst,
  //	  ** dann musst du das ganze mit runden Klammern schreiben:
  //	  **
  //	  ** asm("movl (%0), %%esi;"::"r"(message));
  //	  */
  //	asm("mov %%ecx, (%0) \n\t" //"%0" ist Platzhalter fr Operand
  //		: //1ter Doppelpunkt: output; "a": Identifier fr ein Register.
  //		 "=a" //2ter Doppelpunkt: input; "a": Identifier fr ein Register.
  //			(ulTest)
  //		:
  //		//: "a" //"ecx" steht in der Clobberlist weils verndert wird.
  //		 ); //tell the offset
    DEBUG("Assembler ecx register writetest: written: %lu read: %lu\n",ulECX,ulTest) ;
  }

  //int msr_usleep(int sleep_usec)
//{
//	struct timeval start;
//	struct timeval stop;
//	double sleeptime;
//
//	if (sleep_usec > 100)
//	{pfnRdMsrEx ;
//		printf("Busy waiting doesn't make sense over 100 useconds!\n");
//		return NOTOK;
//	}
//
//	gettimeofday(&start, NULL);
//	do
//	{
//		gettimeofday(&stop, NULL);
//
//		sleeptime = ((stop.tv_sec + (stop.tv_usec / 1000000.) - (start.tv_sec + (start.tv_usec / 1000000.))) * 1000000.);
//	} while (sleeptime < sleep_usec);
//
//  return OK;
//}
#endif //#ifndef _WINDOWS
