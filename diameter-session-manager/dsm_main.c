#include "dsm.h"  /* Always include this before all DSM header files */
#include "dsm_fsm.h"
#include "dsm_tdb.h"
#include "dsm_ccdb.h"

#include "fep_devlib.h"
#include "diam_qmsg.h"
#include "diam_config.h"
#include "ipni_gmp.h"

uint debug_mask[NUM_DEBUG];
int errno_saved;
struct flags flags;
DiamSessMeas meas;
struct callwheel callwheel;
struct ccdb ccdb;
fep_timer_t *ptimer;

char *task_getname ()
{
  char me[] = "task_getname";

  XPRINT_ENTERED;

  return TASK_NAME;
}

char *debug_msg_itoa (uint debug_mask2)
{
  char me[] = "debug_msg_itoa";

  XPRINT_ENTERED;

  if (debug_mask2 == debug_mask[DEBUG_MSG_CLOSE_ENTER])
    return "__CLOSE_ENTER";

  if (debug_mask2 == debug_mask[DEBUG_MSG_CLOSE_EXIT])
    return "__CLOSE_EXIT";

  if (debug_mask2 == debug_mask[DEBUG_MSG_DIAM_ENTER])
    return "__DIAM_ENTER";

  if (debug_mask2 == debug_mask[DEBUG_MSG_DIAM_EXIT])
    return "__DIAM_EXIT";

  if (debug_mask2 == debug_mask[DEBUG_MSG_EXPTCC_ENTER])
    return "__EXPTCC_ENTER";

  if (debug_mask2 == debug_mask[DEBUG_MSG_EXPTCC_EXIT])
    return "__EXPTCC_EXIT";

  if (debug_mask2 == debug_mask[DEBUG_MSG_EXPTG_ENTER])
    return "__EXPTG_ENTER";

  if (debug_mask2 == debug_mask[DEBUG_MSG_EXPTG_EXIT])
    return "__EXPTG_EXIT";

  if (debug_mask2 == debug_mask[DEBUG_MSG_GDI_ENTER])
    return "__GDI_ENTER";

  if (debug_mask2 == debug_mask[DEBUG_MSG_GDI_EXIT])
    return "__GDI_EXIT";

  if (debug_mask2 == debug_mask[DEBUG_MSG_MISC_ENTER])
    return "__MISC_ENTER";

  if (debug_mask2 == debug_mask[DEBUG_MSG_QUEUE_ENTER])
    return "__QUEUE_ENTER";

  if (debug_mask2 == debug_mask[DEBUG_MSG_QUEUE_EXIT])
    return "__QUEUE_EXIT";

  if (debug_mask2 == debug_mask[DEBUG_MSG_UPCP_ENTER])
    return "__UPCP_ENTER";

  if (debug_mask2 == debug_mask[DEBUG_MSG_UPCP_EXIT])
    return "__UPCP_EXIT";

  if (debug_mask2 == debug_mask[DEBUG_MSG_USRM_ENTER])
    return "__USRM_ENTER";

  if (debug_mask2 == debug_mask[DEBUG_MSG_USRM_EXIT])
    return "__USRM_EXIT";

  return "__?";
}

