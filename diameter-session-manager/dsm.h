#ifndef _DSM_H_
#define _DSM_H_

/*
 * Diameter Session Manager (DSM): Manages call context, state machines, &
 * timers for individual sessions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "fep.h"
#include "fep_debug.h"
#include "ipni_fe_cntrs.h"

#define TASK_NAME "DSM"
#define DSM_UNDEF -1
#define DSM_UNDEF_STR "Undefined"
#define DSM_NA_STR "N/A"
#define MAX_SZ_HEX_DUMP 1024
#define MS_PER_SEC 1000  /* 1000 ms per 1 sec */

#define ID_UNDEF 0  /* Undefined ID */
#define ID_OFFSET 1
#define ID_TO_INDEX(i) (i - ID_OFFSET)
#define INDEX_TO_ID(i) (i + ID_OFFSET)

/*
 * Debug masks.
 *   - DEBUG_FREQUENT debugs FREQUENTLY occuring code; use with caution!
 *   - DEBUG_MSG_*_ENTER prints out unaltered message as received (in the
 *     case of DiamCtx) or retrieved (in the case of US) by DSM
 *   - DEBUG_MSG_*_EXIT prints out possibly altered message before leaving
 *     DSM (in the case of DiamCtx) or after update by DSM (in the case of US)
 */
enum debug
{
  DEBUG_DIAMCTX_QUEUE,     /* Index 00, debug mask 0x00000001 */
  DEBUG_DIAMCTX_RECV,      /* Index 01, debug mask 0x00000002 */
  DEBUG_DIAMCTX_XMIT,      /* Index 02, debug mask 0x00000004 */
  DEBUG_DSM,               /* Index 03, debug mask 0x00000008 */
  DEBUG_DSM_1,             /* Index 04, debug mask 0x00000010 */
  DEBUG_DSM_2,             /* Index 05, debug mask 0x00000020 */
  DEBUG_ENTERED,           /* Index 06, debug mask 0x00000040 */
  DEBUG_FREQUENT,          /* Index 07, debug mask 0x00000080 */
  DEBUG_HEX_DUMP,          /* Index 08, debug mask 0x00000100 */
  DEBUG_MSG_CLOSE_ENTER,   /* Index 09, debug mask 0x00000200 */
  DEBUG_MSG_CLOSE_EXIT,    /* Index 10, debug mask 0x00000400 */
  DEBUG_MSG_DIAM_ENTER,    /* Index 11, debug mask 0x00000800 */
  DEBUG_MSG_DIAM_EXIT,     /* Index 12, debug mask 0x00001000 */
  DEBUG_MSG_EXPTCC_ENTER,  /* Index 13, debug mask 0x00002000 */
  DEBUG_MSG_EXPTCC_EXIT,   /* Index 14, debug mask 0x00004000 */
  DEBUG_MSG_EXPTG_ENTER,   /* Index 15, debug mask 0x00008000 */
  DEBUG_MSG_EXPTG_EXIT,    /* Index 16, debug mask 0x00010000 */
  DEBUG_MSG_GDI_ENTER,     /* Index 17, debug mask 0x00020000 */
  DEBUG_MSG_GDI_EXIT,      /* Index 18, debug mask 0x00040000 */
  DEBUG_MSG_MISC_ENTER,    /* Index 19, debug mask 0x00080000 */
  DEBUG_MSG_QUEUE_ENTER,   /* Index 20, debug mask 0x00100000 */
  DEBUG_MSG_QUEUE_EXIT,    /* Index 21, debug mask 0x00200000 */
  DEBUG_MSG_UPCP_ENTER,    /* Index 22, debug mask 0x00400000 */
  DEBUG_MSG_UPCP_EXIT,     /* Index 23, debug mask 0x00800000 */
  DEBUG_MSG_USRM_ENTER,    /* Index 24, debug mask 0x01000000 */
  DEBUG_MSG_USRM_EXIT,     /* Index 25, debug mask 0x02000000 */
  DEBUG_QMSG_RECV,         /* Index 26, debug mask 0x03000000 */
  DEBUG_QMSG_XMIT,         /* Index 27, debug mask 0x08000000 */
  DEBUG_US_REMOVE,         /* Index 28, debug mask 0x10000000 */
};

