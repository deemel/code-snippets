#include "dsm.h"
#include "dsm_fsm.h"
#include "dsm_ccdb.h"

#include "fep_devlib.h"
#include "diam_qmsg.h"
#include "diam_config.h"

/*
 * Needed to use private memory pool
 */
static struct user_session fepmem_list_user_session[USER_SESSION_SIZE];
static ulong fepmem_size_user_session = sizeof (struct user_session);
static Memory fepmem_memory_user_session = DECL_MEM ("User Session", "user_session", 0);
static PrivInfo fepmem_privInfo_user_session = {"User Session", &fepmem_size_user_session, ARRAY_SIZE (fepmem_list_user_session), &fepmem_memory_user_session, fepmem_list_user_session};

static struct dgc_qmsg fepmem_list_dgc_qmsg[DGC_QMSG_SIZE];
static ulong fepmem_size_dgc_qmsg = sizeof (struct dgc_qmsg);
static Memory fepmem_memory_dgc_qmsg = DECL_MEM ("DGC Qmsg", "dgc_qmsg", 0);
static PrivInfo fepmem_privInfo_dgc_qmsg = {"DGC Qmsg", &fepmem_size_dgc_qmsg, ARRAY_SIZE (fepmem_list_dgc_qmsg), &fepmem_memory_dgc_qmsg, fepmem_list_dgc_qmsg};

void diamctx_print (DiamCtx_t *p, uint debug_mask2, char *caller)
{
  char me[] = "diamctx_print";

  int i, j;
  HostConfig *pHostConfig;
  RealmConfig *pRealmConfig;

  XPRINT_ENTERED;

  if (! (fep_debug_mask & debug_mask2))
    return;

  if (! p)  /* NULL Diameter context */
  {
    XPRINT_NULL ("Diameter context", p);
    return;
  }

  FEP_XPRINT (debug_mask2, ("%s (%s):\n", me, caller));

  if (fep_debug_mask & debug_mask[DEBUG_HEX_DUMP])
  {
    FEP_XPRINT (debug_mask2, ("\n"));
    FEP_XPRINT (debug_mask2, ("Diameter Context Hex Dump\n"));
    FEP_XPRINT (debug_mask2, ("===============================================================================\n"));
    dump_memory ("", p, sizeof (*p), DUMP_RELATIVE);
  }

  FEP_XPRINT (debug_mask2, ("\n"));
  FEP_XPRINT (debug_mask2, ("Diameter Context Dump (%s, %s, %s, 0x%x)\n", caller, debug_msg_itoa (debug_mask2), command_code_itoa (p->commandCode, p->flags, p->ccRequestType), p->localTid));
  FEP_XPRINT (debug_mask2, ("===============================================================================\n"));
  FEP_XPRINT (debug_mask2, ("diamctx_type          %d (%s)\n", p->diamctx_type, DIAMCTX_ITOA (p->diamctx_type)));
  FEP_XPRINT (debug_mask2, ("sessionIdOffset       %d\n", p->sessionIdOffset));
  FEP_XPRINT (debug_mask2, ("sessionIdLength       %d\n", p->sessionIdLength));
  FEP_XPRINT (debug_mask2, ("sessionId %s\n", string_hex_print (((fep_buffer_t *) p)->data + p->sessionIdOffset, p->sessionIdLength)));
  FEP_XPRINT (debug_mask2, ("result_code           %d (%s)\n", p->result_code, result_code_itoa (p->result_code)));
  FEP_XPRINT (debug_mask2, ("version               %d\n", p->version));
  FEP_XPRINT (debug_mask2, ("commandCode           %d (%s)\n", p->commandCode, command_code_itoa (p->commandCode, p->flags, p->ccRequestType)));
  FEP_XPRINT (debug_mask2, ("hopByHopId            %lu\n", p->hopByHopId));
  FEP_XPRINT (debug_mask2, ("endToEndId            %lu\n", p->endToEndId));
  FEP_XPRINT (debug_mask2, ("applId                %lu (%s)\n", p->applId, app_id_itoa (p->applId)));
  FEP_XPRINT (debug_mask2, ("flags                 0x%x (%s, %s)\n", p->flags, bit_print (p->flags, sizeof (p->flags)), command_flags_print (p->flags)));

  FEP_XPRINT (debug_mask2, ("proxy_count           %d\n", p->proxy_count));
  if ((p->proxy_count > 0) && (p->proxy_count <= MAX_PROXIES))
  {
    for (j = i = 0; i < p->proxy_count; ++i)
    {
      FEP_XPRINT (debug_mask2, ("proxy_host[%d]         %d\n", i, p->proxy_host[i]));
      FEP_XPRINT (debug_mask2, ("proxy_state_len[%d]    %d\n", i, p->proxy_state_len[i]));
      j += p->proxy_state_len[i];
    }
    FEP_XPRINT (debug_mask2, ("proxy_states %s\n", string_hex_print (p->proxy_states, j)));
  }

  FEP_XPRINT (debug_mask2, ("subIdType             %d (%s)\n", p->subIdType, SUB_ID_TYPE_ITOA (p->subIdType)));
  FEP_XPRINT (debug_mask2, ("subId %s\n", string_hex_print (p->subId, strlen (p->subId))));
  FEP_XPRINT (debug_mask2, ("originHost            %d (%s)\n", p->originHost, ((pHostConfig = conf_get_host (p->originHost)) ? (pHostConfig->fqdnHost) : ((p->originHost == MAX_HOSTs) ? (conf_get_local_host ()) : (DSM_UNDEF_STR)))));
  FEP_XPRINT (debug_mask2, ("originRealm           %d (%s)\n", p->originRealm, ((pRealmConfig = conf_get_realm (p->originRealm)) ? (pRealmConfig->fqdnRealm) : ((p->originRealm == MAX_REALMs) ? (conf_get_local_realm ()) : (DSM_UNDEF_STR)))));
  FEP_XPRINT (debug_mask2, ("destHost              %d (%s)\n", p->destHost, ((pHostConfig = conf_get_host (p->destHost)) ? (pHostConfig->fqdnHost) : ((p->destHost == MAX_HOSTs) ? (conf_get_local_host ()) : (DSM_UNDEF_STR)))));
  FEP_XPRINT (debug_mask2, ("destRealm             %d (%s)\n", p->destRealm, ((pRealmConfig = conf_get_realm (p->destRealm)) ? (pRealmConfig->fqdnRealm) : ((p->destRealm == MAX_REALMs) ? (conf_get_local_realm ()) : (DSM_UNDEF_STR)))));
  FEP_XPRINT (debug_mask2, ("ccRequestType         %d (%s)\n", p->ccRequestType, CCR_TYPE_ITOA (p->ccRequestType)));
  FEP_XPRINT (debug_mask2, ("ccRequestNumber       %lu\n", p->ccRequestNumber));
  FEP_XPRINT (debug_mask2, ("authAppId             %lu (%s)\n", p->authAppId, app_id_itoa (p->authAppId)));
  FEP_XPRINT (debug_mask2, ("originStateId         %lu\n", p->originStateId));
  FEP_XPRINT (debug_mask2, ("localTid              0x%x (%s)\n", p->localTid, bit_print (p->localTid, sizeof (p->localTid))));
  FEP_XPRINT (debug_mask2, ("remoteTid             0x%x (%s)\n", p->remoteTid, bit_print (p->remoteTid, sizeof (p->remoteTid))));
  FEP_XPRINT (debug_mask2, ("packageType           0x%x (%s)\n", p->packageType, package_type_itoa (p->packageType)));
  FEP_XPRINT (debug_mask2, ("timer                 %d\n", p->timer));
  FEP_XPRINT (debug_mask2, ("origSysId             %d\n", p->origSysId));
  FEP_XPRINT (debug_mask2, ("receiverId            %d\n", p->receiverId));
  FEP_XPRINT (debug_mask2, ("invokeId              %d\n", p->invokeId));
  FEP_XPRINT (debug_mask2, ("diamctx_flags         0x%x (%s)\n", (* (ushort *) &p->diamctx_flags), bit_print ((* (ushort *) &p->diamctx_flags), sizeof (p->diamctx_flags))));
  FEP_XPRINT (debug_mask2, ("diamctx_flags.trap    %d (%s)\n", p->diamctx_flags.trap, FLAG_ITOA (p->diamctx_flags.trap)));
  FEP_XPRINT (debug_mask2, ("diamctx_flags.no_diam %d (%s)\n", p->diamctx_flags.no_diam, FLAG_ITOA (p->diamctx_flags.no_diam)));
  FEP_XPRINT (debug_mask2, ("diamctx_flags.no_gdi  %d (%s)\n", p->diamctx_flags.no_gdi, FLAG_ITOA (p->diamctx_flags.no_gdi)));
  FEP_XPRINT (debug_mask2, ("destStateId           %lu (%s)\n", p->destStateId, time_itoa (p->destStateId)));
  FEP_XPRINT (debug_mask2, ("arg_ptr               0x%x\n", p->arg_ptr));
  FEP_XPRINT (debug_mask2, ("cp                    %d\n", p->cp));
  FEP_XPRINT (debug_mask2, ("link                  %d\n", p->link));
  FEP_XPRINT (debug_mask2, ("npk_config            %d\n", p->npk_config));
  FEP_XPRINT (debug_mask2, ("errorString %s\n", p->errorString));
  FEP_XPRINT (debug_mask2, ("\n"));
}

/*
 * Ensure output of functions user_session_print () & user_session_sprint ()
 * are consistent
 */
void user_session_print (struct user_session *p, uint debug_mask2, char *caller)
{
  char me[] = "user_session_print";

  struct dgc_qmsg *q;
  char buffer[1024];
  char *b;
  int i, j;
  HostConfig *pHostConfig;
  RealmConfig *pRealmConfig;

  XPRINT_ENTERED;

  if (! (fep_debug_mask & debug_mask2))
    return;

  if (! p)  /* NULL US */
  {
    XPRINT_NULL ("US", p);
    return;
  }

  FEP_XPRINT (debug_mask2, ("%s (%s):\n", me, caller));

  if (fep_debug_mask & debug_mask[DEBUG_HEX_DUMP])
  {
    FEP_XPRINT (debug_mask2, ("\n"));
    FEP_XPRINT (debug_mask2, ("User Session Hex Dump\n"));
    FEP_XPRINT (debug_mask2, ("===============================================================================\n"));
    dump_memory ("", p, sizeof (*p), DUMP_RELATIVE);
  }

  FEP_XPRINT (debug_mask2, ("\n"));
  FEP_XPRINT (debug_mask2, ("User Session Dump (%s, %s, %s, 0x%x, %s)\n", caller, debug_msg_itoa (debug_mask2), command_code_itoa (p->diameter.command_code, p->diameter.command_flags, p->diameter.cc_request_type), p->gdi.tid_local, STATE_CCAS_ITOA (p->call_context.asm_state)));
  FEP_XPRINT (debug_mask2, ("===============================================================================\n"));
  FEP_XPRINT (debug_mask2, ("id %lu\n", p->id));
  FEP_XPRINT (debug_mask2, ("head 0x%x\n", p->head));
 
  FEP_XPRINT (debug_mask2, ("\n"));
  FEP_XPRINT (debug_mask2, ("User Session Call Context Dump\n"));
  FEP_XPRINT (debug_mask2, ("===============================================================================\n"));
  FEP_XPRINT (debug_mask2, ("asm_id     %d (%s)\n", p->call_context.asm_id, APP_SM_ITOA (p->call_context.asm_id)));
  FEP_XPRINT (debug_mask2, ("asm_state  %d (%s)\n", p->call_context.asm_state, STATE_CCAS_ITOA (p->call_context.asm_state)));
  FEP_XPRINT (debug_mask2, ("tcc_id     %lu\n", p->call_context.tcc_id));
  FEP_XPRINT (debug_mask2, ("tcc_expiry %d ms\n", p->call_context.tcc_expiry));
  FEP_XPRINT (debug_mask2, ("tg_id      %lu\n", p->call_context.tg_id));
  FEP_XPRINT (debug_mask2, ("tg_expiry  %d ms\n", p->call_context.tg_expiry));
  FEP_XPRINT (debug_mask2, ("version    %d\n", p->call_context.version));
  FEP_XPRINT (debug_mask2, ("time_start %lu (%s)\n", p->call_context.time_start, time_itoa (p->call_context.time_start)));
  FEP_XPRINT (debug_mask2, ("time_last  %lu (%s)\n", p->call_context.time_last, time_itoa (p->call_context.time_last)));
  FEP_XPRINT (debug_mask2, ("ref_count  %lu\n", p->call_context.ref_count));
  FEP_XPRINT (debug_mask2, ("cp         %d\n", p->call_context.cp));
  FEP_XPRINT (debug_mask2, ("npk_config %d\n", p->call_context.npk_config));
  FEP_XPRINT (debug_mask2, ("peer_fe    %d\n", p->call_context.peer_fe));
  FEP_XPRINT (debug_mask2, ("flags      0x%x (%s)\n", (* (ushort *) &p->call_context.flags), bit_print ((* (ushort *) &p->call_context.flags), sizeof (p->call_context.flags))));
  FEP_XPRINT (debug_mask2, ("flags.trap %d (%s)\n", p->call_context.flags.trap, FLAG_ITOA (p->call_context.flags.trap)));

  FEP_XPRINT (debug_mask2, ("\n"));
  FEP_XPRINT (debug_mask2, ("User Session Diameter Dump\n"));
  FEP_XPRINT (debug_mask2, ("===============================================================================\n"));
  FEP_XPRINT (debug_mask2, ("version            %d\n", p->diameter.version));
  FEP_XPRINT (debug_mask2, ("command_code       %d (%s)\n", p->diameter.command_code, command_code_itoa (p->diameter.command_code, p->diameter.command_flags, p->diameter.cc_request_type)));
  FEP_XPRINT (debug_mask2, ("command_flags      0x%x (%s, %s)\n", p->diameter.command_flags, bit_print (p->diameter.command_flags, sizeof (p->diameter.command_flags)), command_flags_print (p->diameter.command_flags)));
  FEP_XPRINT (debug_mask2, ("app_id             %lu (%s)\n", p->diameter.app_id, app_id_itoa (p->diameter.app_id)));
  FEP_XPRINT (debug_mask2, ("hop_by_hop         %lu\n", p->diameter.hop_by_hop));
  FEP_XPRINT (debug_mask2, ("end_to_end         %lu\n", p->diameter.end_to_end));
  FEP_XPRINT (debug_mask2, ("session_id_len     %d\n", p->diameter.session_id_len));
  FEP_XPRINT (debug_mask2, ("session_id %s\n", string_hex_print (p->diameter.session_id, p->diameter.session_id_len)));
  FEP_XPRINT (debug_mask2, ("cc_request_type    %d (%s)\n", p->diameter.cc_request_type, CCR_TYPE_ITOA (p->diameter.cc_request_type)));
  FEP_XPRINT (debug_mask2, ("cc_request_number  %lu\n", p->diameter.cc_request_number));
  FEP_XPRINT (debug_mask2, ("auth_app_id        %lu (%s)\n", p->diameter.auth_app_id, app_id_itoa (p->diameter.auth_app_id)));
  FEP_XPRINT (debug_mask2, ("local_host         %d (%s)\n", p->diameter.local_host, ((pHostConfig = conf_get_host (p->diameter.local_host)) ? (pHostConfig->fqdnHost) : ((p->diameter.local_host == MAX_HOSTs) ? (conf_get_local_host ()) : (DSM_UNDEF_STR)))));
  FEP_XPRINT (debug_mask2, ("local_realm        %d (%s)\n", p->diameter.local_realm, ((pRealmConfig = conf_get_realm (p->diameter.local_realm)) ? (pRealmConfig->fqdnRealm) : ((p->diameter.local_realm == MAX_REALMs) ? (conf_get_local_realm ()) : (DSM_UNDEF_STR)))));
  FEP_XPRINT (debug_mask2, ("local_state        %lu (%s)\n", p->diameter.local_state, time_itoa (p->diameter.local_state)));
  FEP_XPRINT (debug_mask2, ("remote_host        %d (%s)\n", p->diameter.remote_host, ((pHostConfig = conf_get_host (p->diameter.remote_host)) ? (pHostConfig->fqdnHost) : ((p->diameter.remote_host == MAX_HOSTs) ? (conf_get_local_host ()) : (DSM_UNDEF_STR)))));
  FEP_XPRINT (debug_mask2, ("remote_realm       %d (%s)\n", p->diameter.remote_realm, ((pRealmConfig = conf_get_realm (p->diameter.remote_realm)) ? (pRealmConfig->fqdnRealm) : ((p->diameter.remote_realm == MAX_REALMs) ? (conf_get_local_realm ()) : (DSM_UNDEF_STR)))));
  FEP_XPRINT (debug_mask2, ("remote_state       %lu\n", p->diameter.remote_state));
  FEP_XPRINT (debug_mask2, ("sub_id_type        %d (%s)\n", p->diameter.sub_id_type, SUB_ID_TYPE_ITOA (p->diameter.sub_id_type)));
  FEP_XPRINT (debug_mask2, ("sub_id %s\n", string_hex_print (p->diameter.sub_id, strlen (p->diameter.sub_id))));

  FEP_XPRINT (debug_mask2, ("proxy_count        %d\n", p->diameter.proxy_count));
  for (j = i = 0; i < p->diameter.proxy_count; ++i)
  {
    FEP_XPRINT (debug_mask2, ("proxy_host[%d]      %d\n", i, p->diameter.proxy_host[i]));
    FEP_XPRINT (debug_mask2, ("proxy_state_len[%d] %d\n", i, p->diameter.proxy_state_len[i]));
    j += p->diameter.proxy_state_len[i];
  }
  FEP_XPRINT (debug_mask2, ("proxy_states %s\n", string_hex_print (p->diameter.proxy_states, j)));

  for (i = 0, q = p->diameter.dgc_qmsg.tqh_first; q; q = q->tqe.tqe_next, ++i)
  {
    b = buffer;
    b += sprintf (b, "dgc_qmsg[%3d] ", i);
    for (j = 0; j < FEP_QMSG_N_PARMS; ++j)
      b += sprintf (b, "0x%x ", q->qmsg[j]);

    FEP_XPRINT (debug_mask2, ("%s\n", buffer));
  }

  FEP_XPRINT (debug_mask2, ("\n"));
  FEP_XPRINT (debug_mask2, ("User Session GDI Dump\n"));
  FEP_XPRINT (debug_mask2, ("===============================================================================\n"));
  FEP_XPRINT (debug_mask2, ("package_type     0x%x (%s)\n", p->gdi.package_type, package_type_itoa (p->gdi.package_type)));
  FEP_XPRINT (debug_mask2, ("package_type_asr 0x%x (%s)\n", p->gdi.package_type_asr, package_type_itoa (p->gdi.package_type_asr)));
  FEP_XPRINT (debug_mask2, ("package_type_rar 0x%x (%s)\n", p->gdi.package_type_rar, package_type_itoa (p->gdi.package_type_rar)));
  FEP_XPRINT (debug_mask2, ("tid_local        0x%x (%s)\n", p->gdi.tid_local, bit_print (p->gdi.tid_local, sizeof (p->gdi.tid_local))));
  FEP_XPRINT (debug_mask2, ("tid_remote       0x%x (%s)\n", p->gdi.tid_remote, bit_print (p->gdi.tid_remote, sizeof (p->gdi.tid_remote))));
  FEP_XPRINT (debug_mask2, ("tid_remote_asr   0x%x (%s)\n", p->gdi.tid_remote_asr, bit_print (p->gdi.tid_remote_asr, sizeof (p->gdi.tid_remote_asr))));
  FEP_XPRINT (debug_mask2, ("tid_remote_rar   0x%x (%s)\n", p->gdi.tid_remote_rar, bit_print (p->gdi.tid_remote_rar, sizeof (p->gdi.tid_remote_rar))));
  FEP_XPRINT (debug_mask2, ("origSysId        %d\n", p->gdi.origSysId));
  FEP_XPRINT (debug_mask2, ("receiverId       %d\n", p->gdi.receiverId));
  FEP_XPRINT (debug_mask2, ("invokeId         %d\n", p->gdi.invokeId));
  FEP_XPRINT (debug_mask2, ("\n"));
}

/*
 * Ensure output of functions user_session_print () & user_session_sprint ()
 * are consistent
 */
