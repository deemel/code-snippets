#include "dsm.h"
#include "dsm_tdb.h"
#include "dsm_ccdb.h"

#include "fep_devlib.h"
#include "diam_qmsg.h"

/*
 * Needed to use private memory pool
 */
static struct callout fepmem_list_callout[USER_SESSION_SIZE];
static ulong fepmem_size_callout = sizeof (struct callout);
static Memory fepmem_memory_callout = DECL_MEM ("Callout", "callout", 0);
static PrivInfo fepmem_privInfo_callout = {"Callout", &fepmem_size_callout, ARRAY_SIZE (fepmem_list_callout), &fepmem_memory_callout, fepmem_list_callout};

void print_size_tdb (uint debug_mask2, char *caller)
{
  char me[] = "print_size_tdb";

  int sz_cw, sz_co, sz_co_max, sz_cw_max;

  XPRINT_ENTERED;

  if (! (fep_debug_mask & debug_mask2))
    return;

  sz_cw = sizeof (struct callwheel);
  sz_co = sizeof (struct callout);
  sz_co_max = sizeof (fepmem_list_callout);
  sz_cw_max = sz_cw + sz_co_max;

  FEP_XPRINT (debug_mask2, ("%s (%s):\n", me, caller));
  FEP_XPRINT (debug_mask2, ("\n"));
  FEP_XPRINT (debug_mask2, ("Timer Wheel Size\n"));
  FEP_XPRINT (debug_mask2, ("===============================================================================\n"));
  FEP_XPRINT (debug_mask2, ("(1) Initial timer wheel size   %10d\n", sz_cw));
  FEP_XPRINT (debug_mask2, ("(2) Timer size                 %10d\n", sz_co));
  FEP_XPRINT (debug_mask2, ("(3) Max outstanding timers     %10d\n", USER_SESSION_SIZE));
  FEP_XPRINT (debug_mask2, ("(4) Max outstanding timer size %10d ((3) * (2))\n", sz_co_max));
  FEP_XPRINT (debug_mask2, ("(5) Max timer wheel size       %10d ((1) + (4))\n", sz_cw_max));
  FEP_XPRINT (debug_mask2, ("\n"));
}

void callout_print (struct callout *p, uint debug_mask2, char *caller)
{
  char me[] = "callout_print";

  XPRINT_ENTERED;

  if (! (fep_debug_mask & debug_mask2))
    return;

  if (! p)  /* NULL callout */
  {
    ++meas.ds_cntr.ds_co_null;
    XPRINT_NULL ("callout", p);

    return;
  }

  FEP_XPRINT (debug_mask2, ("%s (%s):\n", me, caller));

  if (fep_debug_mask & debug_mask[DEBUG_HEX_DUMP])
  {
    FEP_XPRINT (debug_mask2, ("\n"));
    FEP_XPRINT (debug_mask2, ("Callout Hex Dump\n"));
    FEP_XPRINT (debug_mask2, ("===============================================================================\n"));
    dump_memory ("", p, sizeof (*p), DUMP_RELATIVE);
  }

  FEP_XPRINT (debug_mask2, ("\n"));
  FEP_XPRINT (debug_mask2, ("Callout Dump\n"));
  FEP_XPRINT (debug_mask2, ("===============================================================================\n"));
  FEP_XPRINT (debug_mask2, ("id       %lu\n", p->id));
  FEP_XPRINT (debug_mask2, ("head     0x%x\n", p->head));
  FEP_XPRINT (debug_mask2, ("c_func   0x%x\n", p->c_func));
  FEP_XPRINT (debug_mask2, ("c_arg    0x%x\n", p->c_arg));
  FEP_XPRINT (debug_mask2, ("c_time   %d\n", p->c_time));
  FEP_XPRINT (debug_mask2, ("c_period %d\n", p->c_period));
  FEP_XPRINT (debug_mask2, ("\n"));
}

void callout_init (struct callout *p)
{
  char me[] = "callout_init";

  if (fep_debug_mask & debug_mask[DEBUG_FREQUENT])
    XPRINT_ENTERED;

  if (! p)  /* NULL callout */
  {
    ++meas.ds_cntr.ds_co_null;
    XPRINT_NULL ("callout", p);

    ++meas.ds_cntr.ds_tdb_co_init_fail;
    return;
  }

  /*
   * Don't initialize 1st 2 fields because they contain persistent data
   */
  memset (((char *) p + sizeof (p->fifoBlk) + sizeof (p->id)), 0, (sizeof (*p) - sizeof (p->fifoBlk) - sizeof (p->id)));
  ++meas.ds_cntr.ds_tdb_co_init_succ;
}

