#include "dsm.h"
#include "dsm_fsm.h"

/*
 * State Transition Table (STT)
 */
struct ste *stt (short asm_id, short state, short event, int flag_print_fsm)
{
  char me[] = "stt";

  static struct ste ste;

  if (! flag_print_fsm)
    XPRINT_ENTERED;

  /*
   * Determine next STE based on ASM ID, current state, & event
   */
  switch (asm_id)
  {
    case APP_SM_CCAS:

      switch (state)
      {
        case STATE_CCAS_IDLE:

          switch (event)
          {
            case EVENT_CCAS_RX_DIAM_CCR_INIT:

	      ste.state = STATE_CCAS_PROC_REQ;
	      ste.action = ACTION_CCAS_TX_GDI_CCR_01;

	      break;

            case EVENT_CCAS_RX_DIAM_CCR_EVENT:

              ste.state = STATE_CCAS_PROC_REQ_FINAL;
              ste.action = ACTION_CCAS_TX_GDI_CCR_01;

              break;

            case EVENT_CCAS_RX_GDI_ABORT:

              ste.state = STATE_CCAS_IDLE;
              ste.action = ACTION_CCAS_CLOSE_GDI_TRANS;

              break;

            case EVENT_CCAS_RX_QMSG_FE_UNAVAIL:

              ste.state = STATE_CCAS_DISCONTINUING;
              ste.action = ACTION_CCAS_TX_GDI_STR_01;

              break;

            default:

              ++meas.ds_cntr.ds_asm_event_invalid;
	      if (! flag_print_fsm)
		XPRINT_INVALID_EVENT_CCAS;

              ste.state = STATE_CCAS_UNDEF;
              ste.action = ACTION_CCAS_UNDEF;
          }

          break;

        case STATE_CCAS_PROC_REQ:

          switch (event)
          {
            case EVENT_CCAS_RX_GDI_CCA_INIT_SUCC:

              ste.state = STATE_CCAS_OPEN;
              ste.action = ACTION_CCAS_TX_DIAM_CCA_02;

              break;

            case EVENT_CCAS_RX_GDI_CCA_UPDATE_SUCC:

              ste.state = STATE_CCAS_OPEN;
              ste.action = ACTION_CCAS_TX_DIAM_CCA_02;

              break;

            case EVENT_CCAS_RX_DIAM_MSG_VALID:

              ste.state = STATE_CCAS_PROC_REQ;
              ste.action = ACTION_CCAS_QUE_DIAM_MSG;

              break;

            case EVENT_CCAS_RX_GDI_RAR:

              ste.state = STATE_CCAS_PROC_REQ;
              ste.action = ACTION_CCAS_TX_DIAM_RAR_02;

              break;

            case EVENT_CCAS_RX_GDI_ASR:

              ste.state = STATE_CCAS_PROC_REQ;
              ste.action = ACTION_CCAS_TX_DIAM_ASR_02;

              break;

            case EVENT_CCAS_EXP_TG:

              ste.state = STATE_CCAS_REPORTING_ERROR;
              ste.action = ACTION_CCAS_TX_DIAM_CCA_03;

              break;

            case EVENT_CCAS_RX_GDI_CCA_INIT_FAIL:

              ste.state = STATE_CCAS_IDLE;
              ste.action = ACTION_CCAS_TX_DIAM_CCA_01;

              break;

            case EVENT_CCAS_RX_GDI_CCA_UPDATE_FAIL:

              ste.state = STATE_CCAS_IDLE;
              ste.action = ACTION_CCAS_TX_DIAM_CCA_01;

              break;

            case EVENT_CCAS_RX_QMSG_UP_CP:

              ste.state = STATE_CCAS_PROC_REQ;
              ste.action = ACTION_CCAS_UP_US;

              break;

            case EVENT_CCAS_RX_DIAMCTX_GDI_FAIL:

              ste.state = STATE_CCAS_REPORTING_ERROR;
              ste.action = ACTION_CCAS_TX_GDI_REPTERR;

              break;

            case EVENT_CCAS_RX_GDI_ABORT:

              ste.state = STATE_CCAS_PROC_REQ;
              ste.action = ACTION_CCAS_CLOSE_GDI_TRANS;

              break;

            case EVENT_CCAS_RX_QMSG_FE_UNAVAIL:

              ste.state = STATE_CCAS_DISCONTINUING;
              ste.action = ACTION_CCAS_TX_GDI_STR_01;

              break;

            case EVENT_CCAS_RX_DIAM_CCR_INIT_T:
            case EVENT_CCAS_RX_DIAM_CCR_EVENT_T:
            case EVENT_CCAS_RX_DIAM_CCR_UPDATE_T:
            case EVENT_CCAS_RX_DIAM_CCR_TERM_T:
            case EVENT_CCAS_RX_DIAM_STR_T:

	      ste.state = STATE_CCAS_PROC_REQ;
	      ste.action = ACTION_CCAS_UP_US;

	      break;

            default:

              ++meas.ds_cntr.ds_asm_event_invalid;
	      if (! flag_print_fsm)
		XPRINT_INVALID_EVENT_CCAS;

              ste.state = STATE_CCAS_UNDEF;
              ste.action = ACTION_CCAS_UNDEF;
          }

          break;

        case STATE_CCAS_OPEN:

          switch (event)
          {
            case EVENT_CCAS_RX_GDI_RAR:

              ste.state = STATE_CCAS_OPEN;
              ste.action = ACTION_CCAS_TX_DIAM_RAR_01;

              break;

            case EVENT_CCAS_RX_GDI_ASR:

              ste.state = STATE_CCAS_OPEN;
              ste.action = ACTION_CCAS_TX_DIAM_ASR_01;

              break;

            case EVENT_CCAS_RX_DIAM_RAA:

              ste.state = STATE_CCAS_OPEN;
              ste.action = ACTION_CCAS_TX_GDI_RAA;

              break;

            case EVENT_CCAS_RX_DIAM_ASA:

              ste.state = STATE_CCAS_OPEN;
              ste.action = ACTION_CCAS_TX_GDI_ASA;

              break;

            case EVENT_CCAS_EXP_TCC:

              ste.state = STATE_CCAS_DISCONTINUING;
              ste.action = ACTION_CCAS_TX_GDI_STR_01;

              break;

            case EVENT_CCAS_DQ_DIAM_CCR_TERM:
            case EVENT_CCAS_RX_DIAM_CCR_TERM:

              ste.state = STATE_CCAS_PROC_REQ_FINAL;
              ste.action = ACTION_CCAS_TX_GDI_CCR_02;

              break;

            case EVENT_CCAS_DQ_DIAM_STR:
            case EVENT_CCAS_RX_DIAM_STR:

              ste.state = STATE_CCAS_PROC_REQ_FINAL;
              ste.action = ACTION_CCAS_TX_GDI_STR_02;

              break;

            case EVENT_CCAS_DQ_DIAM_CCR_UPDATE:
            case EVENT_CCAS_RX_DIAM_CCR_UPDATE:

              ste.state = STATE_CCAS_PROC_REQ;
              ste.action = ACTION_CCAS_TX_GDI_CCR_02;

              break;

            case EVENT_CCAS_RX_DIAMCTX_GDI_FAIL:

              ste.state = STATE_CCAS_REPORTING_ERROR;
              ste.action = ACTION_CCAS_TX_GDI_REPTERR;

              break;

            case EVENT_CCAS_RX_GDI_ABORT:

              ste.state = STATE_CCAS_OPEN;
              ste.action = ACTION_CCAS_CLOSE_GDI_TRANS;

              break;

            case EVENT_CCAS_RX_QMSG_FE_UNAVAIL:

              ste.state = STATE_CCAS_DISCONTINUING;
              ste.action = ACTION_CCAS_TX_GDI_STR_01;

              break;

            default:

              ++meas.ds_cntr.ds_asm_event_invalid;
	      if (! flag_print_fsm)
		XPRINT_INVALID_EVENT_CCAS;

              ste.state = STATE_CCAS_UNDEF;
              ste.action = ACTION_CCAS_UNDEF;
          }

          break;

        case STATE_CCAS_PROC_REQ_FINAL:

          switch (event)
          {
            case EVENT_CCAS_RX_GDI_CCA_EVENT_SUCC:
            case EVENT_CCAS_RX_GDI_CCA_EVENT_FAIL:

              ste.state = STATE_CCAS_IDLE;
              ste.action = ACTION_CCAS_TX_DIAM_CCA_01;

              break;

            case EVENT_CCAS_RX_GDI_CCA_TERM_SUCC:
            case EVENT_CCAS_RX_GDI_CCA_TERM_FAIL:

              ste.state = STATE_CCAS_IDLE;
              ste.action = ACTION_CCAS_TX_DIAM_CCA_01;

              break;

            case EVENT_CCAS_RX_GDI_STA_SUCC:
            case EVENT_CCAS_RX_GDI_STA_FAIL:

              ste.state = STATE_CCAS_IDLE;
              ste.action = ACTION_CCAS_TX_DIAM_STA;

              break;

            case EVENT_CCAS_EXP_TG:

              ste.state = STATE_CCAS_REPORTING_ERROR;
              ste.action = ACTION_CCAS_TX_DIAM_CCA_03;

              break;

            case EVENT_CCAS_RX_QMSG_UP_CP:

              ste.state = STATE_CCAS_PROC_REQ_FINAL;
              ste.action = ACTION_CCAS_UP_US;

              break;

            case EVENT_CCAS_RX_DIAMCTX_GDI_FAIL:

              ste.state = STATE_CCAS_REPORTING_ERROR;
              ste.action = ACTION_CCAS_TX_GDI_REPTERR;

              break;

            case EVENT_CCAS_RX_GDI_ABORT:

              ste.state = STATE_CCAS_PROC_REQ_FINAL;
              ste.action = ACTION_CCAS_CLOSE_GDI_TRANS;

              break;

            case EVENT_CCAS_RX_QMSG_FE_UNAVAIL:

              ste.state = STATE_CCAS_DISCONTINUING;
              ste.action = ACTION_CCAS_TX_GDI_STR_01;

              break;

            case EVENT_CCAS_RX_DIAM_CCR_INIT_T:
            case EVENT_CCAS_RX_DIAM_CCR_EVENT_T:
            case EVENT_CCAS_RX_DIAM_CCR_UPDATE_T:
            case EVENT_CCAS_RX_DIAM_CCR_TERM_T:
            case EVENT_CCAS_RX_DIAM_STR_T:

	      ste.state = STATE_CCAS_PROC_REQ_FINAL;
	      ste.action = ACTION_CCAS_UP_US;

	      break;

            default:

              ++meas.ds_cntr.ds_asm_event_invalid;
	      if (! flag_print_fsm)
		XPRINT_INVALID_EVENT_CCAS;

              ste.state = STATE_CCAS_UNDEF;
              ste.action = ACTION_CCAS_UNDEF;
          }

          break;

        case STATE_CCAS_REPORTING_ERROR:

          switch (event)
          {
            case EVENT_CCAS_RX_GDI_CCA_INIT_SUCC:
            case EVENT_CCAS_RX_GDI_CCA_INIT_FAIL:
            case EVENT_CCAS_RX_GDI_CCA_EVENT_SUCC:
            case EVENT_CCAS_RX_GDI_CCA_EVENT_FAIL:
            case EVENT_CCAS_RX_GDI_CCA_UPDATE_SUCC:
            case EVENT_CCAS_RX_GDI_CCA_UPDATE_FAIL:
            case EVENT_CCAS_RX_GDI_CCA_TERM_SUCC:
            case EVENT_CCAS_RX_GDI_CCA_TERM_FAIL:
            case EVENT_CCAS_RX_GDI_STA_SUCC:
            case EVENT_CCAS_RX_GDI_STA_FAIL:

              ste.state = STATE_CCAS_REPORTING_ERROR;
              ste.action = ACTION_CCAS_DISC_GDI_MSG;

              break;

            case EVENT_CCAS_RX_GDI_DIM_CLOSE:
            case EVENT_CCAS_EXP_TG:

              ste.state = STATE_CCAS_IDLE;
              ste.action = ACTION_CCAS_STOP_TG;

              break;

            case EVENT_CCAS_RX_DIAMCTX_GDI_FAIL:

              ste.state = STATE_CCAS_REPORTING_ERROR;
              ste.action = ACTION_CCAS_TX_GDI_REPTERR;

              break;

            case EVENT_CCAS_RX_QMSG_UP_CP:

              ste.state = STATE_CCAS_REPORTING_ERROR;
              ste.action = ACTION_CCAS_UP_US;

              break;

            case EVENT_CCAS_RX_GDI_ABORT:

              ste.state = STATE_CCAS_REPORTING_ERROR;
              ste.action = ACTION_CCAS_CLOSE_GDI_TRANS;

              break;

            case EVENT_CCAS_RX_QMSG_FE_UNAVAIL:

              ste.state = STATE_CCAS_DISCONTINUING;
              ste.action = ACTION_CCAS_TX_GDI_STR_01;

              break;

            default:

              ++meas.ds_cntr.ds_asm_event_invalid;
	      if (! flag_print_fsm)
		XPRINT_INVALID_EVENT_CCAS;

              ste.state = STATE_CCAS_UNDEF;
              ste.action = ACTION_CCAS_UNDEF;
          }

          break;

        case STATE_CCAS_DISCONTINUING:

          switch (event)
          {
            case EVENT_CCAS_RX_GDI_STA_SUCC:
            case EVENT_CCAS_RX_GDI_STA_FAIL:
            case EVENT_CCAS_EXP_TG:

              ste.state = STATE_CCAS_IDLE;
              ste.action = ACTION_CCAS_STOP_TG;

              break;

            case EVENT_CCAS_RX_DIAMCTX_GDI_FAIL:

              ste.state = STATE_CCAS_REPORTING_ERROR;
              ste.action = ACTION_CCAS_TX_GDI_REPTERR;

              break;

            case EVENT_CCAS_RX_GDI_ABORT:

              ste.state = STATE_CCAS_DISCONTINUING;
              ste.action = ACTION_CCAS_CLOSE_GDI_TRANS;

              break;

            case EVENT_CCAS_RX_QMSG_FE_UNAVAIL:

              ste.state = STATE_CCAS_DISCONTINUING;
              ste.action = ACTION_CCAS_TX_GDI_STR_01;

              break;

            default:

              ++meas.ds_cntr.ds_asm_event_invalid;
	      if (! flag_print_fsm)
		XPRINT_INVALID_EVENT_CCAS;

              ste.state = STATE_CCAS_UNDEF;
              ste.action = ACTION_CCAS_UNDEF;
          }

          break;

        default:

	  ste.state = STATE_CCAS_IDLE;  /* Initialized state */
	  ste.action = ACTION_CCAS_UNDEF;
      }

      if (! flag_print_fsm)
	FEP_XPRINT (debug_mask[DEBUG_DSM_2], ("%s: ASM ID %d (%s), state0 %d (%s), event %d (%s), action %d (%s), state1 %d (%s)\n", me, asm_id, APP_SM_ITOA (asm_id), state, STATE_CCAS_ITOA (state), event, EVENT_CCAS_ITOA (event), ste.action, ACTION_CCAS_ITOA (ste.action), ste.state, STATE_CCAS_ITOA (ste.state)));
      break;

    default:

      ++meas.ds_cntr.ds_asm_invalid;
      if (! flag_print_fsm)
	FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: Invalid ASM ID %d (%s), state %d, event %d\n", me, asm_id, APP_SM_ITOA (asm_id), state, event));

      ste.state = DSM_UNDEF;
      ste.action = DSM_UNDEF;

      break;
  }