char *user_session_sprint (struct user_session *p)
{
  char me[] = "user_session_sprint";

  static char buffer[4096];

  struct dgc_qmsg *q;
  char buffer2[1024];
  char *b, *b2;
  int i, j;
  HostConfig *pHostConfig;
  RealmConfig *pRealmConfig;

  XPRINT_ENTERED;

  if (! p)  /* NULL US */
  {
    XPRINT_NULL ("US", p);
    return NULL;
  }

  b = buffer;
  b += sprintf (b, "User Session Dump (%s, 0x%x, %s)\n", command_code_itoa (p->diameter.command_code, p->diameter.command_flags, p->diameter.cc_request_type), p->gdi.tid_local, STATE_CCAS_ITOA (p->call_context.asm_state));
  b += sprintf (b, "===============================================================================\n");
  b += sprintf (b, "id %lu\n", p->id);
  b += sprintf (b, "head 0x%x\n", p->head);
 
  b += sprintf (b, "\n");
  b += sprintf (b, "User Session Call Context Dump\n");
  b += sprintf (b, "===============================================================================\n");
  b += sprintf (b, "asm_id     %d (%s)\n", p->call_context.asm_id, APP_SM_ITOA (p->call_context.asm_id));
  b += sprintf (b, "asm_state  %d (%s)\n", p->call_context.asm_state, STATE_CCAS_ITOA (p->call_context.asm_state));
  b += sprintf (b, "tcc_id     %lu\n", p->call_context.tcc_id);
  b += sprintf (b, "tcc_expiry %d ms\n", p->call_context.tcc_expiry);
  b += sprintf (b, "tg_id      %lu\n", p->call_context.tg_id);
  b += sprintf (b, "tg_expiry  %d ms\n", p->call_context.tg_expiry);
  b += sprintf (b, "version    %d\n", p->call_context.version);
  b += sprintf (b, "time_start %lu (%s)\n", p->call_context.time_start, time_itoa (p->call_context.time_start));
  b += sprintf (b, "time_last  %lu (%s)\n", p->call_context.time_last, time_itoa (p->call_context.time_last));
  b += sprintf (b, "ref_count  %lu\n", p->call_context.ref_count);
  b += sprintf (b, "cp         %d\n", p->call_context.cp);
  b += sprintf (b, "npk_config %d\n", p->call_context.npk_config);
  b += sprintf (b, "peer_fe    %d\n", p->call_context.peer_fe);
  b += sprintf (b, "flags      0x%x (%s)\n", (* (ushort *) &p->call_context.flags), bit_print ((* (ushort *) &p->call_context.flags), sizeof (p->call_context.flags)));
  b += sprintf (b, "flags.trap %d (%s)\n", p->call_context.flags.trap, FLAG_ITOA (p->call_context.flags.trap));

  b += sprintf (b, "\n");
  b += sprintf (b, "User Session Diameter Dump\n");
  b += sprintf (b, "===============================================================================\n");
  b += sprintf (b, "version            %d\n", p->diameter.version);
  b += sprintf (b, "command_code       %d (%s)\n", p->diameter.command_code, command_code_itoa (p->diameter.command_code, p->diameter.command_flags, p->diameter.cc_request_type));
  b += sprintf (b, "command_flags      0x%x (%s, %s)\n", p->diameter.command_flags, bit_print (p->diameter.command_flags, sizeof (p->diameter.command_flags)), command_flags_print (p->diameter.command_flags));
  b += sprintf (b, "app_id             %lu (%s)\n", p->diameter.app_id, app_id_itoa (p->diameter.app_id));
  b += sprintf (b, "hop_by_hop         %lu\n", p->diameter.hop_by_hop);
  b += sprintf (b, "end_to_end         %lu\n", p->diameter.end_to_end);
  b += sprintf (b, "session_id_len     %d\n", p->diameter.session_id_len);
  b += sprintf (b, "session_id %s\n", string_hex_print (p->diameter.session_id, p->diameter.session_id_len));
  b += sprintf (b, "cc_request_type    %d (%s)\n", p->diameter.cc_request_type, CCR_TYPE_ITOA (p->diameter.cc_request_type));
  b += sprintf (b, "cc_request_number  %lu\n", p->diameter.cc_request_number);
  b += sprintf (b, "auth_app_id        %lu (%s)\n", p->diameter.auth_app_id, app_id_itoa (p->diameter.auth_app_id));
  b += sprintf (b, "local_host         %d (%s)\n", p->diameter.local_host, ((pHostConfig = conf_get_host (p->diameter.local_host)) ? (pHostConfig->fqdnHost) : ((p->diameter.local_host == MAX_HOSTs) ? (conf_get_local_host ()) : (DSM_UNDEF_STR))));
  b += sprintf (b, "local_realm        %d (%s)\n", p->diameter.local_realm, ((pRealmConfig = conf_get_realm (p->diameter.local_realm)) ? (pRealmConfig->fqdnRealm) : ((p->diameter.local_realm == MAX_REALMs) ? (conf_get_local_realm ()) : (DSM_UNDEF_STR))));
  b += sprintf (b, "local_state        %lu (%s)\n", p->diameter.local_state, time_itoa (p->diameter.local_state));
  b += sprintf (b, "remote_host        %d (%s)\n", p->diameter.remote_host, ((pHostConfig = conf_get_host (p->diameter.remote_host)) ? (pHostConfig->fqdnHost) : ((p->diameter.remote_host == MAX_HOSTs) ? (conf_get_local_host ()) : (DSM_UNDEF_STR))));
  b += sprintf (b, "remote_realm       %d (%s)\n", p->diameter.remote_realm, ((pRealmConfig = conf_get_realm (p->diameter.remote_realm)) ? (pRealmConfig->fqdnRealm) : ((p->diameter.remote_realm == MAX_REALMs) ? (conf_get_local_realm ()) : (DSM_UNDEF_STR))));
  b += sprintf (b, "remote_state       %lu\n", p->diameter.remote_state);
  b += sprintf (b, "sub_id_type        %d (%s)\n", p->diameter.sub_id_type, SUB_ID_TYPE_ITOA (p->diameter.sub_id_type));
  b += sprintf (b, "sub_id %s\n", string_hex_print (p->diameter.sub_id, strlen (p->diameter.sub_id)));

  b += sprintf (b, "proxy_count        %d\n", p->diameter.proxy_count);
  for (j = i = 0; i < p->diameter.proxy_count; ++i)
  {
    b += sprintf (b, "proxy_host[%d]      %d\n", i, p->diameter.proxy_host[i]);
    b += sprintf (b, "proxy_state_len[%d] %d\n", i, p->diameter.proxy_state_len[i]);
    j += p->diameter.proxy_state_len[i];
  }
  b += sprintf (b, "proxy_states %s\n", string_hex_print (p->diameter.proxy_states, j));

  for (i = 0, q = p->diameter.dgc_qmsg.tqh_first; q; q = q->tqe.tqe_next, ++i)
  {
    b2 = buffer2;
    b2 += sprintf (b2, "dgc_qmsg[%3d] ", i);
    for (j = 0; j < FEP_QMSG_N_PARMS; ++j)
      b2 += sprintf (b2, "0x%x ", q->qmsg[j]);

    b += sprintf (b, "%s\n", buffer2);
  }

  b += sprintf (b, "\n");
  b += sprintf (b, "User Session GDI Dump\n");
  b += sprintf (b, "===============================================================================\n");
  b += sprintf (b, "package_type     0x%x (%s)\n", p->gdi.package_type, package_type_itoa (p->gdi.package_type));
  b += sprintf (b, "package_type_asr 0x%x (%s)\n", p->gdi.package_type_asr, package_type_itoa (p->gdi.package_type_asr));
  b += sprintf (b, "package_type_rar 0x%x (%s)\n", p->gdi.package_type_rar, package_type_itoa (p->gdi.package_type_rar));
  b += sprintf (b, "tid_local        0x%x (%s)\n", p->gdi.tid_local, bit_print (p->gdi.tid_local, sizeof (p->gdi.tid_local)));
  b += sprintf (b, "tid_remote       0x%x (%s)\n", p->gdi.tid_remote, bit_print (p->gdi.tid_remote, sizeof (p->gdi.tid_remote)));
  b += sprintf (b, "tid_remote_asr   0x%x (%s)\n", p->gdi.tid_remote_asr, bit_print (p->gdi.tid_remote_asr, sizeof (p->gdi.tid_remote_asr)));
  b += sprintf (b, "tid_remote_rar   0x%x (%s)\n", p->gdi.tid_remote_rar, bit_print (p->gdi.tid_remote_rar, sizeof (p->gdi.tid_remote_rar)));
  b += sprintf (b, "origSysId        %d\n", p->gdi.origSysId);
  b += sprintf (b, "receiverId       %d\n", p->gdi.receiverId);
  b += sprintf (b, "invokeId         %d\n", p->gdi.invokeId);

  return buffer;
}

void print_size_ccdb (uint debug_mask2, char *caller)
{
  char me[] = "print_size_ccdb";

  int sz_ccdb, sz_us, sz_us_max, sz_ccdb_max;
  int sz_us_cc, sz_us_diam, sz_us_gdi, sz_us_overhead;
  float per_sz_us_cc, per_sz_us_diam, per_sz_us_gdi, per_sz_us_overhead;

  XPRINT_ENTERED;

  if (! (fep_debug_mask & debug_mask2))
    return;

  sz_ccdb = sizeof (struct ccdb);
  sz_us = sizeof (struct user_session);
  sz_us_max = sizeof (fepmem_list_user_session);
  sz_ccdb_max = sz_ccdb + sz_us_max;
  sz_us_cc = sizeof (struct us_call_context);
  sz_us_diam = sizeof (struct us_diameter);
  sz_us_gdi = sizeof (struct us_gdi);
  sz_us_overhead = sz_us - sz_us_cc - sz_us_diam - sz_us_gdi;
  per_sz_us_cc = 100 * ((float) sz_us_cc / (float) sz_us);
  per_sz_us_diam = 100 * ((float) sz_us_diam / (float) sz_us);
  per_sz_us_gdi = 100 * ((float) sz_us_gdi / (float) sz_us);
  per_sz_us_overhead = 100 * ((float) sz_us_overhead / (float) sz_us);

  FEP_XPRINT (debug_mask2, ("%s (%s):\n", me, caller));
  FEP_XPRINT (debug_mask2, ("\n"));
  FEP_XPRINT (debug_mask2, ("CCDB Size\n"));
  FEP_XPRINT (debug_mask2, ("===============================================================================\n"));
  FEP_XPRINT (debug_mask2, (" (1) Initial CCDB size                 %10d\n", sz_ccdb));
  FEP_XPRINT (debug_mask2, (" (2) User session size                 %10d\n", sz_us));
  FEP_XPRINT (debug_mask2, (" (3) Max outstanding user sessions     %10d\n", USER_SESSION_SIZE));
  FEP_XPRINT (debug_mask2, (" (4) Max outstanding user session size %10d ((3) * (2))\n", sz_us_max));
  FEP_XPRINT (debug_mask2, (" (5) Max CCDB size                     %10d ((1) + (4))\n", sz_ccdb_max));
  FEP_XPRINT (debug_mask2, ("\n"));
  FEP_XPRINT (debug_mask2, (" (6) User session call context size    %10d (%2.0f percent of (2))\n", sz_us_cc, per_sz_us_cc));
  FEP_XPRINT (debug_mask2, (" (7) User session Diameter size        %10d (%2.0f percent of (2))\n", sz_us_diam, per_sz_us_diam));
  FEP_XPRINT (debug_mask2, (" (8) User session GDI size             %10d (%2.0f percent of (2))\n", sz_us_gdi, per_sz_us_gdi));
  FEP_XPRINT (debug_mask2, (" (9) User session overhead size        %10d (%2.0f percent of (2))\n", sz_us_overhead, per_sz_us_overhead));
  FEP_XPRINT (debug_mask2, ("(10) User session size                 %10d ((6) + (7) + (8) + (9) = (2))\n", sz_us));
  FEP_XPRINT (debug_mask2, ("\n"));
}

void dgc_qmsg_init (struct dgc_qmsg *p)
{
  char me[] = "dgc_qmsg_init";

  if (fep_debug_mask & debug_mask[DEBUG_FREQUENT])
    XPRINT_ENTERED;

  if (! p)  /* NULL DGC qmsg */
  {
    ++meas.ds_cntr.ds_dgc_qmsg_null;
    XPRINT_NULL ("DGC qmsg", p);

    ++meas.ds_cntr.ds_ccdb_dgc_qmsg_init_fail;
    return;
  }

  /*
   * Don't initialize 1st 2 fields because they contain persistent data
   */
  memset (((char *) p + sizeof (p->fifoBlk) + sizeof (p->id)), 0, (sizeof (*p) - sizeof (p->fifoBlk) - sizeof (p->id)));
  ++meas.ds_cntr.ds_ccdb_dgc_qmsg_init_succ;
}

void dgc_qmsg_remove (struct dgc_qmsg **p2)
{
  char me[] = "dgc_qmsg_remove";

  struct dgc_qmsg *p;

  XPRINT_ENTERED;

  if (! p2)  /* NULL DGC qmsg pointer */
  {
    ++meas.ds_cntr.ds_dgc_qmsg_ptr_null;
    XPRINT_NULL ("DGC qmsg pointer", p2);

    ++meas.ds_cntr.ds_ccdb_dgc_qmsg_remove_fail;
    return;
  }

  if (! (p = *p2))  /* NULL DGC qmsg */
  {
    ++meas.ds_cntr.ds_dgc_qmsg_null;
    XPRINT_NULL ("DGC qmsg", p);

    ++meas.ds_cntr.ds_ccdb_dgc_qmsg_remove_fail;
    return;
  }

  if (! p->head)  /* NULL DGC qmsg list */
  {
    ++meas.ds_cntr.ds_dgc_qmsg_list_null;
    XPRINT_NULL ("DGC qmsg list", p);

    ++meas.ds_cntr.ds_ccdb_dgc_qmsg_remove_fail;
    return;
  }

  /*
   * Remove DGC qmsg from list & free memory
   */
  TAILQ_REMOVE (p->head, p, tqe);
  ++meas.ds_cntr.ds_dgc_qmsg_tailq_remove;

  dgc_qmsg_init (p);
  delete_private (&fepmem_memory_dgc_qmsg, p);
  ++meas.ds_cntr.ds_api_delete_private_dgc_qmsg;

  p = *p2 = NULL;
  ++meas.ds_cntr.ds_ccdb_dgc_qmsg_remove_succ;
}

int dgc_qmsg_add (fep_qmsg_t qmsg, DiamCtx_t *pbuf_cc, struct user_session *p)
{
  char me[] = "dgc_qmsg_add";

  fep_buffer_t *pbuf;
  int parm;
  struct dgc_qmsg *new;

  XPRINT_ENTERED;

  if (! pbuf_cc)  /* NULL DiamCtx */
  {
    ++meas.ds_cntr.ds_dc_null;
    XPRINT_NULL ("DiamCtx", pbuf_cc);

    ++meas.ds_cntr.ds_ccdb_dgc_qmsg_add_fail;
    return FAILURE;
  }
  pbuf = (fep_buffer_t *) pbuf_cc;

  if (! p)  /* NULL US */
  {
    ++meas.ds_cntr.ds_us_null;
    XPRINT_NULL ("US", p);

    ++meas.ds_cntr.ds_ccdb_dgc_qmsg_add_fail;
    return FAILURE;
  }

  diamctx_print (pbuf_cc, debug_mask[DEBUG_MSG_QUEUE_ENTER], me);
  user_session_print (p, debug_mask[DEBUG_MSG_QUEUE_ENTER], me);

  if (! p->head)  /* NULL US list */
  {
    ++meas.ds_cntr.ds_us_list_null;
    XPRINT_NULL ("US list", p->head);

    ++meas.ds_cntr.ds_ccdb_dgc_qmsg_add_fail;
    return FAILURE;
  }

  /*
   * Allocate new DGC qmsg memory
   */
  if (! (new = (struct dgc_qmsg *) new_private (&fepmem_memory_dgc_qmsg)))
  {
    ++meas.ds_cntr.ds_api_new_private_fail_us;
    XPRINT_FAILED ("new_private");
    fepmem_memory_print (&fepmem_memory_dgc_qmsg, debug_mask[DEBUG_DSM_2], me);

    ++meas.ds_cntr.ds_ccdb_dgc_qmsg_add_fail;
    return FAILURE;
  }
  ++meas.ds_cntr.ds_api_new_private_succ_us;

  diamctx_print (pbuf_cc, debug_mask[DEBUG_MSG_QUEUE_EXIT], me);
  XPRINT_DIAMCTX_QUEUE;

  /*
   * Failed to convert (& release) buffer to parameter
   */
  if ((parm = buf_to_parm (pbuf)) < 0)
  {
    ++meas.ds_cntr.ds_api_buf_to_parm_fail;
    XPRINT_FAILED_BUF_TO_PARM;

    dgc_qmsg_init (new);
    delete_private (&fepmem_memory_dgc_qmsg, new);
    ++meas.ds_cntr.ds_api_delete_private_dgc_qmsg;

    ++meas.ds_cntr.ds_ccdb_dgc_qmsg_add_fail;
    return FAILURE;
  }

  /*
   * Save or set DGC qmsg data
   */
  new->head = &p->diameter.dgc_qmsg;
  memcpy (new->qmsg, qmsg, sizeof (new->qmsg));
  new->qmsg[1] = parm;

  /*
   * Insert new DGC qmsg to end of list
   */
  TAILQ_INSERT_TAIL (new->head, new, tqe);
  ++meas.ds_cntr.ds_dgc_qmsg_tailq_insert_tail;

  /*
   * Update CC stats
   */
  ++p->call_context.ref_count;
  p->call_context.time_last = time (NULL);

  user_session_print (p, debug_mask[DEBUG_MSG_QUEUE_EXIT], me);

  ++meas.ds_cntr.ds_ccdb_dgc_qmsg_add_succ;
  return SUCCESS;
}

void user_session_init (struct user_session *p)
{
  char me[] = "user_session_init";

  if (fep_debug_mask & debug_mask[DEBUG_FREQUENT])
    XPRINT_ENTERED;

  if (! p)  /* NULL US */
  {
    ++meas.ds_cntr.ds_us_null;
    XPRINT_NULL ("US", p);

    ++meas.ds_cntr.ds_ccdb_us_init_fail;
    return;
  }

  /*
   * Don't initialize 1st 2 fields because they contain persistent data
   */
  memset (((char *) p + sizeof (p->fifoBlk) + sizeof (p->id)), 0, (sizeof (*p) - sizeof (p->fifoBlk) - sizeof (p->id)));

  /*
   * Initialize US
   */
  p->call_context.asm_id = APP_SM_UNDEF;
  p->call_context.asm_state = DSM_UNDEF;
  p->call_context.tcc_id = ID_UNDEF;
  p->call_context.tg_id = ID_UNDEF;
  p->call_context.cp = UNINITIALIZED;
  p->call_context.npk_config = UNINITIALIZED;
  p->call_context.peer_fe = UNINITIALIZED;
  p->diameter.local_host = UNINITIALIZED;
  p->diameter.local_realm = UNINITIALIZED;
  p->diameter.remote_host = UNINITIALIZED;
  p->diameter.remote_realm = UNINITIALIZED;
  p->diameter.sub_id_type = UNINITIALIZED;

  TAILQ_INIT (&p->diameter.dgc_qmsg);
  ++meas.ds_cntr.ds_dgc_qmsg_list_tailq_init;

  ++meas.ds_cntr.ds_ccdb_us_init_succ;
}

struct user_session *user_session_remove (struct user_session **p2)
{
  char me[] = "user_session_remove";

  fep_buffer_t *pbuf;
  DiamCtx_t *pbuf_cc;
  struct user_session *p;
  struct dgc_qmsg_tailq *r;
  struct dgc_qmsg *s;
  int rv, parm;

  XPRINT_ENTERED;

  if (! p2)  /* NULL US pointer*/
  {
    ++meas.ds_cntr.ds_us_ptr_null;
    XPRINT_NULL ("US pointer", p2);

    ++meas.ds_cntr.ds_ccdb_us_remove_fail;
    return NULL;
  }

  if (! (p = *p2))  /* NULL US */
  {
    ++meas.ds_cntr.ds_us_null;
    XPRINT_NULL ("US", p);

    ++meas.ds_cntr.ds_ccdb_us_remove_fail;
    return p;
  }

  if (! p->head)  /* NULL US list */
  {
    ++meas.ds_cntr.ds_us_list_null;
    XPRINT_NULL ("US list", p->head);

    ++meas.ds_cntr.ds_ccdb_us_remove_fail;
    return p;
  }

  /*
   * Cancel Tcc & Tg timers
   */
  callout_unset (&p->call_context.tcc_id);
  callout_unset (&p->call_context.tg_id);

  /*
   * Remove DGC qmsgs associated with US.  There should only be inbound
   * Diameter messages queued.
   */
  r = &p->diameter.dgc_qmsg;
  for (s = r->tqh_first; s; s = r->tqh_first)
  {
    if (! (pbuf = parm_to_buf (s->qmsg[1])))  /* NULL buffer */
    {
      ++meas.ds_cntr.ds_api_parm_to_buf_fail;
      FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: NULL buffer 0x%x, qmsg[1] 0x%x\n", me, pbuf, s->qmsg[1]));

      /*
       * Remove DGC qmsg from list & free memory
       */
      dgc_qmsg_remove (&s);
      continue;
    }
    pbuf_cc = (DiamCtx_t *) pbuf;

    diamctx_print (pbuf_cc, debug_mask[DEBUG_MSG_QUEUE_ENTER], me);

    /*
     * Discarded queued Diameter message.  No need for DGC to respond to the
     * network or to forward to the ISCP.  Tell DGC to just free its DiamCtx
     * resource.
     */
    pbuf_cc->diamctx_flags.no_diam = TRUE;
    pbuf_cc->diamctx_flags.no_gdi = TRUE;
    SPRINTF_ES_DISC_QUEUED_MSG;

    diamctx_print (pbuf_cc, debug_mask[DEBUG_MSG_QUEUE_EXIT], me);
    XPRINT_DIAMCTX_XMIT;

    /*
     * Failed to convert (& release) buffer to parameter
     */
    if ((parm = buf_to_parm (pbuf)) < 0)
    {
      ++meas.ds_cntr.ds_api_buf_to_parm_fail;
      XPRINT_FAILED_BUF_TO_PARM;

      /*
       * Remove DGC qmsg from list & free memory
       */
      dgc_qmsg_remove (&s);
      continue;
    }

    /*
     * Tell sender about "failure" (i.e., DSM will not process this DGC qmsg)
     */
    if ((rv = tell_dgc (MSG_DIAMCTX, parm, FAILURE, DIAMETER_UNABLE_TO_COMPLY)) != SUCCESS)
    {
      ++meas.ds_cntr.ds_api_tell_dgc_fail;
      XPRINT_FAILED_TELL ("tell_dgc");

      /*
       * Free memory associated with DiamCtx
       */
      fep_free_buffer (parm_to_buf (parm));
      ++meas.ds_cntr.ds_api_fep_free_buffer;
    }
    else
    {
      ++meas.ds_cntr.ds_api_tell_dgc_succ;
      XPRINT_QMSG_XMIT (MSG_DIAMCTX);
    }

    /*
     * Remove DGC qmsg from list & free memory
     */
    dgc_qmsg_remove (&s);
  }

  /*
   * Remove US from time-sorted list
   */
  TAILQ_REMOVE (&ccdb.user_session_time, p, tqe_time);
  ++meas.ds_cntr.ds_us_tailq_remove_time;

  /*
   * Remove US from list & free memory
   */
  TAILQ_REMOVE (p->head, p, tqe);
  ++meas.ds_cntr.ds_us_tailq_remove;

  user_session_init (p);
  delete_private (&fepmem_memory_user_session, p);
  ++meas.ds_cntr.ds_api_delete_private_us;

  ++meas.ds_cntr.ds_ccdb_us_remove_succ;
  p = *p2 = NULL;
  return p;
}

void initialize_ccdb ()
{
  char me[] = "initialize_ccdb";

  int i;

  XPRINT_ENTERED;

  fep_create_private (&fepmem_privInfo_dgc_qmsg);
  ++meas.ds_cntr.ds_api_fep_create_private_dgc_qmsg;

  fep_create_private (&fepmem_privInfo_user_session);
  ++meas.ds_cntr.ds_api_fep_create_private_us;

  memset (&ccdb, 0, sizeof (ccdb));
  ccdb.tg_expiry = TG_DEF;
  TAILQ_INIT (&ccdb.user_session_time);
  ++meas.ds_cntr.ds_us_list_tailq_init_time;

  for (i = 0; i < DGC_QMSG_SIZE; ++i)
  {
    fepmem_list_dgc_qmsg[i].id = INDEX_TO_ID (i);
    dgc_qmsg_init (&fepmem_list_dgc_qmsg[i]);
  }

  for (i = 0; i < USER_SESSION_SIZE; ++i)
  {
    fepmem_list_user_session[i].id = INDEX_TO_ID (i);
    user_session_init (&fepmem_list_user_session[i]);

    TAILQ_INIT (&ccdb.user_session[i]);
    ++meas.ds_cntr.ds_us_list_tailq_init;
  }

  print_size_ccdb (debug_mask[DEBUG_DSM_2], me);
}

void finalize_ccdb ()
{
  char me[] = "finalize_ccdb";

  int i;
  struct user_session_tailq *q;
  struct user_session *p;

  XPRINT_ENTERED;

  /*
   * Remove all USs
   */
  q = &ccdb.user_session_time;
  for (p = q->tqh_first; p; p = q->tqh_first)
  {
    XPRINT_US_REMOVE;

    /*
     * Remove US from list & free memory
     */
    if (p = user_session_remove (&p))
    {
      XPRINT_FAILED_US_REMOVE;
    }
  }
}

short set_peer_fe ()
{
  char me[] = "set_peer_fe";

  short peer_fe;
  int fe;
  AssociateConfig *p;

  XPRINT_ENTERED;

  peer_fe = UNINITIALIZED;

  /*
   * Determine peer FE ID
   */
  if ((fe = conf_get_fe_number ()) <= 0)  /* Current FE ID does not exist */
  {
    ++meas.ds_cntr.ds_api_conf_get_fe_number_fail;
    XPRINT_FAILED_CONF_GET_FE_NUMBER;

    ++meas.ds_cntr.ds_ccdb_set_peer_fe_fail;
    return peer_fe;
  }

  if (! (p = conf_get_associate (fe)))  /* Current FE does not exist */
  {
    ++meas.ds_cntr.ds_api_conf_get_associate_fail;
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: conf_get_associate failed, fe %d, p 0x%x\n", me, fe, p));

    ++meas.ds_cntr.ds_ccdb_set_peer_fe_fail;
    return peer_fe;
  }

  ++meas.ds_cntr.ds_ccdb_set_peer_fe_succ;
  peer_fe = p->iNextAssociate;
  return peer_fe;
}

