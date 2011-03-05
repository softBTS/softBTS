#include <common.h>
#include <softbts.h>
#include <logger.h>
#include <bts_init.h>
#include <bts_nm.h>
#include <abis_rsl.h>

static int softbts_msg(char *msg, int sd);
static int state_changed_event_report(int socketd, char *msg,
                                            u_int8_t obj_class,
                                            u_int8_t bts_nr,
                                            u_int8_t trx_nr,
                                            u_int8_t ts_nr, 
                                            u_int8_t op_state, 
                                            u_int8_t av_status, 
                                            u_int8_t adm_state);

static int initial_state_report(int sd, char *msg);

int sw_activate_request(int socketd, char *msg,
                                     u_int8_t obj_class,
                                     u_int8_t bts_nr,
                                     u_int8_t trx_nr,
                                     u_int8_t ts_nr);

static void oml_msg(char *msg, int sd);

static int sw_activate_ack(int socketd, char *msg,
                                 u_int8_t obj_class,
                                 u_int8_t bts_nr,
                                 u_int8_t trx_nr,
                                 u_int8_t ts_nr);

static int opstart_ack(int socketd, char *msg,
                                 u_int8_t obj_class,
                                 u_int8_t bts_nr,
                                 u_int8_t trx_nr,
                                 u_int8_t ts_nr);

static int sw_activated_report(int socketd, char *msg,
                                 u_int8_t obj_class,
                                 u_int8_t bts_nr,
                                 u_int8_t trx_nr,
                                 u_int8_t ts_nr);

static int bts_get_attr(int socketd, char *msg);

static void chan_get_attr(int socketd, char *msg);

static int bts_set_attr_ack(int socketd, char *msg,
                                 u_int8_t obj_class,
                                 u_int8_t bts_nr,
                                 u_int8_t trx_nr,
                                 u_int8_t ts_nr);

static int bts_get_admin_state(int socketd, char *msg, u_int8_t *state);


static int chg_adm_state_ack(int socketd, char *msg,
                                 u_int8_t obj_class,
                                 u_int8_t bts_nr,
                                 u_int8_t trx_nr,
                                 u_int8_t ts_nr);



extern int bts_id_resp;
extern int sw_site_manager;

extern int sw_bts;
extern u_int8_t bts_admin_state;
extern u_int8_t bts_opstate;

extern u_int8_t manager_admin_state;
extern u_int8_t manager_opstate;
extern u_int8_t baseb_transc_admin_state;
extern u_int8_t baseb_transc_opstate;
extern u_int8_t radio_carrier_admin_state;
extern u_int8_t radio_carrier_opstate;
extern u_int8_t channel_admin_state[8];
extern u_int8_t channel_opstate[8];

extern socket_connection_t rsl;
extern socket_connection_t oml;

static const u_int8_t ping[] = { 0, 1, PROTO_SOFTBTS, MSG_PING };
static const u_int8_t pong[] = { 0, 1, PROTO_SOFTBTS, MSG_PONG };
static const u_int8_t id_ack[] = { 0, 1, PROTO_SOFTBTS, MSG_ID_ACK };
static const u_int8_t id_req[] = { 0, 17, PROTO_SOFTBTS, MSG_ID_GET,
                                        0x01, IDTAG_UNIT,
                                        0x01, IDTAG_MACADDR,
                                        0x01, IDTAG_LOCATION1,
                                        0x01, IDTAG_LOCATION2,
                                        0x01, IDTAG_EQUIPVERS,
                                        0x01, IDTAG_SWVERSION,
                                        0x01, IDTAG_UNITNAME,
                                        0x01, IDTAG_SERNR,
                                };

static u_int8_t id_resp[] = {0, 12, PROTO_SOFTBTS, MSG_ID_RESP, 
                              0x09,IDTAG_UNIT,0x08,'1', '8', '0', '0', '/' , '0', '/', '0'
                              };


static u_int8_t init_rsl_link[] = {0, 25, 0xff,
                                    ABIS_OM_MDISC_MANUF,    /* discriminator            */
                                    ABIS_OM_PLACEMENT_ONLY, /* placement                */
                                    0x00,                   /* secuence                 */
/* length deri ne fund te msg */    8,                      /* length                  */
/* http://openbsc.gnumonks.org/trac/wiki/nanoBTS */     
                                    12,                      /* length of com.  string  */ 
                                    'c','o','m','.','i','p','a','c','c','e','s','s',
                                    NM_MT_SOFTBTS_RSL_CONNECT_ACK,
                                    NM_OC_BASEB_TRANSC,
                                    0x00,
                                    0x00,
                                    NM_OC_NULL, 
                                        NM_ATT_SOFTBTS_DST_IP_PORT,
                                        0x0B,
                                        0xBB,
                                   };
#if 0
static u_int8_t init_rsl_link[] = {0, 32, 0xff,
                                    ABIS_OM_MDISC_MANUF,    /* discriminator            */
                                    ABIS_OM_PLACEMENT_ONLY, /* placement                */
                                    0x00,                   /* secuence                 */
/* length deri ne fund te msg */    12,                      /* length                  */
/* http://openbsc.gnumonks.org/trac/wiki/nanoBTS */     
                                    12,                      /* length of com.  string  */ 
                                    'c','o','m','.','i','p','a','c','c','e','s','s',
                                    NM_MT_SOFTBTS_RSL_CONNECT_ACK,
                                    NM_OC_BASEB_TRANSC,
                                    0x00,
                                    0x00,
                                    NM_OC_NULL,
					NM_ATT_SOFTBTS_STREAM_ID,
					0x00, 

                                        NM_ATT_SOFTBTS_DST_IP_PORT,
                                        0x0B,
                                        0xBB,
				        NM_ATT_SOFTBTS_DST_IP,
                                        0xc0,
                                        0xa8,
					0,
					0x1a,
                                    };
#endif

int receive_msg( char *msg, int sd)
{
	msg_head_t *ph;      /* pointer to header of message */
	int len, rc, ret =0;
	u_int8_t  *pdata;      /* pointer to data */


	/* Read 3-byte from header */ 

	ph = (msg_head_t *) msg;  /* initialise pointer to message buffer */
	ret = recv(sd, ph, 3, 0);

	/* Next read <len> bytes of message as specified in header of message */
	len = ntohs(ph->len);
	pdata = &ph->data[0];
	rc = recv(sd, pdata, len, 0);
	if (rc < len ){
		printf("short read ret = %d rc = %d\n",ret,rc);
	}

	

	switch (ph->proto) {

	case PROTO_SOFTBTS:
                debug(DHX,"SOFTBTS RX: %s\n",hexdump(msg,len+3));
		softbts_msg(msg, sd);
		break;

	case PROTO_OML:
		debug(DHX,"OML RX: %s\n",hexdump(msg,len+3));
		oml_msg(msg, sd);
		break;

	case PROTO_RSL:
		debug(DHX,"RSL RX: %s\n",hexdump(msg,len+3));
		rsl_msg(msg, sd);
		break;
	default:
		debug(DNM,"Unknown protocol proto=0x%02x\n",ph->proto);
		logger(WARNING,LOCATION,"Unknown protocol proto=0x%02x\n",ph->proto);
		break;
	}
	/* Detecting closed sockets */
	if( (ret == 0 ) & (rc == 0)){
	return 0;
	}
	else{
	return rc;
	}
}

