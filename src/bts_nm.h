#ifndef BTS_NM_H
#define BTS_NM_H

/* Message discriminator's GSM 08.59*/
#define ABIS_OM_MDISC_FOM               0x80 /* Formatted O&M Messages */
#define ABIS_OM_MDISC_MMI               0x40
#define ABIS_OM_MDISC_TRAU              0x20
#define ABIS_OM_MDISC_MANUF             0x10
/* Placement Indicator */
#define ABIS_OM_PLACEMENT_ONLY          0x80 /*   only: This message is contained within one segment */
#define ABIS_OM_PLACEMENT_FIRST         0x40 /*  first: The first segment of a multi-segment message */
#define ABIS_OM_PLACEMENT_MIDDLE        0x20 /* middle: A middle segment of a multi-segment message */
#define ABIS_OM_PLACEMENT_LAST          0x10 /*   last: The last segment of a multi-segment message */


#define EQ_ID_LENGTH 0x000a
#define EQ_ID        PACKAGE-VERSION 
#define SW_FILE_ID   "120a0020"
#define SW_FILE_VER  "v209b24d3"

#define NM_MT_SOFTBTS_RSL_CONNECT       0xe0  /* received on OML link                 */
#define NM_MT_SOFTBTS_RSL_CONNECT_ACK   0xe1  /* responded via OML link              */
#define NM_ATT_SOFTBTS_DST_IP_PORT      0x81 /* hard coded in init_rsl_link[] message */ 
#define NM_ATT_SOFTBTS_DST_IP           0x80 /* hard coded in init_rsl_link[] message */
#define NM_ATT_SOFTBTS_STREAM_ID        0x85 
#define NM_ATT_SOFTBTS_CGI              0x99

struct softbts_head {
        u_int16_t len;  /* network byte order */
        u_int8_t proto;
        u_int8_t data[0];
} __attribute__ ((packed));
typedef struct softbts_head msg_head_t;

/*  header of OML messages GSM 08.59 */
struct abis_om_hdr {
        u_int8_t        mdisc;	    /* message discriminator */
        u_int8_t        placement;  /* placement indicator   */
        u_int8_t        sequence;   /* sequence number       */
        u_int8_t        length;     /* length Indicator      */
        u_int8_t        data[0];
} __attribute__ ((packed));
typedef struct abis_om_hdr om_hdr_t;

/* 3GPP TS 12.21 V8.0.0 9.3 Object Instance */
struct abis_om_obj_inst {
        u_int8_t        bts_nr;              /* he BTS Number identifies one BTS in a multi cell site     */
        u_int8_t        trx_nr;              /* the Baseband Transceiver number concerned in the message; */
        u_int8_t        ts_nr;               /* radio timeslot (0-7) of the Baseband Transceiver          */ 
} __attribute__ ((packed));
typedef struct abis_om_obj_inst obj_inst_t;


/* 3GPP TS 12.21 V8.0.0 8.2 Structure of Formatted O&M Messages */
struct abis_om_fom_hdr {
        u_int8_t        msg_type;
        u_int8_t        obj_class;
        obj_inst_t      obj_inst;
        u_int8_t        data[0];
} __attribute__ ((packed));
typedef struct abis_om_fom_hdr fom_hdr_t;



/* 3GPP TS 12.21 V8.0.0 Section 9.2: Object Class */
enum abis_nm_obj_class {
        NM_OC_SITE_MANAGER              = 0x00,
        NM_OC_BTS,
        NM_OC_RADIO_CARRIER,
        NM_OC_CHANNEL,
        NM_OC_BASEB_TRANSC,
        NM_OC_NULL                      = 0xff,
};

/* Section 9.4.4: Administrative State */
enum abis_nm_adm_state {
        NM_STATE_LOCKED         = 0x01,
        NM_STATE_UNLOCKED       = 0x02,
        NM_STATE_SHUTDOWN       = 0x03,
        NM_STATE_NULL           = 0xff,
};
struct abis_admin_state{
        u_int8_t        atribute;
        u_int8_t        state;
        u_int8_t        data[0];
} __attribute__ ((packed));
typedef struct abis_admin_state admin_state_t;

