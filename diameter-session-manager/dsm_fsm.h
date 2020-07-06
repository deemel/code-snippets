#ifndef _DSM_FSM_H_
#define _DSM_FSM_H_

/*
 * State Transition Entry (STE)
 */
struct ste
{
  int state;  /* New state */
  int action;
};

/*
 * Application State Machines (ASMs)
 */
enum app_sm
{
  APP_SM_UNDEF = DSM_UNDEF,  /* -01 */
  APP_SM_CCAS,               /*  00 */
};

static const char *app_sm_str[] =
{
  "CCA Server",  /* 00 */
};

#define NUM_APP_SM (APP_SM_CCAS + 1)
#define APP_SM_ITOA(i) (((i >= 0) && (i < NUM_APP_SM)) ? (app_sm_str[i]) : (DSM_UNDEF_STR))

/*
 * ============================================================================
 * Credit Control Application Server State Machine
 * ============================================================================
 */

/*
 * States
 */
enum state_ccas
{
  STATE_CCAS_UNDEF = DSM_UNDEF,  /* -01 */
  STATE_CCAS_DISCONTINUING,      /*  00 */
  STATE_CCAS_IDLE,               /*  01 */
  STATE_CCAS_OPEN,               /*  02 */
  STATE_CCAS_PROC_REQ,           /*  03 */
  STATE_CCAS_PROC_REQ_FINAL,     /*  04 */
  STATE_CCAS_REPORTING_ERROR,    /*  05 */
};

static const char *state_ccas_str[] =
{
  "Discontinuing",             /* 00 */
  "Idle",                      /* 01 */
  "Open",                      /* 02 */
  "Processing Request",        /* 03 */
  "Processing Final Request",  /* 04 */
  "Reporting Error",           /* 05 */
};

#define NUM_STATE_CCAS (STATE_CCAS_REPORTING_ERROR + 1)
#define STATE_CCAS_ITOA(i) (((i >= 0) && (i < NUM_STATE_CCAS)) ? (state_ccas_str[i]) : (DSM_UNDEF_STR))

/*
 * Events
 */
enum event_ccas
{
  EVENT_CCAS_UNDEF = DSM_UNDEF,       /* -01 */
  EVENT_CCAS_DQ_DIAM_CCR_UPDATE,      /*  00 */
  EVENT_CCAS_DQ_DIAM_CCR_TERM,        /*  01 */
  EVENT_CCAS_DQ_DIAM_STR,             /*  02 */
  EVENT_CCAS_EXP_TCC,                 /*  03 */
  EVENT_CCAS_EXP_TG,                  /*  04 */
  EVENT_CCAS_RX_DIAM_ASA,             /*  05 */
  EVENT_CCAS_RX_DIAM_CCR_EVENT,       /*  06 */
  EVENT_CCAS_RX_DIAM_CCR_EVENT_T,     /*  07 */
  EVENT_CCAS_RX_DIAM_CCR_INIT,        /*  08 */
  EVENT_CCAS_RX_DIAM_CCR_INIT_T,      /*  09 */
  EVENT_CCAS_RX_DIAM_CCR_TERM,        /*  10 */
  EVENT_CCAS_RX_DIAM_CCR_TERM_T,      /*  11 */
  EVENT_CCAS_RX_DIAM_CCR_UPDATE,      /*  12 */
  EVENT_CCAS_RX_DIAM_CCR_UPDATE_T,    /*  13 */
  EVENT_CCAS_RX_DIAM_MSG_VALID,       /*  14 */
  EVENT_CCAS_RX_DIAM_RAA,             /*  15 */
  EVENT_CCAS_RX_DIAM_STR,             /*  16 */
  EVENT_CCAS_RX_DIAM_STR_T,           /*  17 */
  EVENT_CCAS_RX_DIAMCTX_GDI_FAIL,     /*  18 */
  EVENT_CCAS_RX_GDI_ABORT,            /*  19 */
  EVENT_CCAS_RX_GDI_ASR,              /*  20 */
  EVENT_CCAS_RX_GDI_CCA_EVENT_FAIL,   /*  21 */
  EVENT_CCAS_RX_GDI_CCA_EVENT_SUCC,   /*  22 */
  EVENT_CCAS_RX_GDI_CCA_INIT_FAIL,    /*  23 */
  EVENT_CCAS_RX_GDI_CCA_INIT_SUCC,    /*  24 */
  EVENT_CCAS_RX_GDI_CCA_TERM_FAIL,    /*  25 */
  EVENT_CCAS_RX_GDI_CCA_TERM_SUCC,    /*  26 */
  EVENT_CCAS_RX_GDI_CCA_UPDATE_FAIL,  /*  27 */
  EVENT_CCAS_RX_GDI_CCA_UPDATE_SUCC,  /*  28 */
  EVENT_CCAS_RX_GDI_DIM_CLOSE,        /*  29 */
  EVENT_CCAS_RX_GDI_RAR,              /*  30 */
  EVENT_CCAS_RX_GDI_STA_FAIL,         /*  31 */
  EVENT_CCAS_RX_GDI_STA_SUCC,         /*  32 */
  EVENT_CCAS_RX_QMSG_FE_UNAVAIL,      /*  33 */
  EVENT_CCAS_RX_QMSG_UP_CP,           /*  34 */
};