static const char *debug_str[] =
{
  "Type of DC to be queued",                           /* 00 */
  "Type of DC received",                               /* 01 */
  "Type of DC to be sent",                             /* 02 */
  "General DSM conditions (errors)",                   /* 03 */
  "General DSM conditions (warnings)",                 /* 04 */
  "General DSM conditions (informational)",            /* 05 */
  "Entrance into functions",                           /* 06 */
  "Debugs associated with FREQUENTLY occuring code",   /* 07 */
  "Memory hex dumps",                                  /* 08 */
  "DC & US in GDI-DIM_Close processing (initial)",     /* 09 */
  "DC & US in GDI-DIM_Close processing (final)",       /* 10 */
  "DC & US in Diameter message processing (initial)",  /* 11 */
  "DC & US in Diameter message processing (final)",    /* 12 */
  "DC & US in Tcc expiration processing (initial)",    /* 13 */
  "DC & US in Tcc expiration processing (final)",      /* 14 */
  "DC & US in Tg expiration processing (initial)",     /* 15 */
  "DC & US in Tg expiration processing (final)",       /* 16 */
  "DC & US in GDI message processing (initial)",       /* 17 */
  "DC & US in GDI message processing (final)",         /* 18 */
  "DC with unrecognized type received",                /* 19 */
  "DC in queued message processing (initial)",         /* 20 */
  "DC in queued message processing (final)",           /* 21 */
  "US in update CP processing (initial)",              /* 22 */
  "US in update CP processing (final)",                /* 23 */
  "DC & US in remove US processing (initial)",         /* 24 */
  "DC & US in remove US processing (final)",           /* 25 */
  "Command code of qmsg received",                     /* 26 */
  "Command code of qmsg sent",                         /* 27 */
  "Session-Id of US to be removed",                    /* 28 */
};

#define NUM_DEBUG (DEBUG_US_REMOVE + 1)
#define DEBUG_ITOA(i) (((i >= 0) && (i < NUM_DEBUG)) ? (debug_str[i]) : (DSM_UNDEF_STR))

#define DEBUG_MASK_DEFAULT        (debug_mask[DEBUG_DSM] | debug_mask[DEBUG_DSM_1] | DEBUG_MASK_EVENTS)
#define DEBUG_MASK_DIAMCTX_ALL    (debug_mask[DEBUG_DIAMCTX_QUEUE] | debug_mask[DEBUG_DIAMCTX_RECV] | debug_mask[DEBUG_DIAMCTX_XMIT])
#define DEBUG_MASK_DSM_ALL        (debug_mask[DEBUG_DSM] | debug_mask[DEBUG_DSM_1] | debug_mask[DEBUG_DSM_2])
#define DEBUG_MASK_EVENTS         (DEBUG_MASK_DIAMCTX_ALL | debug_mask[DEBUG_US_REMOVE])
#define DEBUG_MASK_MSG_ALL        (DEBUG_MASK_MSG_CLOSE_ALL | DEBUG_MASK_MSG_DIAM_ALL | DEBUG_MASK_MSG_EXPTCC_ALL | DEBUG_MASK_MSG_EXPTG_ALL | DEBUG_MASK_MSG_GDI_ALL | DEBUG_MASK_MSG_MISC_ALL | DEBUG_MASK_MSG_QUEUE_ALL | DEBUG_MASK_MSG_UPCP_ALL | DEBUG_MASK_MSG_USRM_ALL)
#define DEBUG_MASK_MSG_CLOSE_ALL  (debug_mask[DEBUG_MSG_CLOSE_ENTER] | debug_mask[DEBUG_MSG_CLOSE_EXIT])
#define DEBUG_MASK_MSG_DIAM_ALL   (debug_mask[DEBUG_MSG_DIAM_ENTER] | debug_mask[DEBUG_MSG_DIAM_EXIT])
#define DEBUG_MASK_MSG_EXPTCC_ALL (debug_mask[DEBUG_MSG_EXPTCC_ENTER] | debug_mask[DEBUG_MSG_EXPTCC_EXIT])
#define DEBUG_MASK_MSG_EXPTG_ALL  (debug_mask[DEBUG_MSG_EXPTG_ENTER] | debug_mask[DEBUG_MSG_EXPTG_EXIT])
#define DEBUG_MASK_MSG_GDI_ALL    (debug_mask[DEBUG_MSG_GDI_ENTER] | debug_mask[DEBUG_MSG_GDI_EXIT])
#define DEBUG_MASK_MSG_MISC_ALL   (debug_mask[DEBUG_MSG_MISC_ENTER])
#define DEBUG_MASK_MSG_QUEUE_ALL (debug_mask[DEBUG_MSG_QUEUE_ENTER] | debug_mask[DEBUG_MSG_QUEUE_EXIT])
#define DEBUG_MASK_MSG_UPCP_ALL   (debug_mask[DEBUG_MSG_UPCP_ENTER] | debug_mask[DEBUG_MSG_UPCP_EXIT])
#define DEBUG_MASK_MSG_USRM_ALL   (debug_mask[DEBUG_MSG_USRM_ENTER] | debug_mask[DEBUG_MSG_USRM_EXIT])
#define DEBUG_MASK_QMSG_ALL       (debug_mask[DEBUG_QMSG_RECV] | debug_mask[DEBUG_QMSG_XMIT])