static int softbts_msg(char *msg, int sd)
{
	int rc;
	msg_head_t *ph;
	ph = (msg_head_t *) msg;
	u_int8_t cmd = ph->data[0];
	
	switch (cmd) {

	case MSG_PING:
		send(sd, pong, sizeof(pong),0);
		if( rc < 0) {
                  logger(ERROR,LOCATION,"send() failed\n");
		  kill_background();
                }
		logger(INFO,LOCATION,"BSC->BTS ping,  BSC<-BTS socket %x pong\n",sd);
		break;

	case MSG_PONG:
		logger(INFO,LOCATION,"BSC->BTS pong, socket %x\n",sd);
		break;

	case MSG_ID_GET:
	printf("id resp send from sd %d\n",sd);		
		rc = send(sd, id_resp, sizeof(id_resp),0);
		if( rc < 0) {
		  logger(ERROR,LOCATION,"send() BSC<-BTS id_resp failed\n");
		  kill_background();
		}
		logger(INFO,LOCATION,"BSC->BTS id_req, BSC<-BTS id_resp socket %x\n",sd);
		debug(DNM,"BSC->BTS id_req, BSC<-BTS id_resp socket %x\n",sd);
		if(!bts_id_resp){
		  initial_state_report(sd, msg); 
		  bts_id_resp = TRUE;

		  /* SW Activate Request - SITE MANAGER */
	          rc =  sw_activate_request(sd,msg,
                              NM_OC_SITE_MANAGER,
                              NM_OC_NULL,
                              NM_OC_NULL,
                              NM_OC_NULL);
        	  logger(INFO,LOCATION,"BTS->BSC Software Activate Request for NM_OC_SITE_MANAGER = OC(%x)\n",NM_OC_SITE_MANAGER);
		  sw_site_manager = NM_MT_SW_ACT_REQ;
		}
		break;

	default:
		logger(INFO,LOCATION,"Unknown softbts-proto comand %x\n",cmd);
		break;
	}
	return 0;
}



/* 6.2.5 SW Activate Request
This message shall be sent by the BTS when the resource represented
by the object instance (Site Manager, BTS, Radio Carrier, or Baseband
Transceiver) has started up. The initialization of mentioned object
instance shall be started with software activation, which may include
software download continuing with attribute setting. */

int sw_activate_request(int socketd, char *msg,
                                     u_int8_t obj_class,
                                     u_int8_t bts_nr,
                                     u_int8_t trx_nr,
                                     u_int8_t ts_nr)
{
        int rc;
  	char *src = SW_FILE_ID, *dst;

        msg_head_t       *head;
        om_hdr_t         *om_hdr;
        fom_hdr_t        *fom_hdr;
        sw_config_t      *sw_config;
        file_id_t        *file_id;
        file_ver_t       *file_ver;

        head    = (msg_head_t *) msg;
        om_hdr  = (om_hdr_t   *)&head->data[0];
        fom_hdr = (fom_hdr_t   *)&om_hdr->data[0];

        sw_config = (sw_config_t *)&fom_hdr->data[0];
        file_id = (file_id_t *)&sw_config->data[1];

        head->len   = 0x2500;                        /* 37 */
        head->proto = 0xff;
          om_hdr->mdisc =  ABIS_OM_MDISC_FOM;
          om_hdr->placement = ABIS_OM_PLACEMENT_ONLY;
          om_hdr->sequence = 0x00;
          om_hdr->length = 0x21;                     /* 33 */

          fom_hdr->msg_type  = NM_MT_SW_ACT_REQ;
          fom_hdr->obj_class = obj_class;
          fom_hdr->obj_inst.bts_nr = bts_nr;
          fom_hdr->obj_inst.trx_nr = trx_nr;
          fom_hdr->obj_inst.ts_nr  = ts_nr;

          sw_config->atribute = NM_ATT_SW_CONFIG;
          sw_config->len = 0x0019;                   /* 25 */   
          sw_config->data[0] = NM_ATT_SW_DESCR;

          file_id->atribute = NM_ATT_FILE_ID;
          file_id->len = sizeof(SW_FILE_ID) -1;     /* without null byte */
          dst = &(file_id->data[0]);
          strcpy(dst,src);
          dst = dst + file_id->len;

          file_ver = (file_ver_t *)dst;
          file_ver->atribute = NM_ATT_FILE_VERSION;
          file_ver->len = sizeof(SW_FILE_VER);     /* with null byte */
          src = SW_FILE_VER;

          dst = &(file_ver->data[0]);

          strcpy(dst,src);
          file_id->len = htons(file_id->len);
          file_ver->len = htons(file_ver->len);
          sw_config->len = htons( sw_config->len);

             rc = send(socketd,msg,ntohs(head->len)+3, 0);
             if (rc < 0){
             logger(INFO,LOCATION,"send() failed\n");
             }
          return rc;

} 

static int sw_activated_report(int socketd, char *msg,
                                 u_int8_t obj_class,
                                 u_int8_t bts_nr,
                                 u_int8_t trx_nr,
                                 u_int8_t ts_nr)
{
        int rc;
        msg_head_t *head;
        om_hdr_t   *om_hdr;
        fom_hdr_t  *fom_hdr;

        head    = (msg_head_t *)msg;
        om_hdr  = (om_hdr_t   *)&head->data[0];
        fom_hdr = (fom_hdr_t   *)&om_hdr->data[0];

        head->len   = 0x0900;
        head->proto = 0xff;
          om_hdr->mdisc =  ABIS_OM_MDISC_FOM;
          om_hdr->placement = ABIS_OM_PLACEMENT_ONLY;
          om_hdr->sequence = 0x00;
          om_hdr->length = 0x05;
             fom_hdr->msg_type  = NM_MT_SW_ACTIVATED_REP;
             fom_hdr->obj_class = obj_class;
             fom_hdr->obj_inst.bts_nr = bts_nr;
             fom_hdr->obj_inst.trx_nr = trx_nr;
             fom_hdr->obj_inst.ts_nr  = ts_nr;

             rc = send(socketd,msg,ntohs(head->len)+3, 0);
             if (rc < 0){
             logger(ERROR,LOCATION,"send() failed\n");
             }
          return rc;
}

/* Meaning of Ack message: The BTS has reset the operational state of the specified object to "enabled"
state.
No BTS function is required to be responsible for testing the operability of the identified resource as
a consequence of this message.
Prior to this message being issued, all necessary physical and logical
preparations (such as repair of equipment, software downloading, parameter setting, etc., as needed)
are expected to have been completed. If the object is in fact not ready to be in an enabled state,
the object will be in a fault condition as a consequence of this message, and the condition shall be
handled by the object's normal fault handling function as the condition is detected.
*/

static int opstart_ack(int socketd, char *msg,
                                 u_int8_t obj_class,
                                 u_int8_t bts_nr,
                                 u_int8_t trx_nr,
                                 u_int8_t ts_nr)
{
        int rc;
        msg_head_t *head;
        om_hdr_t   *om_hdr;
        fom_hdr_t  *fom_hdr;

        head    = (msg_head_t *)msg;
        om_hdr  = (om_hdr_t   *)&head->data[0];
        fom_hdr = (fom_hdr_t   *)&om_hdr->data[0];

        head->len   = 0x0900;
        head->proto = 0xff;
          om_hdr->mdisc =  ABIS_OM_MDISC_FOM;
          om_hdr->placement = ABIS_OM_PLACEMENT_ONLY;
          om_hdr->sequence = 0x00;
          om_hdr->length = 0x05;
             fom_hdr->msg_type  = NM_MT_OPSTART_ACK;
             fom_hdr->obj_class = obj_class;
             fom_hdr->obj_inst.bts_nr = bts_nr;
             fom_hdr->obj_inst.trx_nr = trx_nr;
             fom_hdr->obj_inst.ts_nr  = ts_nr;

             rc = send(socketd,msg,ntohs(head->len)+3, 0);
             if (rc < 0){
             logger(ERROR,LOCATION,"send() failed\n");
             }
          return rc;
}