ushort set_package_type (struct user_session *p)
{
  char me[] = "set_package_type";

  ushort package_type_req;

  XPRINT_ENTERED;

  if (! p)  /* NULL US */
  {
    ++meas.ds_cntr.ds_us_null;
    XPRINT_NULL ("US", p);

    ++meas.ds_cntr.ds_ccdb_set_package_type_fail;
    return PACKAGE_TYPE_UNDEF;
  }

  if (! p->head)  /* NULL US list */
  {
    ++meas.ds_cntr.ds_us_list_null;
    XPRINT_NULL ("US list", p->head);

    ++meas.ds_cntr.ds_ccdb_set_package_type_fail;
    return PACKAGE_TYPE_UNDEF;
  }

  /*
   * Received a Diameter request message
   */
  if (p->diameter.command_flags & DIAM_CMD_FLAG_R)
  {
    /*
     * Local TID not present & remote TID not present
     */
    if ((p->gdi.tid_local <= 0) && (p->gdi.tid_remote <= 0))
    {
      ++meas.ds_cntr.ds_ccdb_set_package_type_succ;
      return PACKAGE_TYPE_REQUEST;
    }

    /*
     * Local TID present & remote TID present
     */
    if ((p->gdi.tid_local > 0) && (p->gdi.tid_remote > 0))
    {
      ++meas.ds_cntr.ds_ccdb_set_package_type_succ;
      return PACKAGE_TYPE_CONV;
    }

    /*
     * Local TID not present & remote TID present, or
     * local TID present & remote TID not present
     */
    ++meas.ds_cntr.ds_ccdb_set_package_type_fail;
    return PACKAGE_TYPE_UNDEF;
  }

  /*
   * Received a Diameter answer or error message
   */

  /*
   * Local TID present & remote TID present
   */
  if ((p->gdi.tid_local > 0) && (p->gdi.tid_remote > 0))
  {
    switch (p->diameter.command_code)
    {
      case DIAM_CMD_ASA:

        package_type_req = p->gdi.package_type_asr;
        break;

      case DIAM_CMD_RAA:

        package_type_req = p->gdi.package_type_rar;
        break;

      default:

        ++meas.ds_cntr.ds_ccdb_set_package_type_fail;
        return PACKAGE_TYPE_UNDEF;
    }

    if (package_type_req != PACKAGE_TYPE_CONV)
    {
      ++meas.ds_cntr.ds_ccdb_set_package_type_fail;
      return PACKAGE_TYPE_UNDEF;
    }

    ++meas.ds_cntr.ds_ccdb_set_package_type_succ;
    return PACKAGE_TYPE_CONV;
  }

  /*
   * Local TID not present or remote TID not present
   */
  ++meas.ds_cntr.ds_ccdb_set_package_type_fail;
  return PACKAGE_TYPE_UNDEF;
}

ulong set_tid_local (short peer_fe, ulong id)
{
  char me[] = "set_tid_local";

  static ulong non_us_id = USER_SESSION_SIZE + 1;

  ulong tid_local;
  int fe, csi;

  XPRINT_ENTERED;

  tid_local = 0;
  if ((fe = conf_get_fe_number ()) <= 0)
  {
    ++meas.ds_cntr.ds_api_conf_get_fe_number_fail;
    XPRINT_FAILED_CONF_GET_FE_NUMBER;

    if (USER_SESSION_ID_VALID (id))
    {
      ++meas.ds_cntr.ds_ccdb_set_tid_local_fail;
      return tid_local;
    }

    /*
     * Despite failure, continue because there's no US associated with this
     * transaction; just generate a unique, non-zero local TID
     */
    fe = 0;
  }

  if (peer_fe <= 0)
  {
    ++meas.ds_cntr.ds_ccdb_set_tid_local_peer_fe_invalid;
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: Invalid peer_fe %d\n", me, peer_fe));

    if (USER_SESSION_ID_VALID (id))
    {
      ++meas.ds_cntr.ds_ccdb_set_tid_local_fail;
      return tid_local;
    }

    /*
     * Despite failure, continue because there's no US associated with this
     * transaction; just generate a unique, non-zero local TID
     */
    peer_fe = 0;
  }

  if ((csi = conf_get_csi_number ()) < 0)
  {
    ++meas.ds_cntr.ds_api_conf_get_csi_number_fail;
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: conf_get_csi_number failed, csi %d\n", me, csi));

    if (USER_SESSION_ID_VALID (id))
    {
      ++meas.ds_cntr.ds_ccdb_set_tid_local_fail;
      return tid_local;
    }

    /*
     * Despite failure, continue because there's no US associated with this
     * transaction; just generate a unique, non-zero local TID
     */
    csi = 0;
  }

  tid_local |= (fe << SHIFT_TID_FE);
  tid_local |= (peer_fe << SHIFT_TID_FE_PEER);
  tid_local |= (csi << SHIFT_TID_CSI);

  if (USER_SESSION_ID_VALID (id))
  {
    tid_local |= (id << SHIFT_TID_CC);
  }
  else
  {
    /*
     * No US associated with this transaction.  Embed a unique, non-US ID into
     * local TID.
     */
    tid_local |= (non_us_id << SHIFT_TID_CC);

    if (++non_us_id > MASK_TID_CC)
      non_us_id = USER_SESSION_SIZE + 1;
  }

  ++meas.ds_cntr.ds_ccdb_set_tid_local_succ;
  return tid_local;
}

short set_asm_id (struct user_session *p)
{
  char me[] = "set_asm_id";

  XPRINT_ENTERED;

  if (! p)  /* NULL US */
  {
    ++meas.ds_cntr.ds_us_null;
    XPRINT_NULL ("US", p);

    ++meas.ds_cntr.ds_ccdb_set_asm_id_fail;
    return APP_SM_UNDEF;
  }

  if (! p->head)  /* NULL US list */
  {
    ++meas.ds_cntr.ds_us_list_null;
    XPRINT_NULL ("US list", p->head);

    ++meas.ds_cntr.ds_ccdb_set_asm_id_fail;
    return APP_SM_UNDEF;
  }

  /*
   * Use IANA-assigned application ID to determine corresponding ASM ID
   */
  switch (p->diameter.app_id)
  {
    case APP_ID_CCA:

      ++meas.ds_cntr.ds_ccdb_set_asm_id_succ;
      return APP_SM_CCAS;

    default:

      ++meas.ds_cntr.ds_ccdb_set_asm_id_app_id_invalid;
      FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: Invalid Diameter application ID %lu\n", me, p->diameter.app_id));

      ++meas.ds_cntr.ds_ccdb_set_asm_id_fail;
      return APP_SM_UNDEF;
  }
}

short set_asm_state (struct user_session *p, short asm_event)
{
  char me[] = "set_asm_state";

  struct ste *ste;

  XPRINT_ENTERED;

  if (! p)  /* NULL US */
  {
    ++meas.ds_cntr.ds_us_null;
    XPRINT_NULL ("US", p);

    ++meas.ds_cntr.ds_ccdb_set_asm_state_fail;
    return DSM_UNDEF;
  }

  if (! p->head)  /* NULL US list */
  {
    ++meas.ds_cntr.ds_us_list_null;
    XPRINT_NULL ("US list", p->head);

    ++meas.ds_cntr.ds_ccdb_set_asm_state_fail;
    return DSM_UNDEF;
  }

  /*
   * Determine next STE based on ASM ID, current state, & event
   */
  ste = (struct ste *) stt (p->call_context.asm_id, p->call_context.asm_state, asm_event, FALSE);
  FEP_XPRINT (debug_mask[DEBUG_DSM_2], ("%s: session_id %s, ASM ID %d (%s), state0 %d (%s), event %d (%s), action %d (%s), state1 %d (%s)\n", me, string_hex_print (p->diameter.session_id, p->diameter.session_id_len), p->call_context.asm_id, APP_SM_ITOA (p->call_context.asm_id), p->call_context.asm_state, STATE_CCAS_ITOA (p->call_context.asm_state), asm_event, EVENT_CCAS_ITOA (asm_event), ste->action, ACTION_CCAS_ITOA (ste->action), ste->state, STATE_CCAS_ITOA (ste->state)));

  ++meas.ds_cntr.ds_ccdb_set_asm_state_succ;
  return ste->state;
}

short set_flags_trap (struct user_session *p)
{
  char me[] = "set_flags_trap";

  XPRINT_ENTERED;

  if (! p)  /* NULL US */
  {
    ++meas.ds_cntr.ds_us_null;
    XPRINT_NULL ("US", p);

    return FALSE;
  }

  if (! p->head)  /* NULL US list */
  {
    ++meas.ds_cntr.ds_us_list_null;
    XPRINT_NULL ("US list", p->head);

    return FALSE;
  }

  if (ccdb.trap.time_start <= 0)
    return FALSE;

  if (strcmp (p->diameter.sub_id, ccdb.trap.sub_id) != 0)
    return FALSE;

  /*
   * Trap this US
   */
  ccdb.trap.us = p;

  ++meas.ds_cntr.ds_us_trap;
  return TRUE;
}

int check_asm_state (struct user_session *p, short asm_event)
{
  char me[] = "check_asm_state";

  struct ste *ste;

  XPRINT_ENTERED;

  if (! p)  /* NULL US */
  {
    ++meas.ds_cntr.ds_us_null;
    XPRINT_NULL ("US", p);

    ++meas.ds_cntr.ds_ccdb_check_asm_state_fail;
    return FAILURE;
  }

  ste = (struct ste *) stt (p->call_context.asm_id, p->call_context.asm_state, asm_event, FALSE);

  /*
   * Event can't occur in current ASM state
   */
  if (ste->action < 0)
  {
    ++meas.ds_cntr.ds_ccdb_check_asm_state_fail;
    return FAILURE;
  }

  /*
   * Event can occur in current ASM state
   */
  ++meas.ds_cntr.ds_ccdb_check_asm_state_succ;
  return SUCCESS;
}

char *string_inconsistent_tid (char *buffer, char *caller, char *tid_type, ulong tid_lhs, ulong tid_rhs)
{
  char me[] = "string_inconsistent_tid";

  char tid_lhs_str[64], tid_rhs_str[64];

  /*
   * Function bit_print () can't be called multiple times on the same line of
   * code because it returns a static string, causing unpredictable &
   * misleading results.  Similar functions behave the same way.
   */
  strcpy (tid_lhs_str, (char *) bit_print (tid_lhs, sizeof (tid_lhs)));
  strcpy (tid_rhs_str, (char *) bit_print (tid_rhs, sizeof (tid_rhs)));
  sprintf (buffer, "%s: LHS %s TID 0x%x (%s) != RHS %s TID 0x%x (%s)", caller, tid_type, tid_lhs, tid_lhs_str, tid_type, tid_rhs, tid_rhs_str);

  return buffer;
}

int qmsg_diamctx_construct (DiamCtx_t *pbuf_cc, struct user_session *p, int flag_repterr)
{
  char me[] = "qmsg_diamctx_construct";

  int i, j;
  ushort package_type_req;
  ulong tid_remote_req;

  XPRINT_ENTERED;

  if (! pbuf_cc)  /* NULL DiamCtx */
  {
    ++meas.ds_cntr.ds_dc_null;
    XPRINT_NULL ("DiamCtx", pbuf_cc);

    ++meas.ds_cntr.ds_ccdb_qmsg_diamctx_construct_fail;
    return FAILURE;
  }

  if (! p)  /* NULL US */
  {
    ++meas.ds_cntr.ds_us_null;
    XPRINT_NULL ("US", p);

    /*
     * Use REQUEST GDI package type & new local TID with non-US ID
     */
    pbuf_cc->packageType = PACKAGE_TYPE_REQUEST;
    pbuf_cc->localTid = set_tid_local (set_peer_fe (), ID_UNDEF);
    pbuf_cc->remoteTid = 0;

    ++meas.ds_cntr.ds_pkg_gdi_begin_xmit;
    ++meas.ds_cntr.ds_ccdb_qmsg_diamctx_construct_succ;
    return SUCCESS;
  }

  if (! p->head)  /* NULL US list */
  {
    ++meas.ds_cntr.ds_us_list_null;
    XPRINT_NULL ("US list", p->head);

    /*
     * Use REQUEST GDI package type & new local TID with non-US ID
     */
    pbuf_cc->packageType = PACKAGE_TYPE_REQUEST;
    pbuf_cc->localTid = set_tid_local (set_peer_fe (), ID_UNDEF);
    pbuf_cc->remoteTid = 0;

    ++meas.ds_cntr.ds_pkg_gdi_begin_xmit;
    ++meas.ds_cntr.ds_ccdb_qmsg_diamctx_construct_succ;
    return SUCCESS;
  }

  /*
   * Construct DiamCtx based on DiamCtx type
   */
  switch (pbuf_cc->diamctx_type)
  {
    case DIAMCTX_DIAM:

      /*
       * Pass data to DGC so it can construct a GDI message
       */

      /*
       * Set GDI package type, local TID, & remote TID in preparation to send
       * a GDI-DIM_ReportError to ISCP
       */
      if (flag_repterr)
      {
        if (p->gdi.tid_remote <= 0)  /* Remote TID not present */
        {
          /*
           * Use REQUEST GDI package type
           */
          pbuf_cc->packageType = p->gdi.package_type = PACKAGE_TYPE_REQUEST;

          /*
           * If there was no previously assigned local TID, use a new local TID
           * with non-US ID, else use previously assigned local TID
           */
          if (p->gdi.tid_local <= 0)
            pbuf_cc->localTid = p->gdi.tid_local = set_tid_local (set_peer_fe (), ID_UNDEF);
          else
            pbuf_cc->localTid = p->gdi.tid_local;

          pbuf_cc->remoteTid = p->gdi.tid_remote = 0;
        }
        else
        {
          /*
           * Remote TID present.  Use CONV GDI package type.
           */
          pbuf_cc->packageType = p->gdi.package_type = PACKAGE_TYPE_CONV;

          /*
           * If there was no previously assigned local TID, use a new local TID
           * with non-US ID, else use previously assigned local TID
           */
          if (p->gdi.tid_local <= 0)
            pbuf_cc->localTid = p->gdi.tid_local = set_tid_local (set_peer_fe (), ID_UNDEF);
          else
            pbuf_cc->localTid = p->gdi.tid_local;

          /*
           * Use previously saved remote TID
           */
          switch (pbuf_cc->commandCode)
          {
            case DIAM_CMD_ASA:
            case DIAM_CMD_RAA:

              if (pbuf_cc->commandCode == DIAM_CMD_ASA)
              {
                package_type_req = p->gdi.package_type_asr;
                tid_remote_req = p->gdi.tid_remote_asr;
              }
              else
              {
                package_type_req = p->gdi.package_type_rar;
                tid_remote_req = p->gdi.tid_remote_rar;
              }

              switch (package_type_req)
              {
                case PACKAGE_TYPE_REQUEST:

                  pbuf_cc->remoteTid = tid_remote_req;
                  break;

                case PACKAGE_TYPE_CONV:

                  pbuf_cc->remoteTid = p->gdi.tid_remote;
                  break;

                default:

                  ++meas.ds_cntr.ds_us_package_type_invalid;
                  XPRINT_INVALID_PKG_TYPE;

                  ++meas.ds_cntr.ds_ccdb_qmsg_diamctx_construct_fail;
                  return FAILURE;
              }

              break;

            default:

              pbuf_cc->remoteTid = p->gdi.tid_remote;
          }
        }

        break;
      }

      /*
       * Set GDI package type, local TID, & remote TID
       */
      pbuf_cc->packageType = p->gdi.package_type;
      pbuf_cc->localTid = p->gdi.tid_local;

      switch (pbuf_cc->commandCode)
      {
        case DIAM_CMD_ASA:
        case DIAM_CMD_RAA:

          if (pbuf_cc->commandCode == DIAM_CMD_ASA)
          {
            package_type_req = p->gdi.package_type_asr;
            tid_remote_req = p->gdi.tid_remote_asr;
          }
          else
          {
            package_type_req = p->gdi.package_type_rar;
            tid_remote_req = p->gdi.tid_remote_rar;
          }

          switch (package_type_req)
          {
            case PACKAGE_TYPE_REQUEST:

              pbuf_cc->remoteTid = tid_remote_req;
              break;

            case PACKAGE_TYPE_CONV:

              pbuf_cc->remoteTid = p->gdi.tid_remote;
              break;

            default:

              ++meas.ds_cntr.ds_us_package_type_invalid;
              XPRINT_INVALID_PKG_TYPE;

              ++meas.ds_cntr.ds_ccdb_qmsg_diamctx_construct_fail;
              return FAILURE;
          }

          break;

        default:

          pbuf_cc->remoteTid = p->gdi.tid_remote;
      }

      /*
       * Set origin data
       */
      pbuf_cc->originHost = p->diameter.remote_host;
      pbuf_cc->originRealm = p->diameter.remote_realm;
      pbuf_cc->originStateId = p->diameter.remote_state;

      break;

    case DIAMCTX_GDI:
    case DIAMCTX_GDI_INT_STR:
    case DIAMCTX_TG_EXP:

      /*
       * DGC does not need to send a Diameter message to the network.  DGC
       * just needs to free its DiamCtx resource, so no need to pass US data
       * to DGC
       */
      if (pbuf_cc->diamctx_flags.no_diam)
        break;

      /*
       * Pass US data to DGC so it can:
       *   - Construct a Diameter message (for DIAMCTX_GDI)
       *   - Construct a GDI message (for DIAMCTX_GDI_INT_STR)
       *   - Construct a Diameter & a GDI message (for DIAMCTX_TG_EXP)
       */

      /*
       * Set GDI package type, local TID, & remote TID in preparation to send
       * a GDI-DIM_ReportError to ISCP
       */
      if (flag_repterr)
      {
        if (p->gdi.tid_remote <= 0)  /* Remote TID not present */
        {
          /*
           * Use REQUEST GDI package type
           */
          pbuf_cc->packageType = p->gdi.package_type = PACKAGE_TYPE_REQUEST;

          /*
           * If there was no previously assigned local TID, use a new local TID
           * with non-US ID, else use previously assigned local TID
           */
          if (p->gdi.tid_local <= 0)
            pbuf_cc->localTid = p->gdi.tid_local = set_tid_local (set_peer_fe (), ID_UNDEF);
          else
            pbuf_cc->localTid = p->gdi.tid_local;

          pbuf_cc->remoteTid = p->gdi.tid_remote = 0;
        }
        else
        {
          /*
           * Remote TID present.  Use CONV GDI package type.
           */
          pbuf_cc->packageType = p->gdi.package_type = PACKAGE_TYPE_CONV;

          /*
           * If there was no previously assigned local TID, use a new local TID
           * with non-US ID, else use previously assigned local TID
           */
          if (p->gdi.tid_local <= 0)
            pbuf_cc->localTid = p->gdi.tid_local = set_tid_local (set_peer_fe (), ID_UNDEF);
          else
            pbuf_cc->localTid = p->gdi.tid_local;

          /*
           * Use previously saved remote TID
           */
          pbuf_cc->remoteTid = p->gdi.tid_remote;
        }

        if (pbuf_cc->diamctx_type != DIAMCTX_TG_EXP)
          break;
      }
      else
      {
        /*
         * Set GDI package type, local TID, & remote TID
         */
        pbuf_cc->packageType = p->gdi.package_type;
        pbuf_cc->localTid = p->gdi.tid_local;
        pbuf_cc->remoteTid = p->gdi.tid_remote;
      }

      /*
       * Pass Tcc or Tg expiration-related data to DGC
       */
      if (pbuf_cc->diamctx_type != DIAMCTX_GDI)
      {
        pbuf_cc->sessionIdOffset = FEP_BUF_DATA_OFF (pbuf_cc) = 0;
        pbuf_cc->sessionIdLength = FEP_BUF_DATA_LEN (pbuf_cc) = p->diameter.session_id_len;
        memcpy (FEP_BUF_SID, p->diameter.session_id, pbuf_cc->sessionIdLength);

        if (pbuf_cc->diamctx_type == DIAMCTX_TG_EXP)
	  SPRINTF_ES_EXP_TIMER ("Tg", p->call_context.tg_expiry);
        else
	  SPRINTF_ES_EXP_TIMER ("Tcc", p->call_context.tcc_expiry);
      }

      pbuf_cc->timer = (short) (p->call_context.tcc_expiry / MS_PER_SEC);

      pbuf_cc->version = p->diameter.version;
      pbuf_cc->commandCode = p->diameter.command_code;
      pbuf_cc->flags = p->diameter.command_flags;
      pbuf_cc->hopByHopId = p->diameter.hop_by_hop;
      pbuf_cc->endToEndId =  p->diameter.end_to_end;
      pbuf_cc->applId = p->diameter.app_id;
      pbuf_cc->authAppId = p->diameter.auth_app_id;
      pbuf_cc->ccRequestType = p->diameter.cc_request_type;
      pbuf_cc->ccRequestNumber = p->diameter.cc_request_number;

      pbuf_cc->proxy_count = p->diameter.proxy_count;
      for (j = i = 0; i < pbuf_cc->proxy_count; ++i)
      {
        pbuf_cc->proxy_host[i] = p->diameter.proxy_host[i];
        pbuf_cc->proxy_state_len[i] = p->diameter.proxy_state_len[i];
        j += pbuf_cc->proxy_state_len[i];
      }
      memcpy (pbuf_cc->proxy_states, p->diameter.proxy_states, j);

      pbuf_cc->subIdType = p->diameter.sub_id_type;
      strcpy (pbuf_cc->subId, p->diameter.sub_id);

      switch (pbuf_cc->diamctx_type)
      {
        case DIAMCTX_GDI:

          /*
           * Set destination data only for answer messages from ISCP (GDI-CCA
           * & GDI-STA); not for request messages from ISCP (GDI-ASR & GDI-RAR)
           */
          if (! (p->diameter.command_flags & DIAM_CMD_FLAG_R))
          {
            pbuf_cc->destHost = p->diameter.remote_host;
            pbuf_cc->destRealm = p->diameter.remote_realm;
            pbuf_cc->destStateId = p->diameter.remote_state;
          }

          break;

        case DIAMCTX_GDI_INT_STR:

          pbuf_cc->originHost = p->diameter.remote_host;
          pbuf_cc->originRealm = p->diameter.remote_realm;
          pbuf_cc->originStateId = p->diameter.remote_state;
          pbuf_cc->destHost = p->diameter.local_host;
          pbuf_cc->destRealm = p->diameter.local_realm;

          break;

        case DIAMCTX_TG_EXP:

          pbuf_cc->originHost = UNINITIALIZED;
          pbuf_cc->originRealm = UNINITIALIZED;
          pbuf_cc->destHost = p->diameter.remote_host;
          pbuf_cc->destRealm = p->diameter.remote_realm;
          pbuf_cc->destStateId = p->diameter.remote_state;

          break;
      }

      break;

    default:

      ++meas.ds_cntr.ds_dc_invalid;
      XPRINT_INVALID_DIAMCTX_TYPE;

      ++meas.ds_cntr.ds_ccdb_qmsg_diamctx_construct_fail;
      return FAILURE;
  }

  /*
   * Pass common data to DGC so it can construct a GDI or Diameter message
   */
  pbuf_cc->diamctx_flags.trap = p->call_context.flags.trap = set_flags_trap (p);
  pbuf_cc->cp = p->call_context.cp;
  pbuf_cc->npk_config = p->call_context.npk_config;

  pbuf_cc->origSysId = p->gdi.origSysId;
  pbuf_cc->receiverId = p->gdi.receiverId;
  pbuf_cc->invokeId = p->gdi.invokeId;

  /*
   * Increment counter
   */
  switch (pbuf_cc->packageType)
  {
    case PACKAGE_TYPE_REQUEST:

      ++meas.ds_cntr.ds_pkg_gdi_begin_xmit;
      break;

    case PACKAGE_TYPE_CONV:

      ++meas.ds_cntr.ds_pkg_gdi_continue_xmit;
      break;

    case PACKAGE_TYPE_RESPONSE:

      ++meas.ds_cntr.ds_pkg_gdi_end_xmit;
      break;

    case PACKAGE_TYPE_ABORT:

      ++meas.ds_cntr.ds_pkg_gdi_abort_xmit;
      break;

    default:

      ++meas.ds_cntr.ds_pkg_gdi_invalid_xmit;
  }

  ++meas.ds_cntr.ds_ccdb_qmsg_diamctx_construct_succ;
  return SUCCESS;
}

