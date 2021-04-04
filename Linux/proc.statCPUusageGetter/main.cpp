/** main.cpp  compile with "-fPIC" for dyn libs, else the e.g. when built as
 *  dynamic library the logger (object) of the x86I&C executable may be used.
 *  Created on: Oct 2, 2012
 *  Author: Stefan Gebauer, M.Sc. Comp. Sc. */
//#include <linux/cpumask.h>
//#include <linux/fs.h>
//#include <linux/gfp.h>
//#include <linux/init.h>
//#include <linux/interrupt.h>
//#include <linux/kernel_stat.h> //kstat_cpu(...)

///DWORD ::GetTimeCountInNanoSeconds(...)
#include <Controller/time/GetTickCount.hpp>
#include <Controller/I_CPUaccess.hpp> //class I_CPUaccess
#ifdef _DEBUG
  #include <Controller/Logger/Logger.hpp> //class Logger
  static Logger //s_logger,
    * s_p_logger = //& s_logger;
    & g_logger;
#endif
///DEBUGN_LOGGER_NAME(...)
#include <preprocessor_macros/logging_preprocessor_macros.h>
#include <fstream>///class std::ifstream
#include <unistd.h>///sysconf(...)

static unsigned long s_user_mode, s_nice, s_system//, s_idlePrevious
  , s_idleCurrent;
static long int s_TicksPerSecond;
//static unsigned long * s_p_ar_Idle = NULL;

struct CPUcoreAttributes
{
  unsigned long IdleTime;
//  uint64_t TimeCountInNanoSeconds;
  long double TimeCountInSeconds;
};
static CPUcoreAttributes * s_p_ar_cpucoreattributes = NULL;

//namespace ProcStatFile{}

//TODO call this function & measure time what is faster (multiple iterations):
//-open the file every time
//-keep the file open and seek to begin every time
/** For all time values(source:shell command "man 5 proc",section "/proc/stat"):
 *"The  amount  of  time,  measured  in units of USER_HZ (1/100ths of a second
 * on most architectures, use sysconf(_SC_CLK_TCK) to obtain the right value),
 * that the system ("cpu"  line)  or  the  specific  CPU ("cpuN" line) spent in
 * various states"
 * Cites from shell command "man 5 proc", section "/proc/stat" */
inline void getUsageViaOpeningProcStatFile(
  unsigned long & timeInUserMode///"Time spent in user mode."
  ///"Time spent in user mode with low priority (nice)
  , unsigned long & timeInNiceMode
  , unsigned long & timeInSysMode///"Time spent in system mode."
  /**"Time  spent  in the idle task.  This value should be
   * USER_HZ times the second entry in the /proc/uptime pseudo-file. */
  , unsigned long & timeInIdleTask
  , uint32_t wCPUcoreID)
{
  static std::ifstream std_ifstr;
  std_ifstr.open("/proc/stat");
  if( std_ifstr.is_open() )
  {
    static std::string std_str;
    std::getline(std_ifstr, std_str);///1st line: CPU overall

    while (wCPUcoreID --)///Until at core ID "wCPUcoreID"
      std::getline(std_ifstr, std_str);
  // std_ifstr.readline();
  // std_ifstr >> str;

    /** Format of a line (shell command "man 5 proc",section "/proc/stat"):
     * cpu>#coreID< user_time nice_time system_time idle_time */
    std_ifstr >> std_str >> timeInUserMode >> timeInNiceMode >> timeInSysMode >>
      timeInIdleTask;
    std_ifstr.close();
  }
}

extern "C"
/** @return usage expressed as float value [0...1] or -1 if an error occured.
 *  @param wCPUcoreID: core ID starting from 0 */