static int sw_activate_ack(int socketd, char *msg,
                                 u_int8_t obj_class,
                                 u_int8_t bts_nr,
                                 u_int8_t trx_nr,
                                 u_int8_t ts_nr)
{
        int rc;
        msg_head_t *head;
        om_hdr_t   *om_hdr;
        fom_hdr_t  *fom_hdr;

        head    = (msg_head_t *)msg;
        om_hdr  = (om_hdr_t   *)&head->data[0];
        fom_hdr = (fom_hdr_t   *)&om_hdr->data[0];

        head->len   = 0x0900;
        head->proto = 0xff;
          om_hdr->mdisc =  ABIS_OM_MDISC_FOM; 
          om_hdr->placement = ABIS_OM_PLACEMENT_ONLY;
          om_hdr->sequence = 0x00;
          om_hdr->length = 0x05;
             fom_hdr->msg_type  = NM_MT_ACTIVATE_SW_ACK;
             fom_hdr->obj_class = obj_class;
             fom_hdr->obj_inst.bts_nr = bts_nr;
             fom_hdr->obj_inst.trx_nr = trx_nr;
             fom_hdr->obj_inst.ts_nr  = ts_nr;

             rc = send(socketd,msg,ntohs(head->len)+3, 0);
             if (rc < 0){
             logger(ERROR,LOCATION,"send() failed\n");
             }
          return rc;
}








static int state_changed_event_report(int socketd, char *msg,
                                            u_int8_t obj_class,
                                            u_int8_t bts_nr,
                                            u_int8_t trx_nr,
                                            u_int8_t ts_nr,
                                            u_int8_t op_state,
                                            u_int8_t av_status,
                                            u_int8_t adm_state)
{
        int rc;
        msg_head_t   *head;
        om_hdr_t     *om_hdr;
        fom_hdr_t    *fom_hdr;
        oper_state_t *oper_state;
        avail_status_t *avail_status;
        admin_state_t  *admin_state;

        head    = (msg_head_t *) msg;
        om_hdr  = (om_hdr_t   *)&head->data[0];
        fom_hdr = (fom_hdr_t   *)&om_hdr->data[0];
        oper_state = (oper_state_t *)&fom_hdr->data[0];
        avail_status = (avail_status_t *)&oper_state->data[0];
        admin_state = (admin_state_t  *)&avail_status->data[0];

        head->len   = 0x1100;                /* 17 */
        head->proto = 0xff;
          om_hdr->mdisc =  ABIS_OM_MDISC_FOM;
          om_hdr->placement = ABIS_OM_PLACEMENT_ONLY;
          om_hdr->sequence = 0x00;
          om_hdr->length = 0x0d;            /* 13 */

          fom_hdr->msg_type  = NM_MT_STATECHG_EVENT_REP;
          fom_hdr->obj_class = obj_class;
          fom_hdr->obj_inst.bts_nr = bts_nr;
          fom_hdr->obj_inst.trx_nr = trx_nr;
          fom_hdr->obj_inst.ts_nr  = ts_nr;

          oper_state->atribute = NM_ATT_OPER_STATE;
          oper_state->oper_state = op_state;

          avail_status->atribute =  NM_ATT_AVAIL_STATUS;
          avail_status->len = 0x0100;       /* 1 */
          avail_status->avail_status = av_status;

          admin_state->atribute = NM_ATT_ADM_STATE;
          admin_state->state = adm_state;

          rc = send(socketd,msg,ntohs(head->len)+3, 0);
             if (rc < 0){
             logger(ERROR,LOCATION,"send() failed \n");
             } 
          return rc;
}


static int bts_set_attr_ack(int socketd, char *msg,
                                 u_int8_t obj_class,
                                 u_int8_t bts_nr,
                                 u_int8_t trx_nr,
                                 u_int8_t ts_nr)
{
        int rc;
        msg_head_t *head;
        om_hdr_t   *om_hdr;
        fom_hdr_t  *fom_hdr;

        head    = (msg_head_t *)msg;
        om_hdr  = (om_hdr_t   *)&head->data[0];
        fom_hdr = (fom_hdr_t   *)&om_hdr->data[0];

        head->len   = 0x0900;
        head->proto = 0xff;
          om_hdr->mdisc =  ABIS_OM_MDISC_FOM;
          om_hdr->placement = ABIS_OM_PLACEMENT_ONLY;
          om_hdr->sequence = 0x00;
          om_hdr->length = 0x05;
             fom_hdr->msg_type  = NM_MT_SET_BTS_ATTR_ACK;
             fom_hdr->obj_class = obj_class;
             fom_hdr->obj_inst.bts_nr = bts_nr;
             fom_hdr->obj_inst.trx_nr = trx_nr;
             fom_hdr->obj_inst.ts_nr  = ts_nr;

             rc = send(socketd,msg,ntohs(head->len)+3, 0);
             if (rc < 0){
             logger(ERROR,LOCATION,"send() failed\n");
             }
          return rc;
}

static int initial_state_report(int sd, char *msg)
{
	int rc, channel;
        logger(INFO,LOCATION,"BTS->BSC Reporting Initial State and Hardware Resourse Availability\n");
        rc = state_changed_event_report(sd, msg,
                                               NM_OC_SITE_MANAGER,
                                               NM_OC_NULL,
                                               NM_OC_NULL,
                                               NM_OC_NULL,
                                               NM_OPSTATE_DISABLED,
                                               NM_AVSTATE_NOT_INSTALLED,
                                               NM_STATE_LOCKED);
        debug(DNM,"OC=SITE MANAGER(%x) INST=(%x,%x,%x) STATE CHANGED: OP_STATE=(%x) AVAIL=(%x) ADM=(%x)\n",
                                               NM_OC_SITE_MANAGER, 
                                               NM_OC_NULL, 
                                               NM_OC_NULL, 
                                               NM_OC_NULL, 
                                               NM_OPSTATE_DISABLED, 
                                               NM_AVSTATE_NOT_INSTALLED, 
                                               NM_STATE_LOCKED);

        rc = state_changed_event_report(sd, msg,
                                               NM_OC_BTS,
                                               0x00,
                                               NM_OC_NULL,
                                               NM_OC_NULL,
                                               NM_OPSTATE_DISABLED,
                                               NM_AVSTATE_NOT_INSTALLED,
                                               NM_STATE_LOCKED);
        debug(DNM,"OC=BTS(%x) INST=(%x,%x,%x) STATE CHANGED: OP_STATE=(%x) AVAIL=(%x) ADM=(%x)\n",
                                               NM_OC_BTS, 
                                               0x00, 
                                               NM_OC_NULL, 
                                               NM_OC_NULL, 
                                               NM_OPSTATE_DISABLED, 
                                               NM_AVSTATE_NOT_INSTALLED, 
                                               NM_STATE_LOCKED);
        rc = state_changed_event_report(sd, msg,
                                               NM_OC_BASEB_TRANSC,
                                               0x00,
                                               0x00,
                                               NM_OC_NULL,
                                               NM_OPSTATE_DISABLED,
                                               NM_AVSTATE_NOT_INSTALLED,
                                               NM_STATE_LOCKED);
        debug(DNM,"OC=BASEBAND TRANSC(%x) INST=(%x,%x,%x) STATE CHANGED: OP_STATE=(%x) AVAIL=(%x) ADM=(%x)\n",
                                               NM_OC_BASEB_TRANSC, 
                                               0x00, 
                                               0x00, 
                                               NM_OC_NULL, 
                                               NM_OPSTATE_DISABLED, 
                                               NM_AVSTATE_NOT_INSTALLED, 
                                               NM_STATE_LOCKED);

        for(channel = 0x0; channel <= 0x07; channel++){
               rc = state_changed_event_report(sd, msg,  
                                               NM_OC_CHANNEL,
                                               0x00,
                                               0x00,
                                               channel, 
                                               NM_OPSTATE_DISABLED, 
                                               NM_AVSTATE_NOT_INSTALLED, 
                                               NM_STATE_LOCKED);
           debug(DNM,"OC=CHANNEL(%x) INST=(%x,%x,%x) STATE CHANGED: OP_STATE=(%x) AVAIL=(%x) ADM=(%x)\n",
                                               NM_OC_CHANNEL, 
                                               0x00, 
                                               0x00, 
                                               channel, 
                                               NM_OPSTATE_DISABLED, 
                                               NM_AVSTATE_NOT_INSTALLED, 
                                               NM_STATE_LOCKED);


         }
        rc = state_changed_event_report(sd, msg,
                                               NM_OC_RADIO_CARRIER,
                                               0x00,
                                               0x00,
                                               NM_OC_NULL,
                                               NM_OPSTATE_DISABLED,
                                               NM_AVSTATE_NOT_INSTALLED,
                                               NM_STATE_LOCKED);
        debug(DNM,"OC= RADIO CARRIER (%x) INST=(%x,%x,%x) STATE CHANGED: OP_STATE=(%x) AVAIL=(%x) ADM=(%x)\n",
                                               NM_OC_RADIO_CARRIER,
                                               0x00, 
                                               0x00, 
                                               NM_OC_NULL, 
                                               NM_OPSTATE_DISABLED, 
                                               NM_AVSTATE_NOT_INSTALLED, 
                                               NM_STATE_LOCKED);

	return 0;
}