/* Section 9.4.7: Availability Status */
enum abis_nm_avail_state {
        NM_AVSTATE_IN_TEST       = 0x00,
        NM_AVSTATE_FAILED        = 0x01,
        NM_AVSTATE_POWER_OFF     = 0x02,
        NM_AVSTATE_OFF_LINE      = 0x03,
        NM_AVSTATE_DEPENDENCY    = 0x05,
        NM_AVSTATE_DEGRADED      = 0x06,
        NM_AVSTATE_NOT_INSTALLED = 0x07,
        NM_AVSTATE_OK            = 0xff,
};

struct abis_avail_status {
        u_int8_t        atribute;
        u_int16_t       len;
        u_int8_t        avail_status;
        u_int8_t        data[0];
} __attribute__ ((packed));
typedef struct abis_avail_status avail_status_t; 

enum abis_nm_op_state {
        NM_OPSTATE_DISABLED     = 0x01,
        NM_OPSTATE_ENABLED      = 0x02,
        NM_OPSTATE_NULL         = 0xff,
};
struct abis_oper_state {
        u_int8_t        atribute;
        u_int8_t        oper_state;
        u_int8_t        data[0];
} __attribute__ ((packed));
typedef struct abis_oper_state oper_state_t;

/* 9.4.18 File Id */
struct abis_file_id {
        u_int8_t  atribute;
        u_int16_t len;
        char      data[0];
} __attribute__ ((packed));
typedef struct abis_file_id file_id_t;

/* 9.4.19 File Version */
struct abis_file_ver {
        u_int8_t  atribute;
        u_int16_t len;
        u_int8_t  data[0];
} __attribute__ ((packed));
typedef struct abis_file_ver file_ver_t;


/* 9.4.61 SW Configuration */
struct abis_sw_config {
        u_int8_t        atribute;
        u_int16_t       len;
        u_int8_t        data[0];
} __attribute__ ((packed));
typedef struct abis_sw_config sw_config_t;

/* 9.4.62 SW Description */
struct abis_sw_descr {
        u_int8_t    atribute;
        file_id_t   file_id;
        file_ver_t  file_ver;
} __attribute__ ((packed));
typedef struct abis_sw_descr sw_descr_t;


/* 9.4.25 Interference level Boundaries */
struct interf_boundaries {
	u_int8_t atribute;
	u_int8_t bound0;
	u_int8_t bound1;
	u_int8_t bound2;
	u_int8_t bound3;
	u_int8_t bound4;
	u_int8_t bound5;
} __attribute__ ((packed));
typedef struct interf_boundaries interf_bound_t;

/* 9.4.24 Intave Parameter - see GSM 05.08 */
struct intave_parameter {
        u_int8_t attribute;
        u_int8_t intave;
} __attribute__ ((packed));
typedef struct intave_parameter intave_param_t;


/* 9.4.14 Connection Failure Criterion */
struct connection_failure_criterion {
	u_int8_t  atribute;
	u_int16_t length;
	u_int8_t  criterion;
	u_int8_t  criteria_value;	
} __attribute__ ((packed));
typedef struct connection_failure_criterion conn_fail_criteria_t;

/* 9.4.53 T200 */
struct t200 {
	u_int8_t atribute;
	u_int8_t SDCCH_5ms;
        u_int8_t FACCH_full_rate_5ms;
        u_int8_t FACCH_half_rate_5ms;
	u_int8_t SACCH_TCH_SAPI0_10ms;
        u_int8_t SACCH_SDCCH_10ms;
        u_int8_t SDCCH_SAPI3_5ms;
        u_int8_t SACCH_TCH_SAPI3_10ms;
} __attribute__ ((packed));
typedef struct t200 t200_t;

/* 9.4.31 Max Timing Advance */
struct max_timing_advance {
	u_int8_t atribute;
	u_int8_t timing_adv;
} __attribute__ ((packed));
typedef struct max_timing_advance max_timing_adv_t;

/* 9.4.39 Overload Period */
struct overload_period {
	u_int8_t  atribute;
	u_int16_t length;
	u_int8_t  period;
} __attribute__ ((packed));
typedef struct overload_period overload_period_t;

/* 9.4.12 CCCH Load Threshold */
struct CCCH_load_treshold {
	u_int8_t atribute;
	u_int8_t treshold;
} __attribute__ ((packed));
typedef struct CCCH_load_treshold CCCH_load_treshold_t;

/* 9.4.11 CCCH Load Indication Period */
struct CCCH_load_indication_period {
	u_int8_t atribute;
	u_int8_t period;
} __attribute__ ((packed));
typedef struct CCCH_load_indication_period CCCH_load_ind_period_t;