static const char *event_ccas_str[] =
{
  "Dequeued Diam-CCR update",                     /* 00 */
  "Dequeued Diam-CCR termination",                /* 01 */
  "Dequeued Diam-STR",                            /* 02 */
  "Expired Tcc",                                  /* 03 */
  "Expired Tg",                                   /* 04 */
  "Received Diam-ASA",                            /* 05 */
  "Received Diam-CCR event",                      /* 06 */
  "Received Diam-CCR event ('T' bit set)",        /* 07 */
  "Received Diam-CCR initial",                    /* 08 */
  "Received Diam-CCR initial ('T' bit set)",      /* 09 */
  "Received Diam-CCR termination",                /* 10 */
  "Received Diam-CCR termination ('T' bit set)",  /* 11 */
  "Received Diam-CCR update",                     /* 12 */
  "Received Diam-CCR update ('T' bit set)",       /* 13 */
  "Received valid Diam message",                  /* 14 */
  "Received Diam-RAA",                            /* 15 */
  "Received Diam-STR",                            /* 16 */
  "Received Diam-STR ('T' bit set)",              /* 17 */
  "Received DiamCtx GDI (failure)",               /* 18 */
  "Received GDI-Abort",                           /* 19 */
  "Received GDI-ASR",                             /* 20 */
  "Received GDI-CCA event (failure)",             /* 21 */
  "Received GDI-CCA event (success)",             /* 22 */
  "Received GDI-CCA initial (failure)",           /* 23 */
  "Received GDI-CCA initial (success)",           /* 24 */
  "Received GDI-CCA termination (failure)",       /* 25 */
  "Received GDI-CCA termination (success)",       /* 26 */
  "Received GDI-CCA update (failure)",            /* 27 */
  "Received GDI-CCA update (success)",            /* 28 */
  "Received GDI-DIM_Close",                       /* 29 */
  "Received GDI-RAR",                             /* 30 */
  "Received GDI-STA (failure)",                   /* 31 */
  "Received GDI-STA (success)",                   /* 32 */
  "Received qmsg FE unavailable",                 /* 33 */
  "Received qmsg update CP",                      /* 34 */
};

#define NUM_EVENT_CCAS (EVENT_CCAS_RX_QMSG_UP_CP + 1)
#define EVENT_CCAS_ITOA(i) (((i >= 0) && (i < NUM_EVENT_CCAS)) ? (event_ccas_str[i]) : (DSM_UNDEF_STR))

/*
 * Actions
 */
enum action_ccas
{
  ACTION_CCAS_UNDEF = DSM_UNDEF,    /* -01 */
  ACTION_CCAS_CLOSE_GDI_TRANS,      /*  00 */
  ACTION_CCAS_DISC_GDI_MSG,         /*  01 */
  ACTION_CCAS_QUE_DIAM_MSG,         /*  02 */
  ACTION_CCAS_STOP_TG,              /*  03 */
  ACTION_CCAS_TX_DIAM_ASR_01,       /*  04 */
  ACTION_CCAS_TX_DIAM_ASR_02,       /*  05 */
  ACTION_CCAS_TX_DIAM_CCA_01,       /*  06 */
  ACTION_CCAS_TX_DIAM_CCA_02,       /*  07 */
  ACTION_CCAS_TX_DIAM_CCA_03,       /*  08 */
  ACTION_CCAS_TX_DIAM_RAR_01,       /*  09 */
  ACTION_CCAS_TX_DIAM_RAR_02,       /*  10 */
  ACTION_CCAS_TX_DIAM_STA,          /*  11 */
  ACTION_CCAS_TX_GDI_ASA,           /*  12 */
  ACTION_CCAS_TX_GDI_CCR_01,        /*  13 */
  ACTION_CCAS_TX_GDI_CCR_02,        /*  14 */
  ACTION_CCAS_TX_GDI_RAA,           /*  15 */
  ACTION_CCAS_TX_GDI_REPTERR,       /*  16 */
  ACTION_CCAS_TX_GDI_STR_01,        /*  17 */
  ACTION_CCAS_TX_GDI_STR_02,        /*  18 */
  ACTION_CCAS_UP_US,                /*  19 */
};

static const char *action_ccas_str[] =
{
  "Close GDI transaction",                                        /* 00 */
  "Discard GDI message",                                          /* 01 */
  "Queue Diam message",                                           /* 02 */
  "Stop Tg",                                                      /* 03 */
  "Send Diam-ASR, Maintain Tcc",                                  /* 04 */
  "Send Diam-ASR, (Re)start Tg",                                  /* 05 */
  "Send Diam-CCA, Stop Tg",                                       /* 06 */
  "Send Diam-CCA, Stop Tg, Start Tcc",                            /* 07 */
  "Send Diam-CCA (DIAMETER_TOO_BUSY), Send GDI-DIM_ReportError, Discard queued messages, Start Tg",                                               /* 08 */
  "Send Diam-RAR, Maintain Tcc",                                  /* 09 */
  "Send Diam-RAR, (Re)start Tg",                                  /* 10 */
  "Send Diam-STA, Stop Tg",                                       /* 11 */
  "Send GDI-ASA, Restart Tcc",                                    /* 12 */
  "Send GDI-CCR, Start Tg",                                       /* 13 */
  "Send GDI-CCR, Stop Tcc, Start Tg",                             /* 14 */
  "Send GDI-RAA, Restart Tcc",                                    /* 15 */
  "Send GDI-DIM_ReportError, Discard queued messages, Start Tg",  /* 16 */
  "Send GDI-STR, Start Tg",                                       /* 17 */
  "Send GDI-STR, Stop Tcc, Start Tg",                             /* 18 */
  "Update US",                                                    /* 19 */
};

#define NUM_ACTION_CCAS (ACTION_CCAS_UP_US + 1)
#define ACTION_CCAS_ITOA(i) (((i >= 0) && (i < NUM_ACTION_CCAS)) ? (action_ccas_str[i]) : (DSM_UNDEF_STR))

#endif  /* _DSM_FSM_H_ */