void initialize_tdb ()
{
  char me[] = "initialize_tdb";

  int i;

  XPRINT_ENTERED;

  fep_create_private (&fepmem_privInfo_callout);
  ++meas.ds_cntr.ds_api_fep_create_private_co;

  memset (&callwheel, 0, sizeof (callwheel));
  for (i = 0; i < CALLWHEELSIZE; ++i)
  {
    TAILQ_INIT (&callwheel.callwheel[i]);
    ++meas.ds_cntr.ds_co_list_tailq_init;
  }

  for (i = 0; i < USER_SESSION_SIZE; ++i)
  {
    fepmem_list_callout[i].id = INDEX_TO_ID (i);
    callout_init (&fepmem_list_callout[i]);
  }

  print_size_tdb (debug_mask[DEBUG_DSM_2], me);
}

void finalize_tdb ()
{
  char me[] = "finalize_tdb";

  int i;
  struct callout_tailq *p;
  struct callout *q;

  XPRINT_ENTERED;

  /*
   * Remove all callouts from all lists
   */
  for (i = 0; i < CALLWHEELSIZE; ++i)
  {
    p = &callwheel.callwheel[i];
    for (q = p->tqh_first; q; q = p->tqh_first)
    {
      /*
       * Remove callout from list & free memory
       */
      TAILQ_REMOVE (p, q, tqe);
      ++meas.ds_cntr.ds_co_tailq_remove;

      callout_init (q);
      delete_private (&fepmem_memory_callout, q);
      ++meas.ds_cntr.ds_api_delete_private_co;
    }
  }
}

ulong callout_set (timeout_t func, void *arg, int expiry)
{
  char me[] = "callout_set";

  struct callout *new;
  int i, offset;

  XPRINT_ENTERED;

  if (expiry <= 0)  /* Meaningless expiry */
  {
    ++meas.ds_cntr.ds_tdb_co_set_expiry_invalid;
    FEP_XPRINT (debug_mask[DEBUG_DSM_1], ("%s: Meaningless expiry %d\n", me, expiry));

    ++meas.ds_cntr.ds_tdb_co_set_fail;
    return ID_UNDEF;
  }

  if (expiry < CALLOUTTIMER)  /* Expiry < minimum callout timer */
  {
    ++meas.ds_cntr.ds_tdb_co_set_expiry_invalid;
    FEP_XPRINT (debug_mask[DEBUG_DSM_1], ("%s: Expiry %d < minimum callout timer %d, expiry defaulting to %d ms\n", me, expiry, CALLOUTTIMER, CALLOUTTIMER));

    expiry = CALLOUTTIMER;
  }

  /*
   * Allocate callout memory
   */
  if (! (new = (struct callout *) new_private (&fepmem_memory_callout)))
  {
    ++meas.ds_cntr.ds_api_new_private_fail_co;
    XPRINT_FAILED ("new_private");
    fepmem_memory_print (&fepmem_memory_callout, debug_mask[DEBUG_DSM_2], me);

    ++meas.ds_cntr.ds_tdb_co_set_fail;
    return ID_UNDEF;
  }
  ++meas.ds_cntr.ds_api_new_private_succ_co;

  new->c_func = func;
  new->c_arg = arg;

  /*
   * # callwheel periods (rotations) =
   * (expiry / CALLWHEELPERIOD) =
   * ((expiry / CALLOUTTIMER) >> CALLWHEELBITS)
   */
  new->c_time = expiry;
  new->c_period = (new->c_time / CALLOUTTIMER) >> CALLWHEELBITS;

  /*
   * Callout list index offset from current index =
   * ((expiry % CALLWHEELPERIOD) / CALLOUTTIMER) =
   * ((expiry / CALLOUTTIMER) & CALLWHEELMASK)
   */
  offset = (new->c_time / CALLOUTTIMER) & CALLWHEELMASK;
  i = callwheel.index + offset;
  if (i >= CALLWHEELSIZE)
    /*
     * Loop around back to beginning of callwheel array.  Index i will never
     * be greater than (CALLWHEELSIZE - 1) because of the following:
     *   - Maximum offset = CALLWHEELMASK = (CALLWHEELSIZE - 1)
     *   - Maximum callwheel.index = CALLWHEELMASK = (CALLWHEELSIZE - 1)
     *   - Therefore maximum i =
     *     ((CALLWHEELSIZE - 1) - (CALLWHEELSIZE - (CALLWHEELSIZE - 1))) =
     *     (CALLWHEELSIZE - 2) < (CALLWHEELSIZE - 1)
     */
    i = offset - (CALLWHEELSIZE - callwheel.index);

  if ((i < 0) || (i >= CALLWHEELSIZE))
  {
    /*
     * Shouldn't reach here
     */
    ++meas.ds_cntr.ds_co_list_invalid;
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: Invalid callwheel index, i %d, offset %d, index %d, c_time %d\n", me, i, offset, callwheel.index, new->c_time));

    callout_init (new);
    delete_private (&fepmem_memory_callout, new);
    ++meas.ds_cntr.ds_api_delete_private_co;

    ++meas.ds_cntr.ds_tdb_co_set_fail;
    return ID_UNDEF;
  }

  /*
   * Insert new callout to end of list
   */
  new->head = &callwheel.callwheel[i];
  TAILQ_INSERT_TAIL (new->head, new, tqe);
  ++meas.ds_cntr.ds_co_tailq_insert_tail;

  callout_print (new, debug_mask[DEBUG_DSM_2], me);

  ++meas.ds_cntr.ds_tdb_co_set_succ;
  return new->id;
}