void print_debug_mask (char *caller)
{
  char me[] = "print_debug_mask";

  int i;

  PRINTF_ENTERED;

  printf ("%s (%s):\n", me, caller);
  printf ("\n");
  printf ("Debug Masks\n");
  printf ("===============================================================================\n");
  for (i = 0; i < NUM_DEBUG; ++i)
    printf ("%02d: 0x%08x: %s\n", i, debug_mask[i], DEBUG_ITOA (i));

  printf ("\n");
  printf ("Select Combination Debug Masks\n");
  printf ("===============================================================================\n");
  printf ("0x%08x: Errors & warnings, type of DC, & Session-Id of US to be removed *\n", DEBUG_MASK_DEFAULT);
  printf ("0x%08x: Type of DC to be queued, received, or to be sent\n", DEBUG_MASK_DIAMCTX_ALL);
  printf ("0x%08x: General DSM conditions (errors, warnings, & informational)\n", DEBUG_MASK_DSM_ALL);
  printf ("0x%08x: Type of DC, & Session-Id of US to be removed\n", DEBUG_MASK_EVENTS);
  printf ("0x%08x: DC & US in any message processing (init & final)\n", DEBUG_MASK_MSG_ALL);
  printf ("0x%08x: DC & US in GDI-DIM_Close processing (init & final)\n", DEBUG_MASK_MSG_CLOSE_ALL);
  printf ("0x%08x: DC & US in Diameter message processing (init & final)\n", DEBUG_MASK_MSG_DIAM_ALL);
  printf ("0x%08x: DC & US in Tcc expiration processing (init & final)\n", DEBUG_MASK_MSG_EXPTCC_ALL);
  printf ("0x%08x: DC & US in Tg expiration processing (init & final)\n", DEBUG_MASK_MSG_EXPTG_ALL);
  printf ("0x%08x: DC & US in GDI message processing (init & final)\n", DEBUG_MASK_MSG_GDI_ALL);
  printf ("0x%08x: DC in queued message processing (init & final)\n", DEBUG_MASK_MSG_QUEUE_ALL);
  printf ("0x%08x: US in update CP processing (init & final)\n", DEBUG_MASK_MSG_UPCP_ALL);
  printf ("0x%08x: DC & US in remove US processing (init & final)\n", DEBUG_MASK_MSG_USRM_ALL);
  printf ("0x%08x: Command code of qmsg received or sent\n", DEBUG_MASK_QMSG_ALL);

  printf ("\n");
}

void initialize_debug_mask ()
{
  char me[] = "initialize_debug_mask";

  int i, sizeof_fep_debug_mask;

  /*
   * # debug masks can't be greater than # bits in process debug mask (e.g.,
   * 4 bytes * 8 bits/byte = 32 bits)
   */
  sizeof_fep_debug_mask = sizeof (fep_debug_mask) * CHAR_BIT;  /* In bits */
  if (NUM_DEBUG > sizeof_fep_debug_mask)
  {
    fprintf (stderr, "# debug masks %d > sizeof (fep_debug_mask) %d bits, exiting\n");
    exit (1);
  }

  /*
   * Set debug masks
   */
  for (i = 0; i < NUM_DEBUG; ++i)
    debug_mask[i] = 1 << i;

  /*
   * Initialize process debug mask
   */
  fep_debug_mask = 0;
  print_debug_mask (me);
}

void finalize ()
{
  char me[] = "finalize";

  int rv;

  XPRINT_ENTERED;

  /*
   * Stop qmsg timer
   */
  if (ptimer)
  {
    if ((rv = fep_timer_stop (ptimer)) < 0)
      XPRINT_FAILED_RV ("fep_timer_stop");

    if ((rv = fep_timer_free (ptimer)) < 0)
      XPRINT_FAILED_RV ("fep_timer_free");
  }

  finalize_fsm ();
  finalize_tdb ();
  finalize_ccdb ();

  fep_dev_close ();
}

void finalize_exit (int status)
{
  char me[] = "finalize_exit";

  XPRINT_ENTERED;

  finalize ();
  exit (status);
}

char *bit_print (ulong hex, int num_bytes)
{
  char me[] = "bit_print";

  static char buffer[128];

  char *b;
  int num_bits, i;
  ulong mask;

  XPRINT_ENTERED;

  b = buffer;

  if ((num_bytes <= 0) || (num_bytes > ((int) sizeof (ulong))))
  {
    b += sprintf (b, "<Invalid num_bytes %d>", num_bytes);
    return buffer;
  }

  num_bits = num_bytes * CHAR_BIT;  /* # bytes * 8 bits/byte = # bits */
  mask = 1 << (num_bits - 1);

  for (i = 1; i <= num_bits; ++i)
  {
    b += sprintf (b, "%c", ((hex & mask) ? ('1') : ('0')));

    if ((i < num_bits) && ((i % CHAR_BIT) == 0))
      b += sprintf (b, " ");

    hex <<= 1;
  }

  return buffer;
}