/* 9.4.44 RACH Busy Threshold */
struct RACCH_busy_threshold {
	u_int8_t atribute;
	u_int8_t threshold;
} __attribute__ ((packed));
typedef struct RACCH_busy_threshold RACCH_busy_threshold_t;

/* 9.4.45 RACH Load Averaging Slots */
struct RACCH_load_averaging_slots {
        u_int8_t atribute;
        u_int8_t load_aver_MSB;
	 u_int8_t load_aver_LSB;
} __attribute__ ((packed));
typedef struct RACCH_load_averaging_slots RACCH_load_averaging_slots_t;

/* 9.4.10 BTS Air Timer */
struct BTS_air_timer {
	u_int8_t atribute;
	u_int8_t T3105;
} __attribute__ ((packed));
typedef struct BTS_air_timer BTS_air_timer_t; 

/* 9.4.37 Ny1 */
struct Ny1 {
	u_int8_t atribute;
	u_int8_t Ny1;
} __attribute__ ((packed));
typedef struct Ny1 Ny1_t;

/* 9.4.8 BCCH ARFCN */
struct BCCH_ARCFN {
	u_int8_t atribute;
	u_int8_t ARCFN_MSB;
        u_int8_t ARFCN_LSB;
} __attribute__ ((packed));
typedef struct BCCH_ARCFN BCCH_ARCFN_t;

/* 9.4.9 BSIC */
struct BSIC {
 	u_int8_t atribute;
	u_int8_t bsic;
} __attribute__ ((packed));
typedef struct BSIC BSIC_t;

/* 9.4.52 Starting Time */
struct starting_time {
	u_int8_t atribute;
	u_int8_t current_FN_MSB;
        u_int8_t current_FN_LSB;
        u_int8_t data[0];
} __attribute__ ((packed));
typedef struct starting_time starting_time_t;


enum abis_nm_msgtype {
        /* SW Download Management Messages */
        NM_MT_LOAD_INIT                 = 0x01,
        NM_MT_LOAD_INIT_ACK,
        NM_MT_LOAD_INIT_NACK,
        NM_MT_LOAD_SEG,
        NM_MT_LOAD_SEG_ACK,
        NM_MT_LOAD_ABORT,
        NM_MT_LOAD_END,
        NM_MT_LOAD_END_ACK,
        NM_MT_LOAD_END_NACK,
        NM_MT_SW_ACT_REQ,               /* BTS->BSC */
        NM_MT_SW_ACT_REQ_ACK,
        NM_MT_SW_ACT_REQ_NACK,
        NM_MT_ACTIVATE_SW,              /* BSC->BTS */
        NM_MT_ACTIVATE_SW_ACK,
        NM_MT_ACTIVATE_SW_NACK,
        NM_MT_SW_ACTIVATED_REP,         /* 0x10 */

       /* Air Interface Management Messages */
        NM_MT_SET_BTS_ATTR              = 0x41,
        NM_MT_SET_BTS_ATTR_ACK,
        NM_MT_SET_BTS_ATTR_NACK,
        NM_MT_SET_RADIO_ATTR,
        NM_MT_SET_RADIO_ATTR_ACK,
        NM_MT_SET_RADIO_ATTR_NACK,
        NM_MT_SET_CHAN_ATTR,
        NM_MT_SET_CHAN_ATTR_ACK,
        NM_MT_SET_CHAN_ATTR_NACK,


        /* State Management and Event Report Messages */
        NM_MT_STATECHG_EVENT_REP        = 0x61,
        NM_MT_FAILURE_EVENT_REP,
        NM_MT_STOP_EVENT_REP,
        NM_MT_STOP_EVENT_REP_ACK,
        NM_MT_STOP_EVENT_REP_NACK,
        NM_MT_REST_EVENT_REP,
        NM_MT_REST_EVENT_REP_ACK,
        NM_MT_REST_EVENT_REP_NACK,
        NM_MT_CHG_ADM_STATE,
        NM_MT_CHG_ADM_STATE_ACK,
        NM_MT_CHG_ADM_STATE_NACK,
        NM_MT_CHG_ADM_STATE_REQ,
        NM_MT_CHG_ADM_STATE_REQ_ACK,
        NM_MT_CHG_ADM_STATE_REQ_NACK,
        NM_MT_REP_OUTST_ALARMS          = 0x93,
        NM_MT_REP_OUTST_ALARMS_ACK,
        NM_MT_REP_OUTST_ALARMS_NACK,