float GetCPUcoreUsage(unsigned short /*uint32_t*/ wCPUcoreID)
{
  static unsigned idle_diff;
//  static uint64_t TimeCountInNanoSeconds;
  static long double TimeCountInSeconds, TimeCountDiffInSeconds;
  static float Ticks, busyTicks;
//  static uint64_t TimeCountDiffNanoSeconds;

  getUsageViaOpeningProcStatFile(s_user_mode, s_nice, s_system, s_idleCurrent, wCPUcoreID);
//  GetTimeCountInNanoSeconds( TimeCountInNanoSeconds);
  GetTimeCountInSeconds( TimeCountInSeconds);
//  DEBUGN_LOGGER_NAME( (* s_p_logger), FULL_FUNC_NAME << "core:" << wCPUcoreID
//    << " TimeCountInNanoSeconds:"    << TimeCountInNanoSeconds)

  //-1 Indicates an error to the core (service / GUI).
//  float fCPUcoreUsage = -1.0 ;
  //fCPUcoreUsage = >>function_name<<([...,]wCPUcoreID[,...]) ;

  idle_diff = s_idleCurrent - //s_idlePrevious;
    //s_p_ar_Idle[wCPUcoreID]
    s_p_ar_cpucoreattributes[wCPUcoreID].IdleTime;
//  s_idlePrevious = s_idleCurrent;
  s_p_ar_cpucoreattributes[wCPUcoreID].IdleTime = s_idleCurrent;

//  s_p_ar_Idle[wCPUcoreID] = s_idleCurrent;
//  TimeCountDiffNanoSeconds = TimeCountInNanoSeconds -
//    s_p_ar_cpucoreattributes[wCPUcoreID].TimeCountInNanoSeconds;
  TimeCountDiffInSeconds = TimeCountInSeconds -
    s_p_ar_cpucoreattributes[wCPUcoreID].TimeCountInSeconds;
//  DEBUGN_LOGGER_NAME( (* s_p_logger), "TimeCountDiffNanoSeconds:"
//    << TimeCountDiffNanoSeconds)
//  s_p_ar_cpucoreattributes[wCPUcoreID].TimeCountInNanoSeconds =
//    TimeCountInNanoSeconds;
  s_p_ar_cpucoreattributes[wCPUcoreID].TimeCountInSeconds =
    TimeCountInSeconds;
  //e.g. 100 ticks /s, a 2s = 200 ticks
  Ticks = (float) s_TicksPerSecond * //( (double) TimeCountDiffNanoSeconds /
    //1000000000.0 );
    TimeCountDiffInSeconds;

  DEBUGN_LOGGER_NAME( (* s_p_logger), "TimeDiff in "
//    "ns:" << TimeCountDiffNanoSeconds
    "s:" << TimeCountDiffInSeconds
    << " idle_diff:" << idle_diff << "Ticks:" << Ticks)
//  return (float) (s_TicksPerSecond - idle_diff) / (float) s_TicksPerSecond;
  //e.g. 2 s a 100 Hz = max. 200 ticks in this timespan, 50 idle ticks: 50 / 200 = 25%

  busyTicks = Ticks - idle_diff;
  return busyTicks
    //idle_diff
    / Ticks;
}

/** @see http://www.cpp-home.com/tutorials/108_3.htm */
void _fini()
{
  DEBUGN_LOGGER_NAME( (* s_p_logger), "_fini")
  if( //s_p_ar_Idle
      s_p_ar_cpucoreattributes )
    delete [] //s_p_ar_Idle;
      s_p_ar_cpucoreattributes;
}

//class I_CPUaccess;///Forward declaration

extern "C"
/** Called by the GUI/ service after loading this dynamic lib.*/
BYTE Init(
  /** Using the class I_CPUaccess is easier to interact with existing sourcecode
   * for the core (GUI/ service) but that makes it compiler-dependent (core and
   * dynamic library need to be compiled with the same compiler).
   * To be independent declare a void pointer instead (and don't use this
   * pointer). */
  I_CPUaccess * pi_cpuaccess
//  void * p_v
  )
{
  DEBUGN_LOGGER_NAME( (* s_p_logger), FULL_FUNC_NAME << " begin")
#ifdef _DEBUG
  s_p_logger->OpenFileA("proc.stat.txt");
  s_p_logger->m_logLevel = LogLevel::log_message_typeINFO;
#endif //#ifdef _DEBUG
  /** "man stat man 5 proc":
   * "The amount of time, measured in units of USER_HZ (1/100ths of a second on
   * most architectures, use sysconf(_SC_CLK_TCK) to obtain the right value),"*/
  s_TicksPerSecond = sysconf(_SC_CLK_TCK);
  fastestSignedDataType numberOfCPUcores;
  if(pi_cpuaccess)
    numberOfCPUcores = pi_cpuaccess->GetNumberOfCPUCores();
  else
    ///Same as shell command "getconf _NPROCESSORS_ONLN" ?
    numberOfCPUcores = sysconf(_SC_NPROCESSORS_ONLN);
  DEBUGN_LOGGER_NAME( (* s_p_logger), FULL_FUNC_NAME << " numberOfCPUcores:"
    << numberOfCPUcores)
  if( numberOfCPUcores)
  {
  //  s_p_ar_Idle = new unsigned long [numberOfCPUcores ];
    s_p_ar_cpucoreattributes = new CPUcoreAttributes[numberOfCPUcores ];
    DEBUGN_LOGGER_NAME( (* s_p_logger), FULL_FUNC_NAME << " s_p_ar_cpucoreattributes:"
      << s_p_ar_cpucoreattributes)
    if( s_p_ar_cpucoreattributes )
    {
      while( numberOfCPUcores -- )
      {
//        getUsage( s_user_mode, s_nice, s_system, s_p_ar_cpucoreattributes[
//          numberOfCPUcores].IdleTime, numberOfCPUcores );
        GetCPUcoreUsage(numberOfCPUcores);
//        GetTimeCountInNanoSeconds( s_p_ar_cpucoreattributes[numberOfCPUcores].
//          TimeCountInNanoSeconds);
        GetTimeCountInSeconds( s_p_ar_cpucoreattributes[numberOfCPUcores].
          TimeCountInSeconds);
//        --numberOfCPUcores;
      }
      return 0;
    }
  }
  return 1;
}
