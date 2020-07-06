#ifndef _DSM_DRIVER_H_
#define _DSM_DRIVER_H_

#include "dsm_tdb.h"

#define DRIVER_MAX_TIMERS 999
#define DRIVER_MAX_TIMER  (3 * CALLWHEELSIZE) /* ms */
#define DRIVER_MAX_EVENT  10  /* Denominator in % of time calculation */
#define DRIVER_SLEEP_TIME 1  /* sec */
#define DRIVER_TIME_UNDEF -71770

#define PRINTF_CO_STATUS(func) printf ("%s: %s, time %6d, #timers %3d\n", me, func, timer.timer[j], timer.num_timers)
#define PRINTF_CO_SUMMARY(func) printf ("%s: %s, #timers %3d, n %3d, #events %3d\n", me, func, timer.num_timers, n, num_events)
#define PRINTF_DRIVER_ENTERED printf ("%s: Entered\n", me)

struct driver_timer
{
  int   index[DRIVER_MAX_TIMERS];
  int   timer[DRIVER_MAX_TIMERS];
  ulong id[DRIVER_MAX_TIMERS];  /* Callout ID */
  int   num_timers;
};

struct driver_flags
{
  uint __unused   : 31;
  uint do_nothing :  1;
};

#endif  /* _DSM_DRIVER_H_ */