void callout_unset (ulong *id)
{
  char me[] = "callout_unset";

  struct callout *p;

  XPRINT_ENTERED;

  if (! id)  /* NULL callout ID pointer */
  {
    ++meas.ds_cntr.ds_co_id_null;
    XPRINT_NULL ("callout ID pointer", id);

    ++meas.ds_cntr.ds_tdb_co_unset_fail;
    return;
  }

  /*
   * Callout may not have been set
   */
  if (! CALLOUT_ID_VALID (*id))  /* Invalid callout ID */
  {
    ++meas.ds_cntr.ds_co_id_invalid;
    FEP_XPRINT (debug_mask[DEBUG_DSM_2], ("%s: Invalid callout ID %lu\n", me, *id));

    *id = ID_UNDEF;  /* Invalidate timer ID */
    ++meas.ds_cntr.ds_tdb_co_unset_fail;
    return;
  }
  p = &fepmem_list_callout[ID_TO_INDEX (*id)];

  if (! p->head)  /* NULL callout list */
  {
    ++meas.ds_cntr.ds_co_list_null;
    XPRINT_NULL ("callout list", p->head);

    *id = ID_UNDEF;  /* Invalidate timer ID */
    ++meas.ds_cntr.ds_tdb_co_unset_fail;
    return;
  }

  callout_print (p, debug_mask[DEBUG_DSM_2], me);

  /*
   * Remove callout from list & free memory
   */
  TAILQ_REMOVE (p->head, p, tqe);
  ++meas.ds_cntr.ds_co_tailq_remove;

  callout_init (p);
  delete_private (&fepmem_memory_callout, p);
  ++meas.ds_cntr.ds_api_delete_private_co;

  *id = ID_UNDEF;  /* Invalidate timer ID */
  ++meas.ds_cntr.ds_tdb_co_unset_succ;
}

void process_timer ()
{
  char me[] = "process_timer";

  struct callout_tailq *p;
  struct callout *q, *r;
  int rv;

  if (fep_debug_mask & debug_mask[DEBUG_FREQUENT])
    XPRINT_ENTERED;

  if ((++callwheel.index) >= CALLWHEELSIZE)
    callwheel.index = 0;

  /*
   * Remove & process expired callouts in list
   */
  p = &callwheel.callwheel[callwheel.index];
  q = p->tqh_first;
  while (q)
  {
    r = q;
    q = q->tqe.tqe_next;

    if (r->c_period <= 0)  /* Expired callout */
    {
      ++meas.ds_cntr.ds_co_expire;
      callout_print (r, debug_mask[DEBUG_DSM_2], me);

      TAILQ_REMOVE (p, r, tqe);
      ++meas.ds_cntr.ds_co_tailq_remove;

      if (r->c_func)
	r->c_func (r->c_arg);
      else
      {
	++meas.ds_cntr.ds_co_callback_null;
	FEP_XPRINT (debug_mask[DEBUG_DSM_1], ("%s: Callback function not specified, c_func 0x%x, c_arg 0x%x\n", me, r->c_func, r->c_arg));
      }

      callout_init (r);
      delete_private (&fepmem_memory_callout, r);
      ++meas.ds_cntr.ds_api_delete_private_co;
    }
    else
      --r->c_period;
  }
}