void print_hash (uint debug_mask2, char *caller)
{
  char me[] = "print_hash";

  int i;

  XPRINT_ENTERED;

  if (! (fep_debug_mask & debug_mask2))
    return;

  FEP_XPRINT (debug_mask2, ("%s (%s):\n", me, caller));
  FEP_XPRINT (debug_mask2, ("\n"));
  FEP_XPRINT (debug_mask2, ("General Purpose Hash Functions\n"));
  FEP_XPRINT (debug_mask2, ("===============================================================================\n"));
  for (i = HASH_UNDEF; i < NUM_HASH; ++i)
    FEP_XPRINT (debug_mask2, ("Hash %02d: &hash_func 0x%08x, %s\n", i, ((i < 0) ? (0) : (hash_func[i])), HASH_ITOA (i)));

  FEP_XPRINT (debug_mask2, ("\n"));
}

void initialize ()
{
  char me[] = "initialize";

  int rv;

  PRINTF_ENTERED;

  if ((rv = fep_dev_open ()) < 0)
  {
    FPRINTF_FAILED_RV_EXIT (stderr, "fep_dev_open");
    exit (1);
  }

  if ((rv = fep_q_init (FEP_Q_DSM)) < 0)
  {
    FPRINTF_FAILED_RV_EXIT (stderr, "fep_q_init");

    fep_dev_close ();
    exit (1);
  }

  memset (&flags, 0, sizeof (flags));
  memset (&meas, 0, sizeof (meas));

  FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: fep_debug_mask 0x%x (%s)\n", me, fep_debug_mask, bit_print (fep_debug_mask, sizeof (fep_debug_mask))));
  XPRINT_PROC_INITIATED ("startup");
  print_hash (debug_mask[DEBUG_DSM_2], me);

  initialize_fsm ();
  initialize_tdb ();
  initialize_ccdb ();

  /*
   * Start qmsg timer
   */
  if (! (ptimer = fep_timer_alloc ()))
  {
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: fep_timer_alloc failed, ptimer 0x%x, exiting\n", me, ptimer));
    finalize_exit (1);
  }

  ptimer->data[0] = MSG_DSM_TIMER;
  ptimer->restart = CALLOUTTIMER;
  ptimer->due_time = msec_clock + ptimer->restart;
  if ((rv = fep_timer_start (ptimer)) < 0)
  {
    XPRINT_FAILED_RV_EXIT ("fep_timer_start");
    finalize_exit (1);
  }
}

void process_shutdown ()
{
  char me[] = "process_shutdown";

  XPRINT_ENTERED;

  XPRINT_PROC_INITIATED ("shutdown");
  finalize_exit (0);
}

void process_config_update ()
{
  char me[] = "process_config_update";

  int rv;

  XPRINT_ENTERED;

  /*
   * Send acknowledgement to TDG
   */
  if ((rv = tell_fem (MSG_FEM_CONFIG_OK, fep_get_qid (), 0, 0)) != SUCCESS)
  {
    ++meas.ds_cntr.ds_api_tell_tdg_fail;
    XPRINT_FAILED_TELL ("tell_tdg");

    ++meas.ds_cntr.ds_main_process_config_update_fail;
    return;
  }

  ++meas.ds_cntr.ds_api_tell_tdg_succ;
  XPRINT_QMSG_XMIT (MSG_FEM_CONFIG_OK);

  ++meas.ds_cntr.ds_main_process_config_update_succ;
}

