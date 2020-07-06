#ifndef _DSM_TDB_H_
#define _DSM_TDB_H_

#include <sys/queue.h>
#include "fep_memory.h"
#include "dsm_ccdb.h"

/*
 * Based on paper, "Redesigning the BSD Callout and Timer Facilities," by
 * Adam M. Costello and George Varghese, WUCS-95-23, 11/2/95
 */

#define CALLWHEELBITS   16
#define CALLWHEELSIZE   (1 << CALLWHEELBITS)  /* 2^16 = 65536 callout slots */
#define CALLWHEELMASK   (CALLWHEELSIZE - 1)  /* 0xFFFF (65535) */
#define CALLOUTTIMER    10  /* ms (also means time per callout slot) */
#define CALLWHEELPERIOD (CALLOUTTIMER * CALLWHEELSIZE)  /* ms */
#define CALLOUT_ID_VALID(i) (((i >= ID_OFFSET) && (i <= USER_SESSION_SIZE)) ? (TRUE) : (FALSE))

typedef void (*timeout_t) (void *);  /* Timeout function type */

/*
 * Equivalent to:
 *
struct callout_tailq
{
  struct callout *tqh_first;  // First entry in queue
  struct callout **tqh_last;  // Ptr to last entry in queue
};
 */
TAILQ_HEAD (callout_tailq, callout);

struct callout
{
  /*
   * Must be 1st field in order to use private memory pool
   */
  FifoBlk *fifoBlk;

  /*
   * Must be 2nd field in order to initialize structure correctly.  (id -
   * ID_OFFSET) = index into fepmem_list_callout.
   */
  ulong id;

  /*
   * Equivalent to:
   *
  struct
  {
    struct callout *tqe_next;   // Next entry in queue
    struct callout **tqe_prev;  // Ptr back to ptr pointing at this entry
  } tqe;
   */
  TAILQ_ENTRY (callout) tqe;
  struct callout_tailq *head;  /* Head of list */

  timeout_t c_func;  /* Function to call */
  void      *c_arg;  /* Function argument */

  /*
   * c_period = (c_time / CALLWHEELPERIOD)
   */
  int c_time;    /* Expiration time (ms) */
  int c_period;  /* # callwheel periods (rotations) */
};

struct callwheel
{
  struct callout_tailq callwheel[CALLWHEELSIZE];
  int index;  /* Current index into callwheel */
};

#endif  /* _DSM_TDB_H_ */
