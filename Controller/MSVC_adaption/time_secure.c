#include "time_secure.h"

#ifdef _WINDOWS
  #include <time.h>

  int _ftime_s(struct
    timeb *  p_timeb)
  {
    _ftime(p_timeb) ;
    //Zero = success.
    return 0 ;
  }

  int localtime_s(
    struct tm * p_tm
   //TODO time_t makes problems with g++
   , const time_t * time
    )
  {
    int nRet = 0 ;
    struct tm * p_tmLocalTimeResult = localtime(time) ;
    if( p_tmLocalTimeResult )
      * p_tm = * p_tmLocalTimeResult ;
    else
      nRet = 1;
    return nRet ;
  }
#endif