void process_get_counters (fep_qmsg_t qmsg)
{
  char me[] = "process_get_counters";

  fep_buffer_t *pbuf;
  int rv, parm, flag_incremented;

  XPRINT_ENTERED;

  flag_incremented = FALSE;

  /*
   * Failed to allocate buffer
   */
  if (! (pbuf = fep_alloc_buffer (sizeof (meas))))
  {
    ++meas.ds_cntr.ds_api_fep_alloc_buffer_fail;
    XPRINT_FAILED_FEP_ALLOC_BUFFER (sizeof (meas));

    if (! flag_incremented)
    {
      ++meas.ds_cntr.ds_main_process_get_counters_fail;
      flag_incremented = TRUE;
    }
  }
  else
  {
    ++meas.ds_cntr.ds_api_fep_alloc_buffer_succ;

    /*
     * Don't initialize data field because it contains persistent data
     */
    memset (pbuf, 0, (sizeof (*pbuf) - sizeof (FEP_BUF_DATA (pbuf))));

    /*
     * Copy measurements
     */
    FEP_BUF_DATA_OFF (pbuf) = 0;
    FEP_BUF_DATA_LEN (pbuf) = sizeof (meas);
    memcpy (FEP_BUF_DATA (pbuf), &meas, FEP_BUF_DATA_LEN (pbuf));
  }

  /*
   * Failed to convert (& release) buffer to parameter
   */
  if ((parm = buf_to_parm (pbuf)) < 0)
  {
    ++meas.ds_cntr.ds_api_buf_to_parm_fail;
    XPRINT_FAILED_BUF_TO_PARM;

    /*
     * Free memory associated with measurements
     */
    fep_free_buffer (pbuf);
    ++meas.ds_cntr.ds_api_fep_free_buffer;

    if (! flag_incremented)
    {
      ++meas.ds_cntr.ds_main_process_get_counters_fail;
      flag_incremented = TRUE;
    }
  }

  /*
   * Send measurements
   */
  if ((rv = tell_fem (MSG_FEM_RET_COUNTERS, qmsg[1], FEP_Q_DSM, parm)) != SUCCESS)
  {
    ++meas.ds_cntr.ds_api_tell_tdg_fail;
    XPRINT_FAILED_TELL ("tell_tdg");

    /*
     * Free memory associated with measurements
     */
    fep_free_buffer (parm_to_buf (parm));
    ++meas.ds_cntr.ds_api_fep_free_buffer;

    if (! flag_incremented)
    {
      ++meas.ds_cntr.ds_main_process_get_counters_fail;
      flag_incremented = TRUE;
    }
  }
  else
  {
    ++meas.ds_cntr.ds_api_tell_tdg_succ;
    XPRINT_QMSG_XMIT (MSG_FEM_RET_COUNTERS);

    if (! flag_incremented)
    {
      ++meas.ds_cntr.ds_main_process_get_counters_succ;
      flag_incremented = TRUE;
    }
  }

  /*
   * Zero measurements
   */
  if (qmsg[3] == OMP_ZERO)
    memset (&meas, 0, sizeof (meas));
}

void process_update_tg (fep_qmsg_t qmsg)
{
  char me[] = "process_update_tg";

  XPRINT_ENTERED;

  if (qmsg[1] < TG_MIN)
  {
    ++meas.ds_cntr.ds_main_process_update_tg_tg_invalid;
    FEP_XPRINT (debug_mask[DEBUG_DSM_1], ("%s: Tg expiry %lu < TG_MIN %d, tg_expiry %d ms (unchanged)\n", me, qmsg[1], TG_MIN, ccdb.tg_expiry));

    ++meas.ds_cntr.ds_main_process_update_tg_fail;
    return;
  }

  ccdb.tg_expiry = qmsg[1];
  ++meas.ds_cntr.ds_main_process_update_tg_succ;
}