int user_session_hash (char *session_id, int session_id_len)
{
  char me[] = "user_session_hash";

  uint hash, hash2;

  XPRINT_ENTERED;

  if (! session_id)  /* NULL session ID */
  {
    ++meas.ds_cntr.ds_ccdb_us_hash_sid_null;
    XPRINT_NULL ("session ID", session_id);

    ++meas.ds_cntr.ds_ccdb_us_hash_fail;
    return DSM_UNDEF;
  }

  /*
   * Invalid session ID length
   */
  if ((session_id_len <= 0) || (session_id_len > (MAX_SZ_OCTETSTRING - 1)))
  {
    ++meas.ds_cntr.ds_ccdb_us_hash_sid_invalid;
    XPRINT_INVALID_SESS_ID_LEN;

    ++meas.ds_cntr.ds_ccdb_us_hash_fail;
    return DSM_UNDEF;
  }

  /*
   * Compute hash & fit in range [0,USER_SESSION_MASK]
   */
  hash = (*hash_func[HASH_RS]) (session_id, session_id_len);
  hash2 = hash & USER_SESSION_MASK;

  FEP_XPRINT (debug_mask[DEBUG_DSM_2], ("%s: session_id %s, length %d, hash %u, hash2 %u\n", me, string_hex_print (session_id, session_id_len), session_id_len, hash, hash2));

  ++meas.ds_cntr.ds_ccdb_us_hash_succ;
  return hash2;
}

struct user_session *user_session_get (char *session_id, short session_id_len, int ccdb_hash_index)
{
  char me[] = "user_session_get";

  struct user_session_tailq *p;
  struct user_session *q, *r;

  XPRINT_ENTERED;

  if (! session_id)  /* NULL session ID */
  {
    ++meas.ds_cntr.ds_ccdb_us_get_sid_null;
    XPRINT_NULL ("session ID", session_id);

    ++meas.ds_cntr.ds_ccdb_us_get_fail;
    return NULL;
  }

  /*
   * Invalid session ID length
   */
  if ((session_id_len <= 0) || (session_id_len > (MAX_SZ_OCTETSTRING - 1)))
  {
    ++meas.ds_cntr.ds_ccdb_us_get_sid_invalid;
    XPRINT_INVALID_SESS_ID_LEN;

    ++meas.ds_cntr.ds_ccdb_us_get_fail;
    return NULL;
  }

  /*
   * Invalid CCDB hash index
   */
  if ((ccdb_hash_index < 0) || (ccdb_hash_index > USER_SESSION_MASK))
  {
    ++meas.ds_cntr.ds_us_list_invalid;
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: Invalid CCDB hash index %d\n", me, ccdb_hash_index));

    ++meas.ds_cntr.ds_ccdb_us_get_fail;
    return NULL;
  }

  /*
   * Find US
   */
  p = &ccdb.user_session[ccdb_hash_index];
  for (q = p->tqh_first; q; q = q->tqe.tqe_next)
  {
    if (q->diameter.session_id_len != session_id_len)
      continue;

    /*
     * Found US
     */
    if (memcmp (q->diameter.session_id, session_id, session_id_len) == 0)
      break;
  }
  ++meas.ds_cntr.ds_ccdb_us_get_succ;

  if (! q)  /* US not found */
    return NULL;

  if (! q->head)  /* NULL US list */
  {
    ++meas.ds_cntr.ds_us_list_null;
    XPRINT_NULL ("US list", q->head);

    /*
     * Fix it & investigate how this happened
     */
    q->head = p;
  }

  return q;
}

/*
 * Save US data
 */
int user_session_save (DiamCtx_t *pbuf_cc, struct user_session *p)
{
  char me[] = "user_session_save";

  int rv, i, j;
  short asm_event, asm_state_prev;
  char buffer[512], *b;

  XPRINT_ENTERED;

  if (! pbuf_cc)  /* NULL DiamCtx */
  {
    ++meas.ds_cntr.ds_dc_null;
    XPRINT_NULL ("DiamCtx", pbuf_cc);

    ++meas.ds_cntr.ds_ccdb_us_save_fail;
    return FAILURE;
  }

  if (! p)  /* NULL US */
  {
    ++meas.ds_cntr.ds_us_null;
    SPRINTF_ES_NULL ("US", p);
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

    ++meas.ds_cntr.ds_ccdb_us_save_fail;
    return FAILURE;
  }

  if (! p->head)  /* NULL US list */
  {
    ++meas.ds_cntr.ds_us_list_null;
    SPRINTF_ES_NULL ("US list", p->head);
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

    ++meas.ds_cntr.ds_ccdb_us_save_fail;
    return FAILURE;
  }

  /*
   * Receipt of Diameter request message with 'T' bit set
   */
  if (pbuf_cc->flags & DIAM_CMD_FLAG_T)
    if (pbuf_cc->endToEndId == p->diameter.end_to_end)
      if (CALLOUT_ID_VALID (p->call_context.tg_id))
      {
        /*
         * Save Diameter AVPs
         */
        p->diameter.hop_by_hop = pbuf_cc->hopByHopId;

        /*
         * Save Diameter Proxy-Info AVP
         */
        p->diameter.proxy_count = pbuf_cc->proxy_count;
        for (j = i = 0; i < p->diameter.proxy_count; ++i)
        {
          p->diameter.proxy_host[i] = pbuf_cc->proxy_host[i];
          p->diameter.proxy_state_len[i] = pbuf_cc->proxy_state_len[i];
          j += p->diameter.proxy_state_len[i];
        }
        memcpy (p->diameter.proxy_states, pbuf_cc->proxy_states, j);

        /*
         * Set CC data
         */
        p->call_context.flags.trap = set_flags_trap (p);

        ++meas.ds_cntr.ds_ccdb_us_save_succ;
        return SUCCESS;
      }

  /*
   * Received a GDI-Abort message (i.e., GDI package type is ABORT) or a
   * GDI-Dim_Close message from the ISCP
   */
  if ((pbuf_cc->packageType == PACKAGE_TYPE_ABORT) || (pbuf_cc->diamctx_type == DIAMCTX_GDI_DIM_CLOSE))
  {
    /*
     * Save package type
     */
    p->gdi.package_type = pbuf_cc->packageType;

    /*
     * Clear local & remote TIDs
     */
    if (pbuf_cc->diamctx_type == DIAMCTX_GDI_DIM_CLOSE)
      p->gdi.tid_local = 0;
    p->gdi.tid_remote = 0;
    p->gdi.tid_remote_asr = 0;
    p->gdi.tid_remote_rar = 0;

    /*
     * Set CC data
     */
    p->call_context.flags.trap = set_flags_trap (p);

    ++meas.ds_cntr.ds_ccdb_us_save_succ;
    return SUCCESS;
  }

  /*
   * Validate Command-Code
   */
  switch (pbuf_cc->commandCode)
  {
    case DIAM_CMD_CCR:  /* or DIAM_CMD_CCA */
    case DIAM_CMD_ASA:  /* or DIAM_CMD_ASR */
    case DIAM_CMD_RAA:  /* or DIAM_CMD_RAR */
    case DIAM_CMD_STR:  /* or DIAM_CMD_STA */

      break;

    default:

      ++meas.ds_cntr.ds_dc_cc_invalid;
      SPRINTF_ES_INVALID_CMD;
      FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s (%s)\n", pbuf_cc->errorString, bit_print (pbuf_cc->flags, sizeof (pbuf_cc->flags))));

      ++meas.ds_cntr.ds_ccdb_us_save_fail;
      return FAILURE;
  }

  /*
   * Save or update US based on DiamCtx type
   */
  switch (pbuf_cc->diamctx_type)
  {
    case DIAMCTX_DIAM:

      /*
       * Configuration data not ready
       */
      if ((rv = conf_get_state ()) != CONF_STATE_READY)
      {
        ++meas.ds_cntr.ds_api_conf_get_state_fail;
        sprintf (pbuf_cc->errorString, "%s: Configuration data not ready, rv %d (%s)", me, rv, CONF_STATE_ITOA (rv));
        FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

        ++meas.ds_cntr.ds_ccdb_us_save_fail;
        return FAILURE;
      }

      /*
       * Newly created US.  Set following items only for Diam-CCR-I or
       * Diam-CCR-E messages.
       */
      if (pbuf_cc->commandCode == DIAM_CMD_CCR)
        if ((pbuf_cc->ccRequestType == DIAM_CC_INITIAL_REQUEST) || (pbuf_cc->ccRequestType == DIAM_CC_EVENT_REQUEST))
        {
          /*
           * Set CC creation time & version
           */
          p->call_context.time_start = time (NULL);
          p->call_context.version = CCDB_VERSION;

          /*
           * Save Diameter Session-Id AVP
           */
          memcpy (p->diameter.session_id, FEP_BUF_SID, pbuf_cc->sessionIdLength);
          p->diameter.session_id_len = pbuf_cc->sessionIdLength;

          /*
           * Save Application-ID before ASM ID, before ASM state
           */
          p->diameter.app_id = pbuf_cc->applId;

          /*
           * Failed to set ASM ID
           */
          if ((p->call_context.asm_id = set_asm_id (p)) < 0)
          {
            sprintf (pbuf_cc->errorString, "%s: set_asm_id failed, asm_id %d (%s)", me, p->call_context.asm_id, APP_SM_ITOA (p->call_context.asm_id));
            FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

            ++meas.ds_cntr.ds_ccdb_us_save_fail;
            return FAILURE;
          }

          /*
           * Failed to update ASM state
           */
          asm_event = DSM_UNDEF;
          asm_state_prev = p->call_context.asm_state;
          if ((p->call_context.asm_state = set_asm_state (p, DSM_UNDEF)) < 0)
          {
            XPRINT_FAILED_SET_ASM_STATE;

            /*
             * Despite unlikely failure, don't return; just continue.  US can
             * only be in STATE_CCAS_IDLE state when newly created.
             */
            p->call_context.asm_state = STATE_CCAS_IDLE;
          }

          /*
           * Should've been initialized in user_session_init already, but
           * re-initialize
           */
          p->gdi.tid_local = 0;
          p->gdi.tid_remote = 0;
          p->gdi.tid_remote_asr = 0;
          p->gdi.tid_remote_rar = 0;
          p->gdi.package_type_asr = 0;
          p->gdi.package_type_rar = 0;
        }

      /*
       * Newly created or existing US.  Set following items for all Diameter
       * messages.
       */

      /*
       * Save Diameter Command-Code, command flags, & peer FE before GDI
       * package type, before local TID
       */
      p->diameter.command_code = pbuf_cc->commandCode;
      p->diameter.command_flags = pbuf_cc->flags;

      if ((p->call_context.peer_fe = set_peer_fe ()) <= 0)
      {
        sprintf (pbuf_cc->errorString, "%s: set_peer_fe failed, peer_fe %d", me, p->call_context.peer_fe);
        FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

        ++meas.ds_cntr.ds_ccdb_us_save_fail;
        return FAILURE;
      }

      if ((p->gdi.package_type = set_package_type (p)) <= 0)
      {
        sprintf (pbuf_cc->errorString, "%s: set_package_type failed, package_type 0x%x (%s)", me, p->gdi.package_type, package_type_itoa (p->gdi.package_type));

        /*
         * Function bit_print () can't be called multiple times on the same
         * line of code because it returns a static string, causing
         * misleading results.  Similar functions (e.g., package_type_itoa)
         * behave the same way.
         */
        b = buffer;
        b += sprintf (b, "%s", pbuf_cc->errorString);
        b += sprintf (b, ", command_code %d (%s)", p->diameter.command_code, command_code_itoa (p->diameter.command_code, p->diameter.command_flags, p->diameter.cc_request_type));
        b += sprintf (b, ", command_flags 0x%x (%s)", p->diameter.command_flags, bit_print (p->diameter.command_flags, sizeof (p->diameter.command_flags)));
        b += sprintf (b, ", tid_local 0x%x (%s)", p->gdi.tid_local, bit_print (p->gdi.tid_local, sizeof (p->gdi.tid_local)));
        b += sprintf (b, ", tid_remote 0x%x (%s)", p->gdi.tid_remote, bit_print (p->gdi.tid_remote, sizeof (p->gdi.tid_remote)));
        b += sprintf (b, ", package_type_asr 0x%x (%s)", p->gdi.package_type_asr, package_type_itoa (p->gdi.package_type_asr));
        b += sprintf (b, ", package_type_rar 0x%x (%s)", p->gdi.package_type_rar, package_type_itoa (p->gdi.package_type_rar));

        FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", buffer));

        ++meas.ds_cntr.ds_ccdb_us_save_fail;
        return FAILURE;
      }

      /*
       * Set GDI local TID based on Command-Code
       */
      switch (p->diameter.command_code)
      {
        case DIAM_CMD_ASA:
        case DIAM_CMD_RAA:

          if (p->gdi.package_type == PACKAGE_TYPE_RESPONSE)
          {
            /*
             * No local TID should've been assigned
             */
            if (p->gdi.tid_local <= 0)
              break;

            /*
             * Attempt to fix by unassigning local TID
             */
            ++meas.ds_cntr.ds_us_tid_local_invalid;
            XPRINT_UNEXPECTED_TID_LOCAL;
            p->gdi.tid_local = 0;

            break;
          }

          if (p->gdi.package_type == PACKAGE_TYPE_CONV)
          {
            /*
             * Local TID should've been assigned
             */
            if (p->gdi.tid_local > 0)
              break;

            /*
             * Attempt to fix by assigning local TID later on
             */
            ++meas.ds_cntr.ds_us_tid_local_invalid;
            XPRINT_UNEXPECTED_TID_LOCAL;
          }

          /*
           * Else (e.g., GDI package type is REQUEST) fall through to
           * default case to assign local TID
           */

        default:

          if ((p->gdi.tid_local = set_tid_local (p->call_context.peer_fe, p->id)) <= 0)
          {
            sprintf (pbuf_cc->errorString, "%s: set_tid_local failed, tid_local 0x%x", me, p->gdi.tid_local);
            FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s (%s)\n", pbuf_cc->errorString, bit_print (p->gdi.tid_local, sizeof (p->gdi.tid_local))));

            ++meas.ds_cntr.ds_ccdb_us_save_fail;
            return FAILURE;
          }
      }

      /*
       * Save following items for all Diameter messages
       */

      /*
       * Set CC data
       */
      p->call_context.tg_expiry = ccdb.tg_expiry;
      p->call_context.flags.trap = set_flags_trap (p);

      /*
       * Save Diameter AVPs
       */
      p->diameter.version = pbuf_cc->version;
      p->diameter.app_id = pbuf_cc->applId;
      p->diameter.hop_by_hop = pbuf_cc->hopByHopId;
      p->diameter.end_to_end = pbuf_cc->endToEndId;
      p->diameter.cc_request_type = pbuf_cc->ccRequestType;
      p->diameter.cc_request_number = pbuf_cc->ccRequestNumber;
      p->diameter.auth_app_id = pbuf_cc->authAppId;
      p->diameter.local_host = pbuf_cc->destHost;
      p->diameter.local_realm = pbuf_cc->destRealm;
      p->diameter.local_state = conf_get_local_state_id ();
      p->diameter.remote_host = pbuf_cc->originHost;
      p->diameter.remote_realm = pbuf_cc->originRealm;
      p->diameter.remote_state = pbuf_cc->originStateId;
      p->diameter.sub_id_type = pbuf_cc->subIdType;
      strcpy (p->diameter.sub_id, pbuf_cc->subId);

      /*
       * Save Diameter Proxy-Info AVP
       */
      p->diameter.proxy_count = pbuf_cc->proxy_count;
      for (j = i = 0; i < p->diameter.proxy_count; ++i)
      {
        p->diameter.proxy_host[i] = pbuf_cc->proxy_host[i];
        p->diameter.proxy_state_len[i] = pbuf_cc->proxy_state_len[i];
        j += p->diameter.proxy_state_len[i];
      }
      memcpy (p->diameter.proxy_states, pbuf_cc->proxy_states, j);

      break;

    case DIAMCTX_GDI:

      /*
       * Update US based on Command-Code
       */
      switch (pbuf_cc->commandCode)
      {
        case DIAM_CMD_CCA:

	  /*
	   * Save package type
	   */
          p->gdi.package_type = pbuf_cc->packageType;

	  /*
	   * Update local & remote TIDs based on GDI package type
	   */
	  switch (p->gdi.package_type)
          {
            case PACKAGE_TYPE_CONV:

	      p->gdi.tid_remote = pbuf_cc->remoteTid;
	      break;

            case PACKAGE_TYPE_RESPONSE:

	      p->gdi.tid_local = 0;
	      p->gdi.tid_remote = 0;
	      p->gdi.tid_remote_asr = 0;
	      p->gdi.tid_remote_rar = 0;

	      break;
          }

          /*
           * Update CC Tcc data
           */
          p->call_context.tcc_expiry = pbuf_cc->timer * MS_PER_SEC;

          break;

        case DIAM_CMD_ASR:

	  /*
	   * Save package type & remote TID
	   */
          p->gdi.package_type_asr = pbuf_cc->packageType;
          switch (pbuf_cc->packageType)
          {
            case PACKAGE_TYPE_REQUEST:

              p->gdi.tid_remote_asr = pbuf_cc->remoteTid;
              break;

            case PACKAGE_TYPE_CONV:

              p->gdi.tid_remote_asr = 0;
              p->gdi.tid_remote = pbuf_cc->remoteTid;
              break;

            default:

              ++meas.ds_cntr.ds_dc_package_type_invalid;
              SPRINTF_ES_INVALID_PKG_TYPE;
              FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s, commandCode %d (%s)\n", pbuf_cc->errorString, pbuf_cc->commandCode, command_code_itoa (pbuf_cc->commandCode, pbuf_cc->flags, pbuf_cc->ccRequestType)));

              ++meas.ds_cntr.ds_ccdb_us_save_fail;
              return FAILURE;
          }

          break;

        case DIAM_CMD_RAR:

	  /*
	   * Save package type & remote TID
	   */
          p->gdi.package_type_rar = pbuf_cc->packageType;
          switch (pbuf_cc->packageType)
          {
            case PACKAGE_TYPE_REQUEST:

              p->gdi.tid_remote_rar = pbuf_cc->remoteTid;
              break;

            case PACKAGE_TYPE_CONV:

              p->gdi.tid_remote_rar = 0;
              p->gdi.tid_remote = pbuf_cc->remoteTid;
              break;

            default:

              ++meas.ds_cntr.ds_dc_package_type_invalid;
              SPRINTF_ES_INVALID_PKG_TYPE;
              FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s, commandCode %d (%s)\n", pbuf_cc->errorString, pbuf_cc->commandCode, command_code_itoa (pbuf_cc->commandCode, pbuf_cc->flags, pbuf_cc->ccRequestType)));

              ++meas.ds_cntr.ds_ccdb_us_save_fail;
              return FAILURE;
          }

          break;

        case DIAM_CMD_STA:

	  /*
	   * Save package type
	   */
          p->gdi.package_type = pbuf_cc->packageType;

	  /*
	   * Update local & remote TIDs
	   */
	  if (p->gdi.package_type == PACKAGE_TYPE_RESPONSE)
          {
	    p->gdi.tid_local = 0;
	    p->gdi.tid_remote = 0;
	    p->gdi.tid_remote_asr = 0;
	    p->gdi.tid_remote_rar = 0;
          }

          break;

        default:

          /*
           * Shouldn't reach here because Command-Code was validated already
           */
          ++meas.ds_cntr.ds_dc_cc_invalid;
          SPRINTF_ES_INVALID_CMD;
          FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s (%s)\n", pbuf_cc->errorString, bit_print (pbuf_cc->flags, sizeof (pbuf_cc->flags))));

          ++meas.ds_cntr.ds_ccdb_us_save_fail;
          return FAILURE;
      }

      /*
       * Save following items for all GDI messages
       */

      /*
       * Set CC data
       */
      p->call_context.flags.trap = set_flags_trap (p);

      /*
       * Save Diameter AVPs
       */
      p->diameter.command_code = pbuf_cc->commandCode;
      p->diameter.command_flags = pbuf_cc->flags;
      p->diameter.cc_request_type = pbuf_cc->ccRequestType;
      p->diameter.local_host = pbuf_cc->originHost;
      p->diameter.local_realm = pbuf_cc->originRealm;
      p->diameter.local_state = conf_get_local_state_id ();

      /*
       * Save GDI data
       */
      p->gdi.origSysId = pbuf_cc->origSysId;
      p->gdi.receiverId = pbuf_cc->receiverId;
      p->gdi.invokeId = pbuf_cc->invokeId;

      break;

    default:

      ++meas.ds_cntr.ds_dc_invalid;
      sprintf (pbuf_cc->errorString, "%s: Unexpected or invalid DiamCtx type %d (%s)\n", me, pbuf_cc->diamctx_type, DIAMCTX_ITOA (pbuf_cc->diamctx_type));
      FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

      ++meas.ds_cntr.ds_ccdb_us_save_fail;
      return FAILURE;
  }

  /*
   * Remember to update ASM state & CC stats in calling function, preferably
   * immediately after return
   */
  ++meas.ds_cntr.ds_ccdb_us_save_succ;
  return SUCCESS;
}

struct user_session *user_session_add (DiamCtx_t *pbuf_cc, int ccdb_hash_index)
{
  char me[] = "user_session_add";

  struct user_session *new;

  XPRINT_ENTERED;

  if (! pbuf_cc)  /* NULL DiamCtx */
  {
    ++meas.ds_cntr.ds_dc_null;
    XPRINT_NULL ("DiamCtx", pbuf_cc);

    ++meas.ds_cntr.ds_ccdb_us_add_fail;
    return NULL;
  }

  /*
   * Invalid CCDB hash index
   */
  if ((ccdb_hash_index < 0) || (ccdb_hash_index > USER_SESSION_MASK))
  {
    ++meas.ds_cntr.ds_us_list_invalid;
    sprintf (pbuf_cc->errorString, "%s: Invalid CCDB hash index %d", me, ccdb_hash_index);
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

    ++meas.ds_cntr.ds_ccdb_us_add_fail;
    return NULL;
  }

  /*
   * Allocate new US memory
   */
  if (! (new = (struct user_session *) new_private (&fepmem_memory_user_session)))
  {
    ++meas.ds_cntr.ds_api_new_private_fail_us;
    SPRINTF_ES_FAILED ("new_private");
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));
    fepmem_memory_print (&fepmem_memory_user_session, debug_mask[DEBUG_DSM_2], me);

    ++meas.ds_cntr.ds_ccdb_us_add_fail;
    return NULL;
  }
  ++meas.ds_cntr.ds_api_new_private_succ_us;
  new->head = &ccdb.user_session[ccdb_hash_index];

  /*
   * Save US data
   */
  if (user_session_save (pbuf_cc, new) != SUCCESS)
  {
    XPRINT_FAILED ("user_session_save");

    user_session_init (new);
    delete_private (&fepmem_memory_user_session, new);
    ++meas.ds_cntr.ds_api_delete_private_us;

    ++meas.ds_cntr.ds_ccdb_us_add_fail;
    return NULL;
  }

  /*
   * Insert new US to end of time-sorted list
   */
  TAILQ_INSERT_TAIL (&ccdb.user_session_time, new, tqe_time);
  ++meas.ds_cntr.ds_us_tailq_insert_tail_time;

  /*
   * Insert new US to end of list
   */
  TAILQ_INSERT_TAIL (new->head, new, tqe);
  ++meas.ds_cntr.ds_us_tailq_insert_tail;

  ++meas.ds_cntr.ds_ccdb_us_add_succ;
  return new;
}