static int bts_get_admin_state(int socketd, char *msg, u_int8_t *state)
{
       
        msg_head_t *head;
        om_hdr_t   *om_hdr;
        fom_hdr_t  *fom_hdr;
	admin_state_t *admin;
	

        head    = (msg_head_t *)msg;
        om_hdr  = (om_hdr_t   *)&head->data[0];
        fom_hdr = (fom_hdr_t   *)&om_hdr->data[0];
	admin = (admin_state_t *) &fom_hdr->data[0];

	*state = admin->state;
        
          return 0;
}

static int chg_adm_state_ack(int socketd, char *msg,
                                 u_int8_t obj_class,
                                 u_int8_t bts_nr,
                                 u_int8_t trx_nr,
                                 u_int8_t ts_nr)
{
        int rc;
        msg_head_t *head;
        om_hdr_t   *om_hdr;
        fom_hdr_t  *fom_hdr;

        head    = (msg_head_t *)msg;
        om_hdr  = (om_hdr_t   *)&head->data[0];
        fom_hdr = (fom_hdr_t   *)&om_hdr->data[0];

        head->len   = 0x0900;
        head->proto = 0xff;
          om_hdr->mdisc =  ABIS_OM_MDISC_FOM;
          om_hdr->placement = ABIS_OM_PLACEMENT_ONLY;
          om_hdr->sequence = 0x00;
          om_hdr->length = 0x05;
             fom_hdr->msg_type  = NM_MT_CHG_ADM_STATE_ACK;
             fom_hdr->obj_class = obj_class;
             fom_hdr->obj_inst.bts_nr = bts_nr;
             fom_hdr->obj_inst.trx_nr = trx_nr;
             fom_hdr->obj_inst.ts_nr  = ts_nr;

             rc = send(socketd,msg,ntohs(head->len)+3, 0);
             if (rc < 0){
             logger(ERROR,LOCATION,"send() failed\n");
             }
          return rc;
}

      

static void chan_get_attr(int socketd, char *msg)
{
	msg_head_t   *head;
        om_hdr_t     *om_hdr;
        fom_hdr_t    *fom_hdr;
	u_int8_t length, atribute_length, type, atribute, *patr;

        head    = (msg_head_t *) msg;
        om_hdr  = (om_hdr_t   *)&head->data[0];
        fom_hdr = (fom_hdr_t   *)&om_hdr->data[0];
        
	length   = om_hdr->length - 5; /* Length from 1st atribute to end */
	type     = fom_hdr->msg_type;
	patr = &fom_hdr->data[0];
	atribute = *patr;
/*FIXME continue work on function */
}

static void radio_get_attr(int socketd, char *msg)
{
	msg_head_t   *head;
        om_hdr_t     *om_hdr;
        fom_hdr_t    *fom_hdr;
	u_int8_t length, atribute_length, type, atribute, size, *patr;

        head    = (msg_head_t *) msg;
        om_hdr  = (om_hdr_t   *)&head->data[0];
        fom_hdr = (fom_hdr_t   *)&om_hdr->data[0];
        
	length   = om_hdr->length - 5; /* Length from 1st atribute to end */
	type     = fom_hdr->msg_type;
	patr = &fom_hdr->data[0];
	atribute = *patr;
/*FIXME continue work on function */
}

static int bts_get_attr(int socketd, char *msg)
{
	msg_head_t   *head;
        om_hdr_t     *om_hdr;
        fom_hdr_t    *fom_hdr;
	u_int8_t length, atribute_length, type, atribute, size, *patr;

        head    = (msg_head_t *) msg;
        om_hdr  = (om_hdr_t   *)&head->data[0];
        fom_hdr = (fom_hdr_t   *)&om_hdr->data[0];
        
	length   = om_hdr->length - 5; /* Length from 1st atribute to end */
	type     = fom_hdr->msg_type;
	patr = &fom_hdr->data[0];
	atribute = *patr;

	while (length != 0){
		switch (atribute){
		 case  NM_ATT_INTERF_BOUND:{
	                       interf_bound_t *interf_bound;
			       interf_bound = (interf_bound_t *)patr;
	   		       atribute = interf_bound->atribute;
                               printf("length %d atribute NM_ATT_INTERF_BOUND:%x\n",length,atribute);
		 	       size = sizeof(interf_bound_t);
			       length = length - size;
		               patr = patr + size;			
			       atribute = *patr;	
			       printf("length %d atribute %x\n",length,atribute);	    	
		       	       break;
  		               }
		case NM_ATT_INTAVE_PARAM:{
			       intave_param_t *intave_parm = (intave_param_t *)patr;
			       size = sizeof(intave_param_t);
			       length = length - size;
                               patr = patr + size;
                               atribute = *patr;
                               printf("length %d atribute NM_ATT_INTAVE_PARAM %x\n",length,atribute);	
			       break;
			       }
		case NM_ATT_CONN_FAIL_CRIT:{ /* type TLV */
			        conn_fail_criteria_t *conn_fail_crit = ( conn_fail_criteria_t *) patr;
				atribute_length = ntohs(conn_fail_crit->length);
			        length = length - (3 + atribute_length);
				patr = patr + 3 + atribute_length;
				atribute = *patr;
                                printf("length %d atribute NM_ATT_CONN_FAIL_CRIT %x\n",length,atribute);
				break;
			        }			        
		case NM_ATT_T200:{
			        t200_t *t200 = (t200_t *) patr;
				size = sizeof(t200_t);
 				length = length - size;
                                patr = patr + size;
				atribute = *patr;
                                printf("length %d atribute NM_ATT_T200%x\n",length,atribute);
                                break;
				}
		 case NM_ATT_MAX_TA:{
				max_timing_adv_t *max_timing = (max_timing_adv_t *) patr;
                                size = sizeof(max_timing_adv_t);
                                length = length - size;
                                patr = patr + size;
                                atribute = *patr;
                                printf("length %d atribute NM_ATT_MAX_TA%x\n",length,atribute);
                                break;
				}
		case NM_ATT_OVERL_PERIOD:{ /* type TLV */
				overload_period_t *overload_period = (overload_period_t *) patr;		
				atribute_length = ntohs(overload_period->length);
                                length = length - (3 + atribute_length);
                                patr = patr + 3 + atribute_length;
                                atribute = *patr;
                                printf("length %d atribute NM_ATT_OVERL_PERIOD%x\n",length,atribute);
                                break;
				}

		case NM_ATT_CCCH_L_T:{
				CCCH_load_treshold_t *load_treshold = (CCCH_load_treshold_t *) patr;
				size = sizeof(CCCH_load_treshold_t);
                                length = length - size;
                                patr = patr + size;
                                atribute = *patr;
                                printf("length %d atribute NM_ATT_CCCH_L_T%x\n",length,atribute);
				break;
				}

		case NM_ATT_CCCH_L_I_P:{
				CCCH_load_ind_period_t *CCCH_load_period = (CCCH_load_ind_period_t *) patr;
				size = sizeof(CCCH_load_ind_period_t);
                                length = length - size;
                                patr = patr + size;
                                atribute = *patr;
                                printf("length %d atribute NM_ATT_CCCH_L_I_P%x\n",length,atribute);
                                break;
				}
		case NM_ATT_RACH_B_THRESH:{
				RACCH_busy_threshold_t *RACCH_treshold = (RACCH_busy_threshold_t *) patr;
				size = sizeof(RACCH_busy_threshold_t);
                                length = length - size;
                                patr = patr + size;
                                atribute = *patr;
                                printf("length %d atribute NM_ATT_RACH_B_THRESH%x\n",length,atribute);
                                break;
				}	
		case NM_ATT_LDAVG_SLOTS:{
				RACCH_load_averaging_slots_t *RACCH_load_averaging = (RACCH_load_averaging_slots_t *) patr;
				size = sizeof(RACCH_load_averaging_slots_t);
                                length = length - size;
                                patr = patr + size;
                                atribute = *patr;
                                printf("length %d atribute NM_ATT_LDAVG_SLOTS%x\n",length,atribute);
                                break;
				}

		case NM_ATT_BTS_AIR_TIMER:{
				BTS_air_timer_t *BTS_air_timer = (BTS_air_timer_t *) patr;
                                size = sizeof(BTS_air_timer_t);
                                length = length - size;
                                patr = patr + size;
                                atribute = *patr;
                                printf("length %d atribute NM_ATT_BTS_AIR_TIMER%x\n",length,atribute);
                                break;
				}

		case NM_ATT_NY1:{
				Ny1_t *Ny1 = ( Ny1_t *) patr;
                                size = sizeof(Ny1_t);
                                length = length - size;
                                patr = patr + size;
                                atribute = *patr;
                                printf("length %d atribute NM_ATT_NY1%x\n",length,atribute);
                                break;
				}

		case NM_ATT_BCCH_ARFCN:{
				BCCH_ARCFN_t *BCCH_ARCFN = (BCCH_ARCFN_t *) patr;
                                size = sizeof(BCCH_ARCFN_t);
                                length = length - size;
                                patr = patr + size;
                                atribute = *patr;
                                printf("length %d atribute NM_ATT_BCCH_ARFCN%x\n",length,atribute);
                                break;
				}

		case NM_ATT_BSIC:{
				BSIC_t *BSIC = ( BSIC_t *) patr;
                                size = sizeof(BSIC_t);
                                length = length - size;
                                patr = patr + size;
                                atribute = *patr;
                                printf("length %d atribute NM_ATT_BSIC %x\n",length,atribute);
                                break;
				}
		case NM_ATT_START_TIME:{
				starting_time_t *starting_time = (starting_time_t *) patr;
				size = sizeof(starting_time_t);
                                length = length - size;
                                patr = patr + size;
                                atribute = *patr;
                                printf("length %d atribute NM_ATT_START_TIME %x\n",length,atribute);
                                break;
				}
		case NM_ATT_SOFTBTS_CGI:{
				size = 10;
				patr = patr +size;
				length = length - size;
				printf("attribute NM_ATT_SOFTBTS_CGI \n");
			        break;
				}
		       default:
		       break;
	        } /* switch */
	 } /* while */	
return 0;			
}