int main (int argc, char **argv)
{
  char me[] = "main";

  int i, rv;
  fep_qmsg_t qmsg;
  char buffer[128];
  char *b;

  initialize_debug_mask ();
  PRINTF_ENTERED;

  initialize ();
  while (TRUE)
  {
    /*
     * Shutdown initiated
     */
    if (flags.shutdown)
      process_shutdown ();

    /*
     * Check whether there is any work to do
     */
    if ((rv = fep_q_receive (&qmsg)) != SUCCESS)
    {
      ++meas.ds_cntr.ds_api_fep_q_receive_fail;
      XPRINT_FAILED_RV ("feq_q_receive");

      continue;
    }

    switch (qmsg[0])  /* qmsg command code */
    {
      case MSG_DSM_TIMER:
      case MSG_FEM_Q_TEST:

        /*
         * Debug frequently occuring qmsgs
         */
        if (fep_debug_mask & debug_mask[DEBUG_FREQUENT])
          XPRINT_QMSG_RECV (qmsg[0]);

	break;

      default:

        XPRINT_QMSG_RECV (qmsg[0]);
    }

    switch (qmsg[0])  /* qmsg command code */
    {
      case MSG_DSM_TIMER:  /* Interval timer expiration */

        process_timer ();
	break;

      case MSG_FEM_CONFIG_UPDATE:  /* Configuration data update */

	process_config_update ();
	break;

      case MSG_FEM_GET_COUNTERS:  /* Get counters request */

	process_get_counters (qmsg);
	break;

      case MSG_FEM_KEY_SET:  /* Key set */

	process_key_set (qmsg);
	break;

      case MSG_FEM_KEY_CLEAR:  /* Key clear */

	process_key_clear (qmsg);
	break;

      case MSG_FEM_KEY_DISPLAY_REQ:  /* Key display request */

	process_key_display_req (qmsg);
	break;

      case MSG_DSM_UPDATE_TG:  /* Tg expiry update */

	process_update_tg (qmsg);
	break;

      case MSG_DSM_UPDATE_CP:  /* Inbound message update */

	process_update_cp (qmsg);
	break;

      case MSG_DIAMCTX:  /* DGC message */

	process_diamctx (qmsg, FALSE);
	break;

      default:

	if (fep_default_qmsg_handler (qmsg) != SUCCESS)
        {
          ++meas.ds_cntr.ds_qmsg_invalid_rcvd;

	  b = buffer;
	  for (i = 0; i < FEP_QMSG_N_PARMS; ++i)
	    b += sprintf (b, "0x%x ", qmsg[i]);

	  FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: Invalid qmsg, qmsg %s\n", me, buffer));
	}
    }  /* switch (qmsg[0]) */

    fflush (stdout);
    fflush (stderr);
  }  /* while (TRUE) */

  /*
   * Shouldn't reach here
   */
  FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: Shouldn't reach here\n", me));
  process_shutdown ();
  return 0;
}

void fepmem_memory_print (Memory *p, uint debug_mask2, char *caller)
{
  char me[] = "fepmem_memory_print";

  XPRINT_ENTERED;

  if (! (fep_debug_mask & debug_mask2))
    return;

  if (! p)  /* NULL memory */
  {
    XPRINT_NULL ("memory", p);
    return;
  }

  FEP_XPRINT (debug_mask2, ("%s (%s):\n", me, caller));
  FEP_XPRINT (debug_mask2, ("\n"));
  FEP_XPRINT (debug_mask2, ("Memory (struct mem) Dump\n"));
  FEP_XPRINT (debug_mask2, ("===============================================================================\n"));
  FEP_XPRINT (debug_mask2, ("mem_text    %s\n", p->mem_text));
  FEP_XPRINT (debug_mask2, ("mem_struct  %s\n", p->mem_struct));
  FEP_XPRINT (debug_mask2, ("mem_fatal   %d\n", p->mem_fatal));
  FEP_XPRINT (debug_mask2, ("mem_address 0x%x\n", p->mem_address));
  FEP_XPRINT (debug_mask2, ("mem_count   %lu\n", p->mem_count));
  FEP_XPRINT (debug_mask2, ("mem_delete  %lu\n", p->mem_delete));
  FEP_XPRINT (debug_mask2, ("mem_fail    %lu\n", p->mem_fail));
  FEP_XPRINT (debug_mask2, ("mem_length  %lu\n", p->mem_length));
  FEP_XPRINT (debug_mask2, ("mem_maxuse  %lu\n", p->mem_maxuse));
  FEP_XPRINT (debug_mask2, ("mem_new     %lu\n", p->mem_new));
  FEP_XPRINT (debug_mask2, ("mem_name    %s\n", p->mem_name));
  FEP_XPRINT (debug_mask2, ("mem_psosid  %lu\n", p->mem_psosid));
  FEP_XPRINT (debug_mask2, ("mem_size    %lu\n", p->mem_size));
  FEP_XPRINT (debug_mask2, ("mem_txtlen  %u\n", p->mem_txtlen));
  FEP_XPRINT (debug_mask2, ("mem_type    %d\n", p->mem_type));
  FEP_XPRINT (debug_mask2, ("\n"));
}

