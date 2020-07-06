#ifndef _DSM_CCDB_H_
#define _DSM_CCDB_H_

#include <sys/queue.h>
#include "fep_memory.h"
#include "diam_qmsg.h"
#include "diam_msg.h"
#include "dsm_tdb.h"

/*
 * CCDB version
 */
#define CCDB_VERSION 0
#define CCDB_SUPPORTED_VERSIONS 0
#define CCDB_NUM_SUPPORTED_VERSIONS 1

/*
 * Max simultaneously opened user sessions
 */
#define USER_SESSION_BITS 18
#define USER_SESSION_SIZE (1 << USER_SESSION_BITS)  /* 2^18 = 262144 sess */
#define USER_SESSION_MASK (USER_SESSION_SIZE - 1)  /* 0x3FFFF (262143) */
#define USER_SESSION_ID_VALID(i) (((i >= ID_OFFSET) && (i <= USER_SESSION_SIZE)) ? (TRUE) : (FALSE))

/*
 * Max DGC qmsgs that can be queued over ALL user sessions
 */
#define DGC_QMSG_SIZE USER_SESSION_SIZE
#define DGC_QMSG_ID_VALID(i) (((i >= ID_OFFSET) && (i <= DGC_QMSG_SIZE)) ? (TRUE) : (FALSE))

/*
 * GDI local TID bit masks & shift values
 */
#define BITS_TID_CC      23  /* Call context (or user session) ID */
#define BITS_TID_CSI      1  /* CSI ID */
#define BITS_TID_FE_PEER  4  /* Peer FE ID */
#define BITS_TID_FE       4  /* FE ID */

#define MASK_TID_CC      0x007FFFFF  /* Max value 8388607 */
#define MASK_TID_CSI     0x00800000
#define MASK_TID_FE_PEER 0x0F000000
#define MASK_TID_FE      0xF0000000

#define SHIFT_TID_CC      (0)
#define SHIFT_TID_CSI     (BITS_TID_CC + SHIFT_TID_CC)
#define SHIFT_TID_FE_PEER (BITS_TID_CSI + SHIFT_TID_CSI)
#define SHIFT_TID_FE      (BITS_TID_FE_PEER + SHIFT_TID_FE_PEER)

#define FEP_BUF_SID ((char *) (FEP_BUF_DATA (pbuf_cc) + pbuf_cc->sessionIdOffset))
#define SUCCESS_QUEUED 0x7  /* Successfully queued Diameter message status */

/*
 * Trap data
 */
struct trap_data
{
  ulong time_start;             /* Time trap created (triggered by GMP) */
  char  sub_id[MAX_SZ_SUB_ID];  /* Subscription-Id-Data */
  struct user_session *us;      /* User session */
};

/*
 * Equivalent to:
 *
struct dgc_qmsg_tailq
{
  struct dgc_qmsg *tqh_first;  // First entry in queue
  struct dgc_qmsg **tqh_last;  // Ptr to last entry in queue
};
 */
TAILQ_HEAD (dgc_qmsg_tailq, dgc_qmsg);

/*
 * DGC qmsg
 */
struct dgc_qmsg
{
  /*
   * Must be 1st field in order to use private memory pool
   */
  FifoBlk *fifoBlk;

  /*
   * Must be 2nd field in order to initialize structure correctly.  (id -
   * ID_OFFSET) = index into fepmem_list_dgc_qmsg.
   */
  ulong id;

  /*
   * Equivalent to:
   *
  struct
  {
    struct dgc_qmsg *tqe_next;   // Next entry in queue
    struct dgc_qmsg **tqe_prev;  // Ptr back to ptr pointing at this entry
  } tqe;
   */
  TAILQ_ENTRY (dgc_qmsg) tqe;
  struct dgc_qmsg_tailq *head;  /* Head of list */

  fep_qmsg_t qmsg;
};

/*
 * Call Context (CC) data.  Mostly provided by DSM.
 */
struct us_call_context
{
  /*
   * Application State Machine (ASM).  ASM IDs are in dsm_fsm.h:enum app_sm.
   * States are in dsm_fsm.h:enum state_ccas.
   */
  short asm_id;     /* ASM ID */
  short asm_state;  /* Current state */

  /*
   * Tcc (waits for CCR update or final message from network) & Tg (waits for
   * GDI response from CP) timers.  Tcc expiration time provided by DGC.
   */
  ulong tcc_id;      /* Tcc timer ID */
  int   tcc_expiry;  /* Tcc expiration time (ms) */
  ulong tg_id;       /* Tg timer ID */
  int   tg_expiry;   /* Tg expiration time (ms) */

  /*
   * Stats
   */
  int   version;     /* CC version */
  ulong time_start;  /* Time created */
  ulong time_last;   /* Time last touched */
  ulong ref_count;   /* # times touched */

  short cp;          /* CP message delivered to */
  short npk_config;  /* N+K config (not used) */
  short peer_fe;     /* Peer (backup) replication FE (not used) */

  struct
  {
    ushort __unused : 15;
    ushort trap     :  1;  /* Trap US (triggered by GMP) */
  } flags;