  return (&ste);
}

void print_fsm (uint debug_mask2, char *caller)
{
  char me[] = "print_fsm";

  int i, j;
  struct ste *ste;

  XPRINT_ENTERED;

  if (! (fep_debug_mask & debug_mask2))
    return;

  FEP_XPRINT (debug_mask2, ("%s (%s):\n", me, caller));
  FEP_XPRINT (debug_mask2, ("\n"));
  FEP_XPRINT (debug_mask2, ("Application State Machines (ASMs)\n"));
  FEP_XPRINT (debug_mask2, ("===============================================================================\n"));
  for (i = APP_SM_UNDEF; i < NUM_APP_SM; ++i)
    FEP_XPRINT (debug_mask2, ("ASM %02d: %s\n", i, APP_SM_ITOA (i)));

  FEP_XPRINT (debug_mask2, ("\n"));
  FEP_XPRINT (debug_mask2, ("%s State Machine\n", APP_SM_ITOA (APP_SM_CCAS)));
  FEP_XPRINT (debug_mask2, ("===============================================================================\n"));
  for (i = STATE_CCAS_UNDEF; i < NUM_STATE_CCAS; ++i)
    FEP_XPRINT (debug_mask2, ("State %02d: %s\n", i, STATE_CCAS_ITOA (i)));

  FEP_XPRINT (debug_mask2, ("\n"));
  for (i = EVENT_CCAS_UNDEF; i < NUM_EVENT_CCAS; ++i)
    FEP_XPRINT (debug_mask2, ("Event %02d: %s\n", i, EVENT_CCAS_ITOA (i)));

  FEP_XPRINT (debug_mask2, ("\n"));
  for (i = ACTION_CCAS_UNDEF; i < NUM_ACTION_CCAS; ++i)
    FEP_XPRINT (debug_mask2, ("Action %02d: %s\n", i, ACTION_CCAS_ITOA (i)));

  FEP_XPRINT (debug_mask2, ("\n"));
  for (i = 0; i < NUM_STATE_CCAS; ++i)
    for (j = 0; j < NUM_EVENT_CCAS; ++j)
    {
      ste = stt (APP_SM_CCAS, i, j, TRUE);
      FEP_XPRINT (debug_mask2, ("(state0,event,action,state1) (%02d,%02d,%02d,%02d)\n", i, j, ste->action, ste->state));
    }

  FEP_XPRINT (debug_mask2, ("\n"));
}

void initialize_fsm ()
{
  char me[] = "initialize_fsm";

  XPRINT_ENTERED;

  print_fsm (debug_mask[DEBUG_DSM_2], me);
}

void finalize_fsm ()
{
  char me[] = "finalize_fsm";

  XPRINT_ENTERED;
}