int hex_dump (char *buffer, char *data, int size, int flag_relative)
{
  char me[] = "hex_dump";

  static int offset_hex = 12;  /* See template below */
  static int offset_ascii = 63;  /* See template below */
  static char hex[] = "0123456789ABCDEF";  /* All possible hex characters */

/*
          1         2         3         4         5         6         7
01234567890123456789012345678901234567890123456789012345678901234567890123456789
[0x01234567] 00 11 22 33 44 55 66 77 88 99 AA BB CC DD EE FF   0123456789ABCDEF
            ^Start of hex dump              Start of ASCII dump^
*/
  static char template[] = "[0x01234567]                                                                    ";  /* 80 characters total per output line */

  ulong bytes_written, addr_data, addr_output;
  char output[128];
  char *b, *p_data, *p_hex, *p_ascii;
  char c;
  int i;

  XPRINT_ENTERED;

  bytes_written = 0;

  if (! buffer)  /* NULL write buffer */
  {
    XPRINT_NULL ("write buffer", buffer);
    return bytes_written;
  }

  if (! data)  /* NULL data source */
  {
    XPRINT_NULL ("data source", data);
    return bytes_written;
  }

  if (size <= 0)  /* Invalid data source size */
  {
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: Invalid data source size %d\n", me, size));
    return bytes_written;
  }

  b = buffer;
  b += sprintf (b, "Size %d ", size);

  if (size > MAX_SZ_HEX_DUMP)  /* Limit hex dump size */
  {
    size = MAX_SZ_HEX_DUMP;
    b += sprintf (b, "(only hex dumping 1st %d bytes)", size);
  }
  b += sprintf (b, "\n");

  /*
   * Point to data & set up addresses
   */
  p_data = data;
  if (flag_relative)  /* Display relative addresses */
  {
    addr_data = 0;
    addr_output = 0;
  }
  else  /* Display absolute addresses */
  {
    addr_data = (ulong) p_data;
    addr_output = addr_data & (~0xF);  /* Round to nearest 16 */
  }

  /*
   * Move hex & ASCII output pointers to appropriate output positions
   */
  p_hex = &output[offset_hex + ((addr_data - addr_output) * 3)];
  p_ascii = &output[offset_ascii + (addr_data - addr_output)];

  /*
   * Copy template into output build area
   */
  strcpy (output, template);

  /*
   * Process each byte of data
   */
  for (i = 1; i <= size; ++i)
  {
    /*
     * Build a space & hex digit
     */
    c = *p_data++;
    *p_hex++ = ' ';
    *p_hex++ = hex[(c >> 4)];
    *p_hex++ = hex[(c & 0xF)];

    /*
     * If character is printable, add ASCII character, else add dot
     */
    *p_ascii++ = ((isprint (c)) ? (c) : ('.'));

    /*
     * Reached end of line or end of data
     */
    ++addr_data;
    if (((addr_data & 0xF) == 0) || (i >= size))
    {
      /*
       * Add address to beginning of output build area.  Remove NULL inserted
       * by sprintf.
       */
      sprintf (output, "[0x%08x]", addr_output);
      output[12] = ' ';
      addr_output += 16;

      /*
       * Print entire output build area
       */
      b += sprintf (b, "%s\n", output);

      /*
       * Move hex & ASCII output pointers to appropriate output positions
       */
      p_hex = &output[offset_hex];
      p_ascii = &output[offset_ascii];

      /*
       * Copy template into output build area
       */
      strcpy (output, template);
    }
  }

  bytes_written = b - buffer;
  return bytes_written;
}

char *time_itoa (ulong timestamp)
{
  char me[] = "time_itoa";

  static char buffer[128];

  struct tm *t;

  XPRINT_ENTERED;

  strcpy (buffer, DSM_UNDEF_STR);

  /*
   * Timestamp undefined (not set)
   */
  if (timestamp <= 0)
    return buffer;

  if (t = (struct tm *) localtime (&timestamp))
    strftime (buffer, 128, "%m/%d/%y, %T", t);

  return buffer;
}