void process_expire_tg (void *arg)
{
  char me[] = "process_expire_tg";

  fep_buffer_t *pbuf;
  DiamCtx_t *pbuf_cc;
  struct user_session *p;
  short asm_event, asm_state_prev;
  int parm, rv, flag_remove_us;

  XPRINT_ENTERED;

  p = (struct user_session *) arg;
  if (! p)  /* NULL US */
  {
    ++meas.ds_cntr.ds_us_null;
    XPRINT_NULL ("US", p);

    ++meas.ds_cntr.ds_ccdb_process_expire_tg_fail;
    return;
  }
  user_session_print (p, debug_mask[DEBUG_MSG_EXPTG_ENTER], me);

  if (! p->head)  /* NULL US list */
  {
    ++meas.ds_cntr.ds_us_list_null;
    XPRINT_NULL ("US list", p->head);

    ++meas.ds_cntr.ds_ccdb_process_expire_tg_fail;
    return;
  }

  /*
   * Reset Tg expiry ID
   */
  p->call_context.tg_id = ID_UNDEF;

  /*
   * Event can't occur in current ASM state
   */
  asm_event = EVENT_CCAS_EXP_TG;
  if (check_asm_state (p, asm_event) != SUCCESS)
  {
    XPRINT_INVALID_STATE_CCAS;
    user_session_print (p, debug_mask[DEBUG_MSG_EXPTG_EXIT], me);

    ++meas.ds_cntr.ds_ccdb_process_expire_tg_fail;
    return;
  }

  /*
   * Using current ASM state, determine whether US needs to be removed &
   * if necessary, tell DGC about expiration & set Tg timer
   */
  switch (p->call_context.asm_state)
  {
    case STATE_CCAS_DISCONTINUING:
    case STATE_CCAS_REPORTING_ERROR:

      flag_remove_us = TRUE;
      break;

    default:

      flag_remove_us = FALSE;

      /*
       * Failed to allocate buffer
       */
      if (! (pbuf = fep_alloc_buffer (sizeof (p->diameter.session_id))))
      {
	++meas.ds_cntr.ds_api_fep_alloc_buffer_fail;
        XPRINT_FAILED_FEP_ALLOC_BUFFER (sizeof (p->diameter.session_id));
      }
      else
      {
	++meas.ds_cntr.ds_api_fep_alloc_buffer_succ;

        /*
         * Don't initialize data field because it contains persistent data
         */
        memset (pbuf, 0, (sizeof (*pbuf) - sizeof (FEP_BUF_DATA (pbuf))));

        pbuf_cc = (DiamCtx_t *) pbuf;
        pbuf_cc->diamctx_type = DIAMCTX_TG_EXP;

        /*
         * Failed to construct DGC DiamCtx qmsg
         */
        if (qmsg_diamctx_construct (pbuf_cc, p, TRUE) != SUCCESS)
        {
          XPRINT_FAILED ("qmsg_diamctx_construct");

          /*
           * Free memory associated with DiamCtx
           */
          fep_free_buffer (pbuf);
          ++meas.ds_cntr.ds_api_fep_free_buffer;

          user_session_print (p, debug_mask[DEBUG_MSG_EXPTG_EXIT], me);

          ++meas.ds_cntr.ds_ccdb_process_expire_tg_fail;
          return;
        }
        diamctx_print (pbuf_cc, debug_mask[DEBUG_MSG_EXPTG_EXIT], me);
        XPRINT_DIAMCTX_XMIT;
      }

      /*
       * Failed to convert (& release) buffer to parameter
       */
      if ((parm = buf_to_parm (pbuf)) < 0)
      {
        ++meas.ds_cntr.ds_api_buf_to_parm_fail;
        XPRINT_FAILED_BUF_TO_PARM;

        /*
         * Free memory associated with DiamCtx
         */
        fep_free_buffer (pbuf);
        ++meas.ds_cntr.ds_api_fep_free_buffer;

	user_session_print (p, debug_mask[DEBUG_MSG_EXPTG_EXIT], me);

        ++meas.ds_cntr.ds_ccdb_process_expire_tg_fail;
        return;
      }

      /*
       * Tell DGC about expiration
       */
      if ((rv = tell_dgc (MSG_DIAMCTX, parm, UNINITIALIZED, DIAMETER_TOO_BUSY)) != SUCCESS)
      {
	++meas.ds_cntr.ds_api_tell_dgc_fail;
        XPRINT_FAILED_TELL ("tell_dgc");

        /*
         * Free memory associated with DiamCtx
         */
        fep_free_buffer (parm_to_buf (parm));
        ++meas.ds_cntr.ds_api_fep_free_buffer;

        ++meas.ds_cntr.ds_ccdb_process_expire_tg_fail;
        return;
      }

      ++meas.ds_cntr.ds_api_tell_dgc_succ;
      XPRINT_QMSG_XMIT (MSG_DIAMCTX);

      ++meas.ds_cntr.ds_msg_tg_expire_xmit;
      ++meas.ds_cntr.ds_ccdb_process_expire_tg_succ;

      /*
       * Failed to set Tg timer
       */
      p->call_context.tg_id = callout_set (process_expire_tg, p, (p->call_context.tg_expiry = ccdb.tg_expiry));
      if (! CALLOUT_ID_VALID (p->call_context.tg_id))
      {
	XPRINT_FAILED_CALLOUT_SET ("tg_expiry", p->call_context.tg_expiry);

        /*
         * Despite unlikely failure, don't return; just continue
         */
      }

      break;
  }

  /*
   * Failed to update ASM state
   */
  asm_state_prev = p->call_context.asm_state;
  if ((p->call_context.asm_state = set_asm_state (p, asm_event)) < 0)
  {
    XPRINT_FAILED_SET_ASM_STATE;

    /*
     * Despite unlikely failure, don't return; just continue.  If US is not
     * removed, it can only be in STATE_CCAS_REPORTING_ERROR state.
     */
    if (! flag_remove_us)
      p->call_context.asm_state = STATE_CCAS_REPORTING_ERROR;
  }

  /*
   * Update CC stats
   */
  ++p->call_context.ref_count;
  p->call_context.time_last = time (NULL);

  user_session_print (p, debug_mask[DEBUG_MSG_EXPTG_EXIT], me);

  /*
   * Remove US
   */
  if (flag_remove_us)
  {
    XPRINT_US_REMOVE;

    if (p = user_session_remove (&p))
    {
      XPRINT_FAILED_US_REMOVE;
    }
  }
}

void process_expire_tcc (void *arg)
{
  char me[] = "process_expire_tcc";

  fep_buffer_t *pbuf;
  DiamCtx_t *pbuf_cc;
  struct user_session *p;
  short asm_event, asm_state_prev;
  int parm, rv;

  XPRINT_ENTERED;

  p = (struct user_session *) arg;
  if (! p)  /* NULL US */
  {
    ++meas.ds_cntr.ds_us_null;
    XPRINT_NULL ("US", p);

    ++meas.ds_cntr.ds_ccdb_process_expire_tcc_fail;
    return;
  }
  user_session_print (p, debug_mask[DEBUG_MSG_EXPTCC_ENTER], me);

  if (! p->head)  /* NULL US list */
  {
    ++meas.ds_cntr.ds_us_list_null;
    XPRINT_NULL ("US list", p->head);

    ++meas.ds_cntr.ds_ccdb_process_expire_tcc_fail;
    return;
  }

  /*
   * Reset Tcc expiry ID
   */
  p->call_context.tcc_id = ID_UNDEF;

  /*
   * Event can't occur in current ASM state
   */
  asm_event = EVENT_CCAS_EXP_TCC;
  if (check_asm_state (p, asm_event) != SUCCESS)
  {
    XPRINT_INVALID_STATE_CCAS;
    user_session_print (p, debug_mask[DEBUG_MSG_EXPTCC_EXIT], me);

    ++meas.ds_cntr.ds_ccdb_process_expire_tcc_fail;
    return;
  }

  /*
   * Failed to allocate buffer
   */
  if (! (pbuf = fep_alloc_buffer (sizeof (p->diameter.session_id))))
  {
    ++meas.ds_cntr.ds_api_fep_alloc_buffer_fail;
    XPRINT_FAILED_FEP_ALLOC_BUFFER (sizeof (p->diameter.session_id));
  }
  else
  {
    ++meas.ds_cntr.ds_api_fep_alloc_buffer_succ;

    /*
     * Don't initialize data field because it contains persistent data
     */
    memset (pbuf, 0, (sizeof (*pbuf) - sizeof (FEP_BUF_DATA (pbuf))));

    /*
     * Construct an internal, FE-generated (i.e., not from network) GDI-STR
     * message to send to ISCP
     */
    pbuf_cc = (DiamCtx_t *) pbuf;
    pbuf_cc->diamctx_type = DIAMCTX_GDI_INT_STR;

    /*
     * Failed to construct DGC DiamCtx qmsg
     */
    if (qmsg_diamctx_construct (pbuf_cc, p, FALSE) != SUCCESS)
    {
      XPRINT_FAILED ("qmsg_diamctx_construct");

      /*
       * Free memory associated with DiamCtx
       */
      fep_free_buffer (pbuf);
      ++meas.ds_cntr.ds_api_fep_free_buffer;

      user_session_print (p, debug_mask[DEBUG_MSG_EXPTCC_EXIT], me);

      ++meas.ds_cntr.ds_ccdb_process_expire_tcc_fail;
      return;
    }
    diamctx_print (pbuf_cc, debug_mask[DEBUG_MSG_EXPTCC_EXIT], me);
    XPRINT_DIAMCTX_XMIT;
  }

  /*
   * Failed to convert (& release) buffer to parameter
   */
  if ((parm = buf_to_parm (pbuf)) < 0)
  {
    ++meas.ds_cntr.ds_api_buf_to_parm_fail;
    XPRINT_FAILED_BUF_TO_PARM;

    /*
     * Free memory associated with DiamCtx
     */
    fep_free_buffer (pbuf);
    ++meas.ds_cntr.ds_api_fep_free_buffer;

    user_session_print (p, debug_mask[DEBUG_MSG_EXPTCC_EXIT], me);

    ++meas.ds_cntr.ds_ccdb_process_expire_tcc_fail;
    return;
  }

  /*
   * Tell DGC about expiration
   */
  if ((rv = tell_dgc (MSG_DIAMCTX, parm, UNINITIALIZED, 0)) != SUCCESS)
  {
    ++meas.ds_cntr.ds_api_tell_dgc_fail;
    XPRINT_FAILED_TELL ("tell_dgc");

    /*
     * Free memory associated with DiamCtx
     */
    fep_free_buffer (parm_to_buf (parm));
    ++meas.ds_cntr.ds_api_fep_free_buffer;

    ++meas.ds_cntr.ds_ccdb_process_expire_tcc_fail;
    return;
  }

  ++meas.ds_cntr.ds_api_tell_dgc_succ;
  XPRINT_QMSG_XMIT (MSG_DIAMCTX);

  ++meas.ds_cntr.ds_msg_gdi_str_internal_xmit;
  ++meas.ds_cntr.ds_ccdb_process_expire_tcc_succ;

  /*
   * Failed to set Tg timer
   */
  p->call_context.tg_id = callout_set (process_expire_tg, p, (p->call_context.tg_expiry = ccdb.tg_expiry));
  if (! CALLOUT_ID_VALID (p->call_context.tg_id))
  {
    XPRINT_FAILED_CALLOUT_SET ("tg_expiry", p->call_context.tg_expiry);

    /*
     * Despite unlikely failure, don't return; just continue
     */
  }

  /*
   * Failed to update ASM state
   */
  asm_state_prev = p->call_context.asm_state;
  if ((p->call_context.asm_state = set_asm_state (p, asm_event)) < 0)
  {
    XPRINT_FAILED_SET_ASM_STATE;

    /*
     * Despite unlikely failure, don't return; just continue.  US can only be
     * in STATE_CCAS_DISCONTINUING state after EVENT_CCAS_EXP_TCC event.
     */
    p->call_context.asm_state = STATE_CCAS_DISCONTINUING;
  }

  /*
   * Update CC stats
   */
  ++p->call_context.ref_count;
  p->call_context.time_last = time (NULL);

  user_session_print (p, debug_mask[DEBUG_MSG_EXPTCC_EXIT], me);
}

/*
 * Process TDG's "update CP" message
 */
void process_update_cp (fep_qmsg_t qmsg)
{
  char me[] = "process_update_cp";

  struct user_session *p;
  ulong id, tid_local;
  short cp, npk_config;
  short asm_event, asm_state_prev;
  char buffer[256];

  XPRINT_ENTERED;

  tid_local = qmsg[1];
  cp = (short) qmsg[2];
  npk_config = (short) qmsg[3];

  /*
   * Consider TDG's "update CP" qmsg as a "DiamCtx" qmsg for debugging purposes
   */
  FEP_XPRINT (debug_mask[DEBUG_DIAMCTX_RECV], ("%s: Local TID 0x%x, CP %d, N+K config %d\n", me, tid_local, cp, npk_config));
  FEP_XPRINT (debug_mask[DEBUG_MSG_UPCP_ENTER], ("%s: %s, Local TID 0x%x, CP %d, N+K config %d\n", me, debug_msg_itoa (debug_mask[DEBUG_MSG_UPCP_ENTER]), tid_local, cp, npk_config));

  /*
   * Failed to determine US ID from GDI local TID
   */
  id = (tid_local & MASK_TID_CC) >> SHIFT_TID_CC;
  if (! USER_SESSION_ID_VALID (id))
  {
    ++meas.ds_cntr.ds_dc_tid_local_invalid;
    XPRINT_INVALID_US_ID (tid_local);

    ++meas.ds_cntr.ds_ccdb_process_update_cp_fail;
    return;
  }
  p = &fepmem_list_user_session[ID_TO_INDEX (id)];
  user_session_print (p, debug_mask[DEBUG_MSG_UPCP_ENTER], me);

  if (! p->head)  /* NULL US list */
  {
    ++meas.ds_cntr.ds_us_list_null;
    XPRINT_NULL ("US list", p->head);

    ++meas.ds_cntr.ds_ccdb_process_update_cp_fail;
    return;
  }

  /*
   * Inconsistent local TIDs
   */
  if (p->gdi.tid_local != tid_local)
  {
    ++meas.ds_cntr.ds_us_tid_local_invalid;
    string_inconsistent_tid (buffer, me, "local", p->gdi.tid_local, tid_local);
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", buffer));

    ++meas.ds_cntr.ds_ccdb_process_update_cp_fail;
    return;
  }

  /*
   * Event can't occur in current ASM state
   */
  asm_event = EVENT_CCAS_RX_QMSG_UP_CP;
  if (check_asm_state (p, asm_event) != SUCCESS)
  {
    XPRINT_INVALID_STATE_CCAS;

    ++meas.ds_cntr.ds_ccdb_process_update_cp_fail;
    return;
  }

  /*
   * Valid US, state, & event.  This is an "update CP" message from TDG.
   */

  /*
   * Update CC data
   */
  p->call_context.cp = cp;
  p->call_context.npk_config = npk_config;

  /*
   * Failed to update ASM state
   */
  asm_state_prev = p->call_context.asm_state;
  if ((p->call_context.asm_state = set_asm_state (p, asm_event)) < 0)
  {
    XPRINT_FAILED_SET_ASM_STATE;

    /*
     * Despite unlikely failure, don't return; just continue.  US can only be
     * in the same state (STATE_CCAS_PROC_REQ or STATE_CCAS_PROC_REQ_FINAL)
     * after EVENT_CCAS_RX_QMSG_UP_CP event.
     */
    p->call_context.asm_state = asm_state_prev;
  }

  /*
   * Update CC stats
   */
  ++p->call_context.ref_count;
  p->call_context.time_last = time (NULL);

  user_session_print (p, debug_mask[DEBUG_MSG_UPCP_EXIT], me);
  ++meas.ds_cntr.ds_ccdb_process_update_cp_succ;
}

/*
 * Process Diameter message
 */