#define FPRINTF_FAILED_RV_EXIT(std, func) fprintf (std, "%s: %s failed, rv %d, exiting\n", me, func, rv)
#define PRINTF_ENTERED if (fep_debug_mask & debug_mask[DEBUG_ENTERED]) printf ("%s: Entered\n", me)
#define SPRINTF_ES_DISC_QUEUED_MSG sprintf (pbuf_cc->errorString, "Discarded queued message")
#define SPRINTF_ES_EXP_TIMER(name, val) sprintf (pbuf_cc->errorString, "Timer %s %d ms expired", name, val)
#define SPRINTF_ES_FAILED(func) sprintf (pbuf_cc->errorString, "%s: %s failed", me, func)
#define SPRINTF_ES_FAILED_CALLOUT_SET(name, val) sprintf (pbuf_cc->errorString, "%s: callout_set failed, %s %d ms", me, name, val)
#define SPRINTF_ES_INVALID_CCR_TYPE sprintf (pbuf_cc->errorString, "%s: Invalid CC-Request-Type AVP %d (%s)", me, pbuf_cc->ccRequestType, CCR_TYPE_ITOA (pbuf_cc->ccRequestType))
#define SPRINTF_ES_INVALID_CMD sprintf (pbuf_cc->errorString, "%s: Unexpected or invalid Command-Code %d (%s), command flags 0x%x", me, pbuf_cc->commandCode, command_code_itoa (pbuf_cc->commandCode, pbuf_cc->flags, pbuf_cc->ccRequestType), pbuf_cc->flags)
#define SPRINTF_ES_INVALID_EVENT_CCAS sprintf (pbuf_cc->errorString, "%s: Unexpected or invalid ASM event %d (%s)", me, asm_event, EVENT_CCAS_ITOA (asm_event))
#define SPRINTF_ES_INVALID_PKG_TYPE sprintf (pbuf_cc->errorString, "%s: Unexpected or invalid packageType 0x%x (%s)", me, pbuf_cc->packageType, package_type_itoa (pbuf_cc->packageType))
#define SPRINTF_ES_INVALID_STATE_CCAS sprintf (pbuf_cc->errorString, "%s: Unexpected or invalid ASM state %d (%s)", me, p->call_context.asm_state, STATE_CCAS_ITOA (p->call_context.asm_state))
#define SPRINTF_ES_NO_US sprintf (pbuf_cc->errorString, "%s: US does not exist, commandCode %d (%s)", me, pbuf_cc->commandCode, command_code_itoa (pbuf_cc->commandCode, pbuf_cc->flags, pbuf_cc->ccRequestType))
#define SPRINTF_ES_NULL(name, p) sprintf (pbuf_cc->errorString, "%s: NULL %s 0x%x", me, name, p)
#define SPRINTF_ES_UNSUPPORTED_CMD sprintf (pbuf_cc->errorString, "%s: Receipt of Command-Code %d (%s), command flags 0x%x unsupported", me, pbuf_cc->commandCode, command_code_itoa (pbuf_cc->commandCode, pbuf_cc->flags, pbuf_cc->ccRequestType), pbuf_cc->flags)
#define XPRINT_DIAMCTX_QUEUE FEP_XPRINT (debug_mask[DEBUG_DIAMCTX_QUEUE], ("%s: Queuing DiamCtx, type %d (%s), Command-Code %d (%s)\n", me, pbuf_cc->diamctx_type, DIAMCTX_ITOA (pbuf_cc->diamctx_type), pbuf_cc->commandCode, command_code_itoa (pbuf_cc->commandCode, pbuf_cc->flags, pbuf_cc->ccRequestType)))
#define XPRINT_DIAMCTX_RECV FEP_XPRINT (debug_mask[DEBUG_DIAMCTX_RECV], ("%s: Received DiamCtx, type %d (%s), Command-Code %d (%s), local TID 0x%x, package type 0x%x (%s), queued %d (%s)\n", me, pbuf_cc->diamctx_type, DIAMCTX_ITOA (pbuf_cc->diamctx_type), pbuf_cc->commandCode, command_code_itoa (pbuf_cc->commandCode, pbuf_cc->flags, pbuf_cc->ccRequestType), pbuf_cc->localTid, pbuf_cc->packageType, ((pbuf_cc->diamctx_type != DIAMCTX_DIAM) ? ((char *) package_type_itoa (pbuf_cc->packageType)) : (DSM_NA_STR)), flag_queued, FLAG_ITOA (flag_queued)));
#define XPRINT_DIAMCTX_XMIT FEP_XPRINT (debug_mask[DEBUG_DIAMCTX_XMIT], ("%s: Sending DiamCtx, type %d (%s), Command-Code %d (%s), local TID 0x%x, package type 0x%x (%s)\n", me, pbuf_cc->diamctx_type, DIAMCTX_ITOA (pbuf_cc->diamctx_type), pbuf_cc->commandCode, command_code_itoa (pbuf_cc->commandCode, pbuf_cc->flags, pbuf_cc->ccRequestType), pbuf_cc->localTid, pbuf_cc->packageType, package_type_itoa (pbuf_cc->packageType)))
#define XPRINT_ENTERED FEP_XPRINT (debug_mask[DEBUG_ENTERED], ("%s: Entered\n", me))
#define XPRINT_ES_FAILED_SET_ASM_STATE FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s, asm_id %d (%s), asm_state_prev %d (%s), asm_event %d (%s)\n", pbuf_cc->errorString, p->call_context.asm_id, APP_SM_ITOA (p->call_context.asm_id), asm_state_prev, STATE_CCAS_ITOA (asm_state_prev), asm_event, EVENT_CCAS_ITOA (asm_event)))
#define XPRINT_ES_INVALID_EVENT_CCAS FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s, asm_id %d (%s), asm_state %d (%s)\n", pbuf_cc->errorString, p->call_context.asm_id, APP_SM_ITOA (p->call_context.asm_id), p->call_context.asm_state, STATE_CCAS_ITOA (p->call_context.asm_state)))
#define XPRINT_ES_INVALID_STATE_CCAS FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s, asm_id %d (%s), commandCode %d (%s)\n", pbuf_cc->errorString, p->call_context.asm_id, APP_SM_ITOA (p->call_context.asm_id), pbuf_cc->commandCode, command_code_itoa (pbuf_cc->commandCode, pbuf_cc->flags, pbuf_cc->ccRequestType)))
#define XPRINT_ES_UNSUPPORTED_CMD FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s, command flags 0x%x (%s)\n", pbuf_cc->errorString, pbuf_cc->flags, bit_print (pbuf_cc->flags, sizeof (pbuf_cc->flags))))
#define XPRINT_FAILED(func) FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: %s failed\n", me, func))
#define XPRINT_FAILED_BUF_TO_PARM FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: buf_to_parm failed, parm %d, pbuf 0x%x\n", me, parm, pbuf))
#define XPRINT_FAILED_CALLOUT_SET(name, val) FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: callout_set failed, %s %d ms", me, name, val))
#define XPRINT_FAILED_CONF_GET_FE_NUMBER FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: conf_get_fe_number failed, fe %d\n", me, fe))
#define XPRINT_FAILED_FEP_ALLOC_BUFFER(size) FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: fep_alloc_buffer failed, size %d\n", me, size))
#define XPRINT_FAILED_PROC_DIAMCTX(func) FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: %s failed, rv %d (%s)\n", me, func, rv, result_code_itoa (rv)))
#define XPRINT_FAILED_RV(func) FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: %s failed, rv %d\n", me, func, rv))
#define XPRINT_FAILED_RV_EXIT(func) FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: %s failed, rv %d, exiting\n", me, func, rv))
#define XPRINT_FAILED_SET_ASM_STATE FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: set_asm_state failed, asm_id %d (%s), asm_state_prev %d (%s), asm_event %d (%s)\n", me, p->call_context.asm_id, APP_SM_ITOA (p->call_context.asm_id), asm_state_prev, STATE_CCAS_ITOA (asm_state_prev), asm_event, EVENT_CCAS_ITOA (asm_event)))
#define XPRINT_FAILED_TELL(func) FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: %s failed, rv %d\n", me, func, rv))
#define XPRINT_FAILED_US(func) FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: %s failed, sessionId %s, length %d\n", me, func, string_hex_print (FEP_BUF_SID, pbuf_cc->sessionIdLength))
#define XPRINT_FAILED_US_HASH FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: user_session_hash failed, sessionId %s, length %d\n", me, string_hex_print (FEP_BUF_SID, pbuf_cc->sessionIdLength), pbuf_cc->sessionIdLength))
#define XPRINT_FAILED_US_REMOVE FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: user_session_remove failed, p 0x%x\n", me, p))
#define XPRINT_INVALID_DIAMCTX_TYPE FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: Unexpected or invalid DiamCtx type %d (%s)\n", me, pbuf_cc->diamctx_type, DIAMCTX_ITOA (pbuf_cc->diamctx_type)))
#define XPRINT_INVALID_EVENT_CCAS FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: Unknown or invalid event %d (%s), asm_id %d (%s), state %d (%s)\n", me, event, EVENT_CCAS_ITOA (event), asm_id, APP_SM_ITOA (asm_id), state, STATE_CCAS_ITOA (state)))
#define XPRINT_INVALID_PKG_TYPE FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: Unexpected or invalid GDI package type 0x%x (%s), command_code %d (%s)\n", me, package_type_req, package_type_itoa (package_type_req), p->diameter.command_code, command_code_itoa (p->diameter.command_code, p->diameter.command_flags, p->diameter.cc_request_type)))
#define XPRINT_INVALID_SESS_ID_LEN FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: Invalid session ID length %d\n", me, session_id_len))
#define XPRINT_INVALID_STATE_CCAS FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: Unexpected or invalid ASM state %d (%s), asm_id %d (%s)\n", me, p->call_context.asm_state, STATE_CCAS_ITOA (p->call_context.asm_state), p->call_context.asm_id, APP_SM_ITOA (p->call_context.asm_id)))
#define XPRINT_INVALID_US_ID(tid) FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: Invalid US ID %lu, local TID 0x%x (%s)\n", me, id, tid, bit_print (tid, sizeof (tid))))
#define XPRINT_NULL(name, p) FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: NULL %s 0x%x\n", me, name, p))
#define XPRINT_PROC_INITIATED(event) FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: %s %s initiated\n", me, TASK_NAME, event))
#define XPRINT_QMSG_RECV(cc) FEP_XPRINT (debug_mask[DEBUG_QMSG_RECV], ("%s: Received qmsg, command code 0x%x\n", me, cc))
#define XPRINT_QMSG_XMIT(cc) FEP_XPRINT (debug_mask[DEBUG_QMSG_XMIT], ("%s: Sent qmsg, command code 0x%x\n", me, cc))
#define XPRINT_UNEXPECTED_TID_LOCAL FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: Unexpected tid_local 0x%x (%s), command_code %d (%s), package_type 0x%x (%s)\n", me, p->gdi.tid_local, bit_print (p->gdi.tid_local, sizeof (p->gdi.tid_local)), p->diameter.command_code, command_code_itoa (p->diameter.command_code, p->diameter.command_flags, p->diameter.cc_request_type), p->gdi.package_type, package_type_itoa (p->gdi.package_type)))
#define XPRINT_US_REMOVE FEP_XPRINT (debug_mask[DEBUG_US_REMOVE], ("%s: Removing US ID 0x%x (%lu), Session-Id %s\n", me, p->id, p->id, string_hex_print (p->diameter.session_id, p->diameter.session_id_len)))