char *string_hex_print (char *string, int length)
{
  char me[] = "string_hex_print";

  static char buffer[512];

  char ascii[MAX_SZ_OCTETSTRING], *b;
  int i;

  XPRINT_ENTERED;

  b = buffer;

  if (! string)
  {
    b += sprintf (b, "<NULL string 0x%x>", string);
    return buffer;
  }

  if ((length <= 0) || (length >= MAX_SZ_OCTETSTRING))
  {
    b += sprintf (b, "<Invalid length %d>", length);
    return buffer;
  }

  if (fep_debug_mask & debug_mask[DEBUG_HEX_DUMP])
    b += sprintf (b, "0x ");

  for (i = 0; i < length; ++i)
  {
    if (fep_debug_mask & debug_mask[DEBUG_HEX_DUMP])
      b += sprintf (b, "%02x ", string[i]);

    /*
     * If character is printable, add ASCII character, else add dot
     */
    ascii[i] = ((isprint (string[i])) ? (string[i]) : ('.'));
  }

  ascii[i] = '\0';
  b += sprintf (b, "(%s)", ascii);

  return buffer;
}

/*
From http://www.partow.net/programming/hashfunctions/index.html:

"A simple hash function from Robert Sedgwicks Algorithms in C book. I've added
some simple optimizations to the algorithm in order to speed up its hashing
process."
*/
uint hash_rs (char *string, int length)
{
  char me[] = "hash_rs";

  char *p;
  uint hash;
  uint a, b;
  int i;

  XPRINT_ENTERED;

  hash = 0;
  a = 63689;
  b = 378551;

  for (p = string, i = 0; i < length; ++p, ++i)
  {
    hash = hash * a + (*p);
    a = a * b;
  }

  return (hash & 0x7FFFFFFF);
}

/*
From http://www.partow.net/programming/hashfunctions/index.html:

"A bitwise hash function written by Justin Sobel."
*/
uint hash_js (char *string, int length)
{
  char me[] = "hash_js";

  char *p;
  uint hash;
  int i;

  XPRINT_ENTERED;

  hash = 1315423911;

  for (p = string, i = 0; i < length; ++p, ++i)
    hash ^= ((hash << 5) + (*p) + (hash >> 2));

  return (hash & 0x7FFFFFFF);
}

/*
From http://www.partow.net/programming/hashfunctions/index.html:

"This hash algorithm is based on work by Peter J. Weinberger of AT&T Bell
Labs."
*/
uint hash_pjw (char *string, int length)
{
  char me[] = "hash_pjw";

  char *p;
  uint hash;
  uint BitsInUnsignedInt;
  uint ThreeQuarters;
  uint OneEighth;
  uint HighBits;
  uint test;
  int i;

  XPRINT_ENTERED;

  hash = 0;
  BitsInUnsignedInt = (uint) (sizeof (uint) * 8);
  ThreeQuarters = (uint) ((BitsInUnsignedInt * 3) / 4);
  OneEighth = (uint) (BitsInUnsignedInt / 8);
  HighBits = (uint) (0xFFFFFFFF << (BitsInUnsignedInt - OneEighth));
  test = 0;

  for (p = string, i = 0; i < length; ++p, ++i)
  {
    hash = (hash << OneEighth) + (*p);

    if ((test = (hash & HighBits)) != 0)
      hash = (hash ^ (test >> ThreeQuarters)) & (~HighBits);
  }

  return (hash & 0x7FFFFFFF);
}

/*
From http://www.partow.net/programming/hashfunctions/index.html:

"Similar to the PJW Hash function, but tweaked for 32-bit processors. Its the
hash function widely used on most UNIX systems."
*/
uint hash_elf (char *string, int length)
{
  char me[] = "hash_elf";

  char *p;
  uint hash;
  uint x;
  int i;

  XPRINT_ENTERED;

  hash = 0;
  x = 0;

  for (p = string, i = 0; i < length; ++p, ++i)
  {
    hash = (hash << 4) + (*p);

    if ((x = (hash & 0xF0000000L)) != 0)
    {
      hash ^= (x >> 24);
      hash &= (~x);
    }
  }

  return (hash & 0x7FFFFFFF);
}

