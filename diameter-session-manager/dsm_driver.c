#include "dsm.h"
#include "dsm_fsm.h"
#include "dsm_tdb.h"
#include "dsm_ccdb.h"
#include "dsm_driver.h"

struct driver_timer timer;
struct driver_flags driver_flags;

void initialize_driver ()
{
  char me[] = "initialize_driver";

  int i;

  PRINTF_DRIVER_ENTERED;

  srand (time (NULL));

  memset (&timer, 0, sizeof (timer));
  memset (&driver_flags, 0, sizeof (driver_flags));

  for (i = 0; i < DRIVER_MAX_TIMERS; ++i)
  {
    timer.index[i] = i;
    timer.timer[i] = DRIVER_TIME_UNDEF;
  }
}

void finalize_driver ()
{
  char me[] = "finalize_driver";

  PRINTF_DRIVER_ENTERED;
}

void driver_timer_callback (void *arg)
{
  char me[] = "driver_timer_callback";

  static int num_expired_timers = 0;
  static int timer_prev = 0;

  int i, timer_curr;

  PRINTF_DRIVER_ENTERED;

  if (! arg)
  {
    printf ("%s: NULL argument 0x%x\n", me, arg);

    flags.shutdown = TRUE;
    return;
  }

  i = *((int *) arg);
  if ((i < 0) || (i >= DRIVER_MAX_TIMERS))
  {
    printf ("%s: Invalid timer index, i %d\n", me, i);

    flags.shutdown = TRUE;
    return;
  }

  timer_curr = timer.timer[i];
  ++num_expired_timers;
  --timer.num_timers;
  printf ("%s: Expired timer %3d/%3d, i %3d, time %6d %c\n", me, num_expired_timers, timer.num_timers, i, timer_curr, ((timer_curr < timer_prev) ? ('*') : (' ')));

  if ((timer.num_timers < 0) || (timer.num_timers > DRIVER_MAX_TIMERS))
  {
    printf ("%s: Invalid num_timers %d\n", me, timer.num_timers);

    flags.shutdown = TRUE;
    return;
  }

  timer_prev = timer_curr;
  timer.timer[i] = DRIVER_TIME_UNDEF;
  timer.id[i] = ID_UNDEF;
}

void driver_random_event ()
{
  char me[] = "driver_random_event";

  static int first_time = TRUE;
  static int num_events;
  static uint start_time;

  uint diff_time;
  int i, j, k, n, event;

  PRINTF_DRIVER_ENTERED;

  if (first_time)
  {
    first_time = FALSE;

    start_time = 0;
    num_events = 0;
  }

  event = ((driver_flags.do_nothing) ? (DRIVER_MAX_EVENT - 1) : (rand () % DRIVER_MAX_EVENT));
  switch (event)
  {
    case 0:  /* (1 / DRIVER_MAX_EVENT) of time, set random # callouts */

      /*
       * All timers set; just return
       */
      if (timer.num_timers >= DRIVER_MAX_TIMERS)
        return;

      ++num_events;

      /*
       * Set n timers
       */
      n = (rand () % (DRIVER_MAX_TIMERS - timer.num_timers)) + 1;
      for (i = 0; i < n; ++i)
      {
        for (j = 0; j < DRIVER_MAX_TIMERS; ++j)
	  if (timer.timer[j] <= 0)  /* Found free slot */
	    break;

        if (j >= DRIVER_MAX_TIMERS)  /* No free slot found */
	  continue;

	/*
	 * Set timer
	 */
        timer.timer[j] = (rand () % (DRIVER_MAX_TIMER - CALLOUTTIMER)) + CALLOUTTIMER;
        timer.id[j] = callout_set (driver_timer_callback, &timer.index[j], timer.timer[j]);

	if (CALLOUT_ID_VALID (timer.id[j]))
        {
	  /*
	   * Successfully set timer
	   */

	  ++timer.num_timers;
	  PRINTF_CO_STATUS ("  co_set");
	}
	else
        {
          printf ("%s: callout_set failed, timer %d", me, timer.timer[j]);
	  flags.shutdown = TRUE;

	  timer.timer[j] = DRIVER_TIME_UNDEF;
	  timer.id[j] = ID_UNDEF;

	  return;
        }
      }  /* for i */

      PRINTF_CO_SUMMARY ("  co_set");
      return;

    case 1:  /* (1 / DRIVER_MAX_EVENT) of time, cancel random # callouts */

      /*
       * No timers set; just return
       */
      if (timer.num_timers <= 0)
        return;

      ++num_events;

      /*
       * Cancel n timers
       */
      n = (rand () % timer.num_timers) + 1;
      for (i = 0; i < n; ++i)
      {
	for (j = (DRIVER_MAX_TIMERS - 1); j >= 0; --j)
	  if (timer.timer[j] > 0)  /* Found timer to cancel */
	    break;

	if (j < 0)  /* No timers set */
	  continue;

        /*
         * Cancel timer
         */
        callout_unset (&timer.id[j]);

	--timer.num_timers;
	PRINTF_CO_STATUS ("co_unset");

        timer.timer[j] = DRIVER_TIME_UNDEF;
      }  /* for i */

      PRINTF_CO_SUMMARY ("co_unset");
      return;

    default:  /* ((DRIVER_MAX_EVENT - 2) / DRIVER_MAX_EVENT) of time, do nothing */

      if (driver_flags.do_nothing)
      {
	diff_time = time (NULL) - start_time;
	if (diff_time >= DRIVER_SLEEP_TIME)
        {
	  /*
	   * Stop doing nothing
	   */
	  start_time = 0;
	  driver_flags.do_nothing = FALSE;

          return;
        }

	/*
	 * Continue doing nothing
	 */
	return;
      }

      /*
       * Start doing nothing
       */
      start_time = time (NULL);
      driver_flags.do_nothing = TRUE;

      return;
  }  /* switch (event) */
}
