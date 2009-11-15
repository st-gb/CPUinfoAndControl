#include "time_secure.h"

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