/*
 * General purpose hash functions.  Refer to
 * www.partow.net/programming/hashfunctions/index.html for more information.
 */
typedef uint (*hash_t) (char *, int);

enum hash
{
  HASH_UNDEF = DSM_UNDEF,  /* -01 */
  HASH_RS,                 /*  00 */
  HASH_JS,                 /*  01 */
  HASH_PJW,                /*  02 */
  HASH_ELF,                /*  03 */
  HASH_BKDR,               /*  04 */
  HASH_SDBM,               /*  05 */
  HASH_DJB,                /*  06 */
  HASH_DEK,                /*  07 */
  HASH_AP,                 /*  08 */
};

uint hash_rs   (char *, int);  /* 00 */
uint hash_js   (char *, int);  /* 01 */
uint hash_pjw  (char *, int);  /* 02 */
uint hash_elf  (char *, int);  /* 03 */
uint hash_bkdr (char *, int);  /* 04 */
uint hash_sdbm (char *, int);  /* 05 */
uint hash_djb  (char *, int);  /* 06 */
uint hash_dek  (char *, int);  /* 07 */
uint hash_ap   (char *, int);  /* 08 */

static const hash_t hash_func[] =
{
  &hash_rs,    /* 00 */
  &hash_js,    /* 01 */
  &hash_pjw,   /* 02 */
  &hash_elf,   /* 03 */
  &hash_bkdr,  /* 04 */
  &hash_sdbm,  /* 05 */
  &hash_djb,   /* 06 */
  &hash_dek,   /* 07 */
  &hash_ap,    /* 08 */
};

static const char *hash_str[] =
{
  "Robert Sedgwicks' algorithm",                   /* 00 */
  "Justin Sobel's algorithm",                      /* 01 */
  "Peter J. Weinberger's algorithm",               /* 02 */
  "Similar to Peter J. Weinberger's algorithm",    /* 03 */
  "Brian Kernighan & Dennis Ritchie's algorithm",  /* 04 */
  "SDBM project's algorithm",                      /* 05 */
  "Daniel J. Bernstein's algorithm",               /* 06 */
  "Donald E. Knuth's algorithm",                   /* 07 */
  "Arash Partow's algorithm",                      /* 08 */
};

#define NUM_HASH (HASH_AP + 1)
#define HASH_ITOA(i) (((i >= 0) && (i < NUM_HASH)) ? (hash_str[i]) : (DSM_UNDEF_STR))

struct flags
{
  uint __unused     : 31;
  uint shutdown     :  1;
};

extern uint debug_mask[NUM_DEBUG];
extern int errno;
extern int errno_saved;
extern struct flags flags;
extern DiamSessMeas meas;
extern struct callwheel callwheel;
extern struct ccdb ccdb;

#endif  /* _DSM_H_ */