int process_diam (fep_qmsg_t qmsg, int flag_queued, DiamCtx_t *pbuf_cc, struct user_session **p2)
{
  char me[] = "process_diam";

  struct user_session *p;
  int ccdb_hash_index;
  short asm_event, asm_state_prev;

  XPRINT_ENTERED;

  if (! pbuf_cc)  /* NULL DiamCtx */
  {
    ++meas.ds_cntr.ds_dc_null;
    XPRINT_NULL ("DiamCtx", pbuf_cc);

    ++meas.ds_cntr.ds_ccdb_process_diam_fail;
    return DIAMETER_UNABLE_TO_COMPLY;
  }

  if (! p2)  /* NULL US pointer */
  {
    ++meas.ds_cntr.ds_us_ptr_null;
    SPRINTF_ES_NULL ("US pointer", p2);
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

    ++meas.ds_cntr.ds_ccdb_process_diam_fail;
    return DIAMETER_UNABLE_TO_COMPLY;
  }

  diamctx_print (pbuf_cc, debug_mask[DEBUG_MSG_DIAM_ENTER], me);

  /*
   * Failed to determine CCDB hash index for US given its session ID
   */
  if ((ccdb_hash_index = user_session_hash (FEP_BUF_SID, pbuf_cc->sessionIdLength)) < 0)
  {
    SPRINTF_ES_FAILED ("user_session_hash");
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s, sessionId %s, length %d\n", pbuf_cc->errorString, string_hex_print (FEP_BUF_SID, pbuf_cc->sessionIdLength), pbuf_cc->sessionIdLength));

    ++meas.ds_cntr.ds_ccdb_process_diam_fail;
    return DIAMETER_UNABLE_TO_COMPLY;
  }

  /*
   * Get US (which may rightly not exist)
   */
  if (p = *p2 = user_session_get (FEP_BUF_SID, pbuf_cc->sessionIdLength, ccdb_hash_index))
  {
    user_session_print (p, debug_mask[DEBUG_MSG_DIAM_ENTER], me);

    if (! p->head)  /* NULL US list */
    {
      ++meas.ds_cntr.ds_us_list_null;
      SPRINTF_ES_NULL ("US list", p->head);
      FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

      ++meas.ds_cntr.ds_ccdb_process_diam_fail;
      return DIAMETER_UNABLE_TO_COMPLY;
    }
  }

  /*
   * Process message based on Command-Code
   */
  switch (pbuf_cc->commandCode)
  {
    case DIAM_CMD_CCR:

      /*
       * Receipt of Diam-CCA unsupported
       */
      if (! (pbuf_cc->flags & DIAM_CMD_FLAG_R))
      {
        if (flag_queued)
          ++meas.ds_cntr.ds_msg_diam_cca_dq;
        else
          ++meas.ds_cntr.ds_msg_diam_cca_rcvd;

	SPRINTF_ES_UNSUPPORTED_CMD;
	XPRINT_ES_UNSUPPORTED_CMD;

        ++meas.ds_cntr.ds_ccdb_process_diam_fail;
	return DIAMETER_COMMAND_UNSUPPORTED;
      }

      if (flag_queued)
        ++meas.ds_cntr.ds_msg_diam_ccr_dq;
      else
        ++meas.ds_cntr.ds_msg_diam_ccr_rcvd;

      /*
       * Receipt of Diam-CCR with 'E' bit set unsupported
       */
      if (pbuf_cc->flags & DIAM_CMD_FLAG_E)
      {
	SPRINTF_ES_UNSUPPORTED_CMD;
	XPRINT_ES_UNSUPPORTED_CMD;

        ++meas.ds_cntr.ds_ccdb_process_diam_fail;
	return DIAMETER_COMMAND_UNSUPPORTED;
      }

      /*
       * Received Diam-CCR message.  Process message based on request type.
       */
      switch (pbuf_cc->ccRequestType)
      {
        case DIAM_CC_INITIAL_REQUEST:
        case DIAM_CC_EVENT_REQUEST:

          if (flag_queued)
          {
            if (pbuf_cc->ccRequestType == DIAM_CC_INITIAL_REQUEST)
              ++meas.ds_cntr.ds_msg_diam_ccr_initial_dq;
            else
              ++meas.ds_cntr.ds_msg_diam_ccr_event_dq;
          }
	  else
          {
            if (pbuf_cc->ccRequestType == DIAM_CC_INITIAL_REQUEST)
              ++meas.ds_cntr.ds_msg_diam_ccr_initial_rcvd;
            else
              ++meas.ds_cntr.ds_msg_diam_ccr_event_rcvd;
          }

          if (p)  /* US exists */
          {
            /*
             * Receipt of Diam-CCR Initial (Diam-CCR-I) or Event (Diam-CCR-E)
             * with 'T' bit set
             */
            if (pbuf_cc->flags & DIAM_CMD_FLAG_T)
              if (pbuf_cc->endToEndId == p->diameter.end_to_end)
                if (CALLOUT_ID_VALID (p->call_context.tg_id))
                {
                  /*
                   * Event can't occur in current ASM state
                   */
                  asm_event = ((pbuf_cc->ccRequestType == DIAM_CC_INITIAL_REQUEST) ? (EVENT_CCAS_RX_DIAM_CCR_INIT_T) : (EVENT_CCAS_RX_DIAM_CCR_EVENT_T));
                  if (check_asm_state (p, asm_event) != SUCCESS)
                  {
                    SPRINTF_ES_INVALID_STATE_CCAS;
                    XPRINT_ES_INVALID_STATE_CCAS;

                    ++meas.ds_cntr.ds_ccdb_process_diam_fail;
                    return DIAMETER_UNABLE_TO_COMPLY;
                  }

                  /*
                   * Awaiting GDI-CCA-I or GDI-CCA-E message from the ISCP.  No
                   * need to forward this Diam-CCR-I or Diam-CCR-E message to
                   * the ISCP again.
                   */
                  pbuf_cc->diamctx_flags.no_gdi = TRUE;

                  /*
                   * Failed to update US data
                   */
                  if (user_session_save (pbuf_cc, p) != SUCCESS)
                  {
                    XPRINT_FAILED ("user_session_save");

                    ++meas.ds_cntr.ds_ccdb_process_diam_fail;
                    return DIAMETER_UNABLE_TO_COMPLY;
                  }

                  break;
                }

            ++meas.ds_cntr.ds_us_invalid;
	    sprintf (pbuf_cc->errorString, "%s: US exists, commandCode %d (%s)", me, pbuf_cc->commandCode, command_code_itoa (pbuf_cc->commandCode, pbuf_cc->flags, pbuf_cc->ccRequestType, pbuf_cc->ccRequestType));
	    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s, asm_id %d (%s), asm_state %d (%s)\n", pbuf_cc->errorString, p->call_context.asm_id, APP_SM_ITOA (p->call_context.asm_id), p->call_context.asm_state, STATE_CCAS_ITOA (p->call_context.asm_state)));

            ++meas.ds_cntr.ds_ccdb_process_diam_fail;
	    return DIAMETER_UNABLE_TO_COMPLY;
          }

          /*
           * US does not exist.  This is a Diam-CCR Initial (Diam-CCR-I) or
	   * Event (Diam-CCR-E) message arriving from the network.
           */

          /*
           * Set ASM event
           */
          asm_event = ((pbuf_cc->ccRequestType == DIAM_CC_INITIAL_REQUEST) ? (EVENT_CCAS_RX_DIAM_CCR_INIT) : (EVENT_CCAS_RX_DIAM_CCR_EVENT));

          /*
           * Failed to add new US
           */
          if (! (p = *p2 = user_session_add (pbuf_cc, ccdb_hash_index)))
          {
	    XPRINT_FAILED ("user_session_add");

            ++meas.ds_cntr.ds_ccdb_process_diam_fail;
	    return DIAMETER_UNABLE_TO_COMPLY;
          }

	  break;

        case DIAM_CC_UPDATE_REQUEST:
        case DIAM_CC_TERMINATION_REQUEST:

          if (flag_queued)
          {
            if (pbuf_cc->ccRequestType == DIAM_CC_UPDATE_REQUEST)
              ++meas.ds_cntr.ds_msg_diam_ccr_update_dq;
            else
              ++meas.ds_cntr.ds_msg_diam_ccr_termination_dq;
          }
          else
          {
            if (pbuf_cc->ccRequestType == DIAM_CC_UPDATE_REQUEST)
              ++meas.ds_cntr.ds_msg_diam_ccr_update_rcvd;
            else
              ++meas.ds_cntr.ds_msg_diam_ccr_termination_rcvd;
          }

          if (! p)  /* US does not exist */
          {
            ++meas.ds_cntr.ds_us_invalid;
	    SPRINTF_ES_NO_US;
	    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

            ++meas.ds_cntr.ds_ccdb_process_diam_fail;
	    return DIAMETER_UNKNOWN_SESSION_ID;
          }

          /*
           * Receipt of Diam-CCR Update (Diam-CCR-U) or Termination
           * (Diam-CCR-T) with 'T' bit set
           */
          if (pbuf_cc->flags & DIAM_CMD_FLAG_T)
            if (pbuf_cc->endToEndId == p->diameter.end_to_end)
              if (CALLOUT_ID_VALID (p->call_context.tg_id))
              {
                /*
                 * Event can't occur in current ASM state
                 */
                asm_event = ((pbuf_cc->ccRequestType == DIAM_CC_UPDATE_REQUEST) ? (EVENT_CCAS_RX_DIAM_CCR_UPDATE_T) : (EVENT_CCAS_RX_DIAM_CCR_TERM_T));
                if (check_asm_state (p, asm_event) != SUCCESS)
                {
                  SPRINTF_ES_INVALID_STATE_CCAS;
                  XPRINT_ES_INVALID_STATE_CCAS;

                  ++meas.ds_cntr.ds_ccdb_process_diam_fail;
                  return DIAMETER_UNABLE_TO_COMPLY;
                }

                /*
                 * Awaiting GDI-CCA-U or GDI-CCA-T message from the ISCP.  No
                 * need to forward this Diam-CCR-U or Diam-CCR-T message to
                 * the ISCP again.
                 */
                pbuf_cc->diamctx_flags.no_gdi = TRUE;

                /*
                 * Failed to update US data
                 */
                if (user_session_save (pbuf_cc, p) != SUCCESS)
                {
                  XPRINT_FAILED ("user_session_save");

                  ++meas.ds_cntr.ds_ccdb_process_diam_fail;
                  return DIAMETER_UNABLE_TO_COMPLY;
                }

                break;
              }

          /*
           * Cancel Tcc timer
           */
          callout_unset (&p->call_context.tcc_id);

          /*
           * Event can't occur in current ASM state
           */
          if (flag_queued)
          {
            asm_event = ((pbuf_cc->ccRequestType == DIAM_CC_UPDATE_REQUEST) ? (EVENT_CCAS_DQ_DIAM_CCR_UPDATE) : (EVENT_CCAS_DQ_DIAM_CCR_TERM));
          }
          else
            if (p->call_context.asm_state == STATE_CCAS_PROC_REQ)
            {
              asm_event = EVENT_CCAS_RX_DIAM_MSG_VALID;
            }
            else
            {
              asm_event = ((pbuf_cc->ccRequestType == DIAM_CC_UPDATE_REQUEST) ? (EVENT_CCAS_RX_DIAM_CCR_UPDATE) : (EVENT_CCAS_RX_DIAM_CCR_TERM));
            }

	  if (check_asm_state (p, asm_event) != SUCCESS)
          {
	    SPRINTF_ES_INVALID_STATE_CCAS;
	    XPRINT_ES_INVALID_STATE_CCAS;

            ++meas.ds_cntr.ds_ccdb_process_diam_fail;
	    return DIAMETER_UNABLE_TO_COMPLY;
	  }

          /*
           * Valid US, state, & event.  This is a Diam-CCR-U or Diam-CCR-T
	   * message from the network.
           */

          if (asm_event == EVENT_CCAS_RX_DIAM_MSG_VALID)
          {
            /*
             * Failed to queue Diameter message
             */
            if (dgc_qmsg_add (qmsg, pbuf_cc, p) != SUCCESS)
            {
              XPRINT_FAILED ("dgc_qmsg_add");

              ++meas.ds_cntr.ds_ccdb_process_diam_fail;
              return DIAMETER_UNABLE_TO_COMPLY;
            }
          }
          else
          {
            /*
             * Failed to update US data
             */
            if (user_session_save (pbuf_cc, p) != SUCCESS)
            {
              XPRINT_FAILED ("user_session_save");

              ++meas.ds_cntr.ds_ccdb_process_diam_fail;
              return DIAMETER_UNABLE_TO_COMPLY;
            }
          }

	  break;

        default:

	  /*
	   * Shouldn't reach here because DGC validates CC-Request-Type AVP
	   * before sending to DSM
	   */
          if (flag_queued)
            ++meas.ds_cntr.ds_msg_diam_ccr_invalid_dq;
          else
            ++meas.ds_cntr.ds_msg_diam_ccr_invalid_rcvd;

          SPRINTF_ES_INVALID_CCR_TYPE;
	  FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

          ++meas.ds_cntr.ds_ccdb_process_diam_fail;
	  return DIAMETER_INVALID_AVP_VALUE;
      }

      break;

    case DIAM_CMD_ASA:
    case DIAM_CMD_RAA:

      /*
       * Receipt of Diam-ASR or Diam-RAR unsupported
       */
      if (pbuf_cc->flags & DIAM_CMD_FLAG_R)
      {
        if (flag_queued)
        {
          if (pbuf_cc->commandCode == DIAM_CMD_ASR)
            ++meas.ds_cntr.ds_msg_diam_asr_dq;
          else
            ++meas.ds_cntr.ds_msg_diam_rar_dq;
        }
        else
        {
          if (pbuf_cc->commandCode == DIAM_CMD_ASR)
            ++meas.ds_cntr.ds_msg_diam_asr_rcvd;
          else
            ++meas.ds_cntr.ds_msg_diam_rar_rcvd;
        }

	SPRINTF_ES_UNSUPPORTED_CMD;
	XPRINT_ES_UNSUPPORTED_CMD;

        ++meas.ds_cntr.ds_ccdb_process_diam_fail;
	return DIAMETER_COMMAND_UNSUPPORTED;
      }

      if (flag_queued)
      {
        if (pbuf_cc->commandCode == DIAM_CMD_ASA)
          ++meas.ds_cntr.ds_msg_diam_asa_dq;
        else
          ++meas.ds_cntr.ds_msg_diam_raa_dq;
      }
      else
      {
        if (pbuf_cc->commandCode == DIAM_CMD_ASA)
          ++meas.ds_cntr.ds_msg_diam_asa_rcvd;
        else
          ++meas.ds_cntr.ds_msg_diam_raa_rcvd;
      }

      /*
       * Receipt of Diam-ASA or Diam-RAA with 'T' bit set unsupported
       */
      if (pbuf_cc->flags & DIAM_CMD_FLAG_T)
      {
	SPRINTF_ES_UNSUPPORTED_CMD;
	XPRINT_ES_UNSUPPORTED_CMD;

        ++meas.ds_cntr.ds_ccdb_process_diam_fail;
	return DIAMETER_COMMAND_UNSUPPORTED;
      }

      if (! p)  /* US does not exist */
      {
        ++meas.ds_cntr.ds_us_invalid;
	SPRINTF_ES_NO_US;
	FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

        ++meas.ds_cntr.ds_ccdb_process_diam_fail;
	return DIAMETER_UNKNOWN_SESSION_ID;
      }

      /*
       * Cancel Tcc timer
       */
      callout_unset (&p->call_context.tcc_id);

      /*
       * Event can't occur in current ASM state
       */
      asm_event = ((pbuf_cc->commandCode == DIAM_CMD_ASA) ? (EVENT_CCAS_RX_DIAM_ASA) : (EVENT_CCAS_RX_DIAM_RAA));
      if (check_asm_state (p, asm_event) != SUCCESS)
      {
        SPRINTF_ES_INVALID_STATE_CCAS;
        XPRINT_ES_INVALID_STATE_CCAS;

        ++meas.ds_cntr.ds_ccdb_process_diam_fail;
        return DIAMETER_UNABLE_TO_COMPLY;
      }

      /*
       * Valid US, state, & event.  This is a Diam-ASA or Diam-RAA message
       * from the network.
       */

      /*
       * Failed to update US data
       */
      if (user_session_save (pbuf_cc, p) != SUCCESS)
      {
	XPRINT_FAILED ("user_session_save");

        ++meas.ds_cntr.ds_ccdb_process_diam_fail;
	return DIAMETER_UNABLE_TO_COMPLY;
      }

      break;

    case DIAM_CMD_STR:

      /*
       * Receipt of Diam-STA unsupported
       */
      if (! (pbuf_cc->flags & DIAM_CMD_FLAG_R))
      {
        if (flag_queued)
          ++meas.ds_cntr.ds_msg_diam_sta_dq;
        else
          ++meas.ds_cntr.ds_msg_diam_sta_rcvd;

	SPRINTF_ES_UNSUPPORTED_CMD;
	XPRINT_ES_UNSUPPORTED_CMD;

        ++meas.ds_cntr.ds_ccdb_process_diam_fail;
	return DIAMETER_COMMAND_UNSUPPORTED;
      }

      if (flag_queued)
        ++meas.ds_cntr.ds_msg_diam_str_dq;
      else
        ++meas.ds_cntr.ds_msg_diam_str_rcvd;

      /*
       * Receipt of Diam-STR with 'E' bit set unsupported
       */
      if (pbuf_cc->flags & DIAM_CMD_FLAG_E)
      {
	SPRINTF_ES_UNSUPPORTED_CMD;
	XPRINT_ES_UNSUPPORTED_CMD;

        ++meas.ds_cntr.ds_ccdb_process_diam_fail;
	return DIAMETER_COMMAND_UNSUPPORTED;
      }

      if (! p)  /* US does not exist */
      {
        ++meas.ds_cntr.ds_us_invalid;
	SPRINTF_ES_NO_US;
	FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

        ++meas.ds_cntr.ds_ccdb_process_diam_fail;
	return DIAMETER_UNKNOWN_SESSION_ID;
      }

      /*
       * Receipt of Diam-STR with 'T' bit set
       */
      if (pbuf_cc->flags & DIAM_CMD_FLAG_T)
        if (pbuf_cc->endToEndId == p->diameter.end_to_end)
          if (CALLOUT_ID_VALID (p->call_context.tg_id))
          {
            /*
             * Event can't occur in current ASM state
             */
            asm_event = EVENT_CCAS_RX_DIAM_STR_T;
            if (check_asm_state (p, asm_event) != SUCCESS)
            {
              SPRINTF_ES_INVALID_STATE_CCAS;
              XPRINT_ES_INVALID_STATE_CCAS;

              ++meas.ds_cntr.ds_ccdb_process_diam_fail;
              return DIAMETER_UNABLE_TO_COMPLY;
            }

            /*
             * Awaiting GDI-STA message from the ISCP.  No need to forward
             * this Diam-STR message to the ISCP again.
             */
            pbuf_cc->diamctx_flags.no_gdi = TRUE;

            /*
             * Failed to update US data
             */
            if (user_session_save (pbuf_cc, p) != SUCCESS)
            {
              XPRINT_FAILED ("user_session_save");

              ++meas.ds_cntr.ds_ccdb_process_diam_fail;
              return DIAMETER_UNABLE_TO_COMPLY;
            }

            break;
          }

      /*
       * Cancel Tcc timer
       */
      callout_unset (&p->call_context.tcc_id);

      /*
       * Event can't occur in current ASM state
       */
      if (p->call_context.asm_state == STATE_CCAS_PROC_REQ)
        asm_event = EVENT_CCAS_RX_DIAM_MSG_VALID;
      else
        asm_event = ((flag_queued) ? (EVENT_CCAS_DQ_DIAM_STR) : (EVENT_CCAS_RX_DIAM_STR));

      if (check_asm_state (p, asm_event) != SUCCESS)
      {
        SPRINTF_ES_INVALID_STATE_CCAS;
        XPRINT_ES_INVALID_STATE_CCAS;

        ++meas.ds_cntr.ds_ccdb_process_diam_fail;
        return DIAMETER_UNABLE_TO_COMPLY;
      }

      /*
       * Valid US, state, & event.  This is a Diam-STR message from the
       * network.
       */

      if (asm_event == EVENT_CCAS_RX_DIAM_MSG_VALID)
      {
        /*
         * Failed to queue Diameter message
         */
        if (dgc_qmsg_add (qmsg, pbuf_cc, p) != SUCCESS)
        {
          XPRINT_FAILED ("dgc_qmsg_add");

          ++meas.ds_cntr.ds_ccdb_process_diam_fail;
          return DIAMETER_UNABLE_TO_COMPLY;
        }
      }
      else
      {
        /*
         * Failed to update US data
         */
        if (user_session_save (pbuf_cc, p) != SUCCESS)
        {
          XPRINT_FAILED ("user_session_save");

          ++meas.ds_cntr.ds_ccdb_process_diam_fail;
          return DIAMETER_UNABLE_TO_COMPLY;
        }
      }

      break;

    default:

      if (flag_queued)
        ++meas.ds_cntr.ds_msg_diam_invalid_dq;
      else
        ++meas.ds_cntr.ds_msg_diam_invalid_rcvd;

      SPRINTF_ES_INVALID_CMD;
      FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s (%s)\n", pbuf_cc->errorString, bit_print (pbuf_cc->flags, sizeof (pbuf_cc->flags))));

      ++meas.ds_cntr.ds_ccdb_process_diam_fail;
      return DIAMETER_COMMAND_UNSUPPORTED;
  }

  switch (asm_event)
  {
    case EVENT_CCAS_DQ_DIAM_CCR_UPDATE:
    case EVENT_CCAS_DQ_DIAM_CCR_TERM:
    case EVENT_CCAS_DQ_DIAM_STR:
    case EVENT_CCAS_RX_DIAM_CCR_INIT:
    case EVENT_CCAS_RX_DIAM_CCR_EVENT:
    case EVENT_CCAS_RX_DIAM_CCR_UPDATE:
    case EVENT_CCAS_RX_DIAM_CCR_TERM:
    case EVENT_CCAS_RX_DIAM_STR:

      /*
       * Failed to construct DGC DiamCtx qmsg
       */
      if (qmsg_diamctx_construct (pbuf_cc, p, FALSE) != SUCCESS)
      {
        SPRINTF_ES_FAILED ("qmsg_diamctx_construct");
        FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

        ++meas.ds_cntr.ds_ccdb_process_diam_fail;
        return DIAMETER_UNABLE_TO_COMPLY;
      }

      /*
       * Failed to set Tg timer
       */
      p->call_context.tg_id = callout_set (process_expire_tg, p, (p->call_context.tg_expiry = ccdb.tg_expiry));
      if (! CALLOUT_ID_VALID (p->call_context.tg_id))
      {
	SPRINTF_ES_FAILED_CALLOUT_SET ("tg_expiry", p->call_context.tg_expiry);
	FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

        ++meas.ds_cntr.ds_ccdb_process_diam_fail;
	return DIAMETER_UNABLE_TO_COMPLY;
      }

      break;

    case EVENT_CCAS_RX_DIAM_ASA:
    case EVENT_CCAS_RX_DIAM_RAA:

      /*
       * Failed to construct DGC DiamCtx qmsg
       */
      if (qmsg_diamctx_construct (pbuf_cc, p, FALSE) != SUCCESS)
      {
        SPRINTF_ES_FAILED ("qmsg_diamctx_construct");
        FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

        ++meas.ds_cntr.ds_ccdb_process_diam_fail;
        return DIAMETER_UNABLE_TO_COMPLY;
      }

      /*
       * Clear remote TID AFTER passing it to DGC in DiamCtx above
       */
      if (p->gdi.package_type == PACKAGE_TYPE_RESPONSE)
	p->gdi.tid_remote = 0;

      /*
       * Failed to set Tcc timer
       */
      p->call_context.tcc_id = callout_set (process_expire_tcc, p, p->call_context.tcc_expiry);
      if (! CALLOUT_ID_VALID (p->call_context.tcc_id))
      {
	SPRINTF_ES_FAILED_CALLOUT_SET ("tcc_expiry", p->call_context.tcc_expiry);
        FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

        ++meas.ds_cntr.ds_ccdb_process_diam_fail;
        return DIAMETER_UNABLE_TO_COMPLY;
      }

      break;

    case EVENT_CCAS_RX_DIAM_MSG_VALID:

      break;

    case EVENT_CCAS_RX_DIAM_CCR_INIT_T:
    case EVENT_CCAS_RX_DIAM_CCR_EVENT_T:
    case EVENT_CCAS_RX_DIAM_CCR_UPDATE_T:
    case EVENT_CCAS_RX_DIAM_CCR_TERM_T:
    case EVENT_CCAS_RX_DIAM_STR_T:

      /*
       * Failed to construct DGC DiamCtx qmsg
       */
      if (qmsg_diamctx_construct (pbuf_cc, p, FALSE) != SUCCESS)
      {
        SPRINTF_ES_FAILED ("qmsg_diamctx_construct");
        FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

        ++meas.ds_cntr.ds_ccdb_process_diam_fail;
        return DIAMETER_UNABLE_TO_COMPLY;
      }

      break;

    default:

      ++meas.ds_cntr.ds_asm_event_invalid;
      SPRINTF_ES_INVALID_EVENT_CCAS;
      XPRINT_ES_INVALID_EVENT_CCAS;

      ++meas.ds_cntr.ds_ccdb_process_diam_fail;
      return DIAMETER_UNABLE_TO_COMPLY;
  }

  /*
   * Failed to update ASM state
   */
  asm_state_prev = p->call_context.asm_state;
  if ((p->call_context.asm_state = set_asm_state (p, asm_event)) < 0)
    if (asm_event == EVENT_CCAS_RX_DIAM_MSG_VALID)
    {
      /*
       * Despite unlikely failure, don't return; just continue.  US can only be
       * in the same state (STATE_CCAS_PROC_REQ) after
       * EVENT_CCAS_RX_DIAM_MSG_VALID event.
       */
      p->call_context.asm_state = asm_state_prev;
    }
    else
    {
      SPRINTF_ES_FAILED ("set_asm_state");
      XPRINT_ES_FAILED_SET_ASM_STATE;

      ++meas.ds_cntr.ds_ccdb_process_diam_fail;
      return DIAMETER_UNABLE_TO_COMPLY;
    }

  /*
   * Update CC stats
   */
  ++p->call_context.ref_count;
  p->call_context.time_last = time (NULL);

  ++meas.ds_cntr.ds_ccdb_process_diam_succ;
  return ((asm_event == EVENT_CCAS_RX_DIAM_MSG_VALID) ? (SUCCESS_QUEUED) : (SUCCESS));
}

/*
 * Process GDI message
 */