        /* Equipment Management Messages */
        NM_MT_CHANGEOVER                = 0x71,
        NM_MT_CHANGEOVER_ACK,
        NM_MT_CHANGEOVER_NACK,
        NM_MT_OPSTART,
        NM_MT_OPSTART_ACK,
        NM_MT_OPSTART_NACK,
        NM_MT_REINIT,
        NM_MT_REINIT_ACK,
        NM_MT_REINIT_NACK,
        NM_MT_SET_SITE_OUT,             /* BS11: get alarm ?!? */
        NM_MT_SET_SITE_OUT_ACK,
        NM_MT_SET_SITE_OUT_NACK,
        NM_MT_CHG_HW_CONF               = 0x90,
        NM_MT_CHG_HW_CONF_ACK,
        NM_MT_CHG_HW_CONF_NACK,

};
/* Section 9.4: Attributes */
enum abis_nm_attr {
        NM_ATT_ABIS_CHANNEL     = 0x01,
        NM_ATT_ADD_INFO,
        NM_ATT_ADD_TEXT,
        NM_ATT_ADM_STATE,
        NM_ATT_ARFCN_LIST,
        NM_ATT_AUTON_REPORT,
        NM_ATT_AVAIL_STATUS,
        NM_ATT_BCCH_ARFCN,
        NM_ATT_BSIC,
        NM_ATT_BTS_AIR_TIMER,
        NM_ATT_CCCH_L_I_P,
        NM_ATT_CCCH_L_T,
        NM_ATT_CHAN_COMB,
        NM_ATT_CONN_FAIL_CRIT,
        NM_ATT_DEST,
        /* res */
        NM_ATT_EVENT_TYPE       = 0x11, /* BS11: file data ?!? */
        NM_ATT_FILE_ID,
        NM_ATT_FILE_VERSION,
        NM_ATT_GSM_TIME,
        NM_ATT_HSN,
        NM_ATT_HW_CONFIG,
        NM_ATT_HW_DESC,
        NM_ATT_INTAVE_PARAM,
        NM_ATT_INTERF_BOUND,
        NM_ATT_LIST_REQ_ATTR,
        NM_ATT_MAIO,
        NM_ATT_MANUF_STATE,
        NM_ATT_MANUF_THRESH,
        NM_ATT_MANUF_ID,
        NM_ATT_MAX_TA,
        NM_ATT_MDROP_LINK,      /* 0x20 */
        NM_ATT_MDROP_NEXT,
        NM_ATT_NACK_CAUSES,
        NM_ATT_NY1,
        NM_ATT_OPER_STATE,
        NM_ATT_OVERL_PERIOD,
        NM_ATT_PHYS_CONF,
        NM_ATT_POWER_CLASS,
        NM_ATT_POWER_THRESH,
        NM_ATT_PROB_CAUSE,
        NM_ATT_RACH_B_THRESH,
        NM_ATT_LDAVG_SLOTS,
        NM_ATT_RAD_SUBC,
        NM_ATT_RF_MAXPOWR_R,
        NM_ATT_SITE_INPUTS,
        NM_ATT_SITE_OUTPUTS,
        NM_ATT_SOURCE,          /* 0x30 */
        NM_ATT_SPEC_PROB,
        NM_ATT_START_TIME,
        NM_ATT_T200,
        NM_ATT_TEI,
        NM_ATT_TEST_DUR,
        NM_ATT_TEST_NO,
        NM_ATT_TEST_REPORT,
        NM_ATT_VSWR_THRESH,
        NM_ATT_WINDOW_SIZE,
        /* Res  */
        NM_ATT_TSC              = 0x40,
        NM_ATT_SW_CONFIG,
        NM_ATT_SW_DESCR,
        NM_ATT_SEVERITY,
        NM_ATT_GET_ARI,
        NM_ATT_HW_CONF_CHG,
        NM_ATT_OUTST_ALARM,
        NM_ATT_FILE_DATA,
        NM_ATT_MEAS_RES,
        NM_ATT_MEAS_TYPE,

};


int receive_msg( char *msg, int sd);

#endif /*  BTS_NM_H */