static void oml_msg(char *msg, int sd)
{
        int rc, channel;
	msg_head_t       *head;
        om_hdr_t         *om_hdr;
        fom_hdr_t        *fom_hdr;

        head    = (msg_head_t *) msg;
        om_hdr  = (om_hdr_t   *)&head->data[0];
        fom_hdr = (fom_hdr_t   *)&om_hdr->data[0];


        switch ( fom_hdr->msg_type) {

        case NM_MT_SW_ACT_REQ_ACK:
		/* SW ACTIVATE REQUEST - SITE MANAGER */
		if(fom_hdr->obj_class == NM_OC_SITE_MANAGER){
		sw_site_manager = NM_MT_SW_ACT_REQ_ACK;
		debug(DNM,"BTS->BSC OC=SITE MANAGER(%x)  NM_MT_SW_ACT_REQ_ACK: = 0x%02x\n",
                      fom_hdr->obj_class,NM_MT_SW_ACT_REQ_ACK);
		}
		/* SW ACTIVATE REQUEST - BTS */
		if(fom_hdr->obj_class == NM_OC_BTS){
                sw_bts = NM_MT_SW_ACT_REQ_ACK;
                debug(DNM,"BTS->BSC OC=BTS(%x)  NM_MT_SW_ACT_REQ_ACK: = 0x%02x\n",
                        fom_hdr->obj_class, NM_MT_SW_ACT_REQ_ACK);
                }
		/* SW ACTIVATE REQUEST - BASEB_TRANSC */
		if(fom_hdr->obj_class == NM_OC_BASEB_TRANSC){
		debug(DNM,"BTS->BSC OC=BASEB_TRANSC(%x)  NM_MT_SW_ACT_REQ_ACK: = 0x%02x\n",
                        fom_hdr->obj_class,  NM_MT_SW_ACT_REQ_ACK);
		}
	break;

	case NM_MT_ACTIVATE_SW:
		/* SW ACTIVATE - SITE MANAGER */
		if(fom_hdr->obj_class == NM_OC_SITE_MANAGER){
   		  sw_site_manager = NM_MT_ACTIVATE_SW;
		  debug(DNM,"BTS<-BSC OC=SITE MANAGER(%x) NM_MT_ACTIVATE_SW = 0x%02x\n",
                        fom_hdr->obj_class, NM_MT_ACTIVATE_SW);
     	          rc = sw_activate_ack(sd,msg,
                                     NM_OC_SITE_MANAGER,
 				     NM_OC_NULL,
                                     NM_OC_NULL,
                                     NM_OC_NULL);
		  debug(DNM,"BTS->BSC OC=SITE MANAGER(%x) NM_MT_ACTIVATE_SW_ACK: = 0x%02x\n",
                        fom_hdr->obj_class, NM_MT_ACTIVATE_SW_ACK);
        	  rc = state_changed_event_report(sd, msg,
                                     NM_OC_SITE_MANAGER,
                                     NM_OC_NULL,
                                     NM_OC_NULL,
                                     NM_OC_NULL,
                                     NM_OPSTATE_ENABLED,
                                     NM_AVSTATE_OK,
                                     NM_STATE_LOCKED);
                  debug(DNM,"OC=SITE MANAGER(%x) INST=(%x,%x,%x) STATE CHANGED: OP_STATE=(%x) AVAIL=OK(%x) ADM=(%x)\n",
                                     NM_OC_SITE_MANAGER, 
                                     NM_OC_NULL, 
                                     NM_OC_NULL, 
                                     NM_OC_NULL, 
                                     NM_OPSTATE_ENABLED, 
                                     NM_AVSTATE_OK, 
                                     NM_STATE_LOCKED);
		}
		/* SW ACTIVATE - BTS */
		if(fom_hdr->obj_class == NM_OC_BTS ){
		  debug(DNM,"BTS<-BSC OC=BTS(%x) NM_MT_ACTIVATE_SW: = 0x%02x\n",fom_hdr->obj_class, NM_MT_ACTIVATE_SW);
		  rc = sw_activate_ack(sd,msg,
                                     NM_OC_BTS,
                                     0x00,
                                     NM_OC_NULL,
                                     NM_OC_NULL);
		  debug(DNM,"BTS->BSC OC=BTS(%x) SW Activate: ACK = 0x%02x\n",fom_hdr->obj_class, NM_MT_ACTIVATE_SW_ACK);
		  rc = state_changed_event_report(sd, msg,
                                     NM_OC_BTS,
                                     0x00,
                                     NM_OC_NULL,
                                     NM_OC_NULL,
                                     NM_OPSTATE_DISABLED,
                                     NM_AVSTATE_DEPENDENCY,
                                     NM_STATE_LOCKED);
                  debug(DNM,"OC=BTS(%x) INST=(%x,%x,%x) STATE CHANGED: OP_STATE=(%x) AVAIL=(%x) ADM=(%x)\n",
                                     NM_OC_BTS, 
                                     0x00, 
                                     NM_OC_NULL, 
                                     NM_OC_NULL, 
                                     NM_OPSTATE_DISABLED, 
                                     NM_AVSTATE_DEPENDENCY, 
                                     NM_STATE_LOCKED);
		}
		/* SW ACTIVATE - BASEB_TRANSC */
		if(fom_hdr->obj_class == NM_OC_BASEB_TRANSC){
		  debug(DNM,"BTS<-BSC OC=BASEB_TRANSC(%x) NM_MT_ACTIVATE_SW = 0x%02x\n",
                        fom_hdr->obj_class, NM_MT_ACTIVATE_SW);
		  rc = sw_activate_ack(sd,msg,
                                     NM_OC_BASEB_TRANSC,
                                     0x00,
                                     0x00,
                                     NM_OC_NULL);
                  debug(DNM,"BTS->BSC OC=BASEB_TRANSC(%x) SW Activate: ACK = 0x%02x\n",
                        fom_hdr->obj_class, NM_MT_ACTIVATE_SW_ACK);
 		 /* Trigering SW Activate Request - OC RADIO CARRIER */
                    rc =  sw_activate_request(sd,msg,
                                   NM_OC_RADIO_CARRIER,
                                   0x00,
                                   0x00,
                                   NM_OC_NULL);
                   logger(INFO,LOCATION,"BTS->BSC Software Activate Request for NM_OC_RADIO_CARRIER = OC(%x)\n",
                          NM_OC_RADIO_CARRIER);
                break; /* duhet break pasi obj_instance behet NM_OC_RADIO_CARRIER pas send te sw_activation_request */
		}

                /* SW ACTIVATE - RADIO CARRIER */
                if(fom_hdr->obj_class == NM_OC_RADIO_CARRIER){
                     debug(DNM,"BTS<-BSC OC= NM_OC_RADIO_CARRIER(%x) NM_MT_ACTIVATE_SW: = 0x%02x\n",
                     fom_hdr->obj_class,NM_MT_ACTIVATE_SW);
		     rc = sw_activate_ack(sd,msg,
                                     NM_OC_RADIO_CARRIER,
                                     0x00,
                                     0x00,
                                     NM_OC_NULL);
                      debug(DNM,"BTS->BSC OC=NM_OC_RADIO_CARRIER(%x) SW Activate: ACK = 0x%02x\n",
                            fom_hdr->obj_class, NM_MT_ACTIVATE_SW_ACK);

		/* Trigering SW Activate Request - OC CHANNEL*/
		      for(channel = 0x0; channel <= 0x07; channel++){
                      rc =  sw_activate_request(sd,msg,
                                   NM_OC_CHANNEL,
                                   0x00,
                                   0x00,
                                   channel);
                      logger(INFO,LOCATION,"BTS->BSC Software Activate Request for NM_OC_CHANNEL = OC(%x)\n",
                          NM_OC_CHANNEL);
		      }
                break;
                }
		/* SW ACTIVATE - CHANNEL */
                if(fom_hdr->obj_class == NM_OC_CHANNEL){
		     debug(DNM,"BTS<-BSC OC= NM_OC_CHANNEL(%x) NM_MT_ACTIVATE_SW: = 0x%02x\n",
                     		     fom_hdr->obj_class,NM_MT_ACTIVATE_SW);
		     rc = sw_activate_ack(sd,msg,
                                     NM_OC_CHANNEL,
                                     0x00,
                                     0x00,
                                     fom_hdr->obj_inst.ts_nr);
                      debug(DNM,"BTS->BSC OC=NM_OC_CHANNEL(%x) SW Activate: ACK = 0x%02x\n",
                            fom_hdr->obj_class, NM_MT_ACTIVATE_SW_ACK);
		      rc = state_changed_event_report(sd, msg,
                                     NM_OC_CHANNEL,
                                     0x00,
                                     0x00,
                                     fom_hdr->obj_inst.ts_nr,
                                     NM_OPSTATE_DISABLED,
                                     NM_AVSTATE_DEPENDENCY,
                                     NM_STATE_LOCKED);
                      debug(DNM,"OC=NM_OC_CHANNEL(%x) INST=(%x,%x,%x) STATE CHANGED: OP_STATE=(%x) AVAIL=(%x) ADM=(%x)\n",
                                     NM_OC_CHANNEL, 
                                     0x00, 
                                     0x00, 
                                     fom_hdr->obj_inst.ts_nr, 
                                     NM_OPSTATE_DISABLED, 
                                     NM_AVSTATE_DEPENDENCY, 
                                     NM_STATE_LOCKED);
		     if(fom_hdr->obj_inst.ts_nr == 0x07){
		       for(channel = 0x00; channel <= 0x07; channel++){
		       	           rc = sw_activated_report(sd,msg,
                                    NM_OC_CHANNEL,
                                    0x00,
                                    0x00,
                                    channel);
		   debug(DNM,"BTS->BSC OC=NM_OC_CHANNEL(%x) NM_MT_SW_ACTIVATED_REP: = 0x%02x\n",
                        fom_hdr->obj_class,NM_MT_SW_ACTIVATED_REP);
		       }			
		     }		
		break;	
        	}	
	break;

	case NM_MT_OPSTART:
                 /* OP START - SITE MANAGER */
		 if(fom_hdr->obj_class == NM_OC_SITE_MANAGER){
		   manager_opstate = NM_OPSTATE_ENABLED;
		   manager_admin_state = NM_STATE_UNLOCKED;
		   rc = opstart_ack(sd, msg,
                                    NM_OC_SITE_MANAGER,
                                    NM_OC_NULL,
                                    NM_OC_NULL,
                                    NM_OC_NULL);
                   debug(DNM,"BTS->BSC OC=SITE MANAGER(%x) NM_MT_OPSTART_ACK: = 0x%02x\n",
                        fom_hdr->obj_class,  NM_MT_OPSTART_ACK);
	           rc = sw_activated_report(sd,msg,
                                    NM_OC_SITE_MANAGER,
                                    NM_OC_NULL,
                                    NM_OC_NULL,
                                    NM_OC_NULL);
		   debug(DNM,"BTS->BSC OC=SITE MANAGER(%x) NM_MT_SW_ACTIVATED_REP: = 0x%02x\n",
                        fom_hdr->obj_class,NM_MT_SW_ACTIVATED_REP);

		   /* Trigering SW Activate Request - BTS */
                   rc =  sw_activate_request(sd,msg,
                                   NM_OC_BTS,
                                   0x00,
                                   NM_OC_NULL,
                                   NM_OC_NULL);
                   logger(INFO,LOCATION,"BTS->BSC Software Activate Request for NM_OC_BTS = OC(%x)\n",NM_OC_BTS);
	           break;
		 }
                 /* OP START - BTS */
                 if(fom_hdr->obj_class == NM_OC_BTS){
		    bts_opstate = NM_OPSTATE_ENABLED;
                    rc = opstart_ack(sd, msg,
                                    NM_OC_BTS,
                                    0x00,
                                    NM_OC_NULL,
                                    NM_OC_NULL);
                    /* Meaning of Ack message: The BTS has reset the operational
                       state of the specified object to "enabled" state. */
                    debug(DNM,"BTS->BSC OC=BTS(%x) SW NM_MT_OPSTART_ACK: = 0x%02x\n",
                         fom_hdr->obj_class, NM_MT_OPSTART_ACK);
                    rc = sw_activated_report(sd,msg,
                                    NM_OC_BTS,
                                    0x00,
                                    NM_OC_NULL,
                                    NM_OC_NULL);
                    debug(DNM,"BTS->BSC OC=BTS(%x) NM_MT_SW_ACTIVATED_REP: = 0x%02x\n",
                          fom_hdr->obj_class,NM_MT_SW_ACTIVATED_REP);
	
		    /* Trigering SW Activate Request - BASEBAND TRANSC */
		    rc =  sw_activate_request(sd,msg,
                                   NM_OC_BASEB_TRANSC,
                                   0x00,
                                   0x00,
                                   NM_OC_NULL);
		   logger(INFO,LOCATION,"BTS->BSC Software Activate Request for NM_OC_BASEB_TRANSC = OC(%x)\n",
                          NM_OC_BASEB_TRANSC);	
		    break;
		 }
		 /* OPSTART - BASEB_TRANSC */
		 if(fom_hdr->obj_class == NM_OC_BASEB_TRANSC){
		    baseb_transc_opstate = NM_OPSTATE_ENABLED;
		    rc = opstart_ack(sd,msg,
			           NM_OC_BASEB_TRANSC,
				   0x00,
				   0x00,
				   NM_OC_NULL);
		 
                    debug(DNM,"BTS->BSC OC= BASEB_TRANSC(%x) SW NM_MT_OPSTART_ACK: = 0x%02x\n",
                          fom_hdr->obj_class, NM_MT_OPSTART_ACK);

                  break;
		 }
                 /* OPSTART - RADIO_CARRIER */
		 if(fom_hdr->obj_class == NM_OC_RADIO_CARRIER){
		    radio_carrier_opstate = NM_OPSTATE_ENABLED;
		    rc = opstart_ack(sd,msg,
			           NM_OC_RADIO_CARRIER,
				   0x00,
				   0x00,
				   NM_OC_NULL);
                    debug(DNM,"BTS->BSC OC=RADIO_CARRIER(%x) SW NM_MT_OPSTART_ACK: = 0x%02x\n",
                          fom_hdr->obj_class, NM_MT_OPSTART_ACK);  
 
                 break;
		  }
                  /* OPSTART - CHANNEL*/
		 if(fom_hdr->obj_class == NM_OC_CHANNEL){
		    channel_opstate[fom_hdr->obj_inst.ts_nr] = NM_OPSTATE_ENABLED; 	
		    rc = opstart_ack(sd,msg,
			           NM_OC_CHANNEL,
				   0x00,
				   0x00,
				   fom_hdr->obj_inst.ts_nr);
                    debug(DNM,"BTS->BSC OC=CHANNEL(%x) SW NM_MT_OPSTART_ACK: = 0x%02x\n",
                          fom_hdr->obj_class, NM_MT_OPSTART_ACK);

		    /* Trigering SW Activated Report - NM_OC_RADIO_CARRIER & OC BASEBAND TRANSCEIVER*/
		    if(	fom_hdr->obj_inst.ts_nr == 0x07){
		    rc = sw_activated_report(sd,msg,
			           NM_OC_RADIO_CARRIER,
				   0x00,
				   0x00,
				   NM_OC_NULL);
                    debug(DNM,"BTS->BSC OC =  RADIO_CARRIER(%x) NM_MT_SW_ACTIVATED_REP: = 0x%02x\n",
                         fom_hdr->obj_class,NM_MT_SW_ACTIVATED_REP);



         /* xxxxxxxxxxxxxxxxxxxx */
		      rc = state_changed_event_report(sd, msg,
                                     NM_OC_SITE_MANAGER,
                                     NM_OC_NULL,
                                     NM_OC_NULL,
                                     NM_OC_NULL,
                                     manager_opstate,
                                     NM_AVSTATE_OK,
                                     manager_admin_state);
                      debug(DNM,"OC=NM_OC_SITE_MANAGER(%x) INST=(%x,%x,%x) STATE CHANGED: OP_STATE=(%x) AVAIL=(%x) ADM=(%x)\n",
                                     NM_OC_SITE_MANAGER, 
                                     NM_OC_NULL, 
                                     NM_OC_NULL,
                                     NM_OC_NULL, 
                                     manager_opstate, 
                                     NM_AVSTATE_OK, 
                                     manager_admin_state); 
/*-------------------*/	
	/* xxxxxxxxxxxxxxxxxxxx */
		      rc = state_changed_event_report(sd, msg,
                                     NM_OC_BTS,
                                     0x00,
                                     NM_OC_NULL,
                                     NM_OC_NULL,
                                     bts_opstate,
                                     NM_AVSTATE_OK,
                                     bts_admin_state);
                      debug(DNM,"OC=NM_OC_BTS(%x) INST=(%x,%x,%x) STATE CHANGED: OP_STATE=(%x) AVAIL=(%x) ADM=(%x)\n",
                                     NM_OC_BTS, 
                                     0x00, 
                                     NM_OC_NULL,
                                     NM_OC_NULL, 
                                     bts_opstate, 
                                     NM_AVSTATE_OK, 
                                     bts_admin_state); 
/*-------------------*/
/********************/
		      for(channel = 0x00; channel <= 0x07; channel++){	
 		      rc = state_changed_event_report(sd, msg,
                                     NM_OC_CHANNEL,
                                     0x00,
                                     0x00,
                                     channel,
                                     channel_opstate[channel],
                                     NM_AVSTATE_OK,
                                     channel_admin_state[channel]);
                     debug(DNM,"OC=NM_OC_CHANNEL(%x) INST=(%x,%x,%x) STATE CHANGED: OP_STATE=(%x) AVAIL=(%x) ADM=(%x)\n",
                                     NM_OC_CHANNEL, 
                                     0x00, 
                                     0x00, 
                                     channel, 
                                     channel_opstate[channel], 
                                     NM_AVSTATE_OK, 
                                     channel_admin_state[channel]);
		     }
/*--------------------*/

		     
		    rc = sw_activated_report(sd,msg,
			           NM_OC_BASEB_TRANSC,
				   0x00,
				   0x00,
				   NM_OC_NULL);
                    debug(DNM," BTS->BSC OC =  BASEB_TRANSC(%x) NM_MT_SW_ACTIVATED_REP: = 0x%02x\n",
                         fom_hdr->obj_class,NM_MT_SW_ACTIVATED_REP);   
		   } 	 
                  break;
 		}
        break;

	case NM_MT_SET_BTS_ATTR:
		  rc = bts_get_attr(sd, msg);
		  debug(DNM,"BTS<-BSC OC = BTS(%x) NM_MT_SET_BTS_ATTR: = 0x%02x\n",fom_hdr->obj_class,NM_MT_SET_BTS_ATTR);
         	  bts_set_attr_ack(sd, msg,
                                   NM_OC_BTS,
                                   0x00,
                                   NM_OC_NULL,
                                   NM_OC_NULL);
		  sw_bts = NM_MT_SET_BTS_ATTR_ACK;
		  debug(DNM,"BTS->BSC OC = BTS(%x) NM_MT_SET_BTS_ATTR_ACK: = 0x%02x\n",fom_hdr->obj_class,NM_MT_SET_BTS_ATTR_ACK);
	break;

        case NM_MT_SET_CHAN_ATTR:
		  chan_get_attr(sd, msg);
		  debug(DNM,"BTS<-BSC OC = NM_OC_CHANNEL(%x) NM_MT_SET_CHAN_ATTR: = 0x%02x\n", 
			fom_hdr->obj_inst.ts_nr,NM_MT_SET_CHAN_ATTR);
         	  bts_set_attr_ack(sd, msg,
                                   NM_OC_CHANNEL,
                                   0x00,
                                   0x00,
                                   fom_hdr->obj_inst.ts_nr);
		  debug(DNM,"BTS->BSC OC = NM_OC_CHANNEL(%x) NM_MT_SET_CHAN_ATTR_ACK: = 0x%02x\n",
			 fom_hdr->obj_inst.ts_nr,NM_MT_SET_CHAN_ATTR_ACK);
        break;

        case NM_MT_SET_RADIO_ATTR:
		  radio_get_attr(sd, msg);
		  debug(DNM,"BTS<-BSC OC = NM_OC_RADIO_CARRIER(%x) NM_MT_SET_RADIO_ATTR: = 0x%02x\n", 
			fom_hdr->obj_class,NM_MT_SET_RADIO_ATTR);
         	  bts_set_attr_ack(sd, msg,
                                   NM_OC_RADIO_CARRIER,
                                   0x00,
                                   0x00,
                                   NM_OC_NULL);
		  debug(DNM,"BTS->BSC OC = NM_OC_RADIO_CARRIER(%x) NM_MT_SET_RADIO_ATTR_ACK: = 0x%02x\n",
			 fom_hdr->obj_class,NM_MT_SET_RADIO_ATTR_ACK);
        break;                 

	case NM_MT_CHG_ADM_STATE:
		if(fom_hdr->obj_class == NM_OC_BTS){
		   bts_get_admin_state(sd, msg, &bts_admin_state);
		   chg_adm_state_ack(sd,msg,
                                   NM_OC_BTS,
                                   0x00, /* BTS nr */
                                   NM_OC_NULL,
                                   NM_OC_NULL);
		   debug(DNM,"BTS->BSC OC=BTS(%x) STATE CHANGED: NM_MT_CHG_ADM_STATE: = 0x%2x\n",NM_OC_BTS,bts_admin_state);
		break;		
		}
		
		if(fom_hdr->obj_class == NM_OC_BASEB_TRANSC){
                   bts_get_admin_state(sd, msg, &baseb_transc_admin_state);
                   debug(DNM,"BTS<-BSC OC=BASEB_TRANSC(%x) NM_MT_CHG_ADM_STATE: = 0x%02x STATE = 0x%02x\n",
                        fom_hdr->obj_class,NM_MT_CHG_ADM_STATE,baseb_transc_admin_state);
                   chg_adm_state_ack(sd,msg,
                                   NM_OC_BASEB_TRANSC,
                                   0x00, /* BTS nr */
                                   0x00, /* TRX nr */
                                   NM_OC_NULL);
                   debug(DNM,"BTS->BSC OC=BASEB_TRANSC(%x) NM_MT_CHG_ADM_STATE_ACK: = 0x%2x\n",
                         NM_OC_BASEB_TRANSC, NM_MT_CHG_ADM_STATE_ACK);
	 
		break;
                }
		   if(fom_hdr->obj_class == NM_OC_RADIO_CARRIER){
                   bts_get_admin_state(sd, msg, &radio_carrier_admin_state);
                   debug(DNM,"BTS<-BSC OC = RADIO_CARRIER(%x) NM_MT_CHG_ADM_STATE: = 0x%02x STATE = 0x%02x\n",
                        fom_hdr->obj_class,NM_MT_CHG_ADM_STATE,radio_carrier_admin_state);
                   chg_adm_state_ack(sd,msg,
                                   NM_OC_RADIO_CARRIER,
                                   0x00, /* BTS nr */
                                   0x00, /* TRX nr */
                                   NM_OC_NULL);
                   debug(DNM,"BTS->BSC OC = RADIO_CARRIER(%x) NM_MT_CHG_ADM_STATE_ACK: = 0x%2x\n",
                         NM_OC_RADIO_CARRIER, NM_MT_CHG_ADM_STATE_ACK);

		break;
                }

		if(fom_hdr->obj_class == NM_OC_CHANNEL){
		   bts_get_admin_state(sd, msg, &channel_admin_state[fom_hdr->obj_inst.ts_nr]);
                   debug(DNM,"BTS<-BSC OC = NM_OC_CHANNEL(%x) NM_MT_CHG_ADM_STATE: = 0x%02x STATE = 0x%02x\n",
                        fom_hdr->obj_class,NM_MT_CHG_ADM_STATE,channel_admin_state[fom_hdr->obj_inst.ts_nr]);
                   chg_adm_state_ack(sd,msg,
                                   NM_OC_CHANNEL,
                                   0x00, /* BTS nr */
                                   0x00, /* TRX nr */
                                   fom_hdr->obj_inst.ts_nr);
                   debug(DNM,"BTS->BSC OC = NM_OC_CHANNEL(%x) NM_MT_CHG_ADM_STATE_ACK: = 0x%2x\n",
                         fom_hdr->obj_class, NM_MT_CHG_ADM_STATE_ACK);
		break;                
		}

	default:
	break;	
	} /* switch end */

	switch (om_hdr->mdisc){

	case ABIS_OM_MDISC_MANUF:
	
	logger(INFO,LOCATION,"BSC->BTS  ABIS_OM_MDISC_MANUF = 0x%2x Manufacturer Identifier-> com.ipaccess\n",
	       ABIS_OM_MDISC_MANUF);
	debug(DNM,"BSC->BTS  ABIS_OM_MDISC_MANUF = 0x%2x Manufacturer Identifier-> com.ipaccess\n",
	       ABIS_OM_MDISC_MANUF);
	rc = send(sd, init_rsl_link, sizeof(init_rsl_link), 0);
	if (rc < 0){
         logger(ERROR,LOCATION,"send() failed\n");
         break;
        }
	debug(DNM,"BSC<-BTS Init RSL Link send on socket %x\n",sd);

		/* xxxxxxxxxxxxxxxxxxxx */
		      rc = state_changed_event_report(sd, msg,
                                     NM_OC_BASEB_TRANSC,
                                     0x00,
                                     0x00,
                                     NM_OC_NULL,
                                     baseb_transc_opstate,
                                     NM_AVSTATE_OK,
                                     baseb_transc_admin_state);
                      debug(DNM,"OC=NM_OC_BASEB_TRANSC(%x) INST=(%x,%x,%x) STATE CHANGED: OP_STATE=(%x) AVAIL=(%x) ADM=(%x)\n",
                                     NM_OC_BASEB_TRANSC, 
                                     0x00, 
                                     0x00, 
                                     NM_OC_NULL, 
                                     baseb_transc_opstate, 
                                     NM_AVSTATE_OK, 
                                     baseb_transc_admin_state); 
/*-------------------*/	
/* xxxxxxxxxxxxxxxxxxxx */
		      rc = state_changed_event_report(sd, msg,
                                     NM_OC_RADIO_CARRIER,
                                     0x00,
                                     0x00,
                                     NM_OC_NULL,
                                     radio_carrier_opstate,
                                     NM_AVSTATE_OK,
                                     radio_carrier_admin_state);
                      debug(DNM,"OC=NM_OC_RADIO_CARRIER(%x) INST=(%x,%x,%x) STATE CHANGED: OP_STATE=(%x) AVAIL=(%x) ADM=(%x)\n",
                                     NM_OC_RADIO_CARRIER, 
                                     0x00, 
                                     0x00, 
                                     NM_OC_NULL, 
                                     radio_carrier_opstate, 
                                     NM_AVSTATE_OK, 
                                     radio_carrier_admin_state); 
/*-------------------*/

        rc = send(rsl.socket.fd, id_ack, sizeof(id_ack),0);
	debug(DNM,"BSC<-BTS PING send on socket %x\n",rsl.socket.fd);
	static u_int8_t null_msg[] = {0, 1, PROTO_RSL,MSG_ID_ACK};
	rc = send(rsl.socket.fd, null_msg, sizeof(null_msg),0);
 //       rc = send(rsl.socket.fd, id_ack, sizeof(id_ack),0);
//	debug(DNM,"BSC<-BTS PING send on socket %x\n",rsl.socket.fd);
	break;
	default:
	break;
	}
}