int process_gdi (fep_qmsg_t qmsg, DiamCtx_t *pbuf_cc, struct user_session **p2)
{
  char me[] = "process_gdi";

  struct user_session *p;
  ulong result_code, id;
  int result_dgc, ccdb_hash_index, flag_remove_us;
  short asm_event, asm_state_prev;
  char buffer[256];

  XPRINT_ENTERED;

  result_dgc = qmsg[2];
  result_code = qmsg[3];

  if (! pbuf_cc)  /* NULL DiamCtx */
    sprintf (buffer, "%s", DSM_NA_STR);
  else
    sprintf (buffer, "0x%x", pbuf_cc->localTid);
  FEP_XPRINT (debug_mask[DEBUG_MSG_GDI_ENTER], ("%s: %s, DGC's qmsg processing result %d (%s), Diameter Result-Code AVP %lu (%s), local TID %s\n", me, debug_msg_itoa (debug_mask[DEBUG_MSG_GDI_ENTER]), result_dgc, STATUS_ITOA (result_dgc), result_code, result_code_itoa (result_code), buffer));

  if (! pbuf_cc)  /* NULL DiamCtx */
  {
    ++meas.ds_cntr.ds_dc_null;
    XPRINT_NULL ("DiamCtx", pbuf_cc);

    ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
    return DIAMETER_UNABLE_TO_COMPLY;
  }

  if (! p2)  /* NULL US pointer */
  {
    ++meas.ds_cntr.ds_us_ptr_null;
    XPRINT_NULL ("US pointer", p2);

    ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
    return DIAMETER_UNABLE_TO_COMPLY;
  }

  diamctx_print (pbuf_cc, debug_mask[DEBUG_MSG_GDI_ENTER], me);

  /*
   * Process message based on DGC's qmsg processing result
   */
  switch (result_dgc)
  {
    case FAILURE:

      /*
       * This is a GDI message from the ISCP that DGC detected an error in
       * while processing it.  Don't overwrite DGC's passed in processing
       * result & Diameter Result-Code AVP.
       */

      /*
       * Find US based on GDI package type
       */
      switch (pbuf_cc->packageType)
      {
        case PACKAGE_TYPE_REQUEST:

          ++meas.ds_cntr.ds_pkg_gdi_begin_rcvd;

          /*
           * Failed to determine CCDB hash index for US given its session ID
           */
          if ((ccdb_hash_index = user_session_hash (FEP_BUF_SID, pbuf_cc->sessionIdLength)) < 0)
          {
            XPRINT_FAILED_US_HASH;

            ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
            return DIAMETER_UNABLE_TO_COMPLY;
          }

          /*
           * NULL US
           */
          if (! (p = *p2 = user_session_get (FEP_BUF_SID, pbuf_cc->sessionIdLength, ccdb_hash_index)))
          {
            ++meas.ds_cntr.ds_us_null;
            XPRINT_NULL ("US", p);

            ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
            return DIAMETER_UNKNOWN_SESSION_ID;
          }
	  user_session_print (p, debug_mask[DEBUG_MSG_GDI_ENTER], me);

          if (! p->head)  /* NULL US list */
          {
            ++meas.ds_cntr.ds_us_list_null;
            XPRINT_NULL ("US list", p->head);

            ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
            return DIAMETER_UNABLE_TO_COMPLY;
          }

	  break;

        case PACKAGE_TYPE_CONV:
        case PACKAGE_TYPE_RESPONSE:

          if (pbuf_cc->packageType == PACKAGE_TYPE_CONV)
            ++meas.ds_cntr.ds_pkg_gdi_continue_rcvd;
          else
            ++meas.ds_cntr.ds_pkg_gdi_end_rcvd;

          /*
           * Failed to determine US ID from GDI local TID
           */
          id = (pbuf_cc->localTid & MASK_TID_CC) >> SHIFT_TID_CC;
          if (! USER_SESSION_ID_VALID (id))
          {
            ++meas.ds_cntr.ds_dc_tid_local_invalid;
            XPRINT_INVALID_US_ID (pbuf_cc->localTid);

            ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
	    return DIAMETER_UNABLE_TO_COMPLY;
          }
	  p = *p2 = &fepmem_list_user_session[ID_TO_INDEX (id)];
	  user_session_print (p, debug_mask[DEBUG_MSG_GDI_ENTER], me);

          if (! p->head)  /* NULL US list */
          {
            ++meas.ds_cntr.ds_us_list_null;
            XPRINT_NULL ("US list", p->head);

            ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
            return DIAMETER_UNABLE_TO_COMPLY;
          }

          /*
           * Inconsistent local TIDs
           */
          if (p->gdi.tid_local != pbuf_cc->localTid)
          {
            ++meas.ds_cntr.ds_us_tid_local_invalid;
	    string_inconsistent_tid (buffer, me, "local", p->gdi.tid_local, pbuf_cc->localTid);
	    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", buffer));

            ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
            return DIAMETER_UNABLE_TO_COMPLY;
          }

	  break;

        default:

          ++meas.ds_cntr.ds_pkg_gdi_invalid_rcvd;
	  FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: Unknown GDI package type 0x%x (%s)\n", me, pbuf_cc->packageType, package_type_itoa (pbuf_cc->packageType)));

          ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
	  return DIAMETER_UNABLE_TO_COMPLY;
      }

      /*
       * Cancel Tg timer
       */
      callout_unset (&p->call_context.tg_id);

      /*
       * Event can't occur in current ASM state
       */
      asm_event = EVENT_CCAS_RX_DIAMCTX_GDI_FAIL;
      if (check_asm_state (p, asm_event) != SUCCESS)
      {
	XPRINT_INVALID_STATE_CCAS;

        ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
        return DIAMETER_UNABLE_TO_COMPLY;
      }

      /*
       * Valid US, state, & event.   This is a GDI message from the ISCP that
       * DGC detected an error in while processing it.
       */

      /*
       * Failed to construct DGC DiamCtx qmsg
       */
      if (qmsg_diamctx_construct (pbuf_cc, p, TRUE) != SUCCESS)
      {
	XPRINT_FAILED ("qmsg_diamctx_construct");

        ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
        return DIAMETER_UNABLE_TO_COMPLY;
      }

      /*
       * Failed to set Tg timer
       */
      p->call_context.tg_id = callout_set (process_expire_tg, p, (p->call_context.tg_expiry = ccdb.tg_expiry));
      if (! CALLOUT_ID_VALID (p->call_context.tg_id))
      {
        XPRINT_FAILED_CALLOUT_SET ("tg_expiry", p->call_context.tg_expiry);

        ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
        return DIAMETER_UNABLE_TO_COMPLY;
      }

      /*
       * Failed to update ASM state
       */
      asm_state_prev = p->call_context.asm_state;
      if ((p->call_context.asm_state = set_asm_state (p, asm_event)) < 0)
      {
        XPRINT_FAILED_SET_ASM_STATE;

        /*
         * Despite unlikely failure, don't return; just continue.  US can only
	 * be in STATE_CCAS_REPORTING_ERROR state after
	 * EVENT_CCAS_RX_DIAMCTX_GDI_FAIL event.
         */
        p->call_context.asm_state = STATE_CCAS_REPORTING_ERROR;
      }

      /*
       * Update CC stats
       */
      ++p->call_context.ref_count;
      p->call_context.time_last = time (NULL);

      ++meas.ds_cntr.ds_ccdb_process_gdi_succ;
      return SUCCESS;

    default:

      /*
       * Find US based on GDI package type
       */
      switch (pbuf_cc->packageType)
      {
        case PACKAGE_TYPE_REQUEST:

          ++meas.ds_cntr.ds_pkg_gdi_begin_rcvd;

          /*
           * Failed to determine CCDB hash index for US given its session ID
           */
          if ((ccdb_hash_index = user_session_hash (FEP_BUF_SID, pbuf_cc->sessionIdLength)) < 0)
          {
            SPRINTF_ES_FAILED ("user_session_hash");
            FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s, sessionId %s, length %d\n", pbuf_cc->errorString, string_hex_print (FEP_BUF_SID, pbuf_cc->sessionIdLength), pbuf_cc->sessionIdLength));

            ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
            return DIAMETER_UNABLE_TO_COMPLY;
          }

          /*
           * NULL US
           */
          if (! (p = *p2 = user_session_get (FEP_BUF_SID, pbuf_cc->sessionIdLength, ccdb_hash_index)))
          {
            ++meas.ds_cntr.ds_us_null;
            SPRINTF_ES_NULL ("US", p);
            FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

            ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
            return DIAMETER_UNKNOWN_SESSION_ID;
          }
	  user_session_print (p, debug_mask[DEBUG_MSG_GDI_ENTER], me);

          if (! p->head)  /* NULL US list */
          {
            ++meas.ds_cntr.ds_us_list_null;
            SPRINTF_ES_NULL ("US list", p->head);
            FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

            ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
            return DIAMETER_UNABLE_TO_COMPLY;
          }

	  break;

        case PACKAGE_TYPE_CONV:
        case PACKAGE_TYPE_RESPONSE:

          if (pbuf_cc->packageType == PACKAGE_TYPE_CONV)
            ++meas.ds_cntr.ds_pkg_gdi_continue_rcvd;
          else
            ++meas.ds_cntr.ds_pkg_gdi_end_rcvd;

          /*
           * Failed to determine US ID from GDI local TID
           */
          id = (pbuf_cc->localTid & MASK_TID_CC) >> SHIFT_TID_CC;
          if (! USER_SESSION_ID_VALID (id))
          {
            ++meas.ds_cntr.ds_dc_tid_local_invalid;
	    sprintf (pbuf_cc->errorString, "%s: Invalid US ID %lu, local TID 0x%x (%s)\n", me, id, pbuf_cc->localTid, bit_print (pbuf_cc->localTid, sizeof (pbuf_cc->localTid)));
            FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

            ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
	    return DIAMETER_UNABLE_TO_COMPLY;
          }
	  p = *p2 = &fepmem_list_user_session[ID_TO_INDEX (id)];
	  user_session_print (p, debug_mask[DEBUG_MSG_GDI_ENTER], me);

          if (! p->head)  /* NULL US list */
          {
            ++meas.ds_cntr.ds_us_list_null;
            SPRINTF_ES_NULL ("US list", p->head);
            FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

            ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
            return DIAMETER_UNABLE_TO_COMPLY;
          }

          /*
           * Inconsistent local TIDs
           */
          if (p->gdi.tid_local != pbuf_cc->localTid)
          {
            ++meas.ds_cntr.ds_us_tid_local_invalid;
	    string_inconsistent_tid (pbuf_cc->errorString, me, "local", p->gdi.tid_local, pbuf_cc->localTid);
            FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

            ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
            return DIAMETER_UNABLE_TO_COMPLY;
          }

	  break;

        default:

          ++meas.ds_cntr.ds_pkg_gdi_invalid_rcvd;
	  sprintf (pbuf_cc->errorString, "%s: Unknown GDI package type 0x%x (%s)\n", me, pbuf_cc->packageType, package_type_itoa (pbuf_cc->packageType));
          FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

          ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
	  return DIAMETER_UNABLE_TO_COMPLY;
      }

      /*
       * Process message based on Command-Code
       */
      switch (pbuf_cc->commandCode)
      {
        case DIAM_CMD_CCA:

          /*
	   * Receipt of GDI-CCR unsupported
           */
          if (pbuf_cc->flags & DIAM_CMD_FLAG_R)
          {
            ++meas.ds_cntr.ds_msg_gdi_ccr_rcvd;
	    SPRINTF_ES_UNSUPPORTED_CMD;
	    XPRINT_ES_UNSUPPORTED_CMD;

            ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
	    return DIAMETER_COMMAND_UNSUPPORTED;
          }
          ++meas.ds_cntr.ds_msg_gdi_cca_rcvd;

          /*
           * Received GDI-CCA message.  Process message based on request type.
           */
          switch (pbuf_cc->ccRequestType)
          {
            case DIAM_CC_INITIAL_REQUEST:

              if (DIAM_RC_SUCCESS (pbuf_cc->result_code))
              {
                ++meas.ds_cntr.ds_msg_gdi_cca_initial_succ_rcvd;
                asm_event = EVENT_CCAS_RX_GDI_CCA_INIT_SUCC;
              }
              else
              {
                ++meas.ds_cntr.ds_msg_gdi_cca_initial_fail_rcvd;
                asm_event = ((pbuf_cc->timer > 0) ? (EVENT_CCAS_RX_GDI_CCA_INIT_SUCC) : (EVENT_CCAS_RX_GDI_CCA_INIT_FAIL));
              }
	      break;

            case DIAM_CC_EVENT_REQUEST:

              if (DIAM_RC_SUCCESS (pbuf_cc->result_code))
              {
                ++meas.ds_cntr.ds_msg_gdi_cca_event_succ_rcvd;
                asm_event = EVENT_CCAS_RX_GDI_CCA_EVENT_SUCC;
              }
              else
              {
                ++meas.ds_cntr.ds_msg_gdi_cca_event_fail_rcvd;
                asm_event = EVENT_CCAS_RX_GDI_CCA_EVENT_FAIL;
              }
	      break;

            case DIAM_CC_UPDATE_REQUEST:

              if (DIAM_RC_SUCCESS (pbuf_cc->result_code))
              {
                ++meas.ds_cntr.ds_msg_gdi_cca_update_succ_rcvd;
                asm_event = EVENT_CCAS_RX_GDI_CCA_UPDATE_SUCC;
              }
              else
              {
                ++meas.ds_cntr.ds_msg_gdi_cca_update_fail_rcvd;
                asm_event = ((pbuf_cc->timer > 0) ? (EVENT_CCAS_RX_GDI_CCA_UPDATE_SUCC) : (EVENT_CCAS_RX_GDI_CCA_UPDATE_FAIL));
              }
	      break;

            case DIAM_CC_TERMINATION_REQUEST:

              if (DIAM_RC_SUCCESS (pbuf_cc->result_code))
              {
                ++meas.ds_cntr.ds_msg_gdi_cca_termination_succ_rcvd;
                asm_event = EVENT_CCAS_RX_GDI_CCA_TERM_SUCC;
              }
              else
              {
                ++meas.ds_cntr.ds_msg_gdi_cca_termination_fail_rcvd;
                asm_event = EVENT_CCAS_RX_GDI_CCA_TERM_FAIL;
              }
	      break;

            default:

	      /*
	       * Shouldn't reach here because DGC validates CC-Request-Type AVP
	       * before sending to DSM
	       */
              if (DIAM_RC_SUCCESS (pbuf_cc->result_code))
                ++meas.ds_cntr.ds_msg_gdi_cca_invalid_succ_rcvd;
              else
                ++meas.ds_cntr.ds_msg_gdi_cca_invalid_fail_rcvd;

              SPRINTF_ES_INVALID_CCR_TYPE;
	      FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

              ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
	      return DIAMETER_INVALID_AVP_VALUE;
          }

          if (p->call_context.asm_state != STATE_CCAS_REPORTING_ERROR)
          {
            /*
             * Cancel Tg timer
             */
            callout_unset (&p->call_context.tg_id);
          }

          /*
           * Event can't occur in current ASM state
           */
          if (check_asm_state (p, asm_event) != SUCCESS)
          {
            SPRINTF_ES_INVALID_STATE_CCAS;
            XPRINT_ES_INVALID_STATE_CCAS;

            ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
            return DIAMETER_UNABLE_TO_COMPLY;
          }

          /*
           * Valid US, state, & event.  This is a GDI-CCA message from the
	   * ISCP.
           */

          if (p->call_context.asm_state == STATE_CCAS_REPORTING_ERROR)
          {
            /*
             * Receipt of GDI-CCA message from ISCP occurred after GDI-CCR Tg
             * expiration & after GDI-DIM_ReportError was sent to ISCP.  Tell
             * DGC not to send a Diam-CCA message to the network & to free its
             * DiamCtx resource.
             */
            pbuf_cc->diamctx_flags.no_diam = TRUE;
          }

          break;

        case DIAM_CMD_ASR:
        case DIAM_CMD_RAR:

          /*
           * Receipt of GDI-ASA or GDI-RAA unsupported
           */
          if (! (pbuf_cc->flags & DIAM_CMD_FLAG_R))
          {
            if (pbuf_cc->commandCode == DIAM_CMD_ASA)
              ++meas.ds_cntr.ds_msg_gdi_asa_rcvd;
            else
              ++meas.ds_cntr.ds_msg_gdi_raa_rcvd;

	    SPRINTF_ES_UNSUPPORTED_CMD;
	    XPRINT_ES_UNSUPPORTED_CMD;

            ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
	    return DIAMETER_COMMAND_UNSUPPORTED;
          }

          if (pbuf_cc->commandCode == DIAM_CMD_ASR)
            ++meas.ds_cntr.ds_msg_gdi_asr_rcvd;
          else
            ++meas.ds_cntr.ds_msg_gdi_rar_rcvd;

          /*
           * Cancel Tg timer
           */
          callout_unset (&p->call_context.tg_id);

          /*
           * Event can't occur in current ASM state
           */
          asm_event = ((pbuf_cc->commandCode == DIAM_CMD_ASR) ? (EVENT_CCAS_RX_GDI_ASR) : (EVENT_CCAS_RX_GDI_RAR));
          if (check_asm_state (p, asm_event) != SUCCESS)
          {
            SPRINTF_ES_INVALID_STATE_CCAS;
            XPRINT_ES_INVALID_STATE_CCAS;

            ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
            return DIAMETER_UNABLE_TO_COMPLY;
          }

          /*
           * Valid US, state, & event.  This is a GDI-ASR or GDI-RAR message
	   * from the ISCP.
           */

          break;

        case DIAM_CMD_STA:

          /*
           * Receipt of GDI-STR unsupported
           */
          if (pbuf_cc->flags & DIAM_CMD_FLAG_R)
          {
            ++meas.ds_cntr.ds_msg_gdi_str_rcvd;
	    SPRINTF_ES_UNSUPPORTED_CMD;
	    XPRINT_ES_UNSUPPORTED_CMD;

            ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
	    return DIAMETER_COMMAND_UNSUPPORTED;
          }
          ++meas.ds_cntr.ds_msg_gdi_sta_rcvd;

          if (p->call_context.asm_state != STATE_CCAS_REPORTING_ERROR)
          {
            /*
             * Cancel Tg timer
             */
            callout_unset (&p->call_context.tg_id);
          }

          /*
           * Event can't occur in current ASM state
           */
          asm_event = ((DIAM_RC_SUCCESS (pbuf_cc->result_code)) ? (EVENT_CCAS_RX_GDI_STA_SUCC) : (EVENT_CCAS_RX_GDI_STA_FAIL));
          if (check_asm_state (p, asm_event) != SUCCESS)
          {
            SPRINTF_ES_INVALID_STATE_CCAS;
            XPRINT_ES_INVALID_STATE_CCAS;

            ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
            return DIAMETER_UNABLE_TO_COMPLY;
          }

          /*
           * Valid US, state, event.  This is a GDI-STA message from the ISCP.
           */

          switch (p->call_context.asm_state)
          {
            case STATE_CCAS_DISCONTINUING:

              /*
               * ISCP responded to an internal, FE-generated (i.e., not from
               * network) GDI-STR message with a GDI-STA message.  Tell DGC
               * not to send a Diam-STA message to the network & to free its
               * DiamCtx resource.
               */
              pbuf_cc->diamctx_flags.no_diam = TRUE;
              break;

            case STATE_CCAS_REPORTING_ERROR:

              /*
               * Receipt of GDI-STA message from ISCP occurred after GDI-STR
               * Tg expiration & after GDI-DIM_ReportError was sent to ISCP.
               * Tell DGC not to send a Diam-STA message to the network & to
               * free its DiamCtx resource.
               */
              pbuf_cc->diamctx_flags.no_diam = TRUE;
              break;
          }

	  break;

        default:

          ++meas.ds_cntr.ds_msg_gdi_invalid_rcvd;
	  SPRINTF_ES_INVALID_CMD;
          FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s (%s)\n", pbuf_cc->errorString, bit_print (pbuf_cc->flags, sizeof (pbuf_cc->flags))));

          ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
          return DIAMETER_COMMAND_UNSUPPORTED;
      }

      /*
       * Discard GDI messages received in the STATE_CCAS_REPORTING_ERROR state
       */
      if (p->call_context.asm_state == STATE_CCAS_REPORTING_ERROR)
        break;

      /*
       * Failed to update US data
       */
      if (user_session_save (pbuf_cc, p) != SUCCESS)
      {
        XPRINT_FAILED ("user_session_save");

        ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
        return DIAMETER_UNABLE_TO_COMPLY;
      }

      /*
       * Failed to construct DGC DiamCtx qmsg
       */
      if (qmsg_diamctx_construct (pbuf_cc, p, FALSE) != SUCCESS)
      {
        SPRINTF_ES_FAILED ("qmsg_diamctx_construct");
        FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

        ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
        return DIAMETER_UNABLE_TO_COMPLY;
      }

      flag_remove_us = FALSE;
      switch (asm_event)
      {
        case EVENT_CCAS_RX_GDI_CCA_INIT_SUCC:
        case EVENT_CCAS_RX_GDI_CCA_UPDATE_SUCC:

          /*
           * Set Tcc timer
           */
          if (p->call_context.tcc_expiry > 0)
          {
            /*
             * Failed to set Tcc timer
             */
            p->call_context.tcc_id = callout_set (process_expire_tcc, p, p->call_context.tcc_expiry);
            if (! CALLOUT_ID_VALID (p->call_context.tcc_id))
            {
	      SPRINTF_ES_FAILED_CALLOUT_SET ("tcc_expiry", p->call_context.tcc_expiry);
	      FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

              ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
	      return DIAMETER_UNABLE_TO_COMPLY;
            }
          }
          else
          {
            flag_remove_us = TRUE;  /* Remove US */
          }

          break;

        case EVENT_CCAS_RX_GDI_ASR:
        case EVENT_CCAS_RX_GDI_RAR:

          if (p->call_context.asm_state == STATE_CCAS_PROC_REQ)
          {
            /*
             * Failed to set Tg timer
             */
            p->call_context.tg_id = callout_set (process_expire_tg, p, (p->call_context.tg_expiry = ccdb.tg_expiry));
            if (! CALLOUT_ID_VALID (p->call_context.tg_id))
            {
	      SPRINTF_ES_FAILED_CALLOUT_SET ("tg_expiry", p->call_context.tg_expiry);
              FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

              ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
              return DIAMETER_UNABLE_TO_COMPLY;
            }
          }
          else
          {
            /*
             * In state STATE_CCAS_OPEN.  Maintain or set Tcc timer.
             */
            if (! CALLOUT_ID_VALID (p->call_context.tcc_id))
            {
              /*
               * Failed to set Tcc timer
               */
              p->call_context.tcc_id = callout_set (process_expire_tcc, p, p->call_context.tcc_expiry);
              if (! CALLOUT_ID_VALID (p->call_context.tcc_id))
              {
                SPRINTF_ES_FAILED_CALLOUT_SET ("tcc_expiry", p->call_context.tcc_expiry);
                FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", pbuf_cc->errorString));

                ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
                return DIAMETER_UNABLE_TO_COMPLY;
              }
            }
          }

          break;

        case EVENT_CCAS_RX_GDI_CCA_INIT_FAIL:
        case EVENT_CCAS_RX_GDI_CCA_EVENT_SUCC:
        case EVENT_CCAS_RX_GDI_CCA_EVENT_FAIL:
        case EVENT_CCAS_RX_GDI_CCA_UPDATE_FAIL:
        case EVENT_CCAS_RX_GDI_CCA_TERM_SUCC:
        case EVENT_CCAS_RX_GDI_CCA_TERM_FAIL:
        case EVENT_CCAS_RX_GDI_STA_SUCC:
        case EVENT_CCAS_RX_GDI_STA_FAIL:

          flag_remove_us = TRUE;  /* Remove US */
	  break;

        default:

          ++meas.ds_cntr.ds_asm_event_invalid;
	  SPRINTF_ES_INVALID_EVENT_CCAS;
	  XPRINT_ES_INVALID_EVENT_CCAS;

          ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
          return DIAMETER_UNABLE_TO_COMPLY;
      }

      /*
       * Failed to update ASM state
       */
      asm_state_prev = p->call_context.asm_state;
      if ((p->call_context.asm_state = set_asm_state (p, asm_event)) < 0)
      {
        SPRINTF_ES_FAILED ("set_asm_state");
	XPRINT_ES_FAILED_SET_ASM_STATE;

        ++meas.ds_cntr.ds_ccdb_process_gdi_fail;
        return DIAMETER_UNABLE_TO_COMPLY;
      }

      /*
       * Update CC stats
       */
      ++p->call_context.ref_count;
      p->call_context.time_last = time (NULL);

      /*
       * Remove US
       */
      if (flag_remove_us)
      {
	user_session_print (p, debug_mask[DEBUG_MSG_GDI_EXIT], me);
	XPRINT_US_REMOVE;

        if (p = *p2 = user_session_remove (&p))
        {
          XPRINT_FAILED_US_REMOVE;
        }
      }

      break;
  }

  ++meas.ds_cntr.ds_ccdb_process_gdi_succ;
  return SUCCESS;
}

/*
 * Process GDI-Abort message
 */
void process_gdi_abort (DiamCtx_t *pbuf_cc)
{
  char me[] = "process_gdi_abort";

  struct user_session *p;
  ulong id;
  short asm_event, asm_state_prev;
  char buffer[256];

  XPRINT_ENTERED;

  if (! pbuf_cc)  /* NULL DiamCtx */
  {
    ++meas.ds_cntr.ds_dc_null;
    XPRINT_NULL ("DiamCtx", pbuf_cc);

    ++meas.ds_cntr.ds_ccdb_process_gdi_abort_fail;
    return;
  }

  diamctx_print (pbuf_cc, debug_mask[DEBUG_MSG_GDI_ENTER], me);

  if (pbuf_cc->packageType != PACKAGE_TYPE_ABORT)
  {
    ++meas.ds_cntr.ds_pkg_gdi_invalid_rcvd;
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: Unexpected or invalid packageType 0x%x (%s)\n", me, pbuf_cc->packageType, package_type_itoa (pbuf_cc->packageType)));

    ++meas.ds_cntr.ds_ccdb_process_gdi_abort_fail;
    return;
  }

  /*
   * Received a GDI-Abort message from the ISCP (i.e., GDI package type is
   * ABORT)
   */
  ++meas.ds_cntr.ds_pkg_gdi_abort_rcvd;

  /*
   * Failed to determine US ID from GDI local TID
   */
  id = (pbuf_cc->localTid & MASK_TID_CC) >> SHIFT_TID_CC;
  if (! USER_SESSION_ID_VALID (id))
  {
    ++meas.ds_cntr.ds_dc_tid_local_invalid;
    XPRINT_INVALID_US_ID (pbuf_cc->localTid);

    ++meas.ds_cntr.ds_ccdb_process_gdi_abort_fail;
    return;
  }
  p = &fepmem_list_user_session[ID_TO_INDEX (id)];
  user_session_print (p, debug_mask[DEBUG_MSG_GDI_ENTER], me);

  if (! p->head)  /* NULL US list */
  {
    ++meas.ds_cntr.ds_us_list_null;
    XPRINT_NULL ("US list", p->head);

    ++meas.ds_cntr.ds_ccdb_process_gdi_abort_fail;
    return;
  }

  /*
   * Inconsistent local TIDs
   */
  if (p->gdi.tid_local != pbuf_cc->localTid)
  {
    ++meas.ds_cntr.ds_us_tid_local_invalid;
    string_inconsistent_tid (buffer, me, "local", p->gdi.tid_local, pbuf_cc->localTid);
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", buffer));

    ++meas.ds_cntr.ds_ccdb_process_gdi_abort_fail;
    return;
  }

  /*
   * Event can't occur in current ASM state
   */
  asm_event = EVENT_CCAS_RX_GDI_ABORT;
  if (check_asm_state (p, asm_event) != SUCCESS)
  {
    XPRINT_INVALID_STATE_CCAS;

    ++meas.ds_cntr.ds_ccdb_process_gdi_abort_fail;
    return;
  }

  /*
   * Valid US.  This is a GDI-Abort message from the ISCP (i.e., GDI package
   * type is ABORT).  Do NOT cancel any timers.
   */

  /*
   * Failed to update US data
   */
  if (user_session_save (pbuf_cc, p) != SUCCESS)
  {
    XPRINT_FAILED ("user_session_save");

    /*
     * Despite unlikely failure, just continue.  Code sniplet is copied from
     * function user_session_save ().
     */

    /*
     * Save package type
     */
    p->gdi.package_type = pbuf_cc->packageType;

    /*
     * Clear local & remote TIDs
     */
    p->gdi.tid_local = 0;
    p->gdi.tid_remote = 0;
    p->gdi.tid_remote_asr = 0;
    p->gdi.tid_remote_rar = 0;
  }

  /*
   * Failed to update ASM state
   */
  asm_state_prev = p->call_context.asm_state;
  if ((p->call_context.asm_state = set_asm_state (p, asm_event)) < 0)
  {
    XPRINT_FAILED_SET_ASM_STATE;

    /*
     * Despite unlikely failure, don't return; just continue.  US can only
     * be in the same state after EVENT_CCAS_RX_GDI_ABORT event.
     */
    p->call_context.asm_state = asm_state_prev;
  }

  /*
   * Update CC stats
   */
  ++p->call_context.ref_count;
  p->call_context.time_last = time (NULL);

  user_session_print (p, debug_mask[DEBUG_MSG_GDI_EXIT], me);
  ++meas.ds_cntr.ds_ccdb_process_gdi_abort_succ;
}