  ulong reserved[4];  /* Reserved for future use */
};

/*
 * Diameter data.  Provided by DGC.
 */
struct us_diameter
{
  /*
   * Diameter header data
   */
  short  version;        /* Diameter version */
  short  command_code;   /* Diameter command code */
  ushort command_flags;  /* RPET command flag bits */
  short  __unused_0;     /* 4-byte alignment */
  ulong  app_id;         /* Application-ID (IANA-assigned) */
  ulong  hop_by_hop;     /* Hop-by-Hop ID */
  ulong  end_to_end;     /* End-to-End ID */

  /*
   * Diameter AVPs
   */
  UTF8String session_id;             /* Session-Id */
  short      session_id_len;         /* Session-Id length */
  short      cc_request_type;        /* CC-Request-Type */
  ulong      cc_request_number;      /* CC-Request-Number */
  ulong      auth_app_id;            /* Auth-Application-Id */
  short      local_host;             /* Local host */
  short      local_realm;            /* Local realm (typically ISCP) */
  ulong      local_state;            /* FE boot time (out Origin-State-Id) */
  short      remote_host;            /* Remote host */
  short      remote_realm;           /* Remote realm */
  ulong      remote_state;           /* Remote state (in Origin-State-Id) */
  short      sub_id_type;            /* Subscription-Id-Type */
  short      __unused_1;             /* 4-byte alignment */
  char       sub_id[MAX_SZ_SUB_ID];  /* Subscription-Id-Data */

  /*
   * Diameter Proxy-Info AVP
   */
  short       proxy_count;                   /* # proxies */
  short       __unused_2;                    /* 4-byte alignment */
  short       proxy_host[MAX_PROXIES];       /* Proxy-Host IDs */
  short       proxy_state_len[MAX_PROXIES];  /* Proxy-State lengths */
  OctetString proxy_states;                  /* Proxy-State data */

  /*
   * Subsequent DGC qmsgs with same session ID are FIFO-queued to be
   * processed serially in the future
   */
  struct dgc_qmsg_tailq dgc_qmsg;

  ulong reserved[4];  /* Reserved for future use */
};

/*
 * GDI transaction data.  Provided by DSM & DGC.
 */
struct us_gdi
{
  /*
   * Package types & TIDs.  Provided by DSM, except remote TID (provided by
   * DGC).
   */
  ushort package_type;      /* Package type */
  ushort package_type_asr;  /* Package type of GDI-ASR */
  ushort package_type_rar;  /* Package type of GDI-RAR */
  short  __unused_0;        /* 4-byte alignment */
  ulong  tid_local;         /* Local (FE) TID */
  ulong  tid_remote;        /* Remote (CP) TID */
  ulong  tid_remote_asr;    /* Remote (CP) TID from GDI-ASR */
  ulong  tid_remote_rar;    /* Remote (CP) TID from GDI-RAR */

  /*
   * Data from GDI request saved for use in GDI answer.  Provided by DGC.
   */
  short origSysId;   /* Originating system ID */
  short receiverId;  /* Receiver ID */
  short invokeId;    /* Invoke ID */
  short __unused_1;  /* 4-byte alignment */

  ulong reserved[4];  /* Reserved for future use */
};

/*
 * Equivalent to:
 *
struct user_session_tailq
{
  struct user_session *tqh_first;  // First entry in queue
  struct user_session **tqh_last;  // Ptr to last entry in queue
};
 */
TAILQ_HEAD (user_session_tailq, user_session);

/*
 * User Session (US) data
 */
struct user_session
{
  /*
   * Must be 1st field in order to use private memory pool
   */
  FifoBlk *fifoBlk;

  /*
   * Must be 2nd field in order to initialize structure correctly.  (id -
   * ID_OFFSET) = index into fepmem_list_user_session.
   */
  ulong id;

  /*
   * Equivalent to:
   *
  struct
  {
    struct user_session *tqe_next;   // Next entry in queue
    struct user_session **tqe_prev;  // Ptr back to ptr pointing at this entry
  } tqe;
   */
  TAILQ_ENTRY (user_session) tqe;
  struct user_session_tailq *head;  /* Head of list */

  struct us_call_context call_context;
  struct us_diameter     diameter;
  struct us_gdi          gdi;

  /*
   * Equivalent to:
   *
  struct
  {
    struct user_session *tqe_next;   // Next entry in queue
    struct user_session **tqe_prev;  // Ptr back to ptr pointing at this entry
  } tqe;
   */
  TAILQ_ENTRY (user_session) tqe_time;  /* Time-sorted link entry */
};

/*
 * Call Context Database (CCDB)
 */
struct ccdb
{
  struct trap_data trap;
  int tg_expiry;  /* Tg expiration time (ms) */
  struct user_session_tailq user_session[USER_SESSION_SIZE];

  /*
   * Head of time-sorted "sequence of events" list.  Sorted by creation time.
   */
  struct user_session_tailq user_session_time;
};

void process_dgc_qmsg (struct user_session *);

#endif  /* _DSM_CCDB_H_ */