/*
From http://www.partow.net/programming/hashfunctions/index.html:

"This hash function comes from Brian Kernighan and Dennis Ritchie's book "The C
Programming Language". It is a simple hash function using a strange set of
possible seeds which all constitute a pattern of 31....31...31 etc, it seems to
be very similar to the DJB hash function."
*/
uint hash_bkdr (char *string, int length)
{
  char me[] = "hash_bkdr";

  char *p;
  uint hash;
  uint seed;
  int i;

  XPRINT_ENTERED;

  hash = 0;
  seed = 131;  /* 31 131 1313 13131 131313 etc */

  for (p = string, i = 0; i < length; ++p, ++i)
    hash = (hash * seed) + (*p);

  return (hash & 0x7FFFFFFF);
}

/*
From http://www.partow.net/programming/hashfunctions/index.html:

"This is the algorithm of choice which is used in the open source SDBM project.
The hash function seems to have a good over-all distribution for many different
data sets. It seems to work well in situations where there is a high variance
in the MSBs of the elements in a data set."
*/
uint hash_sdbm (char *string, int length)
{
  char me[] = "hash_sdbm";

  char *p;
  uint hash;
  int i;

  XPRINT_ENTERED;

  hash = 0;

  for (p = string, i = 0; i < length; ++p, ++i)
    hash = (*p) + (hash << 6) + (hash << 16) - hash;

  return (hash & 0x7FFFFFFF);
}

/*
From http://www.partow.net/programming/hashfunctions/index.html:

"An algorithm produced by Professor Daniel J. Bernstein and shown first to the
world on the usenet newsgroup comp.lang.c. It is one of the most efficient hash
functions ever published."
*/
uint hash_djb (char *string, int length)
{
  char me[] = "hash_djb";

  char *p;
  uint hash;
  int i;

  XPRINT_ENTERED;

  hash = 5381;

  for (p = string, i = 0; i < length; ++p, ++i)
    hash = (hash << 5) + hash + (*p);

  return (hash & 0x7FFFFFFF);
}

/*
From http://www.partow.net/programming/hashfunctions/index.html:

"An algorithm proposed by Donald E. Knuth in The Art Of Computer Programming
Volume 3, under the topic of sorting and search chapter 6.4."
*/
uint hash_dek (char *string, int length)
{
  char me[] = "hash_dek";

  char *p;
  uint hash;
  int i;

  XPRINT_ENTERED;

  hash = strlen (string);

  for (p = string, i = 0; i < length; ++p, ++i)
    hash = ((hash << 5) ^ (hash >> 27)) ^ (*p);

  return (hash & 0x7FFFFFFF);
}

/*
From http://www.partow.net/programming/hashfunctions/index.html:

"An algorithm produced by me Arash Partow. I took ideas from all of the above
hash functions making a hybrid rotative and additive hash function algorithm
based around four primes 3,5,7 and 11. There isn't any real mathematical
analysis explaining why one should use this hash function instead of the others
described above other than the fact that I tired to resemble the design as
close as possible to a simple LFSR. An empirical result which demonstrated the
distributive abilities of the hash algorithm was obtained using a hash-table
with 100003 buckets, hashing The Project Gutenberg Etext of Webster's
Unabridged Dictionary, the longest encountered chain length was 7, the average
chain length was 2, the number of empty buckets was 4579."
*/
uint hash_ap (char *string, int length)
{
  char me[] = "hash_ap";

  char *p;
  uint hash;
  int i;

  XPRINT_ENTERED;

  hash = 0;
  i = 0;

  for (p = string, i = 0; i < length; ++p, ++i)
    if ((i & 1) == 0)
      hash ^= ((hash << 7) ^ (*p) ^ (hash >> 3));
    else
      hash ^= (~((hash << 11) ^ (*p) ^ (hash >> 5)));

  return (hash & 0x7FFFFFFF);
}