/*
 * Process GDI-DIM_Close message
 */
void process_gdi_dim_close (DiamCtx_t *pbuf_cc)
{
  char me[] = "process_gdi_dim_close";

  struct user_session *p;
  ulong id;
  short asm_event, asm_state_prev;
  char buffer[256];

  XPRINT_ENTERED;

  if (! pbuf_cc)  /* NULL DiamCtx */
  {
    ++meas.ds_cntr.ds_dc_null;
    XPRINT_NULL ("DiamCtx", pbuf_cc);

    ++meas.ds_cntr.ds_ccdb_process_gdi_dim_close_fail;
    return;
  }

  ++meas.ds_cntr.ds_msg_gdi_dim_close_rcvd;
  diamctx_print (pbuf_cc, debug_mask[DEBUG_MSG_CLOSE_ENTER], me);

  /*
   * GDI package type should've been set to RESPONSE
   */
  switch (pbuf_cc->packageType)
  {
    case PACKAGE_TYPE_RESPONSE:

      ++meas.ds_cntr.ds_pkg_gdi_end_rcvd;
      break;

    case PACKAGE_TYPE_REQUEST:

      ++meas.ds_cntr.ds_pkg_gdi_begin_rcvd;

    case PACKAGE_TYPE_CONV:

      ++meas.ds_cntr.ds_pkg_gdi_continue_rcvd;

    case PACKAGE_TYPE_ABORT:

      ++meas.ds_cntr.ds_pkg_gdi_abort_rcvd;

    default:

      ++meas.ds_cntr.ds_pkg_gdi_invalid_rcvd;
      FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: Invalid packageType 0x%x (%s)\n", me, pbuf_cc->packageType, package_type_itoa (pbuf_cc->packageType)));

      /*
       * Despite error, just continue because US is about to be removed
       */
  }

  /*
   * Failed to determine US ID from GDI local TID
   */
  id = (pbuf_cc->localTid & MASK_TID_CC) >> SHIFT_TID_CC;
  if (! USER_SESSION_ID_VALID (id))
  {
    ++meas.ds_cntr.ds_dc_tid_local_invalid;
    XPRINT_INVALID_US_ID (pbuf_cc->localTid);

    ++meas.ds_cntr.ds_ccdb_process_gdi_dim_close_fail;
    return;
  }
  p = &fepmem_list_user_session[ID_TO_INDEX (id)];
  user_session_print (p, debug_mask[DEBUG_MSG_CLOSE_ENTER], me);

  if (! p->head)  /* NULL US list */
  {
    ++meas.ds_cntr.ds_us_list_null;
    XPRINT_NULL ("US list", p->head);

    ++meas.ds_cntr.ds_ccdb_process_gdi_dim_close_fail;
    return;
  }

  /*
   * Inconsistent local TIDs
   */
  if (p->gdi.tid_local != pbuf_cc->localTid)
  {
    ++meas.ds_cntr.ds_us_tid_local_invalid;
    string_inconsistent_tid (buffer, me, "local", p->gdi.tid_local, pbuf_cc->localTid);
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", buffer));

    ++meas.ds_cntr.ds_ccdb_process_gdi_dim_close_fail;
    return;
  }

  /*
   * Cancel Tg timer
   */
  callout_unset (&p->call_context.tg_id);

  /*
   * Event can't occur in current ASM state
   */
  asm_event = EVENT_CCAS_RX_GDI_DIM_CLOSE;
  if (check_asm_state (p, asm_event) != SUCCESS)
  {
    XPRINT_INVALID_STATE_CCAS;

    ++meas.ds_cntr.ds_ccdb_process_gdi_dim_close_fail;
    return;
  }

  /*
   * Valid US, state, & event.  This is a GDI-Dim_Close message from the ISCP.
   */

  /*
   * Failed to update US data
   */
  if (user_session_save (pbuf_cc, p) != SUCCESS)
  {
    XPRINT_FAILED ("user_session_save");

    /*
     * Despite unlikely failure, just continue because US is about to be
     * removed.  Code sniplet is copied from function user_session_save ().
     */

    /*
     * Save package type
     */
    p->gdi.package_type = pbuf_cc->packageType;

    /*
     * Clear local & remote TIDs
     */
    p->gdi.tid_local = 0;
    p->gdi.tid_remote = 0;
    p->gdi.tid_remote_asr = 0;
    p->gdi.tid_remote_rar = 0;
  }

  /*
   * Failed to update ASM state
   */
  asm_state_prev = p->call_context.asm_state;
  if ((p->call_context.asm_state = set_asm_state (p, asm_event)) < 0)
  {
    XPRINT_FAILED_SET_ASM_STATE;

    /*
     * Despite unlikely failure, don't return; just continue.  US can only
     * be in STATE_CCAS_IDLE state after EVENT_CCAS_RX_GDI_DIM_CLOSE event.
     */
    p->call_context.asm_state = STATE_CCAS_IDLE;
  }

  /*
   * Update CC stats
   */
  ++p->call_context.ref_count;
  p->call_context.time_last = time (NULL);

  user_session_print (p, debug_mask[DEBUG_MSG_CLOSE_EXIT], me);
  XPRINT_US_REMOVE;

  if (p = user_session_remove (&p))
  {
    XPRINT_FAILED_US_REMOVE;
  }

  ++meas.ds_cntr.ds_ccdb_process_gdi_dim_close_succ;
}

/*
 * Process US remove message
 */
void process_us_remove (DiamCtx_t *pbuf_cc)
{
  char me[] = "process_us_remove";

  struct user_session *p;
  ulong id;
  char buffer[256];

  XPRINT_ENTERED;

  if (! pbuf_cc)  /* NULL DiamCtx */
  {
    ++meas.ds_cntr.ds_dc_null;
    XPRINT_NULL ("DiamCtx", pbuf_cc);

    ++meas.ds_cntr.ds_ccdb_process_us_remove_fail;
    return;
  }

  ++meas.ds_cntr.ds_msg_us_remove_rcvd;
  diamctx_print (pbuf_cc, debug_mask[DEBUG_MSG_USRM_ENTER], me);

  /*
   * Failed to determine US ID from GDI local TID
   */
  id = (pbuf_cc->localTid & MASK_TID_CC) >> SHIFT_TID_CC;
  if (! USER_SESSION_ID_VALID (id))
  {
    ++meas.ds_cntr.ds_dc_tid_local_invalid;
    XPRINT_INVALID_US_ID (pbuf_cc->localTid);

    ++meas.ds_cntr.ds_ccdb_process_us_remove_fail;
    return;
  }
  p = &fepmem_list_user_session[ID_TO_INDEX (id)];
  user_session_print (p, debug_mask[DEBUG_MSG_USRM_ENTER], me);

  if (! p->head)  /* NULL US list */
  {
    ++meas.ds_cntr.ds_us_list_null;
    XPRINT_NULL ("US list", p->head);

    ++meas.ds_cntr.ds_ccdb_process_us_remove_fail;
    return;
  }

  /*
   * Inconsistent local TIDs
   */
  if (p->gdi.tid_local != pbuf_cc->localTid)
  {
    ++meas.ds_cntr.ds_us_tid_local_invalid;
    string_inconsistent_tid (buffer, me, "local", p->gdi.tid_local, pbuf_cc->localTid);
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s\n", buffer));

    ++meas.ds_cntr.ds_ccdb_process_us_remove_fail;
    return;
  }

  /*
   * Cancel Tg timer
   */
  callout_unset (&p->call_context.tg_id);

  /*
   * Valid US, state (can occur in any state), & event.  This is a US remove
   * message from DGC (who received from TDG).
   */

  /*
   * Update CC stats
   */
  ++p->call_context.ref_count;
  p->call_context.time_last = time (NULL);

  user_session_print (p, debug_mask[DEBUG_MSG_USRM_EXIT], me);
  XPRINT_US_REMOVE;

  if (p = user_session_remove (&p))
  {
    XPRINT_FAILED_US_REMOVE;
  }

  ++meas.ds_cntr.ds_ccdb_process_us_remove_succ;
}

/*
 * Process qmsg from DGC
 */
void process_diamctx (fep_qmsg_t qmsg, int flag_queued)
{
  char me[] = "process_diamctx";

  fep_buffer_t *pbuf;
  DiamCtx_t *pbuf_cc;
  struct user_session *p;
  int rv, parm;
  ulong q2, q3;

  XPRINT_ENTERED;

  if (! (pbuf = parm_to_buf (qmsg[1])))  /* NULL buffer */
  {
    ++meas.ds_cntr.ds_api_parm_to_buf_fail;
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: NULL buffer 0x%x, qmsg[1] 0x%x, flag_queued %d (%s)\n", me, pbuf, qmsg[1], flag_queued, FLAG_ITOA (flag_queued)));

    ++meas.ds_cntr.ds_ccdb_process_diamctx_fail;
    return;
  }
  pbuf_cc = (DiamCtx_t *) pbuf;
  p = NULL;

  XPRINT_DIAMCTX_RECV;

  /*
   * Process qmsg based on DiamCtx type
   */
  switch (pbuf_cc->diamctx_type)
  {
    case DIAMCTX_DIAM:

      /*
       * Diameter message was queued
       */
      if ((rv = process_diam (qmsg, flag_queued, pbuf_cc, &p)) == SUCCESS_QUEUED)
      {
        diamctx_print (pbuf_cc, debug_mask[DEBUG_MSG_DIAM_EXIT], me);
	user_session_print (p, debug_mask[DEBUG_MSG_DIAM_EXIT], me);

        return;
      }

      /*
       * Diameter message was processed
       */
      if (rv != SUCCESS)
      {
	XPRINT_FAILED_PROC_DIAMCTX ("process_diam");

        /*
         * Failed to construct DGC DiamCtx qmsg
         */
        if (qmsg_diamctx_construct (pbuf_cc, p, TRUE) != SUCCESS)
          XPRINT_FAILED ("qmsg_diamctx_construct");
      }

      diamctx_print (pbuf_cc, debug_mask[DEBUG_MSG_DIAM_EXIT], me);
      XPRINT_DIAMCTX_XMIT;

      if (p)
	user_session_print (p, debug_mask[DEBUG_MSG_DIAM_EXIT], me);

      /*
       * Failed to convert (& release) buffer to parameter
       */
      if ((parm = buf_to_parm (pbuf)) < 0)
      {
        ++meas.ds_cntr.ds_api_buf_to_parm_fail;
	XPRINT_FAILED_BUF_TO_PARM;

        ++meas.ds_cntr.ds_ccdb_process_diamctx_fail;
        return;
      }

      /*
       * Tell sender about processing result
       */
      if ((rv = tell_dgc (qmsg[0], parm, ((rv == SUCCESS) ? (SUCCESS) : (FAILURE)), rv)) != SUCCESS)
      {
        ++meas.ds_cntr.ds_api_tell_dgc_fail;
	XPRINT_FAILED_TELL ("tell_dgc");

        /*
         * Free memory associated with DiamCtx
         */
        fep_free_buffer (parm_to_buf (parm));
        ++meas.ds_cntr.ds_api_fep_free_buffer;

        ++meas.ds_cntr.ds_ccdb_process_diamctx_fail;
      }
      else
      {
        ++meas.ds_cntr.ds_api_tell_dgc_succ;
        XPRINT_QMSG_XMIT (qmsg[0]);

        ++meas.ds_cntr.ds_ccdb_process_diamctx_succ;
      }

      return;

    case DIAMCTX_GDI:

      /*
       * Received a GDI-Abort message from the ISCP (i.e., GDI package type is
       * ABORT)
       */
      if (pbuf_cc->packageType == PACKAGE_TYPE_ABORT)
      {
        process_gdi_abort (pbuf_cc);

        /*
         * DGC does not want to know about processing result
         */
        ++meas.ds_cntr.ds_ccdb_process_diamctx_succ;
        break;  /* Break to free memory associated with DiamCtx */
      }

      if ((rv = process_gdi (qmsg, pbuf_cc, &p)) != SUCCESS)
      {
	XPRINT_FAILED_PROC_DIAMCTX ("process_gdi");

        /*
         * Failed to construct DGC DiamCtx qmsg
         */
        if (qmsg_diamctx_construct (pbuf_cc, p, TRUE) != SUCCESS)
          XPRINT_FAILED ("qmsg_diamctx_construct");
      }

      diamctx_print (pbuf_cc, debug_mask[DEBUG_MSG_GDI_EXIT], me);
      XPRINT_DIAMCTX_XMIT;

      if (p)
	user_session_print (p, debug_mask[DEBUG_MSG_GDI_EXIT], me);

      /*
       * Failed to convert (& release) buffer to parameter
       */
      if ((parm = buf_to_parm (pbuf)) < 0)
      {
        ++meas.ds_cntr.ds_api_buf_to_parm_fail;
	XPRINT_FAILED_BUF_TO_PARM;

        ++meas.ds_cntr.ds_ccdb_process_diamctx_fail;
        return;
      }

      if ((int) qmsg[2] == FAILURE)
      {
        q2 = qmsg[2];  /* DGC's passed in qmsg processing result */
        q3 = qmsg[3];  /* DGC's passed in Diameter Result-Code AVP */
      }
      else
      {
        q2 = ((rv == SUCCESS) ? (SUCCESS) : (FAILURE));
        q3 = rv;
      }

      /*
       * Tell sender about processing result
       */
      if ((rv = tell_dgc (qmsg[0], parm, q2, q3)) != SUCCESS)
      {
        ++meas.ds_cntr.ds_api_tell_dgc_fail;
        XPRINT_FAILED_TELL ("tell_dgc");

        /*
         * Free memory associated with DiamCtx
         */
        fep_free_buffer (parm_to_buf (parm));
        ++meas.ds_cntr.ds_api_fep_free_buffer;

        ++meas.ds_cntr.ds_ccdb_process_diamctx_fail;
      }
      else
      {
        ++meas.ds_cntr.ds_api_tell_dgc_succ;
        XPRINT_QMSG_XMIT (qmsg[0]);

        ++meas.ds_cntr.ds_ccdb_process_diamctx_succ;
      }

      /*
       * Process queued Diameter message, if any
       */
      process_dgc_qmsg (p);

      return;

    case DIAMCTX_GDI_DIM_CLOSE:

      process_gdi_dim_close (pbuf_cc);

      /*
       * DGC does not want to know about processing result
       */
      ++meas.ds_cntr.ds_ccdb_process_diamctx_succ;
      break;  /* Break to free memory associated with DiamCtx */

    case DIAMCTX_US_REMOVE:

      process_us_remove (pbuf_cc);

      /*
       * DGC does not want to know about processing result
       */
      ++meas.ds_cntr.ds_ccdb_process_diamctx_succ;
      break;  /* Break to free memory associated with DiamCtx */

    default:

      ++meas.ds_cntr.ds_dc_invalid;
      XPRINT_INVALID_DIAMCTX_TYPE;
      diamctx_print (pbuf_cc, debug_mask[DEBUG_MSG_MISC_ENTER], me);

      ++meas.ds_cntr.ds_ccdb_process_diamctx_fail;
      break;  /* Break to free memory associated with DiamCtx */
  }

  /*
   * Free memory associated with DiamCtx
   */
  fep_free_buffer (pbuf);
  ++meas.ds_cntr.ds_api_fep_free_buffer;
}

/*
 * Process queued Diameter message
 */
void process_dgc_qmsg (struct user_session *p)
{
  char me[] = "process_dgc_qmsg";

  struct dgc_qmsg *q;

  XPRINT_ENTERED;

  if (! p)  /* NULL US */
    return;

  if (! (q = p->diameter.dgc_qmsg.tqh_first))  /* NULL DGC qmsg */
    return;

  user_session_print (p, debug_mask[DEBUG_MSG_QUEUE_ENTER], me);

  /*
   * Process queued Diameter message
   */
  process_diamctx (q->qmsg, TRUE);

  /*
   * Remove DGC qmsg from list & free memory
   */
  dgc_qmsg_remove (&q);

  /*
   * Update CC stats
   */
  ++p->call_context.ref_count;
  p->call_context.time_last = time (NULL);

  user_session_print (p, debug_mask[DEBUG_MSG_QUEUE_EXIT], me);
  ++meas.ds_cntr.ds_ccdb_process_dgc_qmsg_succ;
}

/*
 * Process key set from TDG
 */
void process_key_set (fep_qmsg_t qmsg)
{
  char me[] = "process_key_set";

  fep_buffer_t *pbuf;
  int length;

  XPRINT_ENTERED;

  if (! (pbuf = parm_to_buf (qmsg[1])))  /* NULL buffer */
  {
    ++meas.ds_cntr.ds_api_parm_to_buf_fail;
    FEP_XPRINT (debug_mask[DEBUG_DSM], ("%s: NULL buffer 0x%x, qmsg[1] 0x%x\n", me, pbuf, qmsg[1]));

    ++meas.ds_cntr.ds_ccdb_process_key_set_fail;
    return;
  }

  length = FEP_BUF_DATA_LEN (pbuf);
  if (length < SUB_ID_LEN_MIN)
  {
    FEP_XPRINT (debug_mask[DEBUG_DSM_1], ("%s: Key length %d < SUB_ID_LEN_MIN %d, key length defaulting to %d\n", me, length, SUB_ID_LEN_MIN, SUB_ID_LEN_MIN));
    length = SUB_ID_LEN_MIN;
  }
  else
    if (length > SUB_ID_LEN_MAX)
    {
      FEP_XPRINT (debug_mask[DEBUG_DSM_1], ("%s: Key length %d > SUB_ID_LEN_MAX %d, key length defaulting to %d\n", me, length, SUB_ID_LEN_MAX, SUB_ID_LEN_MAX));
      length = SUB_ID_LEN_MAX;
    }

  /*
   * Initialize trap & save key
   */
  memset (&ccdb.trap, 0, sizeof (ccdb.trap));

  ccdb.trap.time_start = time (NULL);
  strncpy (ccdb.trap.sub_id, (char *) pbuf, length);
  ccdb.trap.us = NULL;

  /*
   * Free memory associated with key
   */
  fep_free_buffer (pbuf);
  ++meas.ds_cntr.ds_api_fep_free_buffer;

  ++meas.ds_cntr.ds_ccdb_process_key_set_succ;
}

/*
 * Process key clear from TDG
 */
void process_key_clear (fep_qmsg_t qmsg)
{
  char me[] = "process_key_clear";

  XPRINT_ENTERED;

  memset (&ccdb.trap, 0, sizeof (ccdb.trap));
  ++meas.ds_cntr.ds_ccdb_process_key_clear_succ;
}

/*
 * Process key display request from TDG
 */
void process_key_display_req (fep_qmsg_t qmsg)
{
  char me[] = "process_key_display_req";

  char buffer[4096], *b;
  struct user_session *p;
  fep_buffer_t *pbuf;
  int parm, rv, size, flag_incremented;

  XPRINT_ENTERED;

  b = buffer;
  b += sprintf (b, "Time trap created          %lu (%s)\n", ccdb.trap.time_start, time_itoa (ccdb.trap.time_start));
  b += sprintf (b, "Key (Subscription-Id-Data) %s\n", ccdb.trap.sub_id);
  b += sprintf (b, "User session               0x%x\n", ccdb.trap.us);

  if (p = ccdb.trap.us)
  {
    b += sprintf (b, "\n");
    b += sprintf (b, "%s", user_session_sprint (ccdb.trap.us));
  }

  size = b - buffer + 1;
  flag_incremented = FALSE;

  /*
   * Failed to allocate buffer
   */
  if (! (pbuf = fep_alloc_buffer (size)))
  {
    ++meas.ds_cntr.ds_api_fep_alloc_buffer_fail;
    XPRINT_FAILED_FEP_ALLOC_BUFFER (size);

    if (! flag_incremented)
    {
      ++meas.ds_cntr.ds_ccdb_process_key_display_req_fail;
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
     * Copy trap data
     */
    FEP_BUF_DATA_OFF (pbuf) = 0;
    FEP_BUF_DATA_LEN (pbuf) = size;
    memcpy (FEP_BUF_DATA (pbuf), buffer, FEP_BUF_DATA_LEN (pbuf));
  }

  /*
   * Failed to convert (& release) buffer to parameter
   */
  if ((parm = buf_to_parm (pbuf)) < 0)
  {
    ++meas.ds_cntr.ds_api_buf_to_parm_fail;
    XPRINT_FAILED_BUF_TO_PARM;

    /*
     * Free memory associated with trap data
     */
    fep_free_buffer (pbuf);
    ++meas.ds_cntr.ds_api_fep_free_buffer;

    if (! flag_incremented)
    {
      ++meas.ds_cntr.ds_ccdb_process_key_display_req_fail;
      flag_incremented = TRUE;
    }
  }

  /*
   * Tell TDG about trap data
   */
  if ((rv = tell_fem (MSG_FEM_KEY_DISPLAY_ACK, qmsg[1], parm, UNINITIALIZED)) != SUCCESS)
  {
    ++meas.ds_cntr.ds_api_tell_tdg_fail;
    XPRINT_FAILED_TELL ("tell_tdg");

    /*
     * Free memory associated with trap data
     */
    fep_free_buffer (parm_to_buf (parm));
    ++meas.ds_cntr.ds_api_fep_free_buffer;

    if (! flag_incremented)
    {
      ++meas.ds_cntr.ds_ccdb_process_key_display_req_fail;
      flag_incremented = TRUE;
    }
  }
  else
  {
    ++meas.ds_cntr.ds_api_tell_tdg_succ;
    XPRINT_QMSG_XMIT (MSG_FEM_RET_COUNTERS);

    if (! flag_incremented)
    {
      ++meas.ds_cntr.ds_ccdb_process_key_display_req_succ;
      flag_incremented = TRUE;
    }
  }
}

#if 0  /* TO DO */
/*
- Support queueing of RAA messages in "Process Session Request" state.
*/
#endif
